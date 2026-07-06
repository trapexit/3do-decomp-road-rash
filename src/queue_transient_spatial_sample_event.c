#include "family_resource_binding.h"
#include "rw_semantic_data.h"
#include "spatial_audio_event_binding.h"
#include "spatial_audio_event_queue.h"

static
void
queue_transient_spatial_sample_event_internal(const void                          *sample_image,
                                              const int                           *source_position,
                                              const FamilyResourceBindingIdentity *identity)
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

  entry->event_type = SPATIAL_AUDIO_EVENT_TRANSIENT_SAMPLE;
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

  entry->sample_image = sample_image;
  entry->reserved_zero = 0;
  entry->forward_velocity = 0;
  set_spatial_audio_event_family_binding(index, identity);

  game_audio_state.queued_event_count = count + 1;
  index++;
  game_audio_state.queued_event_write_index = index;
  if(index >= SPATIAL_EVENT_CAPACITY)
    {
      game_audio_state.queued_event_write_index = 0;
    }
}


void
queue_transient_spatial_sample_event(const void *sample_image,
                                     const int  *source_position)
{
  queue_transient_spatial_sample_event_internal(
    sample_image, source_position, 0);
}


int
queue_family_transient_spatial_sample_event(int        selector,
                                            int        family_index,
                                            int        child_index,
                                            const int *source_position)
{
  FamilyResourceBindingIdentity identity;
  const void *sample_image;

  /* The lookup's group lock is sufficient to publish the pointer and its
     generation atomically.  An unrelated family loader may own the global
     render barrier without making this selector unsafe.  Consumption later
     validates the generation and copies the sample while holding that
     global barrier. */
  sample_image = (const void *)lookup_family_resource_child_with_identity(
    selector, family_index, child_index, &identity);
  if(sample_image == 0)
    {
      return 0;
    }

  queue_transient_spatial_sample_event_internal(
    sample_image, source_position, &identity);
  return 1;
}
