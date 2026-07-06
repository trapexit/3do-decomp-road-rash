/* Projected road-side builder reconstructed from the 0x00016F30 unit. */



#include "bss_early_data.h"
#include "bss_late_data.h"
#include "road_geometry.h"
#include "road_texture_cache_residency.h"
#include "roadside_object_rendering.h"
#include "rw_semantic_data.h"

void
populate_textured_road_surface_cache(RoadSegmentLaneRuntime *lane,
                                     RoadRenderSide         *road_side);
void
populate_curved_road_resource_cache(RoadRenderSide *road_side);

static __inline
int
div2_toward_zero(int value)
{
  if(value < 0)
    {
      value += 1;
    }
  return value >> 1;
}


static __inline
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


static __inline
int
scale_value(int value)
{
  return ((int *)*(int *)(int)&(road_renderer_state.reciprocal_table))[value >> 1];
}


static __inline
void
refresh_texture(RoadSegmentLaneRuntime *model,
                      RoadRenderSide         *object)
{
  int *dest;
  int resource;
  int type;

  dest = (int *)object->texture_cache;
  resource = object->owner_node->depth;
  /*
   * The oracle retains a projected side's resolved family pointers until
   * that depth slot is recycled.  This is required while adjacent family
   * intervals overlap the visible road: the shared family group may
   * already contain the next interval even though this side still needs
   * CELs resolved from the preceding interval.
   */
  if(resource == dest[0] || dest[0] < 0)
    {
      return;
    }

  *object->texture_cache = gRoadTextureCacheTemplate;
  reset_road_texture_binding_state(object->texture_binding_state);
  dest[0] = resource;

  type = model->geometry_mode;
  if(type == ROAD_GEOMETRY_MODE_TEXTURED)
    {
      populate_textured_road_surface_cache(model, object);
    }
  else
    {
      if(type == ROAD_GEOMETRY_MODE_CURVED)
        {
          populate_curved_road_resource_cache(object);
        }
      if(type == ROAD_GEOMETRY_MODE_EDGE ||
         type == ROAD_GEOMETRY_MODE_CURVED)
        {
          populate_raised_road_surface_resource_cache(object);
        }
    }
  if(type == ROAD_GEOMETRY_MODE_TEXTURED ||
     type == ROAD_GEOMETRY_MODE_EDGE ||
     type == ROAD_GEOMETRY_MODE_CURVED ||
     type == ROAD_GEOMETRY_MODE_PROFILE)
    {
      record_road_surface_cache_residency(object->texture_binding_state, model);
    }
}


static __inline
void
update_attachment(RoadSegmentLaneRuntime *model,
                        RoadRenderSide         *object,
                        RoadProjectionCursor   *cursor,
                        int                     scale,
                        int                     right)
{
  RoadRenderSide *attachment;
  RoadSegmentLaneRuntime *attachment_model;
  int weight;
  int x;
  int y;
  int target_x;
  int target_y;

  attachment = right ? cursor->right_attachment
                       : cursor->left_attachment;
  if(attachment == 0)
    {
      return;
    }

  attachment->connection_geometry[ROAD_CONNECTION_CENTER_Y] =
    object->connection_geometry[ROAD_CONNECTION_SOURCE_CENTER_Y];
  if(right)
    {
      attachment->connection_geometry[ROAD_CONNECTION_RIGHT_X] =
        object->connection_geometry[ROAD_CONNECTION_SOURCE_RIGHT_X];
    }
  else
    {
      attachment->connection_geometry[ROAD_CONNECTION_LEFT_X] =
        object->connection_geometry[ROAD_CONNECTION_SOURCE_LEFT_X];
    }

  attachment_model = attachment->lane;
  if((right ? attachment_model->connected_right_bound
               : attachment_model->connected_left_bound) !=
     (right ? model->connected_right_bound : model->connected_left_bound))
    {
      attachment->connection_geometry[
        right ? ROAD_CONNECTION_RIGHT_X : ROAD_CONNECTION_LEFT_X] =
        scale * (((right ? attachment_model->connected_right_bound
                             : attachment_model->connected_left_bound) +
                  object->world_x) - cursor->projection_origin_x) +
        ROAD_PROJECTION_SCREEN_X_CENTER;
    }

  if(attachment_model->geometry_mode != ROAD_GEOMETRY_MODE_TEXTURED ||
     (attachment_model->surface_flags & ROAD_SURFACE_FLAG_MARGIN_MASK) ==
     0)
    {
      return;
    }

  weight = attachment_model->edge_margins[right];
  if((attachment_model->surface_flags &
      ROAD_SURFACE_FLAG_REVERSED_MARGIN) != 0)
    {
      weight = ROAD_PROJECTION_DEPTH_STEP - weight;
    }
  x = (right ? attachment_model->shoulder_right
               : attachment_model->shoulder_left) + attachment->world_x;
  y = attachment_model->edge_depth_offsets[right] + attachment->world_y;
  target_x = (right ? model->shoulder_right : model->shoulder_left) +
             object->world_x;
  target_y = model->edge_depth_offsets[right] + object->world_y;

  project_road_world_point(
    x + ((weight * (target_x - x)) >> ROAD_8_8_FRACTION_BITS),
    y + ((weight * (target_y - y)) >> ROAD_8_8_FRACTION_BITS),
    attachment->projection_depth + weight,
    (RoadPoint *)(void *)&attachment->attachment_points[right]);
}


void
project_road_render_side(RoadSegmentLaneRuntime *lane,
                         RoadRenderSide         *road_side,
                         RoadProjectionCursor   *projection_cursor)
{
  RoadSegmentLaneRuntime *model;
  RoadRenderSide *object;
  RoadProjectionCursor *cursor;
  int scale;
  int horizontal;
  int vertical;
  int width;
  int height;
  int outer_width;
  int outer_height;
  int flags;
  int type;
  int value;
  int value2;
  int index;
  int positive;
  int *shape_table;

  model = lane;
  object = road_side;
  cursor = projection_cursor;
  object->active = 1;
  object->lane = model;
  refresh_texture(model, object);

  object->world_x = cursor->world_x;
  object->world_y = cursor->world_y;
  object->projection_depth = cursor->projection_depth;
  scale = scale_value(cursor->projection_depth);
  object->projection_scale = scale;

  horizontal = cursor->projection_origin_y - cursor->world_y;
  value = scale * horizontal + ROAD_PROJECTION_SCREEN_Y_ORIGIN;
  object->horizon_y = value;
  vertical = cursor->world_x - cursor->projection_origin_x;
  object->projection_origin_x =
    scale * vertical + ROAD_PROJECTION_SCREEN_X_CENTER;
  object->projected_lane_unit = scale * ROAD_LANE_SAMPLE_SPACING;
  object->lane_edges[ROAD_LANE_EDGE_LEFT].x =
    scale * (model->road_left + vertical) +
    ROAD_PROJECTION_SCREEN_X_CENTER;
  object->lane_edges[ROAD_LANE_EDGE_RIGHT].x =
    scale * (model->road_right + vertical) +
    ROAD_PROJECTION_SCREEN_X_CENTER;
  object->lane_edges[ROAD_SHOULDER_EDGE_LEFT].x =
    scale * (model->shoulder_left + vertical) +
    ROAD_PROJECTION_SCREEN_X_CENTER;
  object->lane_edges[ROAD_SHOULDER_EDGE_RIGHT].x =
    scale * (model->shoulder_right + vertical) +
    ROAD_PROJECTION_SCREEN_X_CENTER;
  object->lane_edges[ROAD_SHOULDER_EDGE_RIGHT].y = value;
  object->lane_edges[ROAD_SHOULDER_EDGE_LEFT].y = value;
  object->connection_geometry[ROAD_CONNECTION_SOURCE_CENTER_Y] = value;
  object->connection_geometry[ROAD_CONNECTION_SOURCE_LEFT_X] =
    scale * (vertical + model->connected_left_bound) +
    ROAD_PROJECTION_SCREEN_X_CENTER;
  object->connection_geometry[ROAD_CONNECTION_SOURCE_RIGHT_X] =
    scale * (vertical + model->connected_right_bound) +
    ROAD_PROJECTION_SCREEN_X_CENTER;

  type = model->geometry_mode;
  if(type == ROAD_GEOMETRY_MODE_TEXTURED)
    {
      object->lane_edges[ROAD_LANE_EDGE_LEFT].y =
        object->horizon_y - scale * model->edge_depth_offsets[0];
      object->lane_edges[ROAD_LANE_EDGE_RIGHT].y =
        object->horizon_y - scale * model->edge_depth_offsets[1];
      width = model->surface_profile[1].y;
      height = model->surface_profile[0].y;
      flags = model->surface_flags;

      if(flags == 0)
        {
          if(*(int *)(int)&(road_renderer_state.texture_resource_threshold) == 0)
            {
              *(int *)(int)&(road_renderer_state.texture_resource_threshold) = 1;
            }
          object->mode_geometry[ROAD_MODE_GEOMETRY_LEFT_NEAR].y =
            scale * height;
          object->mode_geometry[ROAD_MODE_GEOMETRY_RIGHT_NEAR].y =
            scale * width;
          object->mode_geometry[ROAD_MODE_GEOMETRY_RIGHT_NEAR].x =
            div2_toward_zero(scale * width);
          object->mode_geometry[ROAD_MODE_GEOMETRY_LEFT_NEAR].x =
            -div2_toward_zero(scale * height);
          value = scale_value(
            object->projection_depth + ROAD_PROJECTION_DEPTH_STEP);
          outer_width = value * width;
          outer_height = value * height;
        }
      else if((flags & ROAD_SURFACE_FLAG_DEPTH_ADJUSTED) == 0)
        {
          if((flags & ROAD_SURFACE_FLAG_REVERSED_MARGIN) != 0)
            {
              object->mode_geometry[ROAD_MODE_GEOMETRY_LEFT_NEAR].y =
                scale * height;
              object->mode_geometry[ROAD_MODE_GEOMETRY_RIGHT_NEAR].y =
                scale * width;
              object->mode_geometry[ROAD_MODE_GEOMETRY_LEFT_NEAR].x =
                -div2_toward_zero(scale * height);
              object->mode_geometry[ROAD_MODE_GEOMETRY_RIGHT_NEAR].x =
                div2_toward_zero(scale * width);
              value = object->projection_depth +
                      ROAD_PROJECTION_DEPTH_STEP;
              outer_height = height * scale_value(
                value -
                model->edge_margins[ROAD_TRACK_FIRST_CHANNEL]);
              outer_width = width * scale_value(
                value -
                model->edge_margins[ROAD_TRACK_SECOND_CHANNEL]);
            }
        }
      else
        {
          if(*(int *)(int)&(road_renderer_state.texture_resource_threshold) == 0)
            {
              *(int *)(int)&(road_renderer_state.texture_resource_threshold) =
                object->owner_node->depth;
            }
          value = object->projection_depth;
          outer_height = height * scale_value(
            model->edge_margins[ROAD_TRACK_FIRST_CHANNEL] + value);
          outer_width = width * scale_value(
            model->edge_margins[ROAD_TRACK_SECOND_CHANNEL] + value);
          object->mode_geometry[ROAD_MODE_GEOMETRY_LEFT_NEAR].y =
            outer_height;
          object->mode_geometry[ROAD_MODE_GEOMETRY_RIGHT_NEAR].y =
            outer_width;
          object->mode_geometry[ROAD_MODE_GEOMETRY_LEFT_NEAR].x =
            -div2_toward_zero(outer_height);
          object->mode_geometry[ROAD_MODE_GEOMETRY_RIGHT_NEAR].x =
            div2_toward_zero(outer_width);
          value2 = scale_value(
            value + ROAD_PROJECTION_DEPTH_STEP);
          outer_width = value2 * width;
          outer_height = value2 * height;
        }

      /*
       * The oracle stores incidental r8/sb values for flag-only transition
       * records that have neither margin mode. Those records are handled by
       * the blend renderer (or emit no side geometry), which never reads
       * mode_geometry. Avoid representing those dead register stores as
       * uninitialized C reads.
       */
      if(flags == 0 ||
         (flags & ROAD_SURFACE_FLAG_MARGIN_MASK) != 0)
        {
          object->mode_geometry[ROAD_MODE_GEOMETRY_LEFT_FAR].y =
            outer_height;
          object->mode_geometry[ROAD_MODE_GEOMETRY_RIGHT_FAR].y =
            outer_width;
          object->mode_geometry[ROAD_MODE_GEOMETRY_LEFT_FAR].x =
            -div2_toward_zero(outer_height);
          object->mode_geometry[ROAD_MODE_GEOMETRY_RIGHT_FAR].x =
            div2_toward_zero(outer_width);
        }
      object->transition_geometry[0] =
        model->surface_profile[ROAD_SHOULDER_EDGE_LEFT].y;
      object->transition_geometry[ROAD_MODE_GEOMETRY_LEFT_FAR] =
        model->surface_profile[ROAD_MODE_GEOMETRY_RIGHT_FAR].y;
      cursor->horizon_y = object->horizon_y;
      goto finish;
    }

  if(type == ROAD_GEOMETRY_MODE_PROFILE)
    {
      object->lane_edges[ROAD_LANE_EDGE_LEFT].y = object->horizon_y;
      object->lane_edges[ROAD_LANE_EDGE_RIGHT].y = object->horizon_y;
      cursor->horizon_y = project_road_surface_profile(object);
      if(*(int *)(int)&(road_renderer_state.texture_resource_threshold) < 0)
        {
          *(int *)(int)&(road_renderer_state.texture_resource_threshold) = 0;
        }
      goto finish;
    }

  if(type > ROAD_GEOMETRY_MODE_MAX)
    {
      goto finish;
    }

  value2 = object->projection_origin_x;
  object->mode_geometry[ROAD_MODE_GEOMETRY_LEFT_NEAR].x =
    scale * model->edge_shapes[0].inner_offset + value2;
  value = object->horizon_y;
  object->mode_geometry[ROAD_MODE_GEOMETRY_LEFT_NEAR].y = value;
  object->mode_geometry[ROAD_MODE_GEOMETRY_LEFT_FAR].x =
    scale * model->edge_shapes[0].outer_offset + value2;
  object->mode_geometry[ROAD_MODE_GEOMETRY_LEFT_FAR].y =
    value - scale * model->edge_shapes[0].height;
  object->mode_geometry[ROAD_MODE_GEOMETRY_RIGHT_NEAR].x =
    scale * model->edge_shapes[1].inner_offset + value2;
  object->mode_geometry[ROAD_MODE_GEOMETRY_RIGHT_NEAR].y = value;
  object->mode_geometry[ROAD_MODE_GEOMETRY_RIGHT_FAR].x =
    scale * model->edge_shapes[1].outer_offset + value2;
  object->mode_geometry[ROAD_MODE_GEOMETRY_RIGHT_FAR].y =
    value - scale * model->edge_shapes[1].height;

  if(model->edge_shapes[1].height < 0)
    {
      cursor->horizon_y =
        object->mode_geometry[ROAD_MODE_GEOMETRY_LEFT_FAR].y >
        object->mode_geometry[ROAD_MODE_GEOMETRY_RIGHT_FAR].y
                ? object->mode_geometry[ROAD_MODE_GEOMETRY_LEFT_FAR].y
                : object->mode_geometry[ROAD_MODE_GEOMETRY_RIGHT_FAR].y;
    }
  else if(model->edge_shapes[0].height < 0)
    {
      cursor->horizon_y =
        object->mode_geometry[ROAD_MODE_GEOMETRY_LEFT_FAR].y;
    }
  else
    {
      cursor->horizon_y = value;
    }

  if(object->lane->geometry_mode == ROAD_GEOMETRY_MODE_CURVED)
    {
      positive = object->lane->edge_shapes[1].height > 0;
      object->lane_edges[ROAD_LANE_EDGE_LEFT].y =
        value - scale * model->edge_depth_offsets[0];
      object->lane_edges[ROAD_LANE_EDGE_RIGHT].y =
        value - scale * model->edge_depth_offsets[1];
      object->curve_profile.points[0].x =
        object->lane_edges[ROAD_SHOULDER_EDGE_LEFT].x;
      object->curve_profile.points[0].y =
        object->lane_edges[ROAD_LANE_EDGE_LEFT].y;
      object->curve_profile.points[1].x =
        object->lane_edges[ROAD_SHOULDER_EDGE_LEFT].x;
      object->curve_profile.points[ROAD_PROFILE_MAX_POINT_COUNT].y =
        object->lane_edges[ROAD_LANE_EDGE_RIGHT].y;
      object->curve_profile.points[ROAD_PROFILE_MAX_POINT_COUNT].x =
        object->lane_edges[ROAD_SHOULDER_EDGE_RIGHT].x;
      object->curve_profile.points[ROAD_PROFILE_BASE_POINT_COUNT].x =
        object->lane_edges[ROAD_SHOULDER_EDGE_RIGHT].x;

      shape_table = (int *)(int)&(road_surface_render_globals.configuration[0]);
      index = ROAD_SHOULDER_EDGE_LEFT;
      while(index < ROAD_PROFILE_BASE_POINT_COUNT)
        {
          value2 = mul_div256(
            model->road_right - model->road_left,
            shape_table[index * ROAD_SIDE_COUNT]);
          object->curve_profile.points[index].x =
            scale * value2 + object->lane_edges[ROAD_LANE_EDGE_LEFT].x;
          index++;
        }
      index = 1;
      while(index < ROAD_PROFILE_MAX_POINT_COUNT)
        {
          object->curve_profile.points[index].y =
            value - scale * shape_table[index * ROAD_SIDE_COUNT + 1];
          index++;
        }
      value2 = object->lane_edges[
        ROAD_SHOULDER_EDGE_LEFT + (1 - positive)].x;
      value = div2_toward_zero(object->projected_lane_unit);
      if(positive)
        {
          value = -value;
        }
      object->curve_control.x = value2 + value;
      object->curve_control.y =
        object->curve_profile.points[
          ROAD_MODE_GEOMETRY_RIGHT_FAR - positive].y;
    }
  else
    {
      object->lane_edges[ROAD_LANE_EDGE_LEFT].y = object->horizon_y;
      object->lane_edges[ROAD_LANE_EDGE_RIGHT].y = object->horizon_y;
    }
  object->projected_path_elevation = scale * model->path_elevation;

finish:
  cursor->projection_depth += ROAD_PROJECTION_DEPTH_STEP;
  cursor->world_x += cursor->world_x_step;
  cursor->world_y += cursor->world_y_step;
  update_attachment(model, object, cursor, scale, 0);
  update_attachment(model, object, cursor, scale, 1);
  cursor->left_attachment = object;
  cursor->right_attachment = object;
}
