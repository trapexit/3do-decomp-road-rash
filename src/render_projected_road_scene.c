#include "bss_early_data.h"
#include "roadside_object_rendering.h"

enum ProjectedRoadSceneConstant
{
  ROAD_SCENE_FARTHEST_NODE_INDEX = 31,
  ROAD_SCENE_SHADING_DEPTH_BASE = 32
};

void
reset_track_visibility_scan(void);
void
resume_track_visibility_scan(void);
int
set_cel_shading_depth(int depth);

int
render_projected_road_scene(void)
{
  RoadRenderNode *node;
  int node_index;

  node = &gRoadRenderNodes[ROAD_SCENE_FARTHEST_NODE_INDEX];
  reset_track_visibility_scan();
  render_road_node_surfaces(node);

  node_index = ROAD_SCENE_FARTHEST_NODE_INDEX;
  do
    {
      node--;
      set_cel_shading_depth(ROAD_SCENE_SHADING_DEPTH_BASE - node_index);
      render_road_node_surfaces(node);
      render_road_node_objects(node + 1);
      resume_track_visibility_scan();
      node_index--;
    } while(node_index > 0);

  return render_road_node_objects(node);
}
