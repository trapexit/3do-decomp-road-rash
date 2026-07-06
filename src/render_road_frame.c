#include "bss_early_data.h"
#include "bss_late_data.h"
#include "display_cel_runtime.h"
#include "racer_runtime.h"
#include "rw_semantic_data.h"

enum RoadFrameRenderConstant
{
  ROAD_FRAME_VIEW_WIDTH = 300,
  ROAD_FRAME_DUAL_LANE_COUNT = 2,
  ROAD_FRAME_PRIMARY_LANE_FLAG = 1,
  ROAD_FRAME_PLAYER_SPEED_VISIBILITY_LIMIT = 0x4B0,
  ROAD_FRAME_CLEAR_BOTTOM = 0xE6,
  ROAD_FRAME_CLEAR_COLOR = 0x294C,
  ROAD_FRAME_FIXED_ONE = 0x10000,
  ROAD_FRAME_FIXED_SHIFT = 16
};

void
project_road_render_nodes(unsigned int    packed_track_position,
                          RoadRenderNode *nodes,
                          int            *projection_origin,
                          int             selected_side);
void
render_road_background_layers(DisplayRectangle *screen_bounds);
void
render_race_hud(void);

static
int
fixed_ceiling(int value)
{
  return (value - 1 + ROAD_FRAME_FIXED_ONE) >>
         ROAD_FRAME_FIXED_SHIFT;
}


void
render_road_frame(void)
{
  RacerEntity *player;
  RoadSegmentLaneRuntime *lane;
  RoadSegmentRuntime *segment;
  DisplayRectangle screen_bounds;

  screen_bounds.left = 0;
  screen_bounds.right = ROAD_FRAME_VIEW_WIDTH;
  road_renderer_state.road_horizon = -1;

  player = (RacerEntity *)race_rider_state.player_object;
  lane = player->position_snapshot.track_segment;
  segment = (RoadSegmentRuntime *)lane->resource_handle;
  if(segment->lane_count == ROAD_FRAME_DUAL_LANE_COUNT)
    {
      if((lane->topology_flags & ROAD_FRAME_PRIMARY_LANE_FLAG) == 0)
        {
          road_renderer_state.road_side_selector = ROAD_SIDE_RIGHT;
        }
      else
        {
          road_renderer_state.road_side_selector = ROAD_SIDE_LEFT;
        }
    }

  project_road_render_nodes(
    (unsigned int)road_renderer_state.motion.packed_track_position,
    gRoadRenderNodes,
    &road_renderer_state.motion.projection_origin_x,
    (signed char)road_renderer_state.road_side_selector);

  screen_bounds.top = 0;
  screen_bounds.bottom = fixed_ceiling(road_renderer_state.road_horizon);
  render_road_background_layers(&screen_bounds);

  player = (RacerEntity *)race_rider_state.player_object;
  if(player->paired_state->velocity.forward <=
     ROAD_FRAME_PLAYER_SPEED_VISIBILITY_LIMIT ||
     input_thread_state.chord_processing_active != 0)
    {
      screen_bounds.top =
        fixed_ceiling(road_renderer_state.road_horizon) - 1;
      screen_bounds.bottom = ROAD_FRAME_CLEAR_BOTTOM;
      append_solid_rectangle_cel(
        &screen_bounds, ROAD_FRAME_CLEAR_COLOR);
    }

  render_projected_road_scene();
  render_race_hud();
}
