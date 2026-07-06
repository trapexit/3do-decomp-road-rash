/* Road edge and curved road surface rendering. */



#include "bss_early_data.h"
#include "operamath.h"
#include "render_recessed_road_edge_offsets.h"
#include "display_cel_runtime.h"
#include "road_geometry.h"
#include "rw_semantic_data.h"

typedef struct SignedDivisionResult
{
  unsigned int quotient;
  unsigned int remainder;
} SignedDivisionResult;

__value_in_regs SignedDivisionResult
signed_divide_with_remainder(unsigned int divisor,
                             unsigned int numerator);
int
fixed_tangent_8_8(int angle_index);
CCB *
append_cel_mapping_packet(const CCB *source);
char *
select_cached_object_cel(char **cache,
                         char  *object,
                         int    segment_count,
                         int   *quad);
int
select_road_strip_cel(int *object,
                      int  side,
                      int *quad,
                      int  alternate);
int
fixed_vector_angle_24(int horizontal,
                      int vertical);
void
map_cel_to_horizontal_quad(CCB            *cel,
                           const RoadQuad *quad);
CelData *
select_cel_source_row(CCB *cel,
                      int  source_row);

#define SIGN_BIT_SHIFT 31
#define QUARTER_DIVISOR_SHIFT 2
#define QUARTER_ROUND_BIAS 3
#define BYTE_DIVISOR_SHIFT 8
#define BYTE_ROUND_BIAS 0xFF
#define CELL_FRACTION_MASK 0x3FFF
#define FIXED_PRODUCT_SHIFT 12
#define ANGLE_LOOKUP_SHIFT 14
#define FIXED_WEIGHT_SHIFT 8

#define ROAD_SCREEN_LIMIT 0x012C0000
#define ROAD_WRAP_STEP 0x01000000
#define ROAD_NEAR_CLIP_Y 0x005E0000
#define ROAD_FILL_CLIP_Y 0x00E60000
#define ROAD_WRAP_SPLIT_X 0x002C0000
#define ROAD_HORIZON_SPLIT_OFFSET 0x01D60000
#define ROAD_EDGE_VERTICAL_INSET 0x00006000
#define ROAD_BORDER_MARGIN 0x00014000
#define ROAD_FUNCTION_RESULT 0x5E
#define ROAD_HORIZON_DIVISOR 6

#define ANGLE_FRACTION_MASK 0x00FFFFFFU
#define ANGLE_THREE_QUARTERS 0x00C00000
#define ANGLE_QUARTER 0x00400000
#define ANGLE_EIGHTH 0x00200000

#define ROAD_COORD_PAIR_BYTES 8
#define ROAD_COORD_PAIR_WORDS 2
#define WORD_BYTES 4
#define ROAD_QUAD_WORDS 8
#define ROAD_CURVE_WORDS 12
#define ROAD_SURFACE_SEGMENT_COUNT 5
#define ROAD_VERTICAL_EDGE_COUNT 2
#define ROAD_VERTICAL_SOURCE_SEGMENT_COUNT 4

#define QUAD_FIRST_X 0
#define QUAD_FIRST_Y 1
#define QUAD_SECOND_X 2
#define QUAD_SECOND_Y 3
#define QUAD_THIRD_X 4
#define QUAD_THIRD_Y 5
#define QUAD_FOURTH_X 6
#define QUAD_FOURTH_Y 7
#define QUAD_SECOND_PAIR_WORD QUAD_SECOND_X
#define QUAD_THIRD_PAIR_WORD QUAD_THIRD_X
#define QUAD_FOURTH_PAIR_WORD QUAD_FOURTH_X

#define STRIP_START_X 0
#define STRIP_START_Y 1
#define STRIP_CONTROL_ONE_X 2
#define STRIP_CONTROL_ONE_Y 3
#define STRIP_CONTROL_TWO_X 4
#define STRIP_CONTROL_TWO_Y 5
#define STRIP_CONTROL_THREE_X 6
#define STRIP_CONTROL_THREE_Y 7
#define STRIP_CONTROL_FOUR_X 8
#define STRIP_CONTROL_FOUR_Y 9
#define STRIP_END_X 10
#define STRIP_END_Y 11

#define ROAD_RANGE_X 0
#define ROAD_RANGE_NEAR_Y 1
#define ROAD_RANGE_FAR_Y 7

#define ROAD_OBJECT_ROOT_INDEX 0
#define ROAD_OBJECT_MASK_INDEX 2
#define ROAD_OBJECT_DESCRIPTOR_INDEX 3
#define ROAD_OBJECT_RECORD_TABLE_INDEX (0x14 / 4)
#define ROAD_OBJECT_MATERIAL_INDEX (0xFC / 4)
#define ROAD_OBJECT_SURFACE_TYPE 3

#define ROAD_RECORD_START_X_INDEX (0x80 / 4)
#define ROAD_RECORD_FAR_Y_INDEX (0x84 / 4)

#define ROAD_OBJECT_CENTER_Y_INDEX (0x4C / 4)
#define ROAD_OBJECT_EDGE_LEFT_INDEX (0x50 / 4)
#define ROAD_OBJECT_EDGE_RIGHT_INDEX (0x60 / 4)
#define ROAD_OBJECT_NEAR_LEFT_X_INDEX (0x70 / 4)
#define ROAD_OBJECT_NEAR_LEFT_Y_INDEX (0x74 / 4)
#define ROAD_OBJECT_NEAR_RIGHT_X_INDEX (0x78 / 4)
#define ROAD_OBJECT_NEAR_RIGHT_Y_INDEX (0x7C / 4)
#define ROAD_OBJECT_FAR_LEFT_X_INDEX (0x80 / 4)
#define ROAD_OBJECT_FAR_LEFT_Y_INDEX (0x84 / 4)
#define ROAD_OBJECT_FAR_RIGHT_X_INDEX (0x88 / 4)
#define ROAD_OBJECT_FAR_RIGHT_Y_INDEX (0x8C / 4)
#define ROAD_OBJECT_CURVE_X_INDEX (0xF4 / 4)
#define ROAD_OBJECT_CURVE_Y_INDEX (0xF8 / 4)

#define ROAD_SCREEN_LEFT_OUTER_X_INDEX (0xB4 / 4)
#define ROAD_SCREEN_LEFT_OUTER_Y_INDEX (0xB8 / 4)
#define ROAD_SCREEN_LEFT_INNER_X_INDEX (0xBC / 4)
#define ROAD_SCREEN_LEFT_INNER_Y_INDEX (0xC0 / 4)
#define ROAD_SCREEN_CENTER_LEFT_X_INDEX (0xC4 / 4)
#define ROAD_SCREEN_CENTER_LEFT_Y_INDEX (0xC8 / 4)
#define ROAD_SCREEN_CENTER_RIGHT_X_INDEX (0xCC / 4)
#define ROAD_SCREEN_CENTER_RIGHT_Y_INDEX (0xD0 / 4)
#define ROAD_SCREEN_RIGHT_INNER_X_INDEX (0xD4 / 4)
#define ROAD_SCREEN_RIGHT_INNER_Y_INDEX (0xD8 / 4)
#define ROAD_SCREEN_RIGHT_OUTER_X_INDEX (0xDC / 4)
#define ROAD_SCREEN_RIGHT_OUTER_Y_INDEX (0xE0 / 4)

#define ROAD_DESCRIPTOR_CURVE_START_INDEX (0x134 / 4)
#define ROAD_DESCRIPTOR_CURVE_END_INDEX (0x138 / 4)
#define ROAD_CONFIG_ANGLE_VERTICAL_INDEX 3
#define ROAD_CONFIG_NEAR_WEIGHT_INDEX 4
#define ROAD_CONFIG_ANGLE_HORIZONTAL_INDEX 5
#define ROAD_CONFIG_FAR_WEIGHT_INDEX 6

#define ROAD_COLUMN_PAIR_SHIFT 3
#define ROAD_STRIP_STEP_SHIFT 5
#define ROAD_COLUMN_COUNT 0x20
#define ROAD_COLUMN_MAX_INDEX 0x1F
#define CEL_COLUMN_FIELD_SCALE 0x40
#define CEL_COLUMN_FIELD_MASK 0xFFC0
#define CEL_PRE0_CONTROL_MASK 0xFFFF003F
#define CEL_SOURCE_WORD 2
#define CEL_X_WORD (0x10 / 4)
#define CEL_Y_WORD (0x14 / 4)
#define CEL_VDY_WORD (0x24 / 4)
#define CEL_PRE0_WORD (0x34 / 4)
#define CEL_DIMENSION_PREAMBLE_WORD (0x40 / 4)
#define CEL_PIXC_BASE 0x8000
#define CEL_PIXC_UPPER_BAND 0x8001
#define CEL_PIXC_MIDDLE_BAND 0x8002
#define CEL_PIXC_LOWER_BAND 0x8003

#define EDGE_PACKET_DIRECTION_MASK 0x00070000
#define EDGE_PACKET_LEFT_FLAG 0x00020000
#define EDGE_PACKET_RIGHT_FLAG 0x00040000

#define BORDER_SEGMENT_LEFT 0
#define BORDER_SEGMENT_LEFT_CORNER 1
#define BORDER_SEGMENT_CENTER 2
#define BORDER_SEGMENT_RIGHT_CORNER 3
#define BORDER_SEGMENT_RIGHT 4

static
int
div2_toward_zero(int value)
{
  return (value + (int)((unsigned int)value >> SIGN_BIT_SHIFT)) >> 1;
}


static
int
div4_toward_zero(int value)
{
  if(value < 0)
    {
      value += QUARTER_ROUND_BIAS;
    }
  return value >> QUARTER_DIVISOR_SHIFT;
}


static
int
div256_toward_zero(int value)
{
  if(value < 0)
    {
      value += BYTE_ROUND_BIAS;
    }
  return value >> BYTE_DIVISOR_SHIFT;
}


static
int
align_cell(int value)
{
  return value & ~CELL_FRACTION_MASK;
}


static
void
copy_pair(int       *dst,
                const int *src)
{
  dst[0] = src[0];
  dst[1] = src[1];
}


static
int
object_type(int *object)
{
  return *(unsigned char *)(object[ROAD_OBJECT_DESCRIPTOR_INDEX] +
                            ROAD_DESCRIPTOR_TYPE_OFFSET);
}


static
int
root_type(int *object)
{
  return *(unsigned char *)(
    *(int *)(object[ROAD_OBJECT_ROOT_INDEX] +
             ROAD_ROOT_DESCRIPTOR_OFFSET) +
    ROAD_DESCRIPTOR_TYPE_OFFSET);
}


void
append_wrapped_road_band_cels(const int *band_range,
                              int        texture_row_group)
{
  CCB *cel;
  int horizon_y;
  int source_row;
  int row_step;
  unsigned int pre0;

  horizon_y = road_renderer_state.road_horizon;
  if(band_range[ROAD_RANGE_NEAR_Y] >= horizon_y)
    {
      return;
    }

  source_row = texture_row_group << ROAD_COLUMN_PAIR_SHIFT;
  cel = append_cel_mapping_packet(
    road_surface_render_globals.fallback_ccb);
  cel->ccb_SourcePtr = select_cel_source_row(cel, source_row);
  pre0 = cel->ccb_PRE0 & CEL_PRE0_CONTROL_MASK;
  pre0 |= ((ROAD_COLUMN_MAX_INDEX - source_row) *
           CEL_COLUMN_FIELD_SCALE) & CEL_COLUMN_FIELD_MASK;
  cel->ccb_PRE0 = pre0;

  row_step = (band_range[ROAD_RANGE_FAR_Y] -
              band_range[ROAD_RANGE_NEAR_Y]) >> ROAD_STRIP_STEP_SHIFT;
  cel->ccb_VDY = row_step;
  cel->ccb_XPos = band_range[ROAD_RANGE_X];
  cel->ccb_YPos = band_range[ROAD_RANGE_NEAR_Y];

  if(source_row == 0 ||
     band_range[ROAD_RANGE_NEAR_Y] + row_step * source_row >= horizon_y)
    {
      return;
    }

  /* The wrapped rows start again at the fallback CCB's base source. */
  cel = append_cel_mapping_packet(
    road_surface_render_globals.fallback_ccb);
  pre0 = cel->ccb_PRE0 & CEL_PRE0_CONTROL_MASK;
  pre0 |= ((source_row - 1) * CEL_COLUMN_FIELD_SCALE) &
          CEL_COLUMN_FIELD_MASK;
  cel->ccb_PRE0 = pre0;
  cel->ccb_VDY = row_step;
  cel->ccb_XPos = band_range[ROAD_RANGE_X];
  cel->ccb_YPos = band_range[ROAD_RANGE_NEAR_Y] +
                  row_step * (ROAD_COLUMN_COUNT - source_row);
}


int
render_road_horizon_bands(int *object,
                          int  side)
{
  int *record;
  int range[ROAD_QUAD_WORDS];
  int mask;
  int columns;
  int selector;
  int start;
  int middle;
  int split;
  unsigned int angle;
  SignedDivisionResult division;

  record = (int *)(object[ROAD_OBJECT_RECORD_TABLE_INDEX] +
                   side * ROAD_COORD_PAIR_BYTES);
  range[ROAD_RANGE_FAR_Y] = record[ROAD_RECORD_FAR_Y_INDEX];
  if(range[ROAD_RANGE_FAR_Y] <= road_renderer_state.road_horizon &&
     range[ROAD_RANGE_FAR_Y] <= ROAD_NEAR_CLIP_Y)
    {
      return ROAD_FUNCTION_RESULT;
    }

  angle = (unsigned int)road_renderer_state.motion.road_scroll_phase &
          ANGLE_FRACTION_MASK;
  start = -(int)angle;
  if(start > 0)
    {
      start -= ROAD_WRAP_STEP;
    }

  mask = object[ROAD_OBJECT_MASK_INDEX];
  division = signed_divide_with_remainder(
    (unsigned int)road_surface_render_globals.column_count,
    ROAD_COLUMN_COUNT);
  columns = (int)division.quotient;
  selector = columns - ((columns - 1) & mask) - 1;

  *(int *)((char *)road_surface_render_globals.fallback_ccb +
           CEL_PIXC_OFFSET) = CEL_PIXC_UPPER_BAND;
  range[ROAD_RANGE_X] = record[ROAD_RECORD_START_X_INDEX];
  while(range[ROAD_RANGE_X] > 0)
    {
      range[ROAD_RANGE_X] -= ROAD_WRAP_STEP;
    }
  middle = div2_toward_zero(range[ROAD_RANGE_FAR_Y] +
                                  ROAD_NEAR_CLIP_Y);
  range[ROAD_RANGE_NEAR_Y] = middle;
  while(range[ROAD_RANGE_X] < ROAD_SCREEN_LIMIT)
    {
      append_wrapped_road_band_cels(range, selector);
      range[ROAD_RANGE_X] += ROAD_WRAP_STEP;
    }

  *(int *)((char *)road_surface_render_globals.fallback_ccb +
           CEL_PIXC_OFFSET) = CEL_PIXC_MIDDLE_BAND;
  range[ROAD_RANGE_X] = start;
  range[ROAD_RANGE_FAR_Y] = middle;
  division = signed_divide_with_remainder(
    ROAD_HORIZON_DIVISOR,
    (unsigned int)(middle + ROAD_HORIZON_SPLIT_OFFSET));
  split = (int)division.quotient;
  range[ROAD_RANGE_NEAR_Y] = split;
  while(range[ROAD_RANGE_X] < ROAD_SCREEN_LIMIT)
    {
      append_wrapped_road_band_cels(range, 0);
      range[ROAD_RANGE_X] += ROAD_WRAP_STEP;
    }

  *(int *)((char *)road_surface_render_globals.fallback_ccb +
           CEL_PIXC_OFFSET) = CEL_PIXC_LOWER_BAND;
  range[ROAD_RANGE_X] = start;
  range[ROAD_RANGE_NEAR_Y] = ROAD_NEAR_CLIP_Y;
  range[ROAD_RANGE_FAR_Y] = split;
  while(range[ROAD_RANGE_X] < ROAD_SCREEN_LIMIT)
    {
      append_wrapped_road_band_cels(range, 0);
      range[ROAD_RANGE_X] += ROAD_WRAP_STEP;
    }
  return ROAD_FUNCTION_RESULT;
}


static
int
scaled_product(int first,
                     int second)
{
  return (first >> FIXED_PRODUCT_SHIFT) *
         (second >> FIXED_PRODUCT_SHIFT);
}


static
int
should_emit_edge(int *points,
                       int  side)
{
  int first;
  int second;

  if(side == 1)
    {
      if(points[QUAD_SECOND_X] < points[QUAD_FIRST_X])
        {
          return points[QUAD_SECOND_X] < ROAD_SCREEN_LIMIT;
        }

      first = scaled_product(
        points[QUAD_FOURTH_X] - points[QUAD_THIRD_X],
        points[QUAD_SECOND_Y] - points[QUAD_THIRD_Y]);
      second = scaled_product(
        points[QUAD_FOURTH_Y] - points[QUAD_THIRD_Y],
        points[QUAD_SECOND_X] - points[QUAD_THIRD_X]);
      if(first < second)
        {
          return 1;
        }

      first = scaled_product(
        points[QUAD_SECOND_X] - points[QUAD_FIRST_X],
        points[QUAD_FOURTH_Y] - points[QUAD_FIRST_Y]);
      second = scaled_product(
        points[QUAD_FOURTH_X] - points[QUAD_FIRST_X],
        points[QUAD_SECOND_Y] - points[QUAD_FIRST_Y]);
      return first < second;
    }

  if(points[QUAD_FIRST_X] < points[QUAD_SECOND_X])
    {
      return points[QUAD_SECOND_X] > 0;
    }

  first = scaled_product(
    points[QUAD_FOURTH_X] - points[QUAD_THIRD_X],
    points[QUAD_SECOND_Y] - points[QUAD_THIRD_Y]);
  second = scaled_product(
    points[QUAD_FOURTH_Y] - points[QUAD_THIRD_Y],
    points[QUAD_SECOND_X] - points[QUAD_THIRD_X]);
  if(first > second)
    {
      return 1;
    }

  first = scaled_product(
    points[QUAD_SECOND_X] - points[QUAD_FIRST_X],
    points[QUAD_FOURTH_Y] - points[QUAD_FIRST_Y]);
  second = scaled_product(
    points[QUAD_FOURTH_X] - points[QUAD_FIRST_X],
    points[QUAD_SECOND_Y] - points[QUAD_FIRST_Y]);
  return first > second;
}


static
void
mark_edge_packet(int *object,
                       int  side,
                       int *points)
{
  int source;
  int *packet;

  source = select_road_strip_cel(object, side, points, 0);
  packet = (int *)append_mapped_quad_cel(
    (const CCB *)source, (const DisplayQuad *)points);
  packet[0] &= ~EDGE_PACKET_DIRECTION_MASK;
  if(side == 1)
    {
      packet[0] |= EDGE_PACKET_LEFT_FLAG;
    }
  else
    {
      packet[0] |= EDGE_PACKET_RIGHT_FLAG;
    }
}


static
void
emit_fill_packet(int *object,
                       int *neighbor,
                       int  side,
                       int *points)
{
  int *cel;
  int columns;
  int mask;
  int selector;
  int flags;
  SignedDivisionResult division;

  points[QUAD_FIRST_X] = points[QUAD_THIRD_X];
  if(side == 1)
    {
      points[QUAD_SECOND_X] =
        points[QUAD_THIRD_X] < ROAD_WRAP_SPLIT_X
                ? ROAD_SCREEN_LIMIT
                : points[QUAD_THIRD_X] + ROAD_WRAP_STEP;
      points[QUAD_THIRD_X] =
        points[QUAD_FOURTH_X] < ROAD_WRAP_SPLIT_X
                ? ROAD_SCREEN_LIMIT
                : points[QUAD_FOURTH_X] + ROAD_WRAP_STEP;
    }
  else if(points[QUAD_THIRD_X] > ROAD_WRAP_STEP)
    {
      points[QUAD_SECOND_X] = 0;
      points[QUAD_THIRD_X] = 0;
    }
  else
    {
      points[QUAD_SECOND_X] = points[QUAD_THIRD_X] - ROAD_WRAP_STEP;
      points[QUAD_THIRD_X] = points[QUAD_FOURTH_X] - ROAD_WRAP_STEP;
    }
  points[QUAD_FIRST_Y] = points[QUAD_THIRD_Y];

  *(int *)((char *)road_surface_render_globals.primary_ccb +
           CEL_PIXC_OFFSET) = CEL_PIXC_BASE;
  cel = (int *)append_unmapped_cel(
    (const CCB *)road_surface_render_globals.primary_ccb);
  mask = *(int *)(neighbor[ROAD_OBJECT_MASK_INDEX] +
                  ROAD_MASK_VALUE_OFFSET);
  columns = road_surface_render_globals.column_count;
  division = signed_divide_with_remainder((unsigned int)columns,
                                          ROAD_COLUMN_COUNT);
  selector = (int)division.quotient;
  selector = selector - ((selector - 1) & mask) - 1;
  cel[CEL_SOURCE_WORD] = (int)select_cel_source_row(
    (CCB *)cel, columns * selector);
  cel[CEL_DIMENSION_PREAMBLE_WORD] =
    (int)gCelDimensionPreambleTable[columns];
  flags = cel[CEL_PRE0_WORD] & (int)CEL_PRE0_CONTROL_MASK;
  flags |= ((columns - 1) * CEL_COLUMN_FIELD_SCALE) &
           CEL_COLUMN_FIELD_MASK;
  cel[CEL_PRE0_WORD] = flags;
  map_cel_to_horizontal_quad(
    (CCB *)cel, (const RoadQuad *)(const void *)points);
}


void
render_recessed_road_edge(int *object,
                          int  side)
{
  int *neighbor;
  int points[ROAD_QUAD_WORDS];
  int same_type;
  int emit;

  same_type = object_type(object) == ROAD_OBJECT_SURFACE_TYPE &&
              root_type(object) == ROAD_OBJECT_SURFACE_TYPE;
  if(road_surface_render_globals.disabled != 0 && same_type != 0)
    {
      return;
    }

  neighbor = (int *)object[side];
  copy_pair(points,
                  (int *)((char *)neighbor +
                          ROAD_OBJECT_NEAR_LEFT_X_INDEX * WORD_BYTES +
                          side * ROAD_COORD_PAIR_BYTES));
  copy_pair(points + QUAD_SECOND_PAIR_WORD,
                  (int *)((char *)object +
                          ROAD_OBJECT_NEAR_LEFT_X_INDEX * WORD_BYTES +
                          side * ROAD_COORD_PAIR_BYTES));
  copy_pair(points + QUAD_THIRD_PAIR_WORD,
                  (int *)((char *)object +
                          ROAD_OBJECT_FAR_LEFT_X_INDEX * WORD_BYTES +
                          side * ROAD_COORD_PAIR_BYTES));
  copy_pair(points + QUAD_FOURTH_PAIR_WORD,
                  (int *)((char *)neighbor +
                          ROAD_OBJECT_FAR_LEFT_X_INDEX * WORD_BYTES +
                          side * ROAD_COORD_PAIR_BYTES));
  points[QUAD_FIRST_Y] -= ROAD_EDGE_VERTICAL_INSET;
  points[QUAD_SECOND_Y] -= ROAD_EDGE_VERTICAL_INSET;

  if(should_emit_edge(points, side) != 0)
    {
      mark_edge_packet(object, side, points);
    }

  if(points[QUAD_FOURTH_Y] < ROAD_FILL_CLIP_Y)
    {
      emit_fill_packet(object, neighbor, side, points);
    }

  copy_pair(points,
                  (int *)((char *)neighbor +
                          ROAD_OBJECT_NEAR_LEFT_X_INDEX * WORD_BYTES +
                          side * ROAD_COORD_PAIR_BYTES));
  copy_pair(points + QUAD_SECOND_PAIR_WORD,
                  (int *)((char *)object +
                          ROAD_OBJECT_NEAR_LEFT_X_INDEX * WORD_BYTES +
                          side * ROAD_COORD_PAIR_BYTES));
  copy_pair(points + QUAD_THIRD_PAIR_WORD,
                  (int *)((char *)object +
                          ROAD_OBJECT_FAR_LEFT_X_INDEX * WORD_BYTES +
                          side * ROAD_COORD_PAIR_BYTES));
  copy_pair(points + QUAD_FOURTH_PAIR_WORD,
                  (int *)((char *)neighbor +
                          ROAD_OBJECT_FAR_LEFT_X_INDEX * WORD_BYTES +
                          side * ROAD_COORD_PAIR_BYTES));

  if(object_type(object) == ROAD_OBJECT_SURFACE_TYPE)
    {
      copy_pair(points,
                      (int *)((char *)neighbor +
                              ROAD_OBJECT_CURVE_X_INDEX * WORD_BYTES));
      copy_pair(points + QUAD_SECOND_PAIR_WORD,
                      (int *)((char *)object +
                              ROAD_OBJECT_CURVE_X_INDEX * WORD_BYTES));
      copy_pair(points + QUAD_THIRD_PAIR_WORD,
                      (int *)((char *)object +
                              ROAD_OBJECT_NEAR_LEFT_X_INDEX * WORD_BYTES +
                              side * ROAD_COORD_PAIR_BYTES));
      copy_pair(points + QUAD_FOURTH_PAIR_WORD,
                      (int *)((char *)neighbor +
                              ROAD_OBJECT_NEAR_LEFT_X_INDEX * WORD_BYTES +
                              side * ROAD_COORD_PAIR_BYTES));
      emit = points[QUAD_FIRST_X] > points[QUAD_SECOND_X];
      if(side == 0)
        {
          emit = !emit;
        }
      if(emit != 0)
        {
          append_mapped_quad_cel(
            (const CCB *)select_road_strip_cel(object, side, points, 0),
            (const DisplayQuad *)points);
        }
    }
}


static
int
interpolate(int from,
                  int to,
                  int weight)
{
  return from +
         (int)MulSF16((frac16)(to - from),
                      (frac16)(weight << FIXED_WEIGHT_SHIFT));
}


static
void
build_curve(int *object,
                  int  mode,
                  int *curve)
{
  int *config;

  config = road_surface_render_globals.configuration;
  curve[STRIP_START_X] = object[ROAD_OBJECT_NEAR_LEFT_X_INDEX];
  curve[STRIP_START_Y] = object[ROAD_OBJECT_NEAR_LEFT_Y_INDEX];
  curve[STRIP_END_X] = object[ROAD_OBJECT_NEAR_RIGHT_X_INDEX];
  curve[STRIP_END_Y] = object[ROAD_OBJECT_NEAR_RIGHT_Y_INDEX];

  if(mode != 0)
    {
      curve[STRIP_CONTROL_ONE_X] = object[ROAD_OBJECT_CURVE_X_INDEX];
      curve[STRIP_CONTROL_ONE_Y] =
        object[ROAD_OBJECT_CURVE_Y_INDEX] +
        div4_toward_zero(
          object[ROAD_OBJECT_CURVE_Y_INDEX] -
          curve[STRIP_START_Y]);
      curve[STRIP_CONTROL_FOUR_X] =
        object[ROAD_OBJECT_FAR_RIGHT_X_INDEX];
      curve[STRIP_CONTROL_FOUR_Y] =
        object[ROAD_OBJECT_FAR_RIGHT_Y_INDEX];
      curve[STRIP_CONTROL_THREE_X] = interpolate(
        object[ROAD_OBJECT_CURVE_X_INDEX],
        curve[STRIP_CONTROL_FOUR_X],
        config[ROAD_CONFIG_FAR_WEIGHT_INDEX]);
      curve[STRIP_CONTROL_THREE_Y] = interpolate(
        object[ROAD_OBJECT_CURVE_Y_INDEX],
        curve[STRIP_CONTROL_FOUR_Y],
        config[ROAD_CONFIG_FAR_WEIGHT_INDEX]);
      curve[STRIP_CONTROL_TWO_X] = interpolate(
        object[ROAD_OBJECT_CURVE_X_INDEX],
        curve[STRIP_CONTROL_FOUR_X],
        config[ROAD_CONFIG_NEAR_WEIGHT_INDEX]);
      curve[STRIP_CONTROL_TWO_Y] = interpolate(
        curve[STRIP_CONTROL_ONE_Y],
        curve[STRIP_CONTROL_FOUR_Y],
        config[ROAD_CONFIG_NEAR_WEIGHT_INDEX]);
    }
  else
    {
      curve[STRIP_CONTROL_ONE_X] =
        object[ROAD_OBJECT_FAR_LEFT_X_INDEX];
      curve[STRIP_CONTROL_ONE_Y] =
        object[ROAD_OBJECT_FAR_LEFT_Y_INDEX];
      curve[STRIP_CONTROL_FOUR_X] = object[ROAD_OBJECT_CURVE_X_INDEX];
      curve[STRIP_CONTROL_FOUR_Y] =
        object[ROAD_OBJECT_CURVE_Y_INDEX] +
        div4_toward_zero(
          object[ROAD_OBJECT_CURVE_Y_INDEX] -
          curve[STRIP_END_Y]);
      curve[STRIP_CONTROL_TWO_X] = interpolate(
        curve[STRIP_CONTROL_ONE_X],
        object[ROAD_OBJECT_CURVE_X_INDEX],
        config[ROAD_CONFIG_NEAR_WEIGHT_INDEX]);
      curve[STRIP_CONTROL_TWO_Y] = interpolate(
        curve[STRIP_CONTROL_ONE_Y],
        object[ROAD_OBJECT_CURVE_Y_INDEX],
        config[ROAD_CONFIG_NEAR_WEIGHT_INDEX]);
      curve[STRIP_CONTROL_THREE_X] = interpolate(
        curve[STRIP_CONTROL_ONE_X],
        object[ROAD_OBJECT_CURVE_X_INDEX],
        config[ROAD_CONFIG_FAR_WEIGHT_INDEX]);
      curve[STRIP_CONTROL_THREE_Y] = interpolate(
        curve[STRIP_CONTROL_ONE_Y],
        curve[STRIP_CONTROL_FOUR_Y],
        config[ROAD_CONFIG_FAR_WEIGHT_INDEX]);
    }
}


static
void
update_curve_weights(int *object,
                           int  quarter)
{
  int *config;
  int *descriptor;
  int product;
  int angle;

  if(*(int *)(object[ROAD_OBJECT_MASK_INDEX] + ROAD_MASK_VALUE_OFFSET) !=
     road_surface_render_globals.cached_depth)
    {
      road_surface_render_globals.cached_depth =
        *(int *)(object[ROAD_OBJECT_MASK_INDEX] +
                 ROAD_MASK_VALUE_OFFSET);
      config = road_surface_render_globals.configuration;
      descriptor = (int *)object[ROAD_OBJECT_DESCRIPTOR_INDEX];
      product = config[ROAD_CONFIG_NEAR_WEIGHT_INDEX] *
                (descriptor[ROAD_DESCRIPTOR_CURVE_END_INDEX] -
                 descriptor[ROAD_DESCRIPTOR_CURVE_START_INDEX]);
      product = div256_toward_zero(product);
      angle = fixed_vector_angle_24(
        product,
        config[ROAD_CONFIG_ANGLE_HORIZONTAL_INDEX] -
        config[ROAD_CONFIG_ANGLE_VERTICAL_INDEX]);
      angle = div2_toward_zero(ANGLE_THREE_QUARTERS - angle) -
              ANGLE_QUARTER;
      road_surface_render_globals.cached_sine =
        fixed_tangent_8_8(angle >> ANGLE_LOOKUP_SHIFT) <<
          FIXED_WEIGHT_SHIFT;
      road_surface_render_globals.cached_cosine =
        fixed_tangent_8_8((ANGLE_EIGHTH - angle) >>
                          ANGLE_LOOKUP_SHIFT) <<
          FIXED_WEIGHT_SHIFT;
    }

  (void)quarter;
}


static
void
build_screen_edge(int *object,
                        int *screen)
{
  int quarter;
  int horizontal;
  int vertical;

  quarter = div4_toward_zero(
    object[ROAD_OBJECT_CENTER_Y_INDEX] -
    object[ROAD_SCREEN_LEFT_INNER_Y_INDEX]);
  screen[STRIP_START_X] = align_cell(
    object[ROAD_SCREEN_LEFT_OUTER_X_INDEX] - quarter);
  screen[STRIP_START_Y] =
    align_cell(object[ROAD_OBJECT_CENTER_Y_INDEX]);
  screen[STRIP_CONTROL_ONE_X] = screen[STRIP_START_X];
  screen[STRIP_CONTROL_ONE_Y] =
    align_cell(object[ROAD_SCREEN_LEFT_INNER_Y_INDEX]);
  screen[STRIP_CONTROL_TWO_X] =
    align_cell(object[ROAD_SCREEN_CENTER_LEFT_X_INDEX]);
  screen[STRIP_CONTROL_TWO_Y] = align_cell(
    object[ROAD_SCREEN_CENTER_LEFT_Y_INDEX] - quarter);
  screen[STRIP_CONTROL_THREE_X] =
    align_cell(object[ROAD_SCREEN_CENTER_RIGHT_X_INDEX]);
  screen[STRIP_CONTROL_THREE_Y] = align_cell(
    object[ROAD_SCREEN_CENTER_RIGHT_Y_INDEX] - quarter);
  screen[STRIP_END_X] = align_cell(
    object[ROAD_SCREEN_RIGHT_OUTER_X_INDEX] + quarter);
  screen[STRIP_END_Y] =
    align_cell(object[ROAD_OBJECT_CENTER_Y_INDEX]);
  screen[STRIP_CONTROL_FOUR_X] = screen[STRIP_END_X];
  screen[STRIP_CONTROL_FOUR_Y] =
    align_cell(object[ROAD_SCREEN_RIGHT_INNER_Y_INDEX]);

  update_curve_weights(object, quarter);
  horizontal = (road_surface_render_globals.cached_sine >>
                FIXED_WEIGHT_SHIFT) *
               (quarter >> FIXED_WEIGHT_SHIFT);
  vertical = (road_surface_render_globals.cached_cosine >>
              FIXED_WEIGHT_SHIFT) *
             (quarter >> FIXED_WEIGHT_SHIFT);
  screen[STRIP_CONTROL_ONE_Y] -= horizontal;
  screen[STRIP_CONTROL_FOUR_Y] -= horizontal;
  screen[STRIP_CONTROL_TWO_X] -= vertical;
  screen[STRIP_CONTROL_THREE_X] += vertical;
}


static
void
emit_surface(int *object,
                   int  mode,
                   int *curve,
                   int *screen)
{
  int points[ROAD_QUAD_WORDS];
  int source;
  int index;

  copy_pair(points, curve);
  copy_pair(points + QUAD_FOURTH_PAIR_WORD, screen);
  for(index = 0; index < ROAD_SURFACE_SEGMENT_COUNT; index++)
    {
      copy_pair(
        points + QUAD_SECOND_PAIR_WORD,
        curve + (index + 1) * ROAD_COORD_PAIR_WORDS);
      copy_pair(
        points + QUAD_THIRD_PAIR_WORD,
        screen + (index + 1) * ROAD_COORD_PAIR_WORDS);
      source = select_road_strip_cel(object, mode, points, 0);
      append_mapped_quad_cel(
        (const CCB *)source, (const DisplayQuad *)points);
      copy_pair(points, points + QUAD_SECOND_PAIR_WORD);
      copy_pair(points + QUAD_FOURTH_PAIR_WORD,
                      points + QUAD_THIRD_PAIR_WORD);
    }
}


static
void
emit_border(int *object,
                  int *screen)
{
  int points[ROAD_QUAD_WORDS];
  char *source;
  int index;

  copy_pair(points, screen);
  copy_pair(
    points + QUAD_FOURTH_PAIR_WORD,
    (int *)((char *)object +
            ROAD_SCREEN_LEFT_OUTER_X_INDEX * WORD_BYTES));
  points[QUAD_FIRST_X] -= ROAD_BORDER_MARGIN;
  points[QUAD_FOURTH_X] += ROAD_BORDER_MARGIN;
  source = 0;

  for(index = 0; index < ROAD_SURFACE_SEGMENT_COUNT; index++)
    {
      copy_pair(
        points + QUAD_SECOND_PAIR_WORD,
        screen + (index + 1) * ROAD_COORD_PAIR_WORDS);
      copy_pair(
        points + QUAD_THIRD_PAIR_WORD,
        (int *)((char *)object +
                ROAD_SCREEN_LEFT_INNER_X_INDEX * WORD_BYTES +
                index * ROAD_COORD_PAIR_BYTES));
      switch(index)
        {
        case BORDER_SEGMENT_LEFT:
          points[QUAD_SECOND_X] -= ROAD_BORDER_MARGIN;
          points[QUAD_THIRD_X] += ROAD_BORDER_MARGIN;
          break;
        case BORDER_SEGMENT_LEFT_CORNER:
          points[QUAD_SECOND_X] -= ROAD_BORDER_MARGIN;
          points[QUAD_SECOND_Y] -= ROAD_BORDER_MARGIN;
          points[QUAD_THIRD_Y] += ROAD_BORDER_MARGIN;
          break;
        case BORDER_SEGMENT_CENTER:
          points[QUAD_SECOND_X] += ROAD_BORDER_MARGIN;
          points[QUAD_SECOND_Y] -= ROAD_BORDER_MARGIN;
          points[QUAD_THIRD_Y] += ROAD_BORDER_MARGIN;
          break;
        case BORDER_SEGMENT_RIGHT_CORNER:
        case BORDER_SEGMENT_RIGHT:
          points[QUAD_SECOND_X] += ROAD_BORDER_MARGIN;
          points[QUAD_THIRD_X] -= ROAD_BORDER_MARGIN;
          break;
        }

      if(source == 0)
        {
          source = select_cached_object_cel(
            (char **)(object[ROAD_OBJECT_MATERIAL_INDEX] +
                      ROAD_BORDER_CACHE_OFFSET),
            (char *)object, ROAD_SURFACE_SEGMENT_COUNT, points);
        }
      append_mapped_quad_cel(
        (const CCB *)source, (const DisplayQuad *)points);
      copy_pair(points, points + QUAD_SECOND_PAIR_WORD);
      copy_pair(points + QUAD_FOURTH_PAIR_WORD,
                      points + QUAD_THIRD_PAIR_WORD);
    }
}


static
void
emit_vertical_edges(int *object)
{
  int points[ROAD_QUAD_WORDS];
  char *source;
  int index;

  source = 0;
  for(index = 0; index < ROAD_VERTICAL_EDGE_COUNT; index++)
    {
      copy_pair(
        points,
        (int *)((char *)object +
                ROAD_OBJECT_EDGE_LEFT_INDEX * WORD_BYTES +
                index * ROAD_COORD_PAIR_BYTES));
      if(points[QUAD_FIRST_X] > 0 &&
         points[QUAD_FIRST_X] < ROAD_SCREEN_LIMIT)
        {
          points[QUAD_SECOND_X] =
            *(int *)((char *)object +
                     ROAD_OBJECT_EDGE_RIGHT_INDEX * WORD_BYTES +
                     index * ROAD_COORD_PAIR_BYTES);
          points[QUAD_SECOND_Y] = points[QUAD_FIRST_Y];
          points[QUAD_THIRD_X] = points[QUAD_SECOND_X];
          points[QUAD_THIRD_Y] =
            object[ROAD_OBJECT_CENTER_Y_INDEX];
          points[QUAD_FOURTH_X] = points[QUAD_FIRST_X];
          points[QUAD_FOURTH_Y] =
            object[ROAD_OBJECT_CENTER_Y_INDEX];
          if(source == 0)
            {
              source = select_cached_object_cel(
                (char **)(object[ROAD_OBJECT_MATERIAL_INDEX] +
                          ROAD_VERTICAL_EDGE_CACHE_OFFSET),
                (char *)object, ROAD_VERTICAL_SOURCE_SEGMENT_COUNT,
                points);
            }
          append_mapped_quad_cel(
        (const CCB *)source, (const DisplayQuad *)points);
        }
    }
}


void
render_curved_road_surface(int *object)
{
  int curve[ROAD_CURVE_WORDS];
  int screen[ROAD_CURVE_WORDS];
  int mode;

  object[ROAD_SCREEN_RIGHT_OUTER_Y_INDEX] =
    object[ROAD_OBJECT_CENTER_Y_INDEX];
  object[ROAD_SCREEN_LEFT_OUTER_Y_INDEX] =
    object[ROAD_OBJECT_CENTER_Y_INDEX];
  mode = *(int *)(object[ROAD_OBJECT_DESCRIPTOR_INDEX] +
                  ROAD_DESCRIPTOR_CURVE_MODE_OFFSET) > 0;

  build_curve(object, mode, curve);
  build_screen_edge(object, screen);
  emit_surface(object, mode, curve, screen);
  emit_border(object, screen);
  emit_vertical_edges(object);
}
