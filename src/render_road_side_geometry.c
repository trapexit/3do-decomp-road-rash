/* Reconstructed from the original function at 0x0002F788. */

#include "bss_early_data.h"
#include "bss_late_data.h"
#include "road_geometry.h"
#include "roadside_object_rendering.h"

int
render_road_profile_bands(RoadRenderSide *road_side,
                          int             band_count,
                          RoadSide        side);
void
render_roadside_segment_join(RoadRenderSide *road_side,
                             RoadSide        side);
int
render_road_edge_transition(RoadRenderSide *road_side,
                            RoadSide        side,
                            int             mode);
int
render_textured_roadside_fill(RoadRenderSide *road_side,
                              RoadSide        side);
int
render_procedural_roadside_samples(RoadRenderSide *road_side,
                                   RoadSide        side);
int
render_recessed_road_edge(RoadRenderSide *road_side,
                          RoadSide        side);
int
render_raised_road_edge(RoadRenderSide *road_side,
                        RoadSide        side);
int
render_repeated_roadside_strip(RoadRenderSide *road_side,
                               RoadSide        side);

int
render_road_side_geometry(RoadRenderSide *road_side,
                          RoadSide        side)
{
  RoadSegmentLaneRuntime *lane;
  int geometry_mode;
  int surface_flags;
  int edge_height;

  if(road_side->active == 0)
    {
      return (int)road_side;
    }

  lane = road_side->lane;
  geometry_mode = lane->geometry_mode;
  switch(geometry_mode)
    {
    case ROAD_GEOMETRY_MODE_PROFILE:
      if(road_side->owner_node->visible == 0)
        {
          return render_road_profile_bands(road_side, -1, side);
        }
      surface_flags = lane->surface_flags;
      if(surface_flags == 0)
        {
          render_roadside_segment_join(road_side, side);
          return render_road_profile_bands(road_side, -1, side);
        }
      if((surface_flags & ROAD_SURFACE_FLAG_BLEND_TRANSITION) != 0)
        {
          return render_road_surface_blend_strip(road_side, side, surface_flags);
        }
      if((surface_flags & ROAD_SURFACE_FLAG_MARGIN_MASK) == 0)
        {
          return (int)road_side;
        }
      render_roadside_segment_join(road_side, side);
      return render_repeated_roadside_strip(road_side, side);

    case ROAD_GEOMETRY_MODE_TEXTURED:
      surface_flags = lane->surface_flags;
      if(surface_flags == 0)
        {
          render_road_edge_transition(road_side, side, -1);
          return render_textured_roadside_fill(road_side, side);
        }
      if((surface_flags & ROAD_SURFACE_FLAG_BLEND_TRANSITION) != 0)
        {
          return render_road_surface_blend_strip(road_side, side, surface_flags);
        }
      if((surface_flags & ROAD_SURFACE_FLAG_REVERSED_MARGIN) != 0)
        {
          render_road_edge_transition(road_side, side, -1);
          render_roadside_margin_transition(road_side, side);
          return render_textured_roadside_fill(road_side, side);
        }
      if((surface_flags & ROAD_SURFACE_FLAG_DEPTH_ADJUSTED) == 0)
        {
          return (int)road_side;
        }
      render_road_edge_transition(road_side, side, -1);
      render_roadside_margin_transition(road_side, side);
      render_textured_roadside_fill(road_side, side);
      return render_procedural_roadside_samples(road_side, side);

    case ROAD_GEOMETRY_MODE_EDGE:
    case ROAD_GEOMETRY_MODE_RESERVED:
    case ROAD_GEOMETRY_MODE_CURVED:
      if(road_side->owner_node->visible != 0)
        {
          if(geometry_mode != ROAD_GEOMETRY_MODE_CURVED ||
             road_side->next_sides[ROAD_SIDE_LEFT]->lane->geometry_mode !=
             ROAD_GEOMETRY_MODE_CURVED)
            {
              render_roadside_segment_join(road_side, side);
            }
        }
      edge_height = lane->edge_shapes[side].height;
      if(edge_height <= 0)
        {
          return render_recessed_road_edge(road_side, side);
        }
      return render_raised_road_edge(road_side, side);
    }

  return (int)road_side;
}
