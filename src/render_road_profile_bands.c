#include "cel_dimension_resource.h"
#include "display_cel_runtime.h"
#include "road_geometry.h"
#include "road_surface_cel_selection.h"
#include "rw_semantic_data.h"

enum RoadProfilePointIndex
{
  ROAD_PROFILE_SHOULDER_POINT = 0,
  ROAD_PROFILE_NEAR_POINT = 1,
  ROAD_PROFILE_FAR_POINT = 2,
  ROAD_PROFILE_OUTER_POINT = 3
};

enum RoadProfileNeighborMode
{
  ROAD_PROFILE_NEIGHBOR_FLAT = 0,
  ROAD_PROFILE_NEIGHBOR_TRUNCATED = 1
};

enum RoadProfileResourceConstant
{
  ROAD_PROFILE_SURFACE_RESOURCE_GROUP = 1,
  ROAD_REPEATED_EDGE_RESOURCE_GROUP = 5,
  ROAD_REPEATED_EDGE_RESOURCE_VARIANT = 2,
  ROAD_REPEATED_JOIN_SOURCE_ROW = 0
};

int
lookup_family_resource_child_for_render(int inventory,
                                        int item_type,
                                        int slot);
CCB *
select_profile_band_subdivision_cel(RoadRenderSide *road_side,
                                    RoadSide        side,
                                    const RoadQuad *quad,
                                    int             strip_index);
int
select_repeated_roadside_strip_count(RoadRenderSide *road_side);
void
subdivide_four_point_road_strip(RoadStripDrawContext *context);

__inline
static
void
road_profile_point(RoadPoint            *point,
                   const RoadRenderSide *road_side,
                   RoadSide              side,
                   int                   index)
{
  if(index == ROAD_PROFILE_SHOULDER_POINT)
    {
      point->x = road_side->lane_edges[ROAD_SHOULDER_EDGE_LEFT + side].x;
      point->y = road_side->lane_edges[ROAD_SHOULDER_EDGE_LEFT + side].y;
    }
  else if(index == ROAD_PROFILE_NEAR_POINT)
    {
      point->x = road_side->mode_geometry[ROAD_MODE_GEOMETRY_LEFT_NEAR +
                                          side].x;
      point->y = road_side->mode_geometry[ROAD_MODE_GEOMETRY_LEFT_NEAR +
                                          side].y;
    }
  else if(index == ROAD_PROFILE_FAR_POINT)
    {
      point->x = road_side->mode_geometry[ROAD_MODE_GEOMETRY_LEFT_FAR +
                                          side].x;
      point->y = road_side->mode_geometry[ROAD_MODE_GEOMETRY_LEFT_FAR +
                                          side].y;
    }
  else
    {
      point->x = road_side->transition_geometry[side *
                                                ROAD_SIDE_COUNT];
      point->y = road_side->transition_geometry[side *
                                                ROAD_SIDE_COUNT + 1];
    }
}


__inline
static
int
road_surface_pixc(const RoadRenderSide *road_side,
                  RoadSide              side,
                  int                   index)
{
  return road_side->lane->profile_shading[
    index * ROAD_SIDE_COUNT + side];
}


__inline
static
RoadOctantDirection
road_octant_direction(int dx,
                      int dy)
{
  if(dx > 0)
    {
      if(dy > 0)
        {
          return dy <= dx ? ROAD_OCTANT_RIGHT_DOWN_SHALLOW
                            : ROAD_OCTANT_DOWN_RIGHT_STEEP;
        }
      return -dy <= dx ? ROAD_OCTANT_RIGHT_UP_SHALLOW
                         : ROAD_OCTANT_UP_RIGHT_STEEP;
    }

  if(dy > 0)
    {
      return dy <= -dx ? ROAD_OCTANT_LEFT_DOWN_SHALLOW
                         : ROAD_OCTANT_DOWN_LEFT_STEEP;
    }
  return dy >= dx ? ROAD_OCTANT_LEFT_UP_SHALLOW
                    : ROAD_OCTANT_UP_LEFT_STEEP;
}


static
void
adjust_quad_corners(RoadQuad *quad,
                    RoadSide  side,
                    int       index)
{
  RoadPoint *corners;
  RoadPoint *join_offsets;
  RoadOctantDirection direction[ROAD_QUAD_CORNER_COUNT];
  int same_orientation;
  int corner;
  int opposite;
  int outgoing;
  int join;

  if(road_surface_render_enabled.value == 0)
    {
      return;
    }

  corners = ROAD_QUAD_POINTS(quad);
  join_offsets = (RoadPoint *)quad_join_offsets;
  for(corner = 0; corner < ROAD_QUAD_CORNER_COUNT; corner++)
    {
      int next;

      next = (corner + 1) & (ROAD_QUAD_CORNER_COUNT - 1);
      direction[corner] = road_octant_direction(
        corners[next].x - corners[corner].x,
        corners[next].y - corners[corner].y);
    }

  same_orientation =
    ((side == ROAD_SIDE_RIGHT) ==
     (index >= ROAD_PROFILE_BASE_STRIP_COUNT));
  for(corner = 0; corner < ROAD_QUAD_CORNER_COUNT; corner++)
    {
      opposite =
        (direction[(corner + ROAD_QUAD_CORNER_COUNT - 1) &
                   (ROAD_QUAD_CORNER_COUNT - 1)] +
         ROAD_OPPOSITE_OCTANT_OFFSET) &
        (ROAD_OCTANT_DIRECTION_COUNT - 1);
      outgoing = direction[corner];
      if(opposite == outgoing)
        {
          continue;
        }

      join = opposite + outgoing;
      if((same_orientation && opposite < outgoing) ||
         (!same_orientation && outgoing < opposite))
        {
          join += ROAD_OCTANT_DIRECTION_COUNT;
        }
      join &= ROAD_QUAD_JOIN_COUNT - 1;
      corners[corner].x += join_offsets[join].x;
      corners[corner].y += join_offsets[join].y;
    }
}


static
int
road_profile_quad_is_visible(const RoadQuad *quad,
                             RoadSide        side,
                             RoadFixed       horizon)
{
  int cross0;
  int cross1;
  int cross2;
  int cross3;

  if(side == ROAD_SIDE_LEFT)
    {
      if(quad->top_left.x < 0 && quad->bottom_left.x < 0)
        {
          return 0;
        }
      if(quad->top_right.x > ROAD_SCREEN_RIGHT &&
         quad->bottom_right.x > ROAD_SCREEN_RIGHT)
        {
          return 0;
        }
    }
  else
    {
      if(quad->top_left.x > ROAD_SCREEN_RIGHT &&
         quad->bottom_left.x > ROAD_SCREEN_RIGHT)
        {
          return 0;
        }
      if(quad->top_right.x < 0 && quad->bottom_right.x < 0)
        {
          return 0;
        }
    }

  cross0 = ((quad->bottom_right.y - quad->bottom_left.y) >>
            ROAD_FIXED_CROSS_PRODUCT_SHIFT) *
           ((quad->top_left.x - quad->bottom_left.x) >>
            ROAD_FIXED_CROSS_PRODUCT_SHIFT);
  cross1 = ((quad->top_left.y - quad->bottom_left.y) >>
            ROAD_FIXED_CROSS_PRODUCT_SHIFT) *
           ((quad->bottom_right.x - quad->bottom_left.x) >>
            ROAD_FIXED_CROSS_PRODUCT_SHIFT);
  cross2 = ((quad->top_left.y - quad->top_right.y) >>
            ROAD_FIXED_CROSS_PRODUCT_SHIFT) *
           ((quad->bottom_right.x - quad->top_right.x) >>
            ROAD_FIXED_CROSS_PRODUCT_SHIFT);
  cross3 = ((quad->bottom_right.y - quad->top_right.y) >>
            ROAD_FIXED_CROSS_PRODUCT_SHIFT) *
           ((quad->top_left.x - quad->top_right.x) >>
            ROAD_FIXED_CROSS_PRODUCT_SHIFT);

  if(side == ROAD_SIDE_RIGHT)
    {
      if(cross0 <= cross1 && cross2 <= cross3)
        {
          return 0;
        }
    }
  else if(cross0 >= cross1 && cross2 >= cross3)
    {
      return 0;
    }

  return quad->top_left.y < horizon || quad->top_right.y < horizon ||
         quad->bottom_right.y < horizon || quad->bottom_left.y < horizon;
}


__inline
static
void
reverse_road_quad(RoadQuad *quad)
{
  RoadQuad saved;

  saved = *quad;
  quad->top_left = saved.top_right;
  quad->top_right = saved.top_left;
  quad->bottom_right = saved.bottom_left;
  quad->bottom_left = saved.bottom_right;
}


__inline
static
void
road_world_profile_point(RoadPoint                  *point,
                         const RoadRenderSide       *road_side,
                         RoadSide                    side,
                         int                         index)
{
  int profile_index;

  if(index == ROAD_PROFILE_SHOULDER_POINT)
    {
      point->x = side == ROAD_SIDE_LEFT
                       ? road_side->lane->shoulder_left
                       : road_side->lane->shoulder_right;
      point->y = 0;
    }
  else
    {
      profile_index = (index - 1) * ROAD_SIDE_COUNT + side;
      point->x = road_side->lane->surface_profile[profile_index].x;
      point->y = road_side->lane->surface_profile[profile_index].y;
    }
  point->x += road_side->world_x;
  point->y += road_side->world_y;
}


__inline
static
void
prepare_subdivide_context(RoadStripSubdivisionContext *context,
                          RoadRenderSide              *road_side,
                          RoadRenderSide              *neighbor,
                          RoadSide                     side,
                          RoadFixed                    horizon)
{
  int pair;

  context->draw.side = ROAD_SIDE_LEFT;
  context->draw.projection_depth = neighbor->projection_depth;
  context->draw.horizon_y = horizon;
  context->draw.reserved = 0;
  context->draw.adjust_edges = 0;

  for(pair = 0; pair < ROAD_PROFILE_BASE_POINT_COUNT; pair++)
    {
      road_world_profile_point(&context->source_profile[pair],
                               neighbor, side, pair);
      road_world_profile_point(
        &context->source_profile[pair + ROAD_PROFILE_BASE_POINT_COUNT],
        road_side, side, pair);
    }
}


__inline
static
void
set_subdivide_edges(RoadStripSubdivisionContext *context,
                    int                          index)
{
  context->draw.world_quad.top_left = context->source_profile[index];
  context->draw.world_quad.top_right = context->source_profile[index + 1];
  context->draw.world_quad.bottom_right =
    context->source_profile[index + ROAD_PROFILE_BASE_POINT_COUNT + 1];
  context->draw.world_quad.bottom_left =
    context->source_profile[index + ROAD_PROFILE_BASE_POINT_COUNT];
}


__inline
static
void
set_special_cel_flags(CCB     *cel,
                      RoadSide side,
                      int      index)
{
  cel->ccb_Flags &= ~(CCB_TWD | CCB_ACCW | CCB_ACW);
  if((side == ROAD_SIDE_RIGHT) ==
     (index >= ROAD_PROFILE_BASE_STRIP_COUNT))
    {
      cel->ccb_Flags |= CCB_ACCW;
    }
  else
    {
      cel->ccb_Flags |= CCB_ACW;
    }
}


__inline
static
void
set_direct_packet_flags(CCB     *packet,
                        RoadSide side,
                        int      index)
{
  if((side == ROAD_SIDE_RIGHT) ==
     (index >= ROAD_PROFILE_BASE_STRIP_COUNT))
    {
      packet->ccb_Flags =
        (packet->ccb_Flags & ~(CCB_TWD | CCB_ACW)) | CCB_ACCW;
    }
  else
    {
      packet->ccb_Flags =
        (packet->ccb_Flags & ~(CCB_TWD | CCB_ACCW)) | CCB_ACW;
    }
}


void
render_road_profile_bands(RoadRenderSide *road_side,
                          int             count,
                          RoadSide        side)
{
  RoadRenderSide *neighbor;
  RoadPoint source_points[ROAD_PROFILE_MAX_POINT_COUNT];
  RoadPoint neighbor_points[ROAD_PROFILE_MAX_POINT_COUNT];
  RoadQuad quad;
  int extend_to_screen_edge;
  RoadFixed horizon;
  int needs_subdivision;
  int neighbor_type;
  int index;
  int pixc_index;
  int context_ready;
  CCB *cel;
  CCB *packet;
  RoadStripSubdivisionContext context;

  extend_to_screen_edge = 0;
  context_ready = 0;
  if(count > ROAD_PROFILE_BASE_STRIP_COUNT)
    {
      count = ROAD_PROFILE_BASE_STRIP_COUNT;
    }
  else if(count < 0)
    {
      extend_to_screen_edge = 1;
      count = ROAD_PROFILE_BASE_STRIP_COUNT;
    }

  neighbor = road_side->next_sides[side];
  for(index = 0; index < ROAD_PROFILE_BASE_POINT_COUNT; index++)
    {
      road_profile_point(&source_points[index], road_side, side, index);
    }

  horizon = road_side->owner_node->sort_depth;
  needs_subdivision =
    road_side->projection_depth < ROAD_SUBDIVISION_DEPTH_LIMIT;

  road_profile_point(&neighbor_points[ROAD_PROFILE_SHOULDER_POINT],
                     neighbor, side, ROAD_PROFILE_SHOULDER_POINT);
  neighbor_type = neighbor->lane->geometry_mode;
  if(neighbor_type == ROAD_PROFILE_NEIGHBOR_FLAT)
    {
      for(index = ROAD_PROFILE_NEAR_POINT;
          index < ROAD_PROFILE_BASE_POINT_COUNT; index++)
        {
          neighbor_points[index].x = source_points[index].x;
          neighbor_points[index].y =
            neighbor_points[ROAD_PROFILE_SHOULDER_POINT].y;
        }
    }
  else if(neighbor_type == ROAD_PROFILE_NEIGHBOR_TRUNCATED)
    {
      road_profile_point(&neighbor_points[ROAD_PROFILE_NEAR_POINT],
                         neighbor, side, ROAD_PROFILE_NEAR_POINT);
      road_profile_point(&neighbor_points[ROAD_PROFILE_FAR_POINT],
                         neighbor, side, ROAD_PROFILE_FAR_POINT);
      neighbor_points[ROAD_PROFILE_OUTER_POINT] =
        neighbor_points[ROAD_PROFILE_FAR_POINT];
    }
  else
    {
      for(index = ROAD_PROFILE_NEAR_POINT;
          index < ROAD_PROFILE_BASE_POINT_COUNT; index++)
        {
          road_profile_point(&neighbor_points[index], neighbor, side,
                             index);
        }
      if(extend_to_screen_edge)
        {
          if(side == ROAD_SIDE_LEFT &&
             (source_points[ROAD_PROFILE_OUTER_POINT].x > 0 ||
              neighbor_points[ROAD_PROFILE_OUTER_POINT].x > 0))
            {
              source_points[ROAD_PROFILE_BASE_POINT_COUNT].x =
                source_points[ROAD_PROFILE_OUTER_POINT].x *
                ROAD_SIDE_COUNT -
                source_points[ROAD_PROFILE_FAR_POINT].x;
              if(source_points[ROAD_PROFILE_BASE_POINT_COUNT].x > 0)
                {
                  source_points[ROAD_PROFILE_BASE_POINT_COUNT].x = 0;
                }
              source_points[ROAD_PROFILE_BASE_POINT_COUNT].y =
                source_points[ROAD_PROFILE_FAR_POINT].y;
              neighbor_points[ROAD_PROFILE_BASE_POINT_COUNT].x =
                neighbor_points[ROAD_PROFILE_OUTER_POINT].x *
                ROAD_SIDE_COUNT -
                neighbor_points[ROAD_PROFILE_FAR_POINT].x;
              if(neighbor_points[ROAD_PROFILE_BASE_POINT_COUNT].x > 0)
                {
                  neighbor_points[ROAD_PROFILE_BASE_POINT_COUNT].x = 0;
                }
              neighbor_points[ROAD_PROFILE_BASE_POINT_COUNT].y =
                neighbor_points[ROAD_PROFILE_FAR_POINT].y;
              count++;
            }
          else if(side == ROAD_SIDE_RIGHT &&
                  (source_points[ROAD_PROFILE_OUTER_POINT].x <
                   ROAD_SCREEN_RIGHT ||
                   neighbor_points[ROAD_PROFILE_OUTER_POINT].x <
                   ROAD_SCREEN_RIGHT))
            {
              source_points[ROAD_PROFILE_BASE_POINT_COUNT].x =
                source_points[ROAD_PROFILE_OUTER_POINT].x *
                ROAD_SIDE_COUNT -
                source_points[ROAD_PROFILE_FAR_POINT].x;
              if(source_points[ROAD_PROFILE_BASE_POINT_COUNT].x <
                 ROAD_SCREEN_RIGHT)
                {
                  source_points[ROAD_PROFILE_BASE_POINT_COUNT].x =
                    ROAD_SCREEN_RIGHT;
                }
              source_points[ROAD_PROFILE_BASE_POINT_COUNT].y =
                source_points[ROAD_PROFILE_FAR_POINT].y;
              neighbor_points[ROAD_PROFILE_BASE_POINT_COUNT].x =
                neighbor_points[ROAD_PROFILE_OUTER_POINT].x *
                ROAD_SIDE_COUNT -
                neighbor_points[ROAD_PROFILE_FAR_POINT].x;
              if(neighbor_points[ROAD_PROFILE_BASE_POINT_COUNT].x <
                 ROAD_SCREEN_RIGHT)
                {
                  neighbor_points[ROAD_PROFILE_BASE_POINT_COUNT].x =
                    ROAD_SCREEN_RIGHT;
                }
              neighbor_points[ROAD_PROFILE_BASE_POINT_COUNT].y =
                neighbor_points[ROAD_PROFILE_FAR_POINT].y;
              count++;
            }
        }
    }

  for(index = count - 1; index >= 0; index--)
    {
      quad.top_left = neighbor_points[index];
      quad.top_right = neighbor_points[index + 1];
      quad.bottom_right = source_points[index + 1];
      quad.bottom_left = source_points[index];

      if(!road_profile_quad_is_visible(&quad, side, horizon))
        {
          continue;
        }

      if(needs_subdivision && index < ROAD_PROFILE_BASE_STRIP_COUNT)
        {
          cel = select_profile_band_subdivision_cel(
            road_side, side, &quad, index);
          if(cel == 0)
            {
              cel = display_runtime_globals.fallback_ccb;
              cel->ccb_PIXC = road_surface_pixc(road_side, side, index);
              append_mapped_quad_cel(cel, (const DisplayQuad *)&quad);
              continue;
            }

          if(!context_ready)
            {
              prepare_subdivide_context(&context, road_side, neighbor,
                                        side, horizon);
              context_ready = 1;
            }
          set_subdivide_edges(&context, index);
          context.draw.cel = cel;
          context.draw.screen_quad = &quad;
          context.draw.adjust_edges = 0;
          set_special_cel_flags(cel, side, index);
          cel->ccb_PIXC = road_surface_pixc(road_side, side, index);
          subdivide_four_point_road_strip(&context.draw);
          continue;
        }

      pixc_index = index;
      if(index >= ROAD_PROFILE_BASE_STRIP_COUNT)
        {
          reverse_road_quad(&quad);
          pixc_index = ROAD_PROFILE_BASE_STRIP_COUNT - 1;
        }
      adjust_quad_corners(&quad, side, index);

      cel = select_road_surface_cel(
        road_side, side, &quad, pixc_index);
      cel->ccb_PIXC = road_surface_pixc(road_side, side, pixc_index);
      packet = (CCB *)append_mapped_quad_cel(cel, (const DisplayQuad *)&quad);
      set_direct_packet_flags(packet, side, index);
    }
}


int
render_repeated_roadside_strip(RoadRenderSide *road_side,
                               RoadSide        side)
{
  RoadRenderSide *neighbor;
  CCB **cel_slot;
  CCB *cel;
  RoadQuad points;
  RoadFixed source_step;
  RoadFixed neighbor_step;
  RoadFixed top_y;
  RoadFixed bottom_y;
  int count;
  int visible;

  neighbor = road_side->next_sides[side];
  top_y = road_side->connection_geometry[
    ROAD_CONNECTION_CENTER_Y];
  bottom_y = road_side->connection_geometry[
    ROAD_CONNECTION_SOURCE_CENTER_Y];
  source_step = ROAD_FIXED_FROM_8_8(road_side->projection_scale);
  neighbor_step = ROAD_FIXED_FROM_8_8(neighbor->projection_scale);

  cel_slot = (CCB **)&road_side->texture_cache->join_sources[
    ROAD_REPEATED_JOIN_SOURCE_ROW][side];
  if(*cel_slot == 0)
    {
      *cel_slot = (CCB *)lookup_family_resource_child_for_render(
        road_side->lane->edge_resource_ids[side],
        ROAD_REPEATED_EDGE_RESOURCE_GROUP,
        ROAD_REPEATED_EDGE_RESOURCE_VARIANT);
    }
  cel = (CCB *)select_cel_for_dimensions(
    (char *)*cel_slot, ROAD_FIXED_TO_INTEGER(neighbor_step),
    ROAD_FIXED_TO_INTEGER(bottom_y - top_y));

  if(side == ROAD_SIDE_LEFT)
    {
      source_step = -source_step;
      neighbor_step = -neighbor_step;
    }

  road_profile_point(&points.top_left, neighbor, side,
                     ROAD_PROFILE_SHOULDER_POINT);
  points.top_left.y = top_y;
  points.top_right.x = points.top_left.x + neighbor_step;
  points.top_right.y = top_y;
  road_profile_point(&points.bottom_left, road_side, side,
                     ROAD_PROFILE_SHOULDER_POINT);
  points.bottom_left.y = bottom_y;
  points.bottom_right.x = points.bottom_left.x + source_step;
  points.bottom_right.y = bottom_y;

  count = select_repeated_roadside_strip_count(road_side);
  while(count-- > 0)
    {
      visible = road_quad_is_horizontally_visible(
        road_side, &points,
        side == ROAD_SIDE_RIGHT ? 0 : 1);
      if(visible != 0)
        {
          append_horizontal_quad_cel(cel, (const DisplayQuad *)&points);
        }

      points.top_left.x += neighbor_step;
      points.top_right.x += neighbor_step;
      points.bottom_right.x += source_step;
      points.bottom_left.x += source_step;
      if((side == ROAD_SIDE_LEFT && points.top_left.x <= 0) ||
         (side == ROAD_SIDE_RIGHT &&
          points.top_left.x >= ROAD_SCREEN_RIGHT))
        {
          break;
        }
    }

  return 0;
}
