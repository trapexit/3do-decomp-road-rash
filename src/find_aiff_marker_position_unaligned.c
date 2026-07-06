#include "string.h"
#include "find_aiff_marker_position_offsets.h"

#define AIFF_MARK_CHUNK_ID 0x4D41524B
#define AIFF_MARK_HEADER_WORDS 3
#define AIFF_MARK_ENTRY_WORDS 2
#define AIFF_MARK_HEADER_BYTES 0x0C
#define AIFF_MARK_ENTRY_BYTES 8
#define AIFF_MARK_FIXED_ENTRY_BYTES 6
#define AIFF_MARK_ID_SHIFT 16
#define AIFF_MARK_COUNT_SHIFT 16
#define AIFF_MARK_POSITION_SHIFT 16
#define AIFF_MARK_INDEX_MASK 0xFFFF
#define AIFF_PSTRING_LENGTH_BYTES 1
#define AIFF_ALIGNMENT_BYTES 2
#define AIFF_MARK_COUNT_WORD 2

int
find_aiff_marker_position_unaligned(unsigned char *marker_chunk,
                                    int            marker_id)
{
  int header[AIFF_MARK_HEADER_WORDS];
  int entry[AIFF_MARK_ENTRY_WORDS];
  int target_id;
  int position;
  int index;
  int count;
  unsigned char *cursor;

  target_id = (int)(short)marker_id;
  position = 0;
  index = 0;

  memcpy(header, marker_chunk, AIFF_MARK_HEADER_BYTES);
  if(header[0] != AIFF_MARK_CHUNK_ID)
    {
      return 0;
    }

  if(header[1] <= 0)
    {
      return 0;
    }

  count = ((unsigned int)header[AIFF_MARK_COUNT_WORD]) >>
          AIFF_MARK_COUNT_SHIFT;
  cursor = marker_chunk + AIFF_MARK_FIRST_ENTRY_OFFSET;
  if(count <= 0)
    {
      return 0;
    }

  while(index < count)
    {
      memcpy(entry, cursor, AIFF_MARK_ENTRY_BYTES);
      if(target_id == (entry[0] >> AIFF_MARK_ID_SHIFT))
        {
          position = (((unsigned int)entry[1]) >>
                      AIFF_MARK_POSITION_SHIFT) |
                     ((entry[0] & AIFF_MARK_INDEX_MASK) <<
                      AIFF_MARK_POSITION_SHIFT);
          break;
        }

      cursor += AIFF_MARK_FIXED_ENTRY_BYTES;
      cursor += *cursor + AIFF_PSTRING_LENGTH_BYTES;
      if(((int)cursor & (AIFF_ALIGNMENT_BYTES - 1)) != 0)
        {
          cursor++;
        }
      index = (index + 1) & AIFF_MARK_INDEX_MASK;
    }

  return position;
}


#undef AIFF_MARK_CHUNK_ID
#undef AIFF_MARK_HEADER_WORDS
#undef AIFF_MARK_ENTRY_WORDS
#undef AIFF_MARK_HEADER_BYTES
#undef AIFF_MARK_ENTRY_BYTES
#undef AIFF_MARK_FIXED_ENTRY_BYTES
#undef AIFF_MARK_ID_SHIFT
#undef AIFF_MARK_COUNT_SHIFT
#undef AIFF_MARK_POSITION_SHIFT
#undef AIFF_MARK_INDEX_MASK
#undef AIFF_PSTRING_LENGTH_BYTES
#undef AIFF_ALIGNMENT_BYTES
#undef AIFF_MARK_COUNT_WORD
