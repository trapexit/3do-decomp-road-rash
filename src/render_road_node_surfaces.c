#include "bss_early_data.h"
#include "road_geometry.h"
#include "road_surface_cel_selection.h"
#include "rw_semantic_data.h"

int
render_road_side_geometry(RoadRenderSide *road_side,
                          RoadSide        side);

int
render_road_node_surfaces(RoadRenderNode *node)
{
  RoadRenderSide *road_sides[ROAD_SIDE_COUNT];
  RoadSide selected_side;
  RoadSide other_side;

  road_sides[ROAD_SIDE_LEFT] = node->left_side;
  road_sides[ROAD_SIDE_RIGHT] = node->right_side;

  if(node->node_type == ROAD_RENDER_NODE_SINGLE)
    {
      if(node->visible != 0)
        {
          render_road_surface_strips(road_sides[ROAD_SIDE_LEFT]);
        }
      render_road_side_geometry(
        road_sides[ROAD_SIDE_LEFT], ROAD_SIDE_LEFT);
      return render_road_side_geometry(
        road_sides[ROAD_SIDE_LEFT], ROAD_SIDE_RIGHT);
    }

  selected_side =
    (signed char)road_renderer_state.road_side_selector;
  other_side = ROAD_SIDE_RIGHT - selected_side;
  if(node->visible != 0)
    {
      render_road_surface_strips(road_sides[other_side]);
      render_road_surface_strips(road_sides[selected_side]);
    }

  render_road_side_geometry(road_sides[other_side], other_side);
  render_road_side_geometry(road_sides[selected_side], selected_side);
  return render_road_node_center_geometry(node);
}
