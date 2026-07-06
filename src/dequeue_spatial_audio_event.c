#include "rw_semantic_data.h"
#include "spatial_audio_event_queue.h"

SpatialAudioEvent *
dequeue_spatial_audio_event(void)
{
  int count;
  int index;
  SpatialAudioEvent *entry;

  count = game_audio_state.queued_event_count;
  if(count <= 0)
    {
      return 0;
    }

  index = game_audio_state.queued_event_read_index;
  entry = &gSpatialEventQueue[index];

  count--;
  index++;
  game_audio_state.queued_event_read_index = index;
  game_audio_state.queued_event_count = count;
  if(index >= SPATIAL_EVENT_CAPACITY)
    {
      game_audio_state.queued_event_read_index = 0;
    }

  return entry;
}
