#include <limits.h>

#include "cans_family_validation.h"
#include "family_resource_runtime.h"
#include "front_end_cel_runtime.h"
#include "rw_semantic_data.h"

typedef struct FamilyCansAnimationHeader
{
  CansChunkHeader chunk;
  int version;
  int animation_type;
  int frame_count;
  int frame_rate;
  int start_frame;
  int loop_count;
} FamilyCansAnimationHeader;

enum FamilyCansCoordinateByte
{
  FAMILY_CANS_TOP_HIGH_BYTE = 0,
  FAMILY_CANS_TOP_LOW_BYTE = 1,
  FAMILY_CANS_LEFT_HIGH_BYTE = 2,
  FAMILY_CANS_LEFT_LOW_BYTE = 3,
  FAMILY_CANS_BOTTOM_HIGH_BYTE = 4,
  FAMILY_CANS_BOTTOM_LOW_BYTE = 5,
  FAMILY_CANS_RIGHT_HIGH_BYTE = 6,
  FAMILY_CANS_RIGHT_LOW_BYTE = 7,
  FAMILY_CANS_COORDINATE_BYTE_SHIFT = 8
};

enum FamilyCansValidationCacheConstant
{
  FAMILY_RESOURCE_SELECTOR_ENTRY_CAPACITY =
    FAMILY_RESOURCE_SELECTOR_ENTRY_MASK + 1,
  FAMILY_CANS_VALIDATION_CACHE_COUNT =
    FAMILY_GROUP_COUNT * FAMILY_RESOURCE_SELECTOR_ENTRY_CAPACITY
};

typedef struct FamilyCansValidationCacheEntry
{
  const CansAnimationData *animation;
  unsigned int generation;
  int valid;
} FamilyCansValidationCacheEntry;

static FamilyCansValidationCacheEntry
  family_cans_validation_cache[FAMILY_CANS_VALIDATION_CACHE_COUNT];

static
int
family_cans_range_contains(unsigned int  group_index,
                           const void   *value,
                           unsigned long byte_count)
{
  unsigned long loaded_bytes;
  unsigned long base_address;
  unsigned long end_address;
  unsigned long value_address;
  unsigned long table_bytes;

  if(family_resource_globals.group_count <= 0 ||
     group_index >= (unsigned int)family_resource_globals.group_count ||
     family_resource_globals.group_tables[group_index] == 0 ||
     family_resource_globals.table_bytes <= 0)
    {
      return 0;
    }

  table_bytes = (unsigned long)family_resource_globals.table_bytes;
  loaded_bytes = family_resource_loaded_bytes[group_index];
  if(loaded_bytes == 0U || loaded_bytes > table_bytes ||
     byte_count > loaded_bytes)
    {
      return 0;
    }

  base_address = (unsigned long)
                 family_resource_globals.group_tables[group_index];
  end_address = base_address + loaded_bytes;
  value_address = (unsigned long)value;
  if(end_address < base_address || value_address < base_address)
    {
      return 0;
    }

  return value_address <= end_address - byte_count;
}


static
int
family_cans_pointer_is_at_loaded_end(unsigned int group_index,
                                     const void  *value)
{
  unsigned long base_address;
  unsigned long end_address;
  unsigned long loaded_bytes;
  unsigned long table_bytes;

  if(family_resource_globals.group_count <= 0 ||
     group_index >= (unsigned int)family_resource_globals.group_count ||
     family_resource_globals.group_tables[group_index] == 0 ||
     family_resource_globals.table_bytes <= 0)
    {
      return 0;
    }

  table_bytes = (unsigned long)family_resource_globals.table_bytes;
  loaded_bytes = family_resource_loaded_bytes[group_index];
  if(loaded_bytes == 0U || loaded_bytes > table_bytes)
    {
      return 0;
    }

  base_address = (unsigned long)
                 family_resource_globals.group_tables[group_index];
  end_address = base_address + loaded_bytes;
  if(end_address < base_address)
    {
      return 0;
    }
  return (unsigned long)value == end_address;
}


int
family_cans_animation_loaded_extent(const CansAnimationData *animation,
                                    const void             **loaded_end,
                                    unsigned int            *group_index)
{
  unsigned int scan_index;
  unsigned long allocation_end;
  unsigned long animation_address;
  unsigned long base_address;
  unsigned long end_address;
  unsigned long loaded_bytes;
  unsigned long table_bytes;

  if(loaded_end == 0)
    {
      return FAMILY_CANS_ANIMATION_EXTENT_STALE;
    }
  *loaded_end = 0;
  if(animation == 0)
    {
      return FAMILY_CANS_ANIMATION_EXTENT_STALE;
    }
  if(family_resource_globals.group_count <= 0)
    {
      return FAMILY_CANS_ANIMATION_EXTENT_UNBOUNDED;
    }
  if(family_resource_globals.group_count > FAMILY_GROUP_COUNT ||
     family_resource_globals.table_bytes <= 0)
    {
      return FAMILY_CANS_ANIMATION_EXTENT_STALE;
    }
  animation_address = (unsigned long)animation;
  table_bytes = (unsigned long)family_resource_globals.table_bytes;

  for(scan_index = 0U;
      scan_index < (unsigned int)family_resource_globals.group_count;
      scan_index++)
    {
      if(family_resource_globals.group_tables[scan_index] == 0)
        {
          continue;
        }
      base_address = (unsigned long)
                     family_resource_globals.group_tables[scan_index];
      allocation_end = base_address + table_bytes;
      if(allocation_end < base_address ||
         animation_address < base_address ||
         animation_address >= allocation_end)
        {
          continue;
        }

      /* The containing group is known from here on, even on the stale paths
         below: callers use it to key per-animation caches on the group's
         generation. */
      if(group_index != 0)
        {
          *group_index = scan_index;
        }

      loaded_bytes = family_resource_loaded_bytes[scan_index];
      if(loaded_bytes == 0U || loaded_bytes > table_bytes)
        {
          return FAMILY_CANS_ANIMATION_EXTENT_STALE;
        }
      end_address = base_address + loaded_bytes;
      if(end_address < base_address)
        {
          return FAMILY_CANS_ANIMATION_EXTENT_STALE;
        }
      if(animation_address >= end_address)
        {
          return FAMILY_CANS_ANIMATION_EXTENT_STALE;
        }
      *loaded_end = (const void *)end_address;
      return FAMILY_CANS_ANIMATION_EXTENT_BOUNDED;
    }
  return FAMILY_CANS_ANIMATION_EXTENT_UNBOUNDED;
}


static
int
family_cans_chunk_is_bounded(unsigned int           group_index,
                             const CansChunkHeader *chunk)
{
  unsigned long chunk_bytes;

  if(family_cans_range_contains(
       group_index, chunk, sizeof(CansChunkHeader)) == 0)
    {
      return 0;
    }

  chunk_bytes = chunk->byte_count;
  if(chunk_bytes < sizeof(CansChunkHeader))
    {
      return 0;
    }

  return family_cans_range_contains(
    group_index, chunk, chunk_bytes);
}


static
const
CansChunkHeader *
family_cans_next_chunk(const CansChunkHeader *chunk)
{
  return (const CansChunkHeader *)
         ((const char *)chunk + chunk->byte_count);
}


static
int
family_cans_plut_chunk_is_bounded(unsigned int         group_index,
                                  const CansPlutChunk *plut_chunk)
{
  const CansChunkHeader *chunk;
  unsigned long entry_bytes;
  unsigned long payload_bytes;

  chunk = (const CansChunkHeader *)plut_chunk;
  if(plut_chunk == 0 ||
     family_cans_chunk_is_bounded(group_index, chunk) == 0 ||
     chunk->tag != CANS_CHUNK_TAG_PLUT ||
     chunk->byte_count < offsetof(CansPlutChunk, entries) ||
     plut_chunk->entry_count <= 0)
    {
      return 0;
    }

  payload_bytes = chunk->byte_count -
                  (unsigned long)offsetof(CansPlutChunk, entries);
  entry_bytes = (unsigned long)plut_chunk->entry_count *
                sizeof(plut_chunk->entries[0]);
  if(entry_bytes / sizeof(plut_chunk->entries[0]) !=
     (unsigned long)plut_chunk->entry_count ||
     entry_bytes > payload_bytes)
    {
      return 0;
    }
  return 1;
}


static
int
family_cans_pixel_chunk_is_bounded(unsigned int           group_index,
                                   const CansChunkHeader *chunk,
                                   unsigned long          expected_tag)
{
  int bottom;
  int height;
  int left;
  int right;
  int top;
  int width;
  const CansLegacyReducedPixelDataChunk *reduced_chunk;
  const unsigned char *coordinates;

  if(family_cans_chunk_is_bounded(group_index, chunk) == 0 ||
     chunk->tag != expected_tag)
    {
      return 0;
    }
  if(expected_tag == CANS_CHUNK_TAG_PDAT)
    {
      return chunk->byte_count >= sizeof(CansPixelDataChunk);
    }
  if(expected_tag != CANS_CHUNK_TAG_RPDT ||
     chunk->byte_count < sizeof(CansLegacyReducedPixelDataChunk))
    {
      return 0;
    }

  reduced_chunk = (const CansLegacyReducedPixelDataChunk *)chunk;
  coordinates = (const unsigned char *)&reduced_chunk->top;
  top = (int)(int16)(
    ((unsigned int)coordinates[FAMILY_CANS_TOP_HIGH_BYTE] <<
      FAMILY_CANS_COORDINATE_BYTE_SHIFT) |
    coordinates[FAMILY_CANS_TOP_LOW_BYTE]);
  left = (int)(int16)(
    ((unsigned int)coordinates[FAMILY_CANS_LEFT_HIGH_BYTE] <<
      FAMILY_CANS_COORDINATE_BYTE_SHIFT) |
    coordinates[FAMILY_CANS_LEFT_LOW_BYTE]);
  bottom = (int)(int16)(
    ((unsigned int)coordinates[FAMILY_CANS_BOTTOM_HIGH_BYTE] <<
      FAMILY_CANS_COORDINATE_BYTE_SHIFT) |
    coordinates[FAMILY_CANS_BOTTOM_LOW_BYTE]);
  right = (int)(int16)(
    ((unsigned int)coordinates[FAMILY_CANS_RIGHT_HIGH_BYTE] <<
      FAMILY_CANS_COORDINATE_BYTE_SHIFT) |
    coordinates[FAMILY_CANS_RIGHT_LOW_BYTE]);
  width = right - left;
  height = bottom - top;
  return width >= 0 && width < CEL_DIMENSION_ENTRY_COUNT &&
         height >= 0 && height < CEL_DIMENSION_ENTRY_COUNT;
}


static
int
family_cans_center_chunk_is_bounded(unsigned int                group_index,
                                    const CansCenterPointChunk *center_chunk,
                                    unsigned int                frame_count)
{
  const CansChunkHeader *chunk;
  unsigned long center_bytes;
  unsigned long center_count;
  unsigned long payload_bytes;

  chunk = (const CansChunkHeader *)center_chunk;
  if(family_cans_chunk_is_bounded(group_index, chunk) == 0 ||
     chunk->tag != CANS_CHUNK_TAG_CTPT ||
     chunk->byte_count < offsetof(CansCenterPointChunk, centers) ||
     center_chunk->frame_count != (int)frame_count)
    {
      return 0;
    }

  if(center_chunk->encoding == CANS_CENTER_ENCODING_WIDE)
    {
      center_count = frame_count;
      center_bytes = sizeof(CansPackedPosition);
    }
  else
    {
      center_count = ((unsigned long)frame_count + 1U) >> 1;
      center_bytes = sizeof(CansCompactCenterPair);
    }
  payload_bytes = chunk->byte_count -
                  (unsigned long)offsetof(CansCenterPointChunk, centers);
  if(center_count > payload_bytes / center_bytes)
    {
      return 0;
    }
  return 1;
}


static
int
family_cans_hotspot_chunk_is_bounded(unsigned int            group_index,
                                     const CansHotspotChunk *hotspot_chunk,
                                     unsigned int            frame_count)
{
  const CansChunkHeader *chunk;
  const CansHotspotRecord *record;
  unsigned int frame_index;
  unsigned long available_records;
  unsigned long chunk_end;
  unsigned long payload_bytes;
  unsigned long record_address;
  unsigned long record_count;

  chunk = (const CansChunkHeader *)hotspot_chunk;
  if(family_cans_chunk_is_bounded(group_index, chunk) == 0 ||
     chunk->tag != CANS_CHUNK_TAG_HSPT ||
     chunk->byte_count <
     offsetof(CansHotspotChunk, records) +
     sizeof(CansHotspotRecord))
    {
      return 0;
    }

  record = hotspot_chunk->records;
  chunk_end = (unsigned long)chunk + chunk->byte_count;
  payload_bytes = chunk->byte_count -
                  (unsigned long)offsetof(CansHotspotChunk, records);
  if(hotspot_chunk->relocation_state < 0)
    {
      if(hotspot_chunk->relocation_state == INT_MIN ||
         (unsigned long)(-hotspot_chunk->relocation_state) !=
         payload_bytes ||
         hotspot_chunk->count.decoded_hotspot_count <= 0 ||
         (unsigned long)hotspot_chunk->count.decoded_hotspot_count >
         payload_bytes / sizeof(CansHotspotRecord))
        {
          return 0;
        }
      return 1;
    }
  if((unsigned long)hotspot_chunk->relocation_state != payload_bytes ||
     hotspot_chunk->count.encoded_frame_count != (int)frame_count)
    {
      return 0;
    }

  for(frame_index = 0U; frame_index < frame_count; frame_index++)
    {
      record_address = (unsigned long)record;
      if(record_address > chunk_end)
        {
          return 0;
        }
      available_records = (chunk_end - record_address) /
                          sizeof(CansHotspotRecord);
      if(available_records == 0U)
        {
          return 0;
        }
      record_count = record->control >>
                     CANS_HOTSPOT_RECORD_COUNT_SHIFT;
      if(record_count == 0U || record_count > available_records)
        {
          return 0;
        }
      record += record_count;
    }

  if((unsigned long)record != chunk_end)
    {
      return 0;
    }
  return 1;
}


static
int
family_cans_offset_animation_is_bounded(unsigned int               group_index,
                                        const CansOffsetAnimation *animation)
{
  const CansChunkHeader *chunk;
  const CansReducedPixelDataChunk *frame_chunk;
  unsigned int frame_index;

  if(family_cans_range_contains(
       group_index, animation, sizeof(CansOffsetAnimation)) == 0 ||
     animation->frame_chunks == 0 ||
     family_cans_range_contains(
       group_index, animation->frame_chunks,
       sizeof(CansReducedPixelDataChunk *) *
       ROAD_OBJECT_CEL_SCALE_BUCKET_COUNT) == 0)
    {
      return 0;
    }

  for(frame_index = 0U;
      frame_index < ROAD_OBJECT_CEL_SCALE_BUCKET_COUNT;
      frame_index++)
    {
      frame_chunk = animation->frame_chunks[frame_index];
      chunk = (const CansChunkHeader *)frame_chunk;
      if(frame_chunk == 0 ||
         family_cans_chunk_is_bounded(group_index, chunk) == 0 ||
         chunk->tag != CANS_CHUNK_TAG_RPDT ||
         chunk->byte_count < sizeof(CansReducedPixelDataChunk) ||
         frame_chunk->width_preamble_index < 0 ||
         frame_chunk->width_preamble_index >=
         CEL_DIMENSION_ENTRY_COUNT ||
         frame_chunk->height_preamble_index < 0 ||
         frame_chunk->height_preamble_index >=
         CEL_DIMENSION_ENTRY_COUNT)
        {
          return 0;
        }
    }

  chunk = (const CansChunkHeader *)animation->ccb_chunk;
  if(animation->ccb_chunk == 0 ||
     family_cans_chunk_is_bounded(group_index, chunk) == 0 ||
     chunk->tag != CANS_CHUNK_TAG_CCB ||
     chunk->byte_count < sizeof(CansCcbChunk))
    {
      return 0;
    }

  if(animation->plut_chunk != 0)
    {
      chunk = (const CansChunkHeader *)animation->plut_chunk;
      if(family_cans_plut_chunk_is_bounded(
           group_index, animation->plut_chunk) == 0)
        {
          return 0;
        }
    }
  else if((animation->ccb_chunk->ccb.ccb_Flags &
           CCB_LDPLUT) != 0)
    {
      return 0;
    }

  if(animation->frame_centers != 0)
    {
      if(family_cans_range_contains(
           group_index, animation->frame_centers,
           sizeof(FrontEndCelOffset *) *
           ROAD_OBJECT_CEL_SCALE_BUCKET_COUNT) == 0)
        {
          return 0;
        }
      for(frame_index = 0U;
          frame_index < ROAD_OBJECT_CEL_SCALE_BUCKET_COUNT;
          frame_index++)
        {
          if(animation->frame_centers[frame_index] == 0 ||
             family_cans_range_contains(
               group_index, animation->frame_centers[frame_index],
               sizeof(FrontEndCelOffset)) == 0)
            {
              return 0;
            }
        }
    }

  return 1;
}


static
int
family_cans_chunk_stream_is_bounded(unsigned int           group_index,
                                    const CansChunkHeader *chunk)
{
  const FamilyCansAnimationHeader *animation_header;
  unsigned int frame_count;
  unsigned int frame_index;
  unsigned long frame_tag;
  int has_ccb;
  int has_plut;
  int requires_plut;

  if(family_cans_chunk_is_bounded(group_index, chunk) == 0 ||
     chunk->byte_count < sizeof(FamilyCansAnimationHeader))
    {
      return 0;
    }

  animation_header = (const FamilyCansAnimationHeader *)chunk;
  if(animation_header->frame_count <= 0)
    {
      return 0;
    }
  frame_count = (unsigned int)animation_header->frame_count;
  chunk = family_cans_next_chunk(chunk);
  has_ccb = 0;
  has_plut = 0;
  requires_plut = 0;

  for(;;)
    {
      if(family_cans_chunk_is_bounded(group_index, chunk) == 0)
        {
          return 0;
        }
      if(chunk->tag == CANS_CHUNK_TAG_PDAT ||
         chunk->tag == CANS_CHUNK_TAG_RPDT)
        {
          break;
        }
      if(chunk->tag == CANS_CHUNK_TAG_CCB)
        {
          if(chunk->byte_count < sizeof(CansCcbChunk))
            {
              return 0;
            }
          has_ccb = 1;
          if((((const CansCcbChunk *)chunk)->ccb.ccb_Flags &
              CCB_LDPLUT) != 0)
            {
              requires_plut = 1;
            }
        }
      else if(chunk->tag == CANS_CHUNK_TAG_PLUT)
        {
          if(family_cans_plut_chunk_is_bounded(
               group_index, (const CansPlutChunk *)chunk) == 0)
            {
              return 0;
            }
          has_plut = 1;
        }
      else if(chunk->tag == CANS_CHUNK_TAG_CTPT)
        {
          if(family_cans_center_chunk_is_bounded(
               group_index, (const CansCenterPointChunk *)chunk,
               frame_count) == 0)
            {
              return 0;
            }
        }
      else if(chunk->tag == CANS_CHUNK_TAG_HSPT)
        {
          if(family_cans_hotspot_chunk_is_bounded(
               group_index, (const CansHotspotChunk *)chunk,
               frame_count) == 0)
            {
              return 0;
            }
        }
      chunk = family_cans_next_chunk(chunk);
    }

  if(has_ccb == 0 || (requires_plut != 0 && has_plut == 0))
    {
      return 0;
    }

  frame_tag = chunk->tag;
  for(frame_index = 0U; frame_index < frame_count; frame_index++)
    {
      if(family_cans_pixel_chunk_is_bounded(
           group_index, chunk, frame_tag) == 0)
        {
          return 0;
        }
      chunk = family_cans_next_chunk(chunk);
    }

  if(family_cans_pointer_is_at_loaded_end(group_index, chunk) != 0)
    {
      return 1;
    }
  if(family_cans_range_contains(
       group_index, chunk, sizeof(chunk->tag)) == 0)
    {
      return 0;
    }
  if(chunk->tag == frame_tag)
    {
      /*
       * Some shipped family streams terminate their declared RPDT run
       * with a four-byte RPDT marker.  The next value-tree object begins
       * immediately after that tag, so its OFST tag occupies the position
       * where a normal chunk byte count would live.  Reject a genuine
       * additional pixel chunk, but preserve the oracle's stop-marker
       * behavior when no complete bounded frame follows.
       */
      if(family_cans_pixel_chunk_is_bounded(
           group_index, chunk, frame_tag) != 0)
        {
          return 0;
        }
      return 1;
    }
  if(chunk->tag != CANS_CHUNK_TAG_CCB)
    {
      return 1;
    }

  for(;;)
    {
      if(chunk->tag != CANS_CHUNK_TAG_CCB &&
         chunk->tag != CANS_CHUNK_TAG_PDAT &&
         chunk->tag != CANS_CHUNK_TAG_PLUT)
        {
          return 1;
        }
      if(chunk->tag == CANS_CHUNK_TAG_CCB &&
         (family_cans_chunk_is_bounded(group_index, chunk) == 0 ||
          chunk->byte_count < sizeof(CansCcbChunk)))
        {
          return 0;
        }
      if(chunk->tag == CANS_CHUNK_TAG_PDAT &&
         family_cans_pixel_chunk_is_bounded(
           group_index, chunk, CANS_CHUNK_TAG_PDAT) == 0)
        {
          return 0;
        }
      if(chunk->tag == CANS_CHUNK_TAG_PLUT &&
         family_cans_plut_chunk_is_bounded(
           group_index, (const CansPlutChunk *)chunk) == 0)
        {
          return 0;
        }
      chunk = family_cans_next_chunk(chunk);
      if(family_cans_pointer_is_at_loaded_end(
           group_index, chunk) != 0)
        {
          return 1;
        }
      if(family_cans_range_contains(
           group_index, chunk, sizeof(chunk->tag)) == 0)
        {
          return 0;
        }
    }
}


int
family_cans_animation_has_valid_root(unsigned int             selector,
                                     const CansAnimationData *animation)
{
  const CansChunkHeader *chunk;
  const CansFileOffsetChunk *offset_chunk;
  unsigned int group_index;
  unsigned long chunk_address;
  unsigned long target_address;

  group_index = FAMILY_RESOURCE_GROUP_INDEX(selector);
  if(animation == 0 ||
     family_cans_range_contains(
       group_index, animation, sizeof(CansChunkHeader)) == 0)
    {
      return 0;
    }

  chunk = (const CansChunkHeader *)animation;
  if(chunk->tag == CANS_CHUNK_TAG_OFST)
    {
      offset_chunk = (const CansFileOffsetChunk *)chunk;
      chunk_address = (unsigned long)chunk;
      target_address = chunk_address + offset_chunk->animation_offset;
      if(target_address <= chunk_address ||
         family_cans_range_contains(
           group_index, (const void *)target_address,
           sizeof(CansChunkHeader)) == 0)
        {
          return 0;
        }
      chunk = (const CansChunkHeader *)target_address;
    }

  if(chunk->tag == CANS_CHUNK_TAG_OFSS)
    {
      return family_cans_offset_animation_is_bounded(
        group_index, (const CansOffsetAnimation *)chunk);
    }
  if(chunk->tag != CANS_CHUNK_TAG_ANIM)
    {
      return 0;
    }
  return family_cans_chunk_stream_is_bounded(group_index, chunk);
}


int
family_cans_animation_has_cached_valid_root(
  unsigned int             selector,
  const CansAnimationData *animation)
{
  FamilyCansValidationCacheEntry *entry;
  unsigned int generation;
  unsigned int group_index;
  unsigned int entry_index;
  int valid;

  group_index = FAMILY_RESOURCE_GROUP_INDEX(selector);
  entry_index =
    group_index * FAMILY_RESOURCE_SELECTOR_ENTRY_CAPACITY +
    FAMILY_RESOURCE_ENTRY_INDEX(selector);
  entry = &family_cans_validation_cache[entry_index];
  generation = family_resource_generations[group_index];

  if(generation != 0U &&
     entry->animation == animation &&
     entry->generation == generation)
    {
      return entry->valid;
    }

  valid = family_cans_animation_has_valid_root(selector, animation);
  if(generation != 0U)
    {
      entry->animation = animation;
      entry->generation = generation;
      entry->valid = valid;
    }
  return valid;
}
