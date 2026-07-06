#include "bss_late_data.h"
#include "memory_manager_runtime.h"
#include "track_segment_definition.h"

/* Reconstructed from the original function at 0x00022B5C. */

int
create_road_track_transition_runtime(RoadTrackGraphNode *junction,
                                     int                 sample_direction)
{
  RoadTrackTransitionRuntime *runtime;
  RoadTrackTraversalState *traversal;
  RoadClipPlaybackState *primary_channel;
  RoadClipPlaybackState *secondary_channel;
  int primary_margins[ROAD_TRACK_CHANNEL_COUNT];
  int primary_depth_offsets[ROAD_TRACK_CHANNEL_COUNT];
  int secondary_margins[ROAD_TRACK_CHANNEL_COUNT];
  int secondary_depth_offsets[ROAD_TRACK_CHANNEL_COUNT];
  unsigned char resource_ids[ROAD_TRACK_CHANNEL_COUNT];
  int sample_delta;
  int clearance_found;
  int sample_index;
  int selected_index;
  int extra_clearance;
  unsigned int terrain_tag;

  runtime = (RoadTrackTransitionRuntime *)MEM_NewPointer(
    sizeof(*runtime), MEMORY_REQUEST_ANY,
    MEM_POINTER_OPTIONS_DEFAULT);
  if(runtime == 0)
    {
      return 0;
    }

  traversal = create_road_track_traversal(
    &runtime->traversal, junction->payload.single_link,
    TRACK_TRAVERSAL_REVERSE);
  clearance_found = 0;

  junction->extent.transition_span = ROAD_TRACK_TRANSITION_SCAN_SPAN;
  initialize_track_clip_traversal(
    &traversal->channels[ROAD_TRACK_FIRST_CHANNEL],
    junction->payload.single_link, sample_direction < 0);
  sample_delta = sample_direction * TRACK_POSITION_UNIT;

  primary_channel = &traversal->channels[ROAD_TRACK_FIRST_CHANNEL];
  secondary_channel = &traversal->channels[ROAD_TRACK_SECOND_CHANNEL];

  sample_index = 0;
  do
    {
      advance_road_track_traversal(traversal, sample_delta);

      if(clearance_found == 0 &&
         ROAD_TRACK_TRANSITION_CLEARANCE_THRESHOLD <=
         traversal->left_step)
        {
          extra_clearance = 0;
          terrain_tag = primary_channel->terrain_state.resource_tag;
          if(terrain_tag == ROAD_TERRAIN_TAG_RBLD ||
             terrain_tag == ROAD_TERRAIN_TAG_RHIL)
            {
              sample_road_slope_geometry(
                &primary_channel->slope_geometry_state,
                primary_margins, primary_depth_offsets,
                resource_ids);
              sample_road_slope_geometry(
                &secondary_channel->slope_geometry_state,
                secondary_margins, secondary_depth_offsets,
                resource_ids);
              extra_clearance =
                primary_margins[ROAD_TRACK_SECOND_CHANNEL] +
                secondary_margins[ROAD_TRACK_FIRST_CHANNEL];
            }
          else if(terrain_tag == ROAD_TERRAIN_TAG_RDWD ||
                  terrain_tag == ROAD_TERRAIN_TAG_RMTN)
            {
              sample_road_slope_geometry(
                &primary_channel->slope_geometry_state,
                primary_margins, primary_depth_offsets,
                resource_ids);
              sample_road_slope_geometry(
                &secondary_channel->slope_geometry_state,
                secondary_margins, secondary_depth_offsets,
                resource_ids);
              extra_clearance =
                populate_road_terrain_profile(
                  &primary_channel->terrain_state, 0) +
                primary_margins[ROAD_TRACK_SECOND_CHANNEL] +
                secondary_margins[ROAD_TRACK_FIRST_CHANNEL] +
                populate_road_terrain_profile(
                  &secondary_channel->terrain_state, 0);
            }

          if(ROAD_TRACK_TRANSITION_CLEARANCE_THRESHOLD +
             extra_clearance <=
             traversal->left_step)
            {
              clearance_found = 1;
              selected_index = sample_index;
            }
        }

      sample_index++;
    } while(sample_index < ROAD_TRACK_TRANSITION_SAMPLE_COUNT);

  if(clearance_found == 0)
    {
      selected_index = sample_index - 1;
    }

  runtime->branch_clearance_index = selected_index;
  junction->extent.transition_span = sample_index * TRACK_POSITION_UNIT;
  junction->transition_runtime = runtime;

  return 1;
}
