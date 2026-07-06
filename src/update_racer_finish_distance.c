#include "ai_racer_runtime.h"
#include "rw_semantic_data.h"

int
update_racer_finish_distance(AiRacerObject *racer)
{
  TrackSegmentDefinition *segment_definition;
  int finish_distance;
  int position;
  int reference_position;

  finish_distance = track_runtime_globals.finish_threshold;
  if(finish_distance != 0 && racer->track_segment != 0)
    {
      finish_distance -= racer->position.track_position;
      racer->finish_distance = finish_distance;
      return finish_distance;
    }

  if(racer->track_segment != 0)
    {
      segment_definition =
        racer->track_segment->clip_node->payload.segment_definition;
      reference_position = racer->track_segment->clip_position;
    }
  else
    {
      segment_definition =
        racer->owner->cursor_node->segment_definition;
      reference_position = racer->owner->cursor_position;
    }

  position = racer->position.track_position;
  reference_position =
    ((reference_position ^ position) &
     ~TRACK_POSITION_FRACTION_MASK) ^ position;
  finish_distance =
    (segment_definition->finish_distance <<
      TRACK_POSITION_FRACTION_BITS) - reference_position;
  racer->finish_distance = finish_distance;
  return finish_distance;
}
