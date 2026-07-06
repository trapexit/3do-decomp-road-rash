/* Road edge transitions, span joins, and procedural roadside samples. */



#include "bss_early_data.h"
#include "cel_dimension_resource.h"
#include "render_road_edge_offsets.h"
#include "display_cel_runtime.h"
#include "road_geometry.h"
#include "rw_semantic_data.h"

#define WORD_AT(base, offset) (*(int *)((char *)(base) + (offset)))
#define UBYTE_AT(base, offset) (*(unsigned char *)((char *)(base) + (offset)))

#define ROAD_SIDE_BYTE_STRIDE                 8
#define ROAD_DESCRIPTION_EDGE_FLAG            0x10
#define ROAD_STYLE_GROUP_MASK                 0xC0
#define ROAD_STYLE_VARIANT_MASK               0x3F
#define ROAD_STYLE_DISABLED                   0xFF
#define ROAD_OVERLAY_PIXC                     0x1F
#define ROAD_SAMPLE_INDEX_BIAS                0x100
#define ROAD_CEL_MODE_SURFACE                 2
#define ROAD_CEL_MODE_EDGE                    3
#define ROAD_REQUIRED_LEVEL                   2
#define ROAD_BRIDGE_STATE_KIND                5
#define ROAD_SELECTOR_FINAL                   2
#define ROAD_RANDOM_REPEAT_MASK               3
#define ROAD_MINIMUM_REPEAT                   4
#define ROAD_SAMPLE_CAPACITY                  16
#define ROAD_REDUCED_SAMPLE_CAPACITY          8
#define ROAD_FIXED_SHIFT                      8
#define ROAD_SPAN_SHIFT                       4
#define ROAD_VARIANT_WIDTH_BIAS               1
#define ROAD_CEL_MINIMUM_HEIGHT               1
#define ROAD_CHAIN_NODE_LIMIT                 2
#define ROAD_PIXC_SECOND_SOURCE_INVERT        0x00000002
#define ROAD_PIXC_SECOND_SOURCE_SIGN_EXTEND   0x00000004
#define ROAD_PIXC_DISABLE_OVERFLOW_WRAP       0x00000008

int
lookup_family_resource_entry_for_render(int resource_index,
                                        int resource_type);
int
advance_random_seed(volatile int *seed);
char *
select_cached_object_cel(char **cache_slot,
                         char  *road_object,
                         int    child_index,
                         int   *quad);
char *
select_cached_description_cel(char **cache_slots,
                              char  *description,
                              int    child_index,
                              int    side,
                              int   *quad);
CCB *
select_cached_side_cel(RoadRenderSide *road_side,
                       RoadSide        side,
                       const RoadQuad *quad);
int
road_quad_is_horizontally_visible(const RoadRenderSide *road_side,
                                  const RoadQuad       *quad,
                                  int                   reverse_direction);
int
render_road_side_geometry(char *road_object,
                          int   side);
void
subdivide_four_point_road_strip(RoadStripDrawContext *draw_context);
void
subdivide_three_point_road_strip(RoadStripDrawContext *draw_context);

__inline
static
int
is_within_track_side_bounds(int      value,
                            RoadSide side)
{
  if(side == ROAD_SIDE_LEFT)
    {
      return value > 0;
    }
  return value < ROAD_SCREEN_RIGHT;
}


void
render_road_edge_transition(int     *object,
                            RoadSide side,
                            int      mode)
{
  int *previous;
  char *description;
  char *previous_description;
  CCB *cel;
  RoadQuad points;
  RoadStripDrawContext draw;
  int horizon;
  int projection_depth;
  int crosses;

  previous = (int *)object[side];
  description = (char *)object[
    ROAD_OBJECT_DESCRIPTION_OFFSET / ROAD_WORD_BYTES];
  previous_description = (char *)previous[
    ROAD_OBJECT_DESCRIPTION_OFFSET / ROAD_WORD_BYTES];
  horizon = WORD_AT(
    (int *)object[ROAD_OBJECT_CONTEXT_OFFSET / ROAD_WORD_BYTES],
    ROAD_CONTEXT_HORIZON_OFFSET);

  points.top_left.x = WORD_AT(
    previous, side * ROAD_SIDE_BYTE_STRIDE + ROAD_OBJECT_OUTER_X_OFFSET);
  points.top_left.y = WORD_AT(
    previous, side * ROAD_SIDE_BYTE_STRIDE + ROAD_OBJECT_OUTER_Y_OFFSET);
  points.bottom_left.x = WORD_AT(
    object, side * ROAD_SIDE_BYTE_STRIDE + ROAD_OBJECT_OUTER_X_OFFSET);
  points.bottom_left.y = WORD_AT(
    object, side * ROAD_SIDE_BYTE_STRIDE + ROAD_OBJECT_OUTER_Y_OFFSET);

  if(points.top_left.y < points.bottom_left.y &&
     points.top_left.y <= horizon)
    {
      points.top_right.x = WORD_AT(
        previous, side * ROAD_SIDE_BYTE_STRIDE + ROAD_OBJECT_FAR_X_OFFSET);
      points.bottom_right.x = WORD_AT(
        object, side * ROAD_SIDE_BYTE_STRIDE + ROAD_OBJECT_FAR_X_OFFSET);

      /* Curved transitions can re-enter the screen on their near row. */
      if(road_quad_is_horizontally_visible(
           (RoadRenderSide *)object, &points,
           side == ROAD_SIDE_LEFT) == 0)
        {
          return;
        }

      points.top_left.y = ROAD_FIXED_FLOOR_QUARTER(points.top_left.y);
      points.bottom_left.y = ROAD_FIXED_FLOOR_QUARTER(
        points.bottom_left.y);
      if(mode < 0)
        {
          cel = (CCB *)select_cached_description_cel(
            (char **)(object[
                        ROAD_OBJECT_CEL_LIST_OFFSET / ROAD_WORD_BYTES] +
                      ROAD_CEL_PRIMARY_LIST_OFFSET),
            description + ROAD_DESCRIPTION_CEL_OFFSET,
            0, side, (int *)&points);
        }
      else
        {
          cel = (CCB *)select_cached_object_cel(
            (char **)(object[
                        ROAD_OBJECT_CEL_LIST_OFFSET / ROAD_WORD_BYTES] +
                      ROAD_CEL_PRIMARY_LIST_OFFSET),
            (char *)object, ROAD_CEL_MODE_SURFACE, (int *)&points);
        }

      projection_depth = object[
        ROAD_OBJECT_PROJECTION_DEPTH_OFFSET / ROAD_WORD_BYTES];
      if(projection_depth >= ROAD_SUBDIVISION_DEPTH_LIMIT)
        {
          append_horizontal_quad_cel(cel, (const DisplayQuad *)&points);
        }
      else
        {
          draw.side = side;
          draw.projection_depth = previous[
            ROAD_OBJECT_PROJECTION_DEPTH_OFFSET / ROAD_WORD_BYTES];
          draw.world_quad.top_left.x =
            WORD_AT(previous_description,
                    side * ROAD_WORD_BYTES + ROAD_DESCRIPTION_X_OFFSET) +
            previous[ROAD_OBJECT_WORLD_X_OFFSET / ROAD_WORD_BYTES];
          draw.world_quad.top_left.y =
            WORD_AT(previous_description,
                    side * ROAD_WORD_BYTES + ROAD_DESCRIPTION_Y_OFFSET) +
            previous[ROAD_OBJECT_WORLD_Y_OFFSET / ROAD_WORD_BYTES];
          draw.world_quad.bottom_left.x =
            WORD_AT(description,
                    side * ROAD_WORD_BYTES + ROAD_DESCRIPTION_X_OFFSET) +
            object[ROAD_OBJECT_WORLD_X_OFFSET / ROAD_WORD_BYTES];
          draw.world_quad.bottom_left.y =
            WORD_AT(description,
                    side * ROAD_WORD_BYTES + ROAD_DESCRIPTION_Y_OFFSET) +
            object[ROAD_OBJECT_WORLD_Y_OFFSET / ROAD_WORD_BYTES];
          if(side == ROAD_SIDE_RIGHT)
            {
              draw.world_quad.top_right.x =
                draw.world_quad.top_left.x +
                WORD_AT(previous_description,
                        ROAD_DESCRIPTION_RIGHT_SPAN_OFFSET);
              draw.world_quad.bottom_right.x =
                draw.world_quad.bottom_left.x +
                WORD_AT(description, ROAD_DESCRIPTION_RIGHT_SPAN_OFFSET);
            }
          else
            {
              draw.world_quad.top_right.x =
                draw.world_quad.top_left.x -
                WORD_AT(previous_description,
                        side * ROAD_WORD_BYTES +
                        ROAD_DESCRIPTION_LEFT_SPAN_OFFSET);
              draw.world_quad.bottom_right.x =
                draw.world_quad.bottom_left.x -
                WORD_AT(description,
                        side * ROAD_WORD_BYTES +
                        ROAD_DESCRIPTION_LEFT_SPAN_OFFSET);
            }
          draw.cel = cel;
          draw.screen_quad = &points;
          subdivide_three_point_road_strip(&draw);
        }
    }

  if(side == ROAD_SIDE_LEFT)
    {
      crosses = points.bottom_left.x <= points.top_left.x;
    }
  else
    {
      crosses = points.top_left.x <= points.bottom_left.x;
    }

  if(crosses && points.top_left.y < horizon)
    {
      points.top_right.x = points.top_left.x;
      points.top_right.y = previous[
        ROAD_OBJECT_TRACK_Y_OFFSET / ROAD_WORD_BYTES];
      points.bottom_right.x = points.bottom_left.x;
      points.bottom_right.y = object[
        ROAD_OBJECT_TRACK_Y_OFFSET / ROAD_WORD_BYTES];

      if(mode < 0)
        {
          cel = (CCB *)select_cached_description_cel(
            (char **)(object[
                        ROAD_OBJECT_CEL_LIST_OFFSET / ROAD_WORD_BYTES] +
                      ROAD_CEL_EDGE_LIST_OFFSET),
            description + ROAD_DESCRIPTION_CEL_OFFSET,
            1, side, (int *)&points);
        }
      else
        {
          cel = (CCB *)select_cached_object_cel(
            (char **)(object[
                        ROAD_OBJECT_CEL_LIST_OFFSET / ROAD_WORD_BYTES] +
                      ROAD_CEL_EDGE_LIST_OFFSET),
            (char *)object, ROAD_CEL_MODE_EDGE, (int *)&points);
        }

      projection_depth = object[
        ROAD_OBJECT_PROJECTION_DEPTH_OFFSET / ROAD_WORD_BYTES];
      if(projection_depth < ROAD_SUBDIVISION_DEPTH_LIMIT)
        {
          if(side == ROAD_SIDE_LEFT)
            {
              points.top_left.x -= ROAD_FIXED_HALF;
              points.top_right.x -= ROAD_FIXED_HALF;
              points.bottom_right.x -= ROAD_FIXED_HALF;
              points.bottom_left.x -= ROAD_FIXED_HALF;
            }
          else
            {
              points.top_left.x += ROAD_FIXED_HALF;
              points.top_right.x += ROAD_FIXED_HALF;
              points.bottom_right.x += ROAD_FIXED_HALF;
              points.bottom_left.x += ROAD_FIXED_HALF;
            }

          points.top_left.y -= ROAD_FIXED_THREE_QUARTERS;
          points.bottom_left.y -= ROAD_FIXED_THREE_QUARTERS;
          points.top_right.y += ROAD_FIXED_THREE_QUARTERS;
          points.bottom_right.y += ROAD_FIXED_THREE_QUARTERS;

          draw.side = side;
          draw.projection_depth = previous[
            ROAD_OBJECT_PROJECTION_DEPTH_OFFSET / ROAD_WORD_BYTES];
          draw.horizon_y = horizon;
          draw.world_quad.top_left.x =
            WORD_AT(previous_description,
                    side * ROAD_WORD_BYTES + ROAD_DESCRIPTION_X_OFFSET) +
            previous[ROAD_OBJECT_WORLD_X_OFFSET / ROAD_WORD_BYTES];
          draw.world_quad.top_left.y =
            WORD_AT(previous_description,
                    side * ROAD_WORD_BYTES + ROAD_DESCRIPTION_Y_OFFSET) +
            previous[ROAD_OBJECT_WORLD_Y_OFFSET / ROAD_WORD_BYTES];
          draw.world_quad.top_right.x = draw.world_quad.top_left.x;
          draw.world_quad.top_right.y = previous[
            ROAD_OBJECT_WORLD_Y_OFFSET / ROAD_WORD_BYTES];
          draw.world_quad.bottom_right.x =
            WORD_AT(description,
                    side * ROAD_WORD_BYTES + ROAD_DESCRIPTION_X_OFFSET) +
            object[ROAD_OBJECT_WORLD_X_OFFSET / ROAD_WORD_BYTES];
          draw.world_quad.bottom_right.y = object[
            ROAD_OBJECT_WORLD_Y_OFFSET / ROAD_WORD_BYTES];
          draw.world_quad.bottom_left.x =
            draw.world_quad.bottom_right.x;
          draw.world_quad.bottom_left.y =
            WORD_AT(description,
                    side * ROAD_WORD_BYTES + ROAD_DESCRIPTION_Y_OFFSET) +
            object[ROAD_OBJECT_WORLD_Y_OFFSET / ROAD_WORD_BYTES];
          draw.cel = cel;
          draw.screen_quad = &points;
          draw.adjust_edges = 1;
          subdivide_four_point_road_strip(&draw);
        }
      else
        {
          points.top_left.y -= ROAD_FIXED_THREE_EIGHTHS;
          points.bottom_left.y -= ROAD_FIXED_THREE_EIGHTHS;
          points.top_right.y += ROAD_FIXED_THREE_EIGHTHS;
          points.bottom_right.y += ROAD_FIXED_THREE_EIGHTHS;

          if(projection_depth < ROAD_TRANSITION_INSET_DEPTH_LIMIT)
            {
              if(side == ROAD_SIDE_LEFT)
                {
                  points.bottom_right.x -= ROAD_FIXED_HALF;
                  points.bottom_left.x -= ROAD_FIXED_HALF;
                }
              else
                {
                  points.bottom_right.x += ROAD_FIXED_HALF;
                  points.bottom_left.x += ROAD_FIXED_HALF;
                }
            }
          append_mapped_quad_cel(cel, (const DisplayQuad *)&points);
        }
    }

  if((UBYTE_AT(description, ROAD_DESCRIPTION_FLAGS_OFFSET) &
      ROAD_DESCRIPTION_EDGE_FLAG) != 0 &&
     WORD_AT(object,
             side * ROAD_SIDE_BYTE_STRIDE + ROAD_OBJECT_OUTER_Y_OFFSET) <
     horizon)
    {
      points.top_left.x = WORD_AT(
        object, side * ROAD_SIDE_BYTE_STRIDE + ROAD_OBJECT_OUTER_X_OFFSET);
      points.top_left.y = WORD_AT(
        object, side * ROAD_SIDE_BYTE_STRIDE + ROAD_OBJECT_OUTER_Y_OFFSET);
      points.top_right.x = WORD_AT(
        object, side * ROAD_SIDE_BYTE_STRIDE + ROAD_OBJECT_FAR_X_OFFSET);
      points.bottom_right.x = points.top_right.x;
      points.bottom_left.x = WORD_AT(
        object, side * ROAD_SIDE_BYTE_STRIDE + ROAD_OBJECT_OUTER_X_OFFSET);
      points.bottom_left.y = object[
        ROAD_OBJECT_TRACK_Y_OFFSET / ROAD_WORD_BYTES];
      cel = (CCB *)select_cached_description_cel(
        (char **)(object[
                    ROAD_OBJECT_CEL_LIST_OFFSET / ROAD_WORD_BYTES] +
                  ROAD_CEL_EDGE_LIST_OFFSET),
        description + ROAD_DESCRIPTION_CEL_OFFSET,
        1, side, (int *)&points);
      append_horizontal_quad_cel(cel, (const DisplayQuad *)&points);
    }
}


int
render_road_span_geometry(int *state,
                          int  selector,
                          int  level)
{
  int *left;
  int *right;
  char *left_description;
  char *right_description;
  CCB *cel;
  CCB *packet;
  RoadQuad points;
  int result;

  left = (int *)state[ROAD_STATE_LEFT_OFFSET / ROAD_WORD_BYTES];
  right = (int *)state[ROAD_STATE_RIGHT_OFFSET / ROAD_WORD_BYTES];
  left_description = (char *)left[
    ROAD_OBJECT_DESCRIPTION_OFFSET / ROAD_WORD_BYTES];
  right_description = (char *)right[
    ROAD_OBJECT_DESCRIPTION_OFFSET / ROAD_WORD_BYTES];

  if(selector == 0)
    {
      if(level != 1 && level != ROAD_REQUIRED_LEVEL)
        {
          return render_road_center_fill((RoadRenderNode *)state);
        }

      render_road_edge_transition(left, ROAD_SIDE_RIGHT, -1);
      render_road_edge_transition(right, ROAD_SIDE_LEFT, -1);
      points.top_left.x = WORD_AT(
        (int *)state[
          ROAD_STATE_LEFT_ANCHOR_OFFSET / ROAD_WORD_BYTES],
        ROAD_OBJECT_FAR_EDGE_X_OFFSET);
      points.top_left.y = WORD_AT(
        (int *)state[
          ROAD_STATE_LEFT_ANCHOR_OFFSET / ROAD_WORD_BYTES],
        ROAD_OBJECT_INNER_Y_OFFSET);
      points.top_right.x = WORD_AT(
        (int *)state[
          ROAD_STATE_RIGHT_ANCHOR_OFFSET / ROAD_WORD_BYTES],
        ROAD_OBJECT_FAR_X_OFFSET);
      points.top_right.y = WORD_AT(
        (int *)state[
          ROAD_STATE_RIGHT_ANCHOR_OFFSET / ROAD_WORD_BYTES],
        ROAD_OBJECT_OUTER_Y_OFFSET);
      points.bottom_right.x = right[
        ROAD_OBJECT_FAR_X_OFFSET / ROAD_WORD_BYTES];
      points.bottom_right.y = right[
        ROAD_OBJECT_OUTER_Y_OFFSET / ROAD_WORD_BYTES];
      points.bottom_left.x = left[
        ROAD_OBJECT_FAR_EDGE_X_OFFSET / ROAD_WORD_BYTES];
      points.bottom_left.y = left[
        ROAD_OBJECT_INNER_Y_OFFSET / ROAD_WORD_BYTES];
      cel = (CCB *)select_cached_description_cel(
        (char **)(left[
                    ROAD_OBJECT_CEL_LIST_OFFSET / ROAD_WORD_BYTES] +
                  ROAD_CEL_JOIN_LIST_OFFSET),
        left_description + ROAD_DESCRIPTION_CEL_OFFSET,
        ROAD_CEL_MODE_SURFACE, ROAD_SIDE_RIGHT,
        (int *)&points);
      result = (int)append_mapped_quad_cel(
        cel, (const DisplayQuad *)&points);
      if(level != ROAD_REQUIRED_LEVEL)
        {
          return result;
        }
      if(((int *)state[0])[
           ROAD_STATE_KIND_OFFSET / ROAD_WORD_BYTES] ==
         ROAD_BRIDGE_STATE_KIND)
        {
          points.top_left.x = left[
            ROAD_OBJECT_FAR_EDGE_X_OFFSET / ROAD_WORD_BYTES];
          points.top_left.y = left[
            ROAD_OBJECT_INNER_Y_OFFSET / ROAD_WORD_BYTES];
          points.top_right.x = left[
            ROAD_OBJECT_INNER_X_OFFSET / ROAD_WORD_BYTES];
          points.top_right.y = left[
            ROAD_OBJECT_INNER_Y_OFFSET / ROAD_WORD_BYTES];
          points.bottom_right.x = points.top_right.x;
          points.bottom_right.y = left[
            ROAD_OBJECT_TRACK_Y_OFFSET / ROAD_WORD_BYTES];
          points.bottom_left.x = points.top_left.x;
          points.bottom_left.y = points.bottom_right.y;
          cel = (CCB *)select_cached_description_cel(
            (char **)(left[
                        ROAD_OBJECT_CEL_LIST_OFFSET / ROAD_WORD_BYTES] +
                      ROAD_CEL_BRIDGE_LIST_OFFSET),
            left_description + ROAD_DESCRIPTION_CEL_OFFSET,
            ROAD_CEL_MODE_EDGE, ROAD_SIDE_RIGHT,
            (int *)&points);
          append_mapped_quad_cel(cel, (const DisplayQuad *)&points);

          points.top_left.x = points.top_right.x;
          points.bottom_left.x = points.bottom_right.x;
          points.top_right.x = right[
            ROAD_OBJECT_FAR_X_OFFSET / ROAD_WORD_BYTES];
          points.bottom_right.x = points.top_right.x;
          cel = (CCB *)select_cached_description_cel(
            (char **)(right[
                        ROAD_OBJECT_CEL_LIST_OFFSET / ROAD_WORD_BYTES] +
                      ROAD_CEL_BRIDGE_LIST_OFFSET),
            right_description + ROAD_DESCRIPTION_CEL_OFFSET,
            ROAD_CEL_MODE_EDGE, ROAD_SIDE_LEFT,
            (int *)&points);
          append_mapped_quad_cel(cel, (const DisplayQuad *)&points);

          points.top_left.x = points.top_right.x;
          points.bottom_left.x = points.bottom_right.x;
          points.top_right.x = right[
            ROAD_OBJECT_OUTER_X_OFFSET / ROAD_WORD_BYTES];
          points.bottom_right.x = points.top_right.x;
          cel = (CCB *)select_cached_description_cel(
            (char **)(right[
                        ROAD_OBJECT_CEL_LIST_OFFSET / ROAD_WORD_BYTES] +
                      ROAD_CEL_BRIDGE_LIST_OFFSET),
            right_description + ROAD_DESCRIPTION_CEL_OFFSET,
            ROAD_CEL_MODE_EDGE, ROAD_SIDE_LEFT,
            (int *)&points);
          append_mapped_quad_cel(cel, (const DisplayQuad *)&points);
        }
    }
  else if(selector == 1)
    {
      if(((int *)state[0])[
           ROAD_STATE_KIND_OFFSET / ROAD_WORD_BYTES] ==
         ROAD_BRIDGE_STATE_KIND)
        {
          points.top_left.x = left[
            ROAD_OBJECT_FAR_EDGE_X_OFFSET / ROAD_WORD_BYTES];
          points.top_left.y =
            left[ROAD_OBJECT_INNER_Y_OFFSET / ROAD_WORD_BYTES] -
            left[ROAD_SIDE_CHAIN_HEIGHT_OFFSET / ROAD_WORD_BYTES];
          points.top_right.x = right[
            ROAD_OBJECT_FAR_X_OFFSET / ROAD_WORD_BYTES];
          points.top_right.y = points.top_left.y;
          points.bottom_right.x = points.top_right.x;
          points.bottom_right.y = right[
            ROAD_OBJECT_OUTER_Y_OFFSET / ROAD_WORD_BYTES];
          points.bottom_left.x = points.top_left.x;
          points.bottom_left.y = left[
            ROAD_OBJECT_INNER_Y_OFFSET / ROAD_WORD_BYTES];
          packet = select_cached_side_cel(
            (RoadRenderSide *)left, ROAD_SIDE_RIGHT, &points);
          packet->ccb_PIXC = ROAD_OVERLAY_PIXC;
          packet = append_unmapped_cel(packet);
          map_fixed_road_quad_to_cel(packet, &points);
        }
    }
  else if(selector != ROAD_SELECTOR_FINAL)
    {
      return (int)state;
    }

  render_road_side_geometry((char *)left, ROAD_SIDE_RIGHT);
  return render_road_side_geometry((char *)right, ROAD_SIDE_LEFT);
}


int
render_procedural_roadside_samples(int     *object,
                                   RoadSide side)
{
  char *description;
  int *side_state;
  int *chain;
  int *chain_node;
  char *cel;
  char *previous_cel;
  CCB *draw;
  CCB *bridge_draw;
  char *entries[ROAD_SAMPLE_CAPACITY];
  unsigned char variants[ROAD_SAMPLE_CAPACITY];
  unsigned char style;
  RoadQuad points;
  int group;
  int max_variant;
  unsigned int variant_mask;
  int variant;
  int previous_variant;
  int scale;
  int step;
  int limit;
  int repeat;
  int count;
  int index;
  int seed;
  int position;
  int next_position;
  int side_end;
  int current_left;
  int current_span;
  int previous_span;
  int span_pixels;
  int node_step;
  int gap;
  int node_count;
  int result;

  description = (char *)object[
    ROAD_OBJECT_DESCRIPTION_OFFSET / ROAD_WORD_BYTES];
  style = UBYTE_AT(description, side + ROAD_DESCRIPTION_STYLE_OFFSET);
  side_state = (int *)((char *)object + side * ROAD_SIDE_BYTE_STRIDE);
  if(style == ROAD_STYLE_DISABLED &&
     WORD_AT(side_state, ROAD_SIDE_CHAIN_OFFSET) == 0)
    {
      return (int)object;
    }

  chain = (int *)WORD_AT(object, side * ROAD_WORD_BYTES);
  if(chain != 0)
    {
      chain = (int *)WORD_AT(chain, side * ROAD_WORD_BYTES);
    }

  group = style & ROAD_STYLE_GROUP_MASK;
  max_variant = style & ROAD_STYLE_VARIANT_MASK;
  variant_mask =
    (1U << (road_renderer_state.bit_length[max_variant] +
            ROAD_VARIANT_WIDTH_BIAS)) - 1;
  result = WORD_AT(
    description,
    side * ROAD_WORD_BYTES + ROAD_DESCRIPTION_LEFT_SPAN_OFFSET);
  if(result < 0)
    {
      result = -result;
    }
  scale = road_renderer_state
          .reciprocal_table[
    (object[
       ROAD_OBJECT_PROJECTION_DEPTH_OFFSET /
       ROAD_WORD_BYTES] +
     result) >> 1];
  step = scale << ROAD_FIXED_SHIFT;
  if(side == ROAD_SIDE_LEFT)
    {
      step = -step;
    }

  if(WORD_AT(
       (int *)object[ROAD_OBJECT_CONTEXT_OFFSET / ROAD_WORD_BYTES],
       ROAD_CONTEXT_RESOURCE_COUNTER_OFFSET) <=
     road_renderer_state.texture_resource_threshold)
    {
      limit = ROAD_SAMPLE_CAPACITY;
    }
  else
    {
      limit = ROAD_REDUCED_SAMPLE_CAPACITY;
    }

  side_end = WORD_AT(side_state, ROAD_SIDE_END_OFFSET);
  position = WORD_AT(side_state, ROAD_SIDE_POSITION_OFFSET);
  repeat = 0;
  cel = 0;
  variant = 0;
  count = 0;
  previous_variant = -1;
  seed = WORD_AT(
    (int *)object[
      ROAD_OBJECT_CONTEXT_OFFSET / ROAD_WORD_BYTES],
    ROAD_CONTEXT_RESOURCE_COUNTER_OFFSET) +
         side;

  while(count < limit &&
        is_within_track_side_bounds(position - step, side))
    {
      if(repeat <= 0)
        {
          result = advance_random_seed(&seed);
          repeat = pixel_repeat_counts[result & ROAD_RANDOM_REPEAT_MASK];
          if(count == 0)
            {
              if(repeat < ROAD_MINIMUM_REPEAT)
                {
                  repeat = ROAD_MINIMUM_REPEAT;
                }
              variant = max_variant;
            }
          else if(max_variant == 0)
            {
              variant = 0;
            }
          else
            {
              result = advance_random_seed(&seed);
              variant = (unsigned int)result & variant_mask;
              while(variant > max_variant)
                {
                  variant -= max_variant;
                }
              if(variant == previous_variant)
                {
                  variant--;
                  if(variant < 0)
                    {
                      variant = max_variant;
                    }
                }
            }

          cel = (char *)lookup_family_resource_entry_for_render(
            group | variant, 1);
          if(UBYTE_AT(cel, ROAD_CEL_HEIGHT_BYTE_OFFSET) <=
             ROAD_CEL_MINIMUM_HEIGHT)
            {
              variant--;
              if(variant < 0)
                {
                  variant = max_variant;
                }
              cel = (char *)lookup_family_resource_entry_for_render(
                group | variant, 1);
            }
        }

      previous_variant = variant;
      entries[count] = cel;
      variants[count] = (unsigned char)variant;
      position += step;
      repeat--;
      count++;
    }

  previous_variant = -1;
  previous_cel = 0;
  draw = 0;
  current_span = 0;
  previous_span = 0;
  next_position = position;
  position -= step;
  index = count - 1;
  if(index < 0)
    {
      return position;
    }

  while(index >= 0)
    {
      variant = variants[index];
      if(variant != previous_variant)
        {
          cel = entries[index];
          if(cel != 0)
            {
              current_span =
                UBYTE_AT(cel, ROAD_CEL_HEIGHT_BYTE_OFFSET) <<
                  ROAD_SPAN_SHIFT;
            }

          if(current_span < previous_span && chain != 0 &&
             previous_cel != 0)
            {
              points.top_right.x = next_position;
              points.top_right.y = current_left;
              points.bottom_right.x = next_position;
              points.bottom_right.y = side_end;
              chain_node = chain;
              node_count = 0;
              do
                {
                  if(chain_node == 0)
                    {
                      break;
                    }

                  points.bottom_left.y =
                    WORD_AT(chain_node,
                            side * ROAD_SIDE_BYTE_STRIDE +
                            ROAD_OBJECT_OUTER_Y_OFFSET);
                  node_step = chain_node[
                    ROAD_OBJECT_CHAIN_STEP_OFFSET / ROAD_WORD_BYTES];
                  points.top_left.y =
                    points.bottom_left.y - node_step * previous_span;
                  span_pixels =
                    ROAD_SAMPLE_INDEX_BIAS +
                    (index << ROAD_FIXED_SHIFT);
                  if(side == ROAD_SIDE_LEFT)
                    {
                      points.top_left.x =
                        chain_node[
                          ROAD_OBJECT_FAR_X_OFFSET / ROAD_WORD_BYTES] -
                        node_step * span_pixels;
                      gap = points.top_left.x - points.top_right.x;
                    }
                  else
                    {
                      points.top_left.x = node_step * span_pixels +
                                          chain_node[
                        ROAD_OBJECT_FAR_EDGE_X_OFFSET /
                        ROAD_WORD_BYTES];
                      gap = points.top_right.x - points.top_left.x;
                    }
                  points.bottom_left.x = points.top_left.x;

                  if(gap > 0 &&
                     is_within_track_side_bounds(points.top_left.x, side))
                    {
                      bridge_draw = (CCB *)select_cel_for_dimensions(
                        previous_cel, ROAD_FIXED_TO_INTEGER(gap),
                        ROAD_FIXED_TO_INTEGER(
                          node_step * previous_span));
                      if(bridge_draw == 0)
                        {
                          bridge_draw = display_runtime_globals.fallback_ccb;
                        }
                      bridge_draw->ccb_PIXC =
                        ROAD_PIXC_SECOND_SOURCE_SIGN_EXTEND |
                        ROAD_PIXC_SECOND_SOURCE_INVERT;
                      append_mapped_quad_cel(bridge_draw, (const DisplayQuad *)&points);
                    }

                  points.top_right = points.top_left;
                  points.bottom_right = points.bottom_left;
                  chain_node = (int *)WORD_AT(
                    chain_node, side * ROAD_WORD_BYTES);
                  node_count++;
                } while(node_count < ROAD_CHAIN_NODE_LIMIT);
            }

          if(cel != 0)
            {
              span_pixels = scale * current_span;
              current_left = side_end - span_pixels;
              if(step < 0)
                {
                  gap = ROAD_FIXED_TO_INTEGER(-step);
                }
              else
                {
                  gap = ROAD_FIXED_TO_INTEGER(step);
                }
              draw = (CCB *)select_cel_for_dimensions(
                cel, gap, ROAD_FIXED_TO_INTEGER(span_pixels));
              if(draw == 0)
                {
                  draw = display_runtime_globals.fallback_ccb;
                }
              draw->ccb_PIXC = ROAD_PIXC_DISABLE_OVERFLOW_WRAP;
              points.top_left.x = current_left;
              points.top_left.y = position;
              points.top_right.x = side_end;
              points.top_right.y = next_position;
              map_fixed_road_bounds_to_cel(
                draw, (const RoadCelBounds *)(const void *)&points);
            }
        }

      if(is_within_track_side_bounds(position, side))
        {
          append_shaded_cel(draw);
        }
      next_position = position;
      position -= step;
      if(draw != 0)
        {
          draw->ccb_XPos = position;
        }
      previous_variant = variant;
      previous_span = current_span;
      previous_cel = cel;
      index--;
    }

  return current_span;
}
