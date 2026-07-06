/* Projected road-node builder reconstructed from the 0x00016508 unit. */



#include "bss_early_data.h"
#include "bss_late_data.h"
#include "road_geometry.h"
#include "rw_semantic_data.h"

enum RoadProjectionConstant
{
  ROAD_PROJECTED_NODE_COUNT = 33
};

void
project_road_render_side(RoadSegmentLaneRuntime *lane,
                         RoadRenderSide         *road_side,
                         RoadProjectionCursor   *projection_cursor);

// Keep small operations inline so per-node values can stay in registers.
// The larger record and visibility passes remain out of line.

static
__inline
int
node_type(RoadSegmentRuntime *node)
{
  return node->lane_count;
}


static
__inline
int
mul_div256(int left,
                 int right)
{
  int value;

  value = left * right;
  if(value < 0)
    {
      value += ROAD_8_8_FRACTION_MASK;
    }
  return value >> ROAD_8_8_FRACTION_BITS;
}


static
__inline
void
copy_transform(RoadProjectionCursor *dest,
                     RoadProjectionCursor *source)
{
  *dest = *source;
}


static
__inline
void
set_next_sides(RoadRenderSide *side,
                     RoadRenderSide *next)
{
  side->next_sides[0] = next;
  side->next_sides[1] = next;
}


static
__inline
void
clear_next_sides(RoadRenderSide *side)
{
  side->next_sides[0] = 0;
  side->next_sides[1] = 0;
}


static
__inline
unsigned int
texture_cache_index(int depth_)
{
  unsigned int magnitude = (unsigned int)depth_;
  unsigned int folded;
  unsigned int quotient;
  unsigned int remainder;

  if(depth_ < 0)
    magnitude = (0U - magnitude);

  // Depth is a signed track position shifted right eight bits. Its magnitude
  // is at most 0x800000. Since 1024 == 1 (mod 33), folding preserves the
  // remainder and leaves at most 9214. Over that range this reciprocal is exact.
  folded = ((magnitude & 1023U) + (magnitude >> 10));
  quotient = ((folded * 1986U) >> 16);
  remainder = (folded - (quotient * ROAD_PROJECTED_NODE_COUNT));
  return (depth_ < 0) ? (0U - remainder) : remainder;
}

static
__inline
void
assign_texture_cache(RoadRenderSide *side_,
                     unsigned int cache_index_)
{
  side_->texture_cache = &gRoadTextureCachePool[cache_index_];
  side_->texture_binding_state = &gRoadTextureBindingStates[cache_index_];
}


__inline
static
void
prepare_single_record(RoadRenderNode       *record,
                            RoadSegmentRuntime   *node,
                            RoadProjectionCursor *cursor)
{
  RoadRenderSide *object;
  RoadRenderSide *target;

  record->segment = node;
  record->node_type = ROAD_RENDER_NODE_SINGLE;
  record->depth = node->track_position >> ROAD_8_8_FRACTION_BITS;
  object = record->left_side;
  target = (record + 1)->left_side;
  set_next_sides(object, target);
  assign_texture_cache(object, texture_cache_index(record->depth));
  project_road_render_side(node->selected_lanes[0], object, cursor);
  record->sort_depth = cursor->horizon_y;
}


__inline
static
void
prepare_dual_record(RoadRenderNode       *record,
                          RoadSegmentRuntime   *node,
                          int                   type,
                          RoadProjectionCursor *left_cursor,
                          RoadProjectionCursor *right_cursor)
{
  RoadRenderSide *left;
  RoadRenderSide *right;
  unsigned int cache_index;

  record->segment = node;
  record->node_type = type;
  record->depth = node->track_position >> ROAD_8_8_FRACTION_BITS;
  left = record->left_side;
  right = record->right_side;
  set_next_sides(left, (record + 1)->left_side);
  set_next_sides(right, (record + 1)->right_side);
  cache_index = texture_cache_index(record->depth);
  assign_texture_cache(left, cache_index);
  assign_texture_cache(right, (cache_index + ROAD_RENDER_NODE_COUNT));
  project_road_render_side(
    node->selected_lanes[0], left, left_cursor);
  project_road_render_side(
    node->selected_lanes[1], right, right_cursor);
  record->sort_depth = left_cursor->horizon_y > right_cursor->horizon_y
                    ? left_cursor->horizon_y
                    : right_cursor->horizon_y;
}


static
void
update_visibility(RoadRenderNode *records,
                        int             count)
{
  RoadRenderNode *record;
  RoadSegmentRuntime *node;
  RoadSegmentRuntime *first_node;
  RoadRenderSide *target_side;
  int previous;
  int value;
  int index;
  int shift;
  int selected_side;
  int behind;
  int first_model_type_zero;
  unsigned int mask;
  unsigned char visible;

  previous = ROAD_SCREEN_ROAD_BOTTOM;
  records[0].sort_depth = previous;
  record = records;
  index = 0;
  first_node = records[0].segment;

  while(index < count)
    {
      if(record->node_type == ROAD_RENDER_NODE_DUAL)
        {
          node = record->segment;
          first_model_type_zero =
            first_node->selected_lanes[0]->geometry_mode == 0;
          behind = gRoadRenderDepths[index] < node->left_step;
          shift = node->transition_type & ROAD_8_8_FRACTION_MASK;
          mask = shift < ROAD_INTEGER_BIT_COUNT ? (1U << shift) : 0;
          selected_side =
            *(int *)(int)&(road_renderer_state.road_side_selector) >>
            ROAD_PACKED_HIGH_BYTE_SHIFT;
          target_side = selected_side != ROAD_SIDE_LEFT
                              ? record->left_side
                              : record->right_side;

          if((mask & ROAD_RENDER_TRANSITION_VISIBILITY_MASK) == 0)
            {
              if(records[0].node_type == ROAD_RENDER_NODE_DUAL &&
                 (behind ||
                  (first_model_type_zero &&
                   first_node->transition_amount < 1)))
                {
                  target_side->active = 0;
                }
            }
          else if(behind ||
                  (first_model_type_zero &&
                   node->transition_amount < 0))
            {
              target_side->active = 0;
            }
        }

      value = record->sort_depth;
      if(previous <= value)
        {
          record->sort_depth = previous;
          value = previous;
        }
      if(value < (record + 1)->left_side->horizon_y ||
         record->left_side->horizon_y <
         (record + 1)->left_side->horizon_y)
        {
          visible = 0;
        }
      else
        {
          visible = 1;
        }
      record->visible = visible;

      previous = value;
      record++;
      index++;
    }

  *(int *)(int)&(road_renderer_state.road_horizon) = previous;
}


void
project_road_render_nodes(unsigned int    packed,
                          RoadRenderNode *records,
                          int            *origin,
                          int             side)
{
  RoadProjectionCursor cursors[ROAD_SIDE_COUNT];
  RoadProjectionCursor *left_cursor;
  RoadProjectionCursor *right_cursor;
  RoadSegmentRuntime *node;
  RoadRenderNode *record;
  RoadSegmentLaneRuntime *model;
  RoadRenderSide *target;
  RoadRenderSide *other;
  int inverse_side;
  int remaining;
  int fraction;
  int blend;
  int type;
  int value;
  int *scale_table;
  unsigned int cache_index;

  left_cursor = &cursors[0];
  right_cursor = &cursors[1];
  inverse_side = 1 - side;
  node = gRoadSegmentEffectWorkspace.segment_index
         .by_track_index[((int)packed >> ROAD_8_8_FRACTION_BITS) &
                         ROAD_TRACK_INDEX_MASK];
  type = node_type(node);
  fraction = packed & ROAD_8_8_FRACTION_MASK;
  *(unsigned char *)(int)&(road_surface_render_globals) = 0;
  remaining = (ROAD_PROJECTED_NODE_COUNT - 1);
  blend = ROAD_PROJECTION_DEPTH_STEP - fraction;
  left_cursor->projection_depth = blend;
  model = node->selected_lanes[0];

  if(type == ROAD_RENDER_NODE_SINGLE)
    {
      *(unsigned char *)(int)&(road_surface_render_globals) =
        (model->geometry_mode == ROAD_GEOMETRY_MODE_CURVED);
      left_cursor->world_x_step =
        mul_div256(model->path_curvature_step, blend);
      left_cursor->world_x = 0;
      left_cursor->world_y_step = model->path_elevation_step;
      left_cursor->projection_origin_x = origin[0];
      left_cursor->projection_origin_y = origin[1];
      left_cursor->left_attachment = 0;
      left_cursor->right_attachment = 0;
      left_cursor->world_y =
        -((left_cursor->world_y_step * fraction) >>
          ROAD_8_8_FRACTION_BITS);

      record = records;
      while(remaining >= 0 &&
            node_type(node) == ROAD_RENDER_NODE_SINGLE)
        {
          prepare_single_record(record, node, left_cursor);
          node = node->next;
          model = node->selected_lanes[0];
          if(node_type(node) == ROAD_RENDER_NODE_SINGLE)
            {
              left_cursor->world_x_step += model->path_curvature_step;
              left_cursor->world_y_step = model->path_elevation_step;
            }
          remaining--;
          record++;
        }

      if(remaining < 0)
        {
          clear_next_sides((record - 1)->left_side);
        }
      else
        {
          if(node->transition_type == ROAD_RENDER_NODE_DUAL)
            {
              copy_transform(right_cursor, left_cursor);
              left_cursor->right_attachment = 0;
              right_cursor->left_attachment = 0;
              (record - 1)->left_side->next_sides[1] =
                record->right_side;
            }
          else
            {
              value = track_runtime_globals.selector;
              *(unsigned char *)(int)&(road_renderer_state.road_side_selector) =
                (unsigned char)value;
              copy_transform(right_cursor, left_cursor);
              if(value == 0)
                {
                  right_cursor->world_x_step =
                    left_cursor->world_x_step + node->right_step;
                  right_cursor->world_x =
                    left_cursor->world_x + node->left_step;
                }
              else
                {
                  left_cursor->world_x_step =
                    right_cursor->world_x_step - node->right_step;
                  left_cursor->world_x =
                    right_cursor->world_x - node->left_step;
                }
              cursors[inverse_side].left_attachment = 0;
              cursors[inverse_side].right_attachment = 0;
              target = value == 0 ? record->left_side : record->right_side;
              set_next_sides((record - 1)->left_side, target);
            }

          while(remaining >= 0)
            {
              model = node->selected_lanes[0];
              left_cursor->world_x_step += model->path_curvature_step;
              left_cursor->world_y_step = model->path_elevation_step;
              model = node->selected_lanes[1];
              right_cursor->world_x_step += model->path_curvature_step;
              right_cursor->world_y_step = model->path_elevation_step;

              prepare_dual_record(record, node, node_type(node),
                                 left_cursor, right_cursor);
              node = node->next;
              record++;
              remaining--;
            }
          clear_next_sides((record - 1)->left_side);
          clear_next_sides((record - 1)->right_side);
        }
    }
  else
    {
      model = node->selected_lanes[side];
      cursors[side].world_x_step =
        mul_div256(model->path_curvature_step, blend);
      cursors[side].world_x = 0;
      if(side == 0)
        {
          right_cursor->world_x_step =
            left_cursor->world_x_step + node->right_step;
          right_cursor->world_x = node->left_step;
        }
      else
        {
          left_cursor->world_x_step =
            right_cursor->world_x_step - node->right_step;
          left_cursor->world_x = -node->left_step;
        }
      left_cursor->projection_origin_x = origin[0];
      left_cursor->projection_origin_y = origin[1];
      left_cursor->left_attachment = 0;
      left_cursor->right_attachment = 0;
      right_cursor->left_attachment = 0;
      right_cursor->right_attachment = 0;
      model = node->selected_lanes[0];
      left_cursor->world_y_step = model->path_elevation_step;
      model = node->selected_lanes[1];
      right_cursor->world_y_step = model->path_elevation_step;
      left_cursor->world_y =
        -((left_cursor->world_y_step * fraction) >>
          ROAD_8_8_FRACTION_BITS);
      right_cursor->world_y =
        -((right_cursor->world_y_step * fraction) >>
          ROAD_8_8_FRACTION_BITS);
      left_cursor->projection_depth = blend;
      right_cursor->projection_depth = blend;
      right_cursor->projection_origin_x = left_cursor->projection_origin_x;
      right_cursor->projection_origin_y = left_cursor->projection_origin_y;

      record = records;
      while(remaining >= 0 &&
            node_type(node) == ROAD_RENDER_NODE_DUAL)
        {
          prepare_dual_record(record, node, ROAD_RENDER_NODE_DUAL,
                             left_cursor, right_cursor);
          node = node->next;
          model = node->selected_lanes[0];
          left_cursor->world_x_step += model->path_curvature_step;
          left_cursor->world_y_step = model->path_elevation_step;
          model = node->selected_lanes[1];
          right_cursor->world_x_step += model->path_curvature_step;
          right_cursor->world_y_step = model->path_elevation_step;
          remaining--;
          record++;
        }

      if(remaining < 0)
        {
          clear_next_sides((record - 1)->left_side);
          clear_next_sides((record - 1)->right_side);
        }
      else
        {
          if(node->transition_type == ROAD_RENDER_NODE_SINGLE)
            {
              left_cursor->left_attachment = (record - 1)->left_side;
              left_cursor->right_attachment = (record - 1)->right_side;
              target = record->left_side;
              set_next_sides(left_cursor->left_attachment, target);
              set_next_sides(left_cursor->right_attachment, target);
              scale_table = *(int **)(int)&(road_renderer_state.reciprocal_table);
              left_cursor->left_attachment
              ->connection_geometry[ROAD_CONNECTION_RIGHT_X] =
                scale_table[left_cursor->projection_depth >> 1] *
                (left_cursor->left_attachment
                 ->lane->connected_right_bound +
                 left_cursor->world_x - origin[0]) +
                ROAD_PROJECTION_SCREEN_X_CENTER;
              left_cursor->right_attachment
              ->connection_geometry[ROAD_CONNECTION_LEFT_X] =
                scale_table[left_cursor->projection_depth >> 1] *
                (left_cursor->right_attachment
                 ->lane->connected_left_bound +
                 left_cursor->world_x - origin[0]) +
                ROAD_PROJECTION_SCREEN_X_CENTER;
            }
          else
            {
              if(side == 1)
                {
                  left_cursor->world_y_step = right_cursor->world_y_step;
                  left_cursor->world_y = right_cursor->world_y;
                  left_cursor->world_x_step = right_cursor->world_x_step;
                  left_cursor->world_x = right_cursor->world_x;
                  left_cursor->projection_depth =
                    right_cursor->projection_depth;
                  left_cursor->projection_origin_x =
                    right_cursor->projection_origin_x;
                  left_cursor->projection_origin_y =
                    right_cursor->projection_origin_y;
                  left_cursor->horizon_y = right_cursor->horizon_y;
                }
              left_cursor->left_attachment = side == 0
                    ? (record - 1)->left_side
                    : (record - 1)->right_side;
              target = record->left_side;
              set_next_sides(left_cursor->left_attachment, target);
              other = inverse_side == 0
                            ? (record - 1)->left_side
                            : (record - 1)->right_side;
              set_next_sides(other, other);
              other->connection_geometry[ROAD_CONNECTION_CENTER_Y] =
                other->connection_geometry[
                  ROAD_CONNECTION_SOURCE_CENTER_Y];
              other->connection_geometry[ROAD_CONNECTION_LEFT_X] =
                other->connection_geometry[ROAD_CONNECTION_SOURCE_LEFT_X];
              other->connection_geometry[ROAD_CONNECTION_RIGHT_X] =
                other->connection_geometry[
                  ROAD_CONNECTION_SOURCE_RIGHT_X];
              other->connection_geometry[ROAD_CONNECTION_SECONDARY_Y] =
                other->connection_geometry[
                  ROAD_CONNECTION_SOURCE_SECONDARY_Y];
              left_cursor->right_attachment = left_cursor->left_attachment;
            }

          while(remaining >= 0 &&
                node_type(node) == ROAD_RENDER_NODE_SINGLE)
            {
              record->segment = node;
              record->node_type = ROAD_RENDER_NODE_SINGLE;
              record->depth =
                node->track_position >> ROAD_8_8_FRACTION_BITS;
              target = (record + 1)->left_side;
              set_next_sides(record->left_side, target);
              cache_index = texture_cache_index(record->depth);
              assign_texture_cache(record->left_side, cache_index);
              assign_texture_cache(record->right_side,
                                   (cache_index + ROAD_RENDER_NODE_COUNT));
              project_road_render_side(
                node->selected_lanes[0], record->left_side,
                left_cursor);
              record->sort_depth = left_cursor->horizon_y;
              node = node->next;
              model = node->selected_lanes[0];
              if(node_type(node) == ROAD_RENDER_NODE_SINGLE)
                {
                  left_cursor->world_x_step += model->path_curvature_step;
                  left_cursor->world_y_step = model->path_elevation_step;
                }
              remaining--;
              record++;
            }
          clear_next_sides((record - 1)->left_side);
        }
    }

  update_visibility(records, ROAD_PROJECTED_NODE_COUNT);
}
