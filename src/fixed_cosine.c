/* Routines that the retail binary placed at the start of its RW section. */
#include "rw_semantic_data.h"

typedef struct BitReader
{
  const unsigned char *cursor;
  unsigned int bits;
  int available;
} BitReader;

enum
{
  BIG_ENDIAN_BYTE_0 = 0,
  BIG_ENDIAN_BYTE_1 = 1,
  BIG_ENDIAN_BYTE_2 = 2,
  BIG_ENDIAN_BYTE_3 = 3,
  BIG_ENDIAN_SHIFT_0 = 24,
  BIG_ENDIAN_SHIFT_1 = 16,
  BIG_ENDIAN_SHIFT_2 = 8,
  BIT_READER_WORD_BYTES = 4,
  BIT_READER_WORD_BITS = 32,
  BIT_READER_TOP_BIT_SHIFT = 31,

  FIXED_ANGLE_QUARTER_TURN = 0x00400000,
  FIXED_ANGLE_HALF_TURN = 0x00800000,
  FIXED_ANGLE_TURN_MASK = 0x00FFFFFF,
  FIXED_SINE_TABLE_INDEX_SHIFT = 10,
  FIXED_SINE_TABLE_FRACTION_MASK = 0x3FF,
  FIXED_SINE_TABLE_INTERVAL = 0x400,
  FIXED_SINE_TABLE_VALUE_SHIFT = 10,
  FIXED_SINE_INTERPOLATION_SHIFT = 15,

  CINEPAK_CHUNK_TYPE_OFFSET = 0,
  CINEPAK_CHUNK_LENGTH_HIGH_OFFSET = 1,
  CINEPAK_CHUNK_LENGTH_MIDDLE_OFFSET = 2,
  CINEPAK_CHUNK_LENGTH_LOW_OFFSET = 3,
  CINEPAK_CHUNK_LENGTH_HIGH_SHIFT = 16,
  CINEPAK_CHUNK_LENGTH_MIDDLE_SHIFT = 8,
  CINEPAK_CHUNK_HEADER_BYTES = 4,
  CINEPAK_CODEBOOK_ENTRY_BYTES = 6,
  CINEPAK_CODEBOOK_SLOT_COUNT = 256,
  CINEPAK_FLAG_WORD_BYTES = 4,
  CINEPAK_FLAG_WORD_SLOT_MASK = 31,
  CINEPAK_V4_FULL_CHUNK = 0x20,
  CINEPAK_V4_SELECTIVE_CHUNK = 0x21,
  CINEPAK_V1_FULL_CHUNK = 0x22,
  CINEPAK_V1_SELECTIVE_CHUNK = 0x23,
  CINEPAK_V4_TABLE_OFFSET = 0x3100,
  CINEPAK_V1_TABLE_OFFSET = 0x800,

  CINEPAK_CODEBOOK_LUMA_0 = 0,
  CINEPAK_CODEBOOK_LUMA_1 = 1,
  CINEPAK_CODEBOOK_LUMA_2 = 2,
  CINEPAK_CODEBOOK_LUMA_3 = 3,
  CINEPAK_CODEBOOK_CHROMA_HORIZONTAL = 4,
  CINEPAK_CODEBOOK_CHROMA_VERTICAL = 5,
  CINEPAK_COMPONENT_ROW_STRIDE = 8,
  CINEPAK_COMPONENT_WORD_BYTES = 4,
  CINEPAK_GREEN_SHIFT = 5,
  CINEPAK_RED_SHIFT = 10,
  CINEPAK_PIXEL_PAIR_SHIFT = 16,
  CINEPAK_SECOND_LUMA_BIAS = 6,
  CINEPAK_THIRD_LUMA_BIAS = 4,
  CINEPAK_FOURTH_LUMA_BIAS = 2,
  CINEPAK_V1_COMPONENT_SCALE = 64,
  CINEPAK_DIAGONAL_HALF_DIVISOR = 2,
  CINEPAK_DIAGONAL_SHIFT = 3,
  CINEPAK_V1_ENTRY_BYTES = 0x20,
  CINEPAK_V1_MIDDLE_FIRST_OFFSET = 8,
  CINEPAK_V1_MIDDLE_SECOND_OFFSET = 0x0C,
  CINEPAK_V1_HORIZONTAL_FIRST_OFFSET = 0x10,
  CINEPAK_V1_HORIZONTAL_SECOND_OFFSET = 0x14,
  CINEPAK_V4_DESTINATION_WORDS = 2,
  CINEPAK_V1_DESTINATION_WORDS = 8,

  CINEPAK_BLOCK_SIDE_PIXELS = 4,
  CINEPAK_DESTINATION_STRIDE_DIVISOR = 2,
  CINEPAK_PALETTE_PAIR_BYTES = 8,
  CINEPAK_ROW_PAIR_WORDS = 4,
  CINEPAK_PAIR_FIRST_WORD = 0,
  CINEPAK_PAIR_SECOND_WORD = 1,
  CINEPAK_ROW_FIRST_PAIR_OFFSET = 0,
  CINEPAK_ROW_SECOND_PAIR_OFFSET = 2,
  CINEPAK_BLOCK_TOP_LEFT_INDEX = 0,
  CINEPAK_BLOCK_TOP_RIGHT_INDEX = 1,
  CINEPAK_BLOCK_BOTTOM_LEFT_INDEX = 2,
  CINEPAK_BLOCK_BOTTOM_RIGHT_INDEX = 3,
  CINEPAK_V1_POINTER_WORD_OFFSET = 0x820,
  CINEPAK_V1_WORD_1_OFFSET = 0x804,
  CINEPAK_V1_WORD_2_OFFSET = 0x808,
  CINEPAK_V1_WORD_3_OFFSET = 0x80C,
  CINEPAK_V1_WORD_4_OFFSET = 0x810,
  CINEPAK_V1_WORD_5_OFFSET = 0x814,
  CINEPAK_V1_WORD_6_OFFSET = 0x818,
  CINEPAK_V1_WORD_7_OFFSET = 0x81C
};

#define CINEPAK_FLAG_TOP_BIT 0x80000000U
#define CINEPAK_PIXEL_LOW_MASK 0xFFFFU

static
unsigned int
read_be32(const unsigned char *source)
{
  return ((unsigned int)source[BIG_ENDIAN_BYTE_0] << BIG_ENDIAN_SHIFT_0) |
         ((unsigned int)source[BIG_ENDIAN_BYTE_1] << BIG_ENDIAN_SHIFT_1) |
         ((unsigned int)source[BIG_ENDIAN_BYTE_2] << BIG_ENDIAN_SHIFT_2) |
         source[BIG_ENDIAN_BYTE_3];
}


static
int
read_bit(BitReader *reader)
{
  int result;

  if(reader->available == 0)
    {
      reader->bits = read_be32(reader->cursor);
      reader->cursor += BIT_READER_WORD_BYTES;
      reader->available = BIT_READER_WORD_BITS;
    }
  result = (int)(reader->bits >> BIT_READER_TOP_BIT_SHIFT);
  reader->bits <<= 1;
  reader->available--;
  return result;
}


static
int
signed_byte(unsigned char value)
{
  return (int)(signed char)value;
}


int
fixed_sine(unsigned int angle)
{
  unsigned int *table;
  unsigned int index;
  unsigned int fraction;
  unsigned int value;
  unsigned int lower;
  unsigned int upper;
  int negate;

  if((angle & FIXED_ANGLE_QUARTER_TURN) != 0)
    {
      angle = FIXED_ANGLE_HALF_TURN - angle;
    }
  angle &= FIXED_ANGLE_TURN_MASK;
  negate = angle >= FIXED_ANGLE_HALF_TURN;
  if(negate)
    {
      angle -= FIXED_ANGLE_HALF_TURN;
    }

  table = operamath_quarter_sine_table;
  index = angle >> FIXED_SINE_TABLE_INDEX_SHIFT;
  fraction = angle & FIXED_SINE_TABLE_FRACTION_MASK;
  /* lower * (interval - fraction) + fraction * upper equals
     lower * interval + fraction * (upper - lower) for the unsigned
     intermediates here, so the interpolation needs one multiply. */
  lower = table[index] >> FIXED_SINE_TABLE_VALUE_SHIFT;
  upper = table[index + 1] >> FIXED_SINE_TABLE_VALUE_SHIFT;
  value = (lower * FIXED_SINE_TABLE_INTERVAL +
           (upper - lower) * fraction) >>
          FIXED_SINE_INTERPOLATION_SHIFT;
  return negate ? -(int)value : (int)value;
}


int
fixed_cosine(int angle)
{
  return fixed_sine((unsigned int)angle + FIXED_ANGLE_QUARTER_TURN);
}


static
unsigned int
packed_color_2(const unsigned char *entry,
               int                  table_base)
{
  int row_a;
  int row_b;
  int row_c;
  int first;
  int second;

  row_a = table_base +
          signed_byte(entry[CINEPAK_CODEBOOK_CHROMA_HORIZONTAL]) *
          CINEPAK_COMPONENT_ROW_STRIDE;
  row_c = table_base +
          signed_byte(entry[CINEPAK_CODEBOOK_CHROMA_VERTICAL]) *
          CINEPAK_COMPONENT_ROW_STRIDE;
  row_b = table_base +
          ((signed_byte(entry[CINEPAK_CODEBOOK_CHROMA_VERTICAL]) *
            CINEPAK_COMPONENT_ROW_STRIDE +
            signed_byte(entry[CINEPAK_CODEBOOK_CHROMA_HORIZONTAL]) *
            CINEPAK_COMPONENT_WORD_BYTES) >>
           CINEPAK_DIAGONAL_SHIFT) *
          -CINEPAK_COMPONENT_WORD_BYTES;

  first = *(int *)(row_a +
                   entry[CINEPAK_CODEBOOK_LUMA_0] *
                   CINEPAK_COMPONENT_WORD_BYTES) |
          (*(int *)(row_b +
                    entry[CINEPAK_CODEBOOK_LUMA_0] *
                    CINEPAK_COMPONENT_WORD_BYTES) <<
           CINEPAK_GREEN_SHIFT) |
          (*(int *)(row_c +
                    entry[CINEPAK_CODEBOOK_LUMA_0] *
                    CINEPAK_COMPONENT_WORD_BYTES) <<
           CINEPAK_RED_SHIFT);
  second = *(int *)(row_a +
                    (entry[CINEPAK_CODEBOOK_LUMA_1] +
                     CINEPAK_SECOND_LUMA_BIAS) *
                    CINEPAK_COMPONENT_WORD_BYTES) |
           (*(int *)(row_b +
                     (entry[CINEPAK_CODEBOOK_LUMA_1] +
                      CINEPAK_SECOND_LUMA_BIAS) *
                     CINEPAK_COMPONENT_WORD_BYTES) <<
            CINEPAK_GREEN_SHIFT) |
           (*(int *)(row_c +
                     (entry[CINEPAK_CODEBOOK_LUMA_1] +
                      CINEPAK_SECOND_LUMA_BIAS) *
                     CINEPAK_COMPONENT_WORD_BYTES) <<
            CINEPAK_RED_SHIFT);
  return ((unsigned int)first << CINEPAK_PIXEL_PAIR_SHIFT) |
         ((unsigned int)second & CINEPAK_PIXEL_LOW_MASK);
}


static
unsigned int
packed_color_2_second(const unsigned char *entry,
                      int                  table_base)
{
  int row_a;
  int row_b;
  int row_c;
  int first;
  int second;

  row_a = table_base +
          signed_byte(entry[CINEPAK_CODEBOOK_CHROMA_HORIZONTAL]) *
          CINEPAK_COMPONENT_ROW_STRIDE;
  row_c = table_base +
          signed_byte(entry[CINEPAK_CODEBOOK_CHROMA_VERTICAL]) *
          CINEPAK_COMPONENT_ROW_STRIDE;
  row_b = table_base +
          ((signed_byte(entry[CINEPAK_CODEBOOK_CHROMA_VERTICAL]) *
            CINEPAK_COMPONENT_ROW_STRIDE +
            signed_byte(entry[CINEPAK_CODEBOOK_CHROMA_HORIZONTAL]) *
            CINEPAK_COMPONENT_WORD_BYTES) >>
           CINEPAK_DIAGONAL_SHIFT) *
          -CINEPAK_COMPONENT_WORD_BYTES;

  first = *(int *)(row_a +
                   (entry[CINEPAK_CODEBOOK_LUMA_2] +
                    CINEPAK_THIRD_LUMA_BIAS) *
                   CINEPAK_COMPONENT_WORD_BYTES) |
          (*(int *)(row_b +
                    (entry[CINEPAK_CODEBOOK_LUMA_2] +
                     CINEPAK_THIRD_LUMA_BIAS) *
                    CINEPAK_COMPONENT_WORD_BYTES) <<
           CINEPAK_GREEN_SHIFT) |
          (*(int *)(row_c +
                    (entry[CINEPAK_CODEBOOK_LUMA_2] +
                     CINEPAK_THIRD_LUMA_BIAS) *
                    CINEPAK_COMPONENT_WORD_BYTES) <<
           CINEPAK_RED_SHIFT);
  second = *(int *)(row_a +
                    (entry[CINEPAK_CODEBOOK_LUMA_3] +
                     CINEPAK_FOURTH_LUMA_BIAS) *
                    CINEPAK_COMPONENT_WORD_BYTES) |
           (*(int *)(row_b +
                     (entry[CINEPAK_CODEBOOK_LUMA_3] +
                      CINEPAK_FOURTH_LUMA_BIAS) *
                     CINEPAK_COMPONENT_WORD_BYTES) <<
            CINEPAK_GREEN_SHIFT) |
           (*(int *)(row_c +
                     (entry[CINEPAK_CODEBOOK_LUMA_3] +
                      CINEPAK_FOURTH_LUMA_BIAS) *
                     CINEPAK_COMPONENT_WORD_BYTES) <<
            CINEPAK_RED_SHIFT);
  return ((unsigned int)first << CINEPAK_PIXEL_PAIR_SHIFT) |
         ((unsigned int)second & CINEPAK_PIXEL_LOW_MASK);
}


static
void
decode_two_pixel_entry(const unsigned char *entry,
                       unsigned int        *destination,
                       int                  table_base)
{
  destination[CINEPAK_PAIR_FIRST_WORD] =
    packed_color_2(entry, table_base);
  destination[CINEPAK_PAIR_SECOND_WORD] =
    packed_color_2_second(entry, table_base);
}


static
void
decode_four_pixel_entry(const unsigned char *entry,
                        unsigned int        *destination,
                        int                  table_base)
{
  int horizontal;
  int vertical;
  int diagonal;
  int index;

  horizontal = signed_byte(entry[CINEPAK_CODEBOOK_CHROMA_HORIZONTAL]) *
               CINEPAK_V1_COMPONENT_SCALE;
  vertical = signed_byte(entry[CINEPAK_CODEBOOK_CHROMA_VERTICAL]) *
             CINEPAK_V1_COMPONENT_SCALE;
  diagonal =
    (vertical + horizontal / CINEPAK_DIAGONAL_HALF_DIVISOR) >>
    CINEPAK_DIAGONAL_SHIFT;

  for(index = 0; index < CINEPAK_BLOCK_SIDE_PIXELS; index++)
    {
      int base;
      int upper;
      int lower;

      base = table_base + entry[index] * CINEPAK_V1_ENTRY_BYTES;
      upper = *(int *)(base + vertical) << CINEPAK_RED_SHIFT |
              *(int *)(base -
                       diagonal * CINEPAK_COMPONENT_WORD_BYTES +
                       CINEPAK_V1_MIDDLE_FIRST_OFFSET) <<
              CINEPAK_GREEN_SHIFT |
              *(int *)(base + horizontal +
                       CINEPAK_V1_HORIZONTAL_FIRST_OFFSET);
      lower = *(int *)(base + vertical + CINEPAK_COMPONENT_WORD_BYTES) <<
              CINEPAK_RED_SHIFT |
              *(int *)(base -
                       diagonal * CINEPAK_COMPONENT_WORD_BYTES +
                       CINEPAK_V1_MIDDLE_SECOND_OFFSET) <<
              CINEPAK_GREEN_SHIFT |
              *(int *)(base + horizontal +
                       CINEPAK_V1_HORIZONTAL_SECOND_OFFSET);
      destination[index * CINEPAK_V4_DESTINATION_WORDS] =
        (unsigned int)upper;
      destination[index * CINEPAK_V4_DESTINATION_WORDS +
                  CINEPAK_PAIR_SECOND_WORD] = (unsigned int)lower;
    }
}


void
decode_cinepak_codebook(unsigned char *chunk,
                        unsigned int  *destination,
                        int            table_base)
{
  unsigned int length;
  unsigned int slots;
  unsigned int index;
  unsigned int mask;
  int remaining;

  length =
    ((unsigned int)chunk[CINEPAK_CHUNK_LENGTH_HIGH_OFFSET] <<
      CINEPAK_CHUNK_LENGTH_HIGH_SHIFT) |
    ((unsigned int)chunk[CINEPAK_CHUNK_LENGTH_MIDDLE_OFFSET] <<
      CINEPAK_CHUNK_LENGTH_MIDDLE_SHIFT) |
    chunk[CINEPAK_CHUNK_LENGTH_LOW_OFFSET];
  if(length == 0)
    {
      return;
    }

  if(chunk[CINEPAK_CHUNK_TYPE_OFFSET] == CINEPAK_V4_FULL_CHUNK)
    {
      table_base += CINEPAK_V4_TABLE_OFFSET;
      remaining = (int)length - CINEPAK_CHUNK_HEADER_BYTES;
      chunk += CINEPAK_CHUNK_HEADER_BYTES;
      while(remaining >= CINEPAK_CODEBOOK_ENTRY_BYTES)
        {
          decode_two_pixel_entry(chunk, destination, table_base);
          chunk += CINEPAK_CODEBOOK_ENTRY_BYTES;
          destination += CINEPAK_V4_DESTINATION_WORDS;
          remaining -= CINEPAK_CODEBOOK_ENTRY_BYTES;
        }
      return;
    }

  if(chunk[CINEPAK_CHUNK_TYPE_OFFSET] == CINEPAK_V4_SELECTIVE_CHUNK)
    {
      table_base += CINEPAK_V4_TABLE_OFFSET;
      chunk += CINEPAK_CHUNK_HEADER_BYTES;
      mask = 0;
      for(slots = 0; slots < CINEPAK_CODEBOOK_SLOT_COUNT; slots++)
        {
          if((slots & CINEPAK_FLAG_WORD_SLOT_MASK) == 0)
            {
              mask = read_be32(chunk);
              chunk += CINEPAK_FLAG_WORD_BYTES;
            }
          if((mask & CINEPAK_FLAG_TOP_BIT) != 0)
            {
              decode_two_pixel_entry(chunk, destination, table_base);
              chunk += CINEPAK_CODEBOOK_ENTRY_BYTES;
            }
          destination += CINEPAK_V4_DESTINATION_WORDS;
          mask <<= 1;
        }
      return;
    }

  if(chunk[CINEPAK_CHUNK_TYPE_OFFSET] == CINEPAK_V1_FULL_CHUNK)
    {
      table_base += CINEPAK_V1_TABLE_OFFSET;
      remaining = (int)length - CINEPAK_CHUNK_HEADER_BYTES;
      chunk += CINEPAK_CHUNK_HEADER_BYTES;
      while(remaining >= CINEPAK_CODEBOOK_ENTRY_BYTES)
        {
          decode_four_pixel_entry(chunk, destination, table_base);
          chunk += CINEPAK_CODEBOOK_ENTRY_BYTES;
          destination += CINEPAK_V1_DESTINATION_WORDS;
          remaining -= CINEPAK_CODEBOOK_ENTRY_BYTES;
        }
      return;
    }

  if(chunk[CINEPAK_CHUNK_TYPE_OFFSET] == CINEPAK_V1_SELECTIVE_CHUNK)
    {
      table_base += CINEPAK_V1_TABLE_OFFSET;
      chunk += CINEPAK_CHUNK_HEADER_BYTES;
      mask = 0;
      for(index = 0; index < CINEPAK_CODEBOOK_SLOT_COUNT; index++)
        {
          if((index & CINEPAK_FLAG_WORD_SLOT_MASK) == 0)
            {
              mask = read_be32(chunk);
              chunk += CINEPAK_FLAG_WORD_BYTES;
            }
          if((mask & CINEPAK_FLAG_TOP_BIT) != 0)
            {
              decode_four_pixel_entry(chunk, destination, table_base);
              chunk += CINEPAK_CODEBOOK_ENTRY_BYTES;
            }
          destination += CINEPAK_V1_DESTINATION_WORDS;
          mask <<= 1;
        }
    }
}


static
void
copy_palette_pairs(int                 *top,
                   int                 *bottom,
                   int                  palette,
                   const unsigned char *indices)
{
  int *source;

  source = (int *)(palette +
                   indices[CINEPAK_BLOCK_TOP_LEFT_INDEX] *
                   CINEPAK_PALETTE_PAIR_BYTES);
  top[CINEPAK_ROW_FIRST_PAIR_OFFSET + CINEPAK_PAIR_FIRST_WORD] =
    source[CINEPAK_PAIR_FIRST_WORD];
  top[CINEPAK_ROW_FIRST_PAIR_OFFSET + CINEPAK_PAIR_SECOND_WORD] =
    source[CINEPAK_PAIR_SECOND_WORD];
  source = (int *)(palette +
                   indices[CINEPAK_BLOCK_TOP_RIGHT_INDEX] *
                   CINEPAK_PALETTE_PAIR_BYTES);
  top[CINEPAK_ROW_SECOND_PAIR_OFFSET + CINEPAK_PAIR_FIRST_WORD] =
    source[CINEPAK_PAIR_FIRST_WORD];
  top[CINEPAK_ROW_SECOND_PAIR_OFFSET + CINEPAK_PAIR_SECOND_WORD] =
    source[CINEPAK_PAIR_SECOND_WORD];
  source = (int *)(palette +
                   indices[CINEPAK_BLOCK_BOTTOM_LEFT_INDEX] *
                   CINEPAK_PALETTE_PAIR_BYTES);
  bottom[CINEPAK_ROW_FIRST_PAIR_OFFSET + CINEPAK_PAIR_FIRST_WORD] =
    source[CINEPAK_PAIR_FIRST_WORD];
  bottom[CINEPAK_ROW_FIRST_PAIR_OFFSET + CINEPAK_PAIR_SECOND_WORD] =
    source[CINEPAK_PAIR_SECOND_WORD];
  source = (int *)(palette +
                   indices[CINEPAK_BLOCK_BOTTOM_RIGHT_INDEX] *
                   CINEPAK_PALETTE_PAIR_BYTES);
  bottom[CINEPAK_ROW_SECOND_PAIR_OFFSET + CINEPAK_PAIR_FIRST_WORD] =
    source[CINEPAK_PAIR_FIRST_WORD];
  bottom[CINEPAK_ROW_SECOND_PAIR_OFFSET + CINEPAK_PAIR_SECOND_WORD] =
    source[CINEPAK_PAIR_SECOND_WORD];
}


static
void
copy_palette_block(int         *top,
                   int         *bottom,
                   int          palette,
                   unsigned int index)
{
  int *source;

  source = (int *)(palette + index * CINEPAK_V1_ENTRY_BYTES);
  top[CINEPAK_BLOCK_TOP_LEFT_INDEX] =
    (int)((char *)source + CINEPAK_V1_POINTER_WORD_OFFSET);
  top[CINEPAK_BLOCK_TOP_RIGHT_INDEX] =
    *(int *)((char *)source + CINEPAK_V1_WORD_1_OFFSET);
  top[CINEPAK_BLOCK_BOTTOM_LEFT_INDEX] =
    *(int *)((char *)source + CINEPAK_V1_WORD_2_OFFSET);
  top[CINEPAK_BLOCK_BOTTOM_RIGHT_INDEX] =
    *(int *)((char *)source + CINEPAK_V1_WORD_3_OFFSET);
  bottom[CINEPAK_BLOCK_TOP_LEFT_INDEX] =
    *(int *)((char *)source + CINEPAK_V1_WORD_4_OFFSET);
  bottom[CINEPAK_BLOCK_TOP_RIGHT_INDEX] =
    *(int *)((char *)source + CINEPAK_V1_WORD_5_OFFSET);
  bottom[CINEPAK_BLOCK_BOTTOM_LEFT_INDEX] =
    *(int *)((char *)source + CINEPAK_V1_WORD_6_OFFSET);
  bottom[CINEPAK_BLOCK_BOTTOM_RIGHT_INDEX] =
    *(int *)((char *)source + CINEPAK_V1_WORD_7_OFFSET);
}


void
decode_cinepak_intra_blocks(int           *destination,
                            int            stride,
                            int            width,
                            int            height,
                            int            palette,
                            int            unused,
                            unsigned char *source)
{
  BitReader reader;
  int *top;
  int *bottom;
  int x;

  (void)unused;
  reader.cursor = source;
  reader.bits = 0;
  reader.available = 0;
  top = destination;
  bottom = destination + stride / CINEPAK_DESTINATION_STRIDE_DIVISOR;

  while(height > 0)
    {
      for(x = 0; x < width; x += CINEPAK_BLOCK_SIDE_PIXELS)
        {
          if(read_bit(&reader) != 0)
            {
              copy_palette_pairs(top, bottom, palette, reader.cursor);
              reader.cursor += CINEPAK_BLOCK_SIDE_PIXELS;
            }
          else
            {
              copy_palette_block(top, bottom, palette, *reader.cursor++);
            }
          top += CINEPAK_ROW_PAIR_WORDS;
          bottom += CINEPAK_ROW_PAIR_WORDS;
        }
      top += stride - width;
      bottom += stride - width;
      height -= CINEPAK_BLOCK_SIDE_PIXELS;
    }
}


void
decode_cinepak_inter_blocks(int           *destination,
                            int            stride,
                            int            width,
                            int            height,
                            int            palette,
                            int            unused,
                            unsigned char *source)
{
  BitReader reader;
  int *top;
  int *bottom;
  int x;

  (void)unused;
  reader.cursor = source;
  reader.bits = 0;
  reader.available = 0;
  top = destination;
  bottom = destination + stride / CINEPAK_DESTINATION_STRIDE_DIVISOR;

  while(height > 0)
    {
      for(x = 0; x < width; x += CINEPAK_BLOCK_SIDE_PIXELS)
        {
          if(read_bit(&reader) != 0)
            {
              if(read_bit(&reader) != 0)
                {
                  copy_palette_pairs(top, bottom, palette, reader.cursor);
                  reader.cursor += CINEPAK_BLOCK_SIDE_PIXELS;
                }
              else
                {
                  copy_palette_block(top, bottom, palette, *reader.cursor++);
                }
            }
          top += CINEPAK_ROW_PAIR_WORDS;
          bottom += CINEPAK_ROW_PAIR_WORDS;
        }
      top += stride - width;
      bottom += stride - width;
      height -= CINEPAK_BLOCK_SIDE_PIXELS;
    }
}


void
decode_cinepak_solid_blocks(int           *destination,
                            int            stride,
                            int            width,
                            int            height,
                            int            palette,
                            int            unused,
                            unsigned char *source)
{
  int *top;
  int *bottom;
  int *entry;
  int x;

  (void)unused;
  top = destination;
  bottom = destination + stride / CINEPAK_DESTINATION_STRIDE_DIVISOR;
  while(height > 0)
    {
      for(x = 0; x < width; x += CINEPAK_BLOCK_SIDE_PIXELS)
        {
          entry = (int *)(palette + CINEPAK_V1_TABLE_OFFSET +
                          *source++ * CINEPAK_V1_ENTRY_BYTES);
          top[CINEPAK_BLOCK_TOP_LEFT_INDEX] =
            entry[CINEPAK_BLOCK_TOP_LEFT_INDEX];
          top[CINEPAK_BLOCK_TOP_RIGHT_INDEX] =
            entry[CINEPAK_BLOCK_TOP_RIGHT_INDEX];
          top[CINEPAK_BLOCK_BOTTOM_LEFT_INDEX] =
            entry[CINEPAK_BLOCK_BOTTOM_LEFT_INDEX];
          top[CINEPAK_BLOCK_BOTTOM_RIGHT_INDEX] =
            entry[CINEPAK_BLOCK_BOTTOM_RIGHT_INDEX];
          bottom[CINEPAK_BLOCK_TOP_LEFT_INDEX] =
            entry[CINEPAK_ROW_PAIR_WORDS + CINEPAK_BLOCK_TOP_LEFT_INDEX];
          bottom[CINEPAK_BLOCK_TOP_RIGHT_INDEX] =
            entry[CINEPAK_ROW_PAIR_WORDS + CINEPAK_BLOCK_TOP_RIGHT_INDEX];
          bottom[CINEPAK_BLOCK_BOTTOM_LEFT_INDEX] =
            entry[CINEPAK_ROW_PAIR_WORDS +
                  CINEPAK_BLOCK_BOTTOM_LEFT_INDEX];
          bottom[CINEPAK_BLOCK_BOTTOM_RIGHT_INDEX] =
            entry[CINEPAK_ROW_PAIR_WORDS +
                  CINEPAK_BLOCK_BOTTOM_RIGHT_INDEX];
          top += CINEPAK_ROW_PAIR_WORDS;
          bottom += CINEPAK_ROW_PAIR_WORDS;
        }
      top += stride - width;
      bottom += stride - width;
      height -= CINEPAK_BLOCK_SIDE_PIXELS;
    }
}
