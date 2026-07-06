#include "string.h"

#include "bss_early_data.h"
#include "cans_validated_frame.h"
#include "family_resource_runtime.h"
#include "front_end_cel_runtime.h"
#include "rw_semantic_data.h"

#define CANS_READ_SIGNED_BE16(coordinate) \
        ((int)(int16)((((uint32)((const uint8 *)(coordinate))[0]) << 8) | \
                      ((const uint8 *)(coordinate))[1]))

typedef struct CansFrameStreamHeader
{
  CansChunkHeader chunk;
  int version;
  int animation_type;
  int frame_count;
  int frame_rate;
  int start_frame;
  int loop_count;
} CansFrameStreamHeader;


__inline
static
CansChunkHeader *
next_cans_chunk(CansChunkHeader *chunk)
{
  return (CansChunkHeader *)((char *)chunk + chunk->byte_count);
}


static
__inline
int
cans_family_chunk_header_is_readable(const CansChunkHeader *chunk,
                                     const void            *loaded_end)
{
  unsigned long chunk_address;
  unsigned long end_address;

  if(loaded_end == 0)
    {
      return 1;
    }
  chunk_address = (unsigned long)chunk;
  end_address = (unsigned long)loaded_end;
  if(chunk_address > end_address ||
     sizeof(CansChunkHeader) > end_address - chunk_address)
    {
      return 0;
    }
  return 1;
}


static
__inline
int
cans_family_chunk_is_bounded(const CansChunkHeader *chunk,
                             const void            *loaded_end)
{
  unsigned long chunk_address;
  unsigned long chunk_bytes;
  unsigned long end_address;

  if(loaded_end == 0)
    {
      return 1;
    }
  if(cans_family_chunk_header_is_readable(chunk, loaded_end) == 0)
    {
      return 0;
    }
  chunk_address = (unsigned long)chunk;
  chunk_bytes = chunk->byte_count;
  end_address = (unsigned long)loaded_end;
  if(chunk_bytes < sizeof(CansChunkHeader) ||
     chunk_bytes > end_address - chunk_address)
    {
      return 0;
    }
  return 1;
}


static
__inline
int
cans_family_range_is_readable(const void   *value,
                              unsigned long byte_count,
                              const void   *loaded_end)
{
  unsigned long value_address;
  unsigned long end_address;

  if(loaded_end == 0)
    {
      return 1;
    }
  value_address = (unsigned long)value;
  end_address = (unsigned long)loaded_end;
  if(value_address > end_address ||
     byte_count > end_address - value_address)
    {
      return 0;
    }
  return 1;
}


static
__inline
int
cans_family_chunk_has_minimum_bytes(const CansChunkHeader *chunk,
                                    unsigned long          minimum_bytes,
                                    const void            *loaded_end)
{
  if(loaded_end == 0)
    {
      return 1;
    }
  return cans_family_chunk_is_bounded(chunk, loaded_end) != 0 &&
         chunk->byte_count >= minimum_bytes;
}


__inline
static
int
cans_family_plut_chunk_is_readable(const CansPlutChunk *plut_chunk,
                                   const void          *loaded_end)
{
  const CansChunkHeader *chunk;
  unsigned long entry_bytes;
  unsigned long payload_bytes;

  if(loaded_end == 0)
    {
      return 1;
    }
  chunk = (const CansChunkHeader *)plut_chunk;
  if(cans_family_chunk_has_minimum_bytes(
       chunk, offsetof(CansPlutChunk, entries), loaded_end) == 0 ||
     plut_chunk->entry_count <= 0)
    {
      return 0;
    }
  payload_bytes = chunk->byte_count -
                  (unsigned long)offsetof(CansPlutChunk, entries);
  entry_bytes = (unsigned long)plut_chunk->entry_count *
                sizeof(plut_chunk->entries[0]);
  return entry_bytes / sizeof(plut_chunk->entries[0]) ==
         (unsigned long)plut_chunk->entry_count &&
         entry_bytes <= payload_bytes;
}


__inline
static
int
cans_family_hotspot_run_is_readable(const CansHotspotRecord *record,
                                    int                      count,
                                    const void              *chunk_end,
                                    const void              *loaded_end)
{
  unsigned long available_records;
  unsigned long record_address;
  unsigned long end_address;

  if(loaded_end == 0)
    {
      return 1;
    }
  if(count <= 0)
    {
      return 0;
    }
  record_address = (unsigned long)record;
  end_address = (unsigned long)chunk_end;
  if(record_address > end_address)
    {
      return 0;
    }
  available_records = (end_address - record_address) /
                      sizeof(CansHotspotRecord);
  return (unsigned long)count <= available_records;
}


__inline
static
int
round_half_toward_zero(int value)
{
  return (value +
          (int)((unsigned int)value >> CANS_WORD_SIGN_BIT_SHIFT)) >> 1;
}


__inline
static
int
decode_ccb_extent(int value)
{
  if(value < 0)
    {
      return (int)(1U <<
                   (value & CANS_CCB_DIMENSION_EXPONENT_MASK));
    }
  return value & CANS_CCB_DIMENSION_LITERAL_MASK;
}


__inline
static
int
hotspot_record_count(const CansHotspotRecord *record)
{
  return (int)(record->control >> CANS_HOTSPOT_RECORD_COUNT_SHIFT);
}


/* The chunk chain from an animation's first pixel chunk is fixed for as long
   as the resource stays loaded, but the frame walk re-validates it from the
   first frame on every decode.  Remember the chunks that walk has already
   validated, per animation.  The entry is keyed on the containing family
   group's generation as well as the animation and its loaded extent, so a
   resource reload drops it. */
enum CansFrameChunkCacheValue
{
  CANS_FRAME_CHUNK_CACHE_SLOTS = 16,
  CANS_FRAME_CHUNK_CACHE_DEPTH = 64
};

typedef struct CansFrameChunkCacheEntry
{
  const CansAnimationData *animation;
  const void *loaded_end;
  unsigned int generation;
  int chunk_count;
  CansChunkHeader *chunks[CANS_FRAME_CHUNK_CACHE_DEPTH];
} CansFrameChunkCacheEntry;

static CansFrameChunkCacheEntry
  cans_frame_chunk_cache[CANS_FRAME_CHUNK_CACHE_SLOTS];

static
CansFrameChunkCacheEntry *
cans_frame_chunk_cache_for(const CansAnimationData *animation,
                           const void              *loaded_end,
                           unsigned int             generation)
{
  CansFrameChunkCacheEntry *entry;
  unsigned int slot;

  if(generation == 0U || loaded_end == 0 || animation == 0)
    {
      return 0;
    }
  slot = (unsigned int)(((unsigned long)animation >> 4) &
                        (CANS_FRAME_CHUNK_CACHE_SLOTS - 1));
  entry = &cans_frame_chunk_cache[slot];
  if(entry->animation != animation ||
     entry->loaded_end != loaded_end ||
     entry->generation != generation)
    {
      entry->animation = animation;
      entry->loaded_end = loaded_end;
      entry->generation = generation;
      entry->chunk_count = 0;
    }
  return entry;
}


static
void
decode_cans_animation_frame_output(CansAnimationData        *animation,
                                   int                       frame,
                                   CansAnimationFrameOutput *output)
{
  CansChunkHeader *chunk;
  CansFileOffsetChunk *offset_chunk;
  CansFrameStreamHeader *stream_header;
  CansCcbChunk *ccb_chunk;
  CansCenterPointChunk *center_chunk;
  CansHotspotChunk *hotspot_chunk;
  CansHotspotRecord *hotspot_frame;
  CansHotspotRecord *hotspot_record;
  CansHotspotRecord *hotspot_endpoint;
  CansPackedPosition *wide_center;
  CansCompactCenterPair *compact_center;
  CansPixelDataChunk *pixel_chunk;
  CansLegacyReducedPixelDataChunk *reduced_chunk;
  CansPlutChunk *plut_chunk;
  CansHotspotVector *vector;
  FrontEndCelOffset *decoded_hotspot;
  CCB *tail_ccb;
  const void *hotspot_chunk_end;
  uint32 tag;
  int metadata_request;
  int has_explicit_center;
  int has_ccb;
  int uses_full_pixel_data;
  int half_frame;
  int odd_frame;
  int count;
  int i;
  int vector_index;
  int base_x;
  int base_y;
  int decoded_x;
  int decoded_y;
  int value;
  int frames_to_skip;
  int top;
  int left;
  int bottom;
  int right;
  int extent_status;
  const void *loaded_end;
  unsigned long chunk_address;
  unsigned long end_address;
  unsigned long offset_bytes;
  unsigned int extent_group_index;
  unsigned int frame_chunk_generation;
  CansFrameChunkCacheEntry *frame_chunk_cache;
  int known_frames;
  int record_index;

  /* Metadata callers intentionally pass the 0x1C-byte metadata prefix
     through this legacy API.  Clear only that public prefix for the
     sentinel request; a full-frame request owns the complete 0x3C-byte
     output, including direct-color streams that legitimately omit PLUT. */
  /* Clear the scalar fields directly.  The hotspot vector array is only ever
     read below hotspot_count, which is cleared here, so zeroing its bytes
     would be dead work on every decode.  The metadata prefix stops short of
     animation_flags, matching the size it used to clear. */
  output->ccb = 0;
  output->plut = 0;
  output->pixel_data = 0;
  output->center_x = 0;
  output->center_y = 0;
  output->hotspot_count = 0;
  output->auxiliary.hotspots = 0;
  if(frame != CANS_METADATA_FRAME)
    {
      /* The hotspot vectors are filled by record index, not by count, so an
         entry can stay unwritten while hotspot_count passes it.  Clear them
         exactly as the full-struct clear did. */
      output->auxiliary.hotspot_vectors[0].start_x = 0;
      output->auxiliary.hotspot_vectors[0].start_y = 0;
      output->auxiliary.hotspot_vectors[0].end_x = 0;
      output->auxiliary.hotspot_vectors[0].end_y = 0;
      output->auxiliary.hotspot_vectors[1].start_x = 0;
      output->auxiliary.hotspot_vectors[1].start_y = 0;
      output->auxiliary.hotspot_vectors[1].end_x = 0;
      output->auxiliary.hotspot_vectors[1].end_y = 0;
      output->animation_flags = 0;
    }
  tail_ccb = 0;
  chunk = (CansChunkHeader *)animation;
  loaded_end = 0;
  extent_group_index = (unsigned int)FAMILY_GROUP_COUNT;
  extent_status = family_cans_animation_loaded_extent(
    animation, &loaded_end, &extent_group_index);
  if(extent_status == FAMILY_CANS_ANIMATION_EXTENT_STALE)
    {
      goto reject_frame;
    }
  if(cans_family_chunk_header_is_readable(chunk, loaded_end) == 0)
    {
      goto reject_frame;
    }
  if(chunk->tag == CANS_CHUNK_TAG_OFST)
    {
      offset_chunk = (CansFileOffsetChunk *)chunk;
      if(loaded_end != 0)
        {
          chunk_address = (unsigned long)chunk;
          end_address = (unsigned long)loaded_end;
          offset_bytes = offset_chunk->animation_offset;
          if(offset_bytes == 0U || chunk_address > end_address ||
             offset_bytes > end_address - chunk_address)
            {
              goto reject_frame;
            }
          chunk = (CansChunkHeader *)(chunk_address + offset_bytes);
        }
      else
        {
          chunk = (CansChunkHeader *)
                  ((char *)chunk + offset_chunk->animation_offset);
        }
      if(cans_family_chunk_header_is_readable(
           chunk, loaded_end) == 0)
        {
          goto reject_frame;
        }
    }

  metadata_request = (frame == CANS_METADATA_FRAME);
  has_explicit_center = metadata_request;
  if(metadata_request)
    {
      frame = 0;
    }
  else
    {
      output->animation_flags = 0;
      output->hotspot_count = 0;
    }
  if(frame < 0)
    {
      goto reject_frame;
    }

  tag = chunk->tag;
  if(tag != CANS_CHUNK_TAG_ANIM)
    {
      if(tag == CANS_CHUNK_TAG_OFSS)
        {
          if(loaded_end != 0 &&
             frame >= ROAD_OBJECT_CEL_SCALE_BUCKET_COUNT)
            {
              goto reject_frame;
            }
          CANS_NewGetAnimFrame(
            (CansOffsetAnimation *)chunk, frame, output);
          return;
        }
      if(!metadata_request)
        {
          goto reject_frame;
        }
    }

  if(loaded_end != 0)
    {
      stream_header = (CansFrameStreamHeader *)chunk;
      if(cans_family_chunk_has_minimum_bytes(
           chunk, sizeof(CansFrameStreamHeader), loaded_end) == 0 ||
         stream_header->frame_count <= 0 ||
         frame >= stream_header->frame_count)
        {
          goto reject_frame;
        }
    }

  output->ccb = 0;
  odd_frame = frame & 1;
  half_frame = round_half_toward_zero(frame);
  uses_full_pixel_data = 0;
  has_ccb = 0;

  for(;;)
    {
      if(cans_family_chunk_header_is_readable(
           chunk, loaded_end) == 0)
        {
          goto reject_frame;
        }
      tag = chunk->tag;
      if(tag == CANS_CHUNK_TAG_PDAT)
        {
          uses_full_pixel_data = 1;
          break;
        }
      if(tag == CANS_CHUNK_TAG_RPDT)
        {
          break;
        }
      if(cans_family_chunk_is_bounded(chunk, loaded_end) == 0)
        {
          goto reject_frame;
        }

      if(tag < CANS_CHUNK_TAG_PDAT)
        {
          if(tag == CANS_CHUNK_TAG_CCB)
            {
              if(cans_family_chunk_has_minimum_bytes(
                   chunk, sizeof(CansCcbChunk), loaded_end) == 0)
                {
                  goto reject_frame;
                }
              ccb_chunk = (CansCcbChunk *)chunk;
              output->ccb = &ccb_chunk->ccb;
              tail_ccb = output->ccb;
              tail_ccb->ccb_NextPtr = 0;
              has_ccb = 1;
            }
          else if(tag == CANS_CHUNK_TAG_CTPT)
            {
              if(cans_family_chunk_has_minimum_bytes(
                   chunk, offsetof(CansCenterPointChunk, centers),
                   loaded_end) == 0)
                {
                  goto reject_frame;
                }
              center_chunk = (CansCenterPointChunk *)chunk;
              has_explicit_center = 1;
              if(center_chunk->encoding ==
                 CANS_CENTER_ENCODING_WIDE)
                {
                  wide_center = &center_chunk->centers.wide[frame];
                  if(loaded_end != 0 &&
                     cans_family_range_is_readable(
                       wide_center, sizeof(*wide_center),
                       (const char *)chunk + chunk->byte_count) == 0)
                    {
                      goto reject_frame;
                    }
                  output->center_x = wide_center->x;
                  output->center_y = wide_center->y;
                }
              else
                {
                  compact_center =
                    &center_chunk->centers.compact[half_frame];
                  if(loaded_end != 0 &&
                     cans_family_range_is_readable(
                       compact_center, sizeof(*compact_center),
                       (const char *)chunk + chunk->byte_count) == 0)
                    {
                      goto reject_frame;
                    }
                  if(odd_frame == 0)
                    {
                      output->center_x = compact_center->even_x;
                      output->center_y = compact_center->even_y;
                    }
                  else
                    {
                      output->center_x = compact_center->odd_x;
                      output->center_y = compact_center->odd_y;
                    }
                }
            }
          else if(tag == CANS_CHUNK_TAG_HSPT)
            {
              if(cans_family_chunk_has_minimum_bytes(
                   chunk,
                   offsetof(CansHotspotChunk, records) +
                   sizeof(CansHotspotRecord),
                   loaded_end) == 0)
                {
                  goto reject_frame;
                }
              hotspot_chunk = (CansHotspotChunk *)chunk;
              hotspot_frame = hotspot_chunk->records;
              hotspot_chunk_end = 0;
              if(loaded_end != 0)
                {
                  hotspot_chunk_end = (const void *)
                                      ((unsigned long)chunk + chunk->byte_count);
                }
              if(metadata_request)
                {
                  output->auxiliary.hotspots =
                    (FrontEndCelOffset *)hotspot_frame;
                  if(hotspot_chunk->relocation_state < 0)
                    {
                      output->hotspot_count =
                        (int)hotspot_chunk->count.decoded_hotspot_count;
                    }
                  else
                    {
                      count = hotspot_record_count(hotspot_frame);
                      if(cans_family_hotspot_run_is_readable(
                           hotspot_frame, count, hotspot_chunk_end,
                           loaded_end) == 0)
                        {
                          goto reject_frame;
                        }
                      hotspot_chunk->relocation_state =
                        -hotspot_chunk->relocation_state;
                      output->hotspot_count = count;
                      hotspot_chunk->count.decoded_hotspot_count = count;
                      for(i = count; i != 0; i--)
                        {
                          decoded_x = hotspot_frame->position.x;
                          decoded_y =
                            (int)(int16)hotspot_frame->position.y;
                          decoded_hotspot =
                            (FrontEndCelOffset *)hotspot_frame;
                          decoded_hotspot->x = decoded_x;
                          decoded_hotspot->y = decoded_y;
                          hotspot_frame++;
                        }
                    }
                }
              else
                {
                  for(i = 0; i < frame; i++)
                    {
                      count = hotspot_record_count(hotspot_frame);
                      if(cans_family_hotspot_run_is_readable(
                           hotspot_frame, count, hotspot_chunk_end,
                           loaded_end) == 0)
                        {
                          goto reject_frame;
                        }
                      hotspot_frame += count;
                    }
                  if(cans_family_hotspot_run_is_readable(
                       hotspot_frame, 1, hotspot_chunk_end,
                       loaded_end) == 0)
                    {
                      goto reject_frame;
                    }
                  if((hotspot_frame->control &
                      CANS_HOTSPOT_CENTER_PRESENT) != 0)
                    {
                      output->center_x = hotspot_frame->position.x;
                      output->center_y = hotspot_frame->position.y;
                      has_explicit_center = 1;

                      i = frame + 1;
                      while(i < hotspot_chunk->count.encoded_frame_count)
                        {
                          count = hotspot_record_count(hotspot_frame);
                          if(cans_family_hotspot_run_is_readable(
                               hotspot_frame, count,
                               hotspot_chunk_end, loaded_end) == 0)
                            {
                              goto reject_frame;
                            }
                          hotspot_frame += count;
                          i++;
                        }

                      count = hotspot_record_count(hotspot_frame);
                      if(cans_family_hotspot_run_is_readable(
                           hotspot_frame, count, hotspot_chunk_end,
                           loaded_end) == 0)
                        {
                          goto reject_frame;
                        }
                      for(i = 1; i + 1 < count;
                          i += CANS_HOTSPOT_ENDPOINT_STRIDE)
                        {
                          hotspot_record = &hotspot_frame[i];
                          hotspot_endpoint = hotspot_record + 1;
                          if(((hotspot_record->control &
                               CANS_HOTSPOT_CENTER_PRESENT) != 0) &&
                             ((hotspot_endpoint->control &
                               CANS_HOTSPOT_CENTER_PRESENT) != 0))
                            {
                              vector_index =
                                round_half_toward_zero(i);
                              if(vector_index >=
                                 CANS_MAX_HOTSPOT_VECTORS)
                                {
                                  break;
                                }
                              base_x = hotspot_frame->position.x;
                              base_y = hotspot_frame->position.y;
                              vector = &output->auxiliary.hotspot_vectors[
                                vector_index];
                              vector->start_x =
                                (hotspot_record->position.x - base_x) *
                                CANS_HOTSPOT_COORDINATE_SCALE;
                              vector->start_y =
                                (hotspot_record->position.y - base_y) *
                                CANS_HOTSPOT_COORDINATE_SCALE;
                              vector->end_x =
                                (hotspot_endpoint->position.x - base_x) *
                                CANS_HOTSPOT_COORDINATE_SCALE;
                              vector->end_y =
                                (hotspot_endpoint->position.y - base_y) *
                                CANS_HOTSPOT_COORDINATE_SCALE;
                              output->hotspot_count++;
                            }
                        }
                    }
                }
            }
        }
      else if(tag == CANS_CHUNK_TAG_PLUT)
        {
          plut_chunk = (CansPlutChunk *)chunk;
          if(cans_family_plut_chunk_is_readable(
               plut_chunk, loaded_end) == 0)
            {
              goto reject_frame;
            }
          output->plut = plut_chunk->entries;
        }

      /* The chunk was already proved bounded above, and nothing between that
         check and here writes the chunk header (the CCB, centre and hotspot
         handlers only touch fields at or after offset 8, and the hotspot
         relocation stays inside the chunk's own record run), so re-testing
         the same predicate here could only ever repeat the earlier verdict. */
      chunk = next_cans_chunk(chunk);
    }

  /*
   * Direct-color CELs legitimately omit PLUT.  The oracle continues from
   * the first PDAT/RPDT chunk after finding a CCB, whether or not a PLUT
   * chunk appeared.  Requiring PLUT here made the front-end's uncoded title
   * and menu CELs decode to a null CCB after movie playback.
   */
  if(has_ccb == 0)
    {
      goto reject_frame;
    }

  /* Remember which chunks the frame walk has already validated for this
     animation, so a frame inside that range needs no walk and a later frame
     resumes from the last validated chunk.  Every check a full walk performs
     still happens: the skipped ones were performed when the entry was
     recorded, against the same loaded extent and resource generation. */
  frame_chunk_generation = 0;
  if(extent_group_index < (unsigned int)FAMILY_GROUP_COUNT)
    {
      frame_chunk_generation = family_resource_generations[extent_group_index];
    }
  frame_chunk_cache = cans_frame_chunk_cache_for(
    animation, loaded_end, frame_chunk_generation);

  frames_to_skip = frame;
  if(frame_chunk_cache != 0 &&
     frame < frame_chunk_cache->chunk_count)
    {
      chunk = frame_chunk_cache->chunks[frame];
    }
  else
    {
      known_frames = 0;
      record_index = 0;
      if(frame_chunk_cache != 0)
        {
          known_frames = frame_chunk_cache->chunk_count;
          if(known_frames > 0)
            {
              chunk = frame_chunk_cache->chunks[known_frames - 1];
              frames_to_skip = frame - (known_frames - 1);
              record_index = known_frames;
            }
          else
            {
              frame_chunk_cache->chunks[0] = chunk;
              frame_chunk_cache->chunk_count = 1;
              record_index = 1;
            }
        }
      while(frames_to_skip != 0)
        {
          frames_to_skip--;
          if(cans_family_chunk_is_bounded(chunk, loaded_end) == 0)
            {
              goto reject_frame;
            }
          chunk = next_cans_chunk(chunk);
          if(cans_family_chunk_header_is_readable(
               chunk, loaded_end) == 0)
            {
              goto reject_frame;
            }
          tag = uses_full_pixel_data ?
                CANS_CHUNK_TAG_PDAT : CANS_CHUNK_TAG_RPDT;
          if(chunk->tag != tag)
            {
              goto reject_frame;
            }
          if(frame_chunk_cache != 0 &&
             record_index < CANS_FRAME_CHUNK_CACHE_DEPTH)
            {
              frame_chunk_cache->chunks[record_index] = chunk;
              frame_chunk_cache->chunk_count = record_index + 1;
              record_index++;
            }
        }
    }

  if(cans_family_chunk_is_bounded(chunk, loaded_end) == 0)
    {
      goto reject_frame;
    }

  if(uses_full_pixel_data)
    {
      if(cans_family_chunk_has_minimum_bytes(
           chunk, sizeof(CansPixelDataChunk), loaded_end) == 0)
        {
          goto reject_frame;
        }
      pixel_chunk = (CansPixelDataChunk *)chunk;
      output->pixel_data = (CelData *)pixel_chunk->pixels;
    }
  else
    {
      if(cans_family_chunk_has_minimum_bytes(
           chunk, sizeof(CansLegacyReducedPixelDataChunk),
           loaded_end) == 0)
        {
          goto reject_frame;
        }
      reduced_chunk = (CansLegacyReducedPixelDataChunk *)chunk;
      output->pixel_data = (CelData *)reduced_chunk->pixels;

      top = CANS_READ_SIGNED_BE16(&reduced_chunk->top);
      left = CANS_READ_SIGNED_BE16(&reduced_chunk->left);
      bottom = CANS_READ_SIGNED_BE16(&reduced_chunk->bottom);
      right = CANS_READ_SIGNED_BE16(&reduced_chunk->right);

      if(right - left < 0 ||
         right - left >= CEL_DIMENSION_ENTRY_COUNT ||
         bottom - top < 0 ||
         bottom - top >= CEL_DIMENSION_ENTRY_COUNT)
        {
          goto reject_frame;
        }

      output->ccb->ccb_Width =
        (int)gCelDimensionPreambleTable[right - left];
      output->ccb->ccb_Height =
        (int)gCelDimensionPreambleTable[bottom - top];
      output->center_x -= left;
      output->center_y -= top;
    }

  if(!has_explicit_center)
    {
      value = (int)output->ccb->ccb_Width;
      output->center_x =
        round_half_toward_zero(decode_ccb_extent(value));
      value = (int)output->ccb->ccb_Height;
      output->center_y = decode_ccb_extent(value);
    }

  output->ccb->ccb_PLUTPtr = output->plut;
  output->ccb->ccb_SourcePtr = output->pixel_data;

  chunk = next_cans_chunk(chunk);
  if(loaded_end != 0 &&
     (unsigned long)chunk == (unsigned long)loaded_end)
    {
      return;
    }
  if(cans_family_range_is_readable(
       chunk, sizeof(chunk->tag), loaded_end) == 0)
    {
      return;
    }
  tag = chunk->tag;
  if(tag != CANS_CHUNK_TAG_CCB)
    {
      return;
    }
  if(cans_family_chunk_header_is_readable(
       chunk, loaded_end) == 0)
    {
      return;
    }
  if(tail_ccb == 0)
    {
      return;
    }

  for(;;)
    {
      tag = chunk->tag;
      if(tag == CANS_CHUNK_TAG_CCB)
        {
          if(cans_family_chunk_has_minimum_bytes(
               chunk, sizeof(CansCcbChunk), loaded_end) == 0)
            {
              return;
            }
          ccb_chunk = (CansCcbChunk *)chunk;
          tail_ccb->ccb_NextPtr = &ccb_chunk->ccb;
          tail_ccb = &ccb_chunk->ccb;
          tail_ccb->ccb_NextPtr = 0;
        }
      else if(tag == CANS_CHUNK_TAG_PDAT)
        {
          if(cans_family_chunk_has_minimum_bytes(
               chunk, sizeof(CansPixelDataChunk), loaded_end) == 0)
            {
              return;
            }
          pixel_chunk = (CansPixelDataChunk *)chunk;
          tail_ccb->ccb_SourcePtr = (CelData *)pixel_chunk->pixels;
        }
      else if(tag == CANS_CHUNK_TAG_PLUT)
        {
          plut_chunk = (CansPlutChunk *)chunk;
          if(cans_family_plut_chunk_is_readable(
               plut_chunk, loaded_end) == 0)
            {
              return;
            }
          tail_ccb->ccb_PLUTPtr = plut_chunk->entries;
        }
      else
        {
          return;
        }
      if(cans_family_chunk_is_bounded(chunk, loaded_end) == 0)
        {
          return;
        }
      chunk = next_cans_chunk(chunk);
      if(loaded_end != 0 &&
         (unsigned long)chunk == (unsigned long)loaded_end)
        {
          return;
        }
      if(cans_family_range_is_readable(
           chunk, sizeof(chunk->tag), loaded_end) == 0)
        {
          return;
        }
    }

reject_frame:
  output->ccb = 0;
  output->animation_flags = 0;
  return;
}


void
CANS_GetAnimFrame(CansAnimationData        *animation,
                  int                       frame,
                  CansAnimationFrameOutput *output)
{
  /*
   * The oracle API accepts CANS_METADATA_FRAME with a caller-owned
   * CansAnimationMetadata object cast to the larger frame-output type.
   * Decode into a complete temporary so every legacy success and failure
   * path, including the OFSS delegate, can retain its original writes
   * without touching the caller beyond the 0x1C-byte metadata prefix.
   */
  if(frame == CANS_METADATA_FRAME)
    {
      CansAnimationFrameOutput metadata_frame;

      decode_cans_animation_frame_output(animation, frame, &metadata_frame);
      memcpy(output, &metadata_frame, sizeof(CansAnimationMetadata));
      return;
    }

  decode_cans_animation_frame_output(animation, frame, output);
}


void
CANS_GetValidatedFamilyAnimFrame(CansAnimationData        *animation,
                                 int                       frame,
                                 CansAnimationFrameOutput *output)
{
  /*
   * Family callers may use this only while the render barrier keeps the
   * resource generation stable and after the complete stream validator has
   * accepted this animation.  Re-query the loaded extent so frame selection
   * and every decoder read remain bounded without repeating root validation.
   */
  decode_cans_animation_frame_output(animation, frame, output);
}
