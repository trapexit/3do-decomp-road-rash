#include "race_audio_runtime.h"

enum TrackRelativeLateralOffsetConstant
{
  TRACK_RELATIVE_SEGMENT_INDEX_MASK = 0x7F,
  TRACK_RELATIVE_OFFSET_UNAVAILABLE = 0x7FFFFFFF,
  TRACK_RELATIVE_OFFSET_HALF_SHIFT = 1,
  TRACK_RELATIVE_OFFSET_SIGN_SHIFT = 31
};

int
calculate_track_relative_lateral_offset(const RacerEntity *racer)
{
  const RoadSegmentRuntime *racer_segment;
  const RoadSegmentRuntime *view_segment;
  int track_position;
  int base_step;
  int step_delta;
  int scaled_delta;
  int racer_left_step;
  int view_left_step;
  int combined_step;

  if(racer == 0 || racer->track_segment == 0)
    {
      return TRACK_RELATIVE_OFFSET_UNAVAILABLE;
    }

  racer_segment =
    (const RoadSegmentRuntime *)racer->track_segment->resource_handle;
  track_position =
    track_simulation_globals.motion_source.packed_track_position;
  view_segment = gRoadSegmentEffectWorkspace.segment_index
                 .by_track_index[
    (track_position >> TRACK_POSITION_FRACTION_BITS) &
    TRACK_RELATIVE_SEGMENT_INDEX_MASK];

  base_step = racer_segment->left_step;
  step_delta = racer_segment->next->left_step - base_step;
  scaled_delta =
    (racer->position_z & TRACK_POSITION_FRACTION_MASK) * step_delta;
  if(scaled_delta < 0)
    {
      scaled_delta += TRACK_POSITION_FRACTION_MASK;
    }
  racer_left_step =
    base_step + (scaled_delta >> TRACK_POSITION_FRACTION_BITS);

  base_step = view_segment->left_step;
  step_delta = view_segment->next->left_step - base_step;
  scaled_delta =
    (track_position & TRACK_POSITION_FRACTION_MASK) * step_delta;
  if(scaled_delta < 0)
    {
      scaled_delta += TRACK_POSITION_FRACTION_MASK;
    }
  view_left_step =
    base_step + (scaled_delta >> TRACK_POSITION_FRACTION_BITS);

  combined_step = racer_left_step + view_left_step;
  combined_step +=
    (int)((unsigned int)combined_step >>
          TRACK_RELATIVE_OFFSET_SIGN_SHIFT);
  return combined_step >> TRACK_RELATIVE_OFFSET_HALF_SHIFT;
}
