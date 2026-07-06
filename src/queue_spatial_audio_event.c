#include "rw_semantic_data.h"
#include "spatial_audio_event_binding.h"
#include "spatial_audio_event_queue.h"

void
queue_spatial_audio_event(int        event_type,
                          const int *source_position,
                          const int *source_forward_velocity)
{
  int count;
  int index;
  SpatialAudioEvent *entry;

  count = game_audio_state.queued_event_count;
  if(count >= SPATIAL_EVENT_CAPACITY)
    {
      return;
    }

  index = game_audio_state.queued_event_write_index;
  entry = &gSpatialEventQueue[index];

  entry->event_type = event_type;
  if(source_position != 0)
    {
      entry->position_x =
        source_position[SPATIAL_AUDIO_SOURCE_POSITION_X];
      entry->position_y =
        source_position[SPATIAL_AUDIO_SOURCE_POSITION_Y];
      entry->position_z =
        source_position[SPATIAL_AUDIO_SOURCE_POSITION_Z];
    }
  else
    {
      entry->position_x = 0;
      entry->position_y = 0;
      entry->position_z = 0;
    }

  entry->sample_image = 0;
  entry->reserved_zero = 0;
  set_spatial_audio_event_family_binding(index, 0);
  if(source_forward_velocity == 0)
    {
      entry->forward_velocity = 0;
    }
  else
    {
      entry->forward_velocity = *source_forward_velocity;
    }

  game_audio_state.queued_event_count = count + 1;
  index++;
  game_audio_state.queued_event_write_index = index;
  if(index >= SPATIAL_EVENT_CAPACITY)
    {
      game_audio_state.queued_event_write_index = 0;
    }
}
