#include "aiff_sample_runtime.h"
#include "rw_semantic_data.h"
#include "spatial_audio_event_binding.h"

/* SpatialAudioEvent is an exact seven-word oracle record.  The sample pointer
   at +0x10 is borrowed, so reusable-family ownership lives beside the ring. */
static unsigned int
  spatial_audio_event_family_generations[SPATIAL_EVENT_CAPACITY];
static unsigned char
  spatial_audio_event_family_selectors[SPATIAL_EVENT_CAPACITY];

static
int
spatial_audio_event_queue_index(const SpatialAudioEvent *event)
{
  int index;

  for(index = 0; index < SPATIAL_EVENT_CAPACITY; index++)
    {
      if(event == &gSpatialEventQueue[index])
        {
          return index;
        }
    }
  return -1;
}


void
reset_all_spatial_audio_event_family_bindings(void)
{
  int index;

  for(index = 0; index < SPATIAL_EVENT_CAPACITY; index++)
    {
      spatial_audio_event_family_selectors[index] = 0;
      spatial_audio_event_family_generations[index] = 0;
    }
}


void
set_spatial_audio_event_family_binding(int                                  queue_index,
                                       const FamilyResourceBindingIdentity *identity)
{
  if(queue_index < 0 || queue_index >= SPATIAL_EVENT_CAPACITY)
    {
      return;
    }

  if(identity == 0)
    {
      spatial_audio_event_family_selectors[queue_index] = 0;
      spatial_audio_event_family_generations[queue_index] = 0;
    }
  else
    {
      spatial_audio_event_family_selectors[queue_index] =
        (unsigned char)identity->selector;
      spatial_audio_event_family_generations[queue_index] =
        identity->generation;
    }
}


void
clear_spatial_audio_event_family_binding(const SpatialAudioEvent *event)
{
  int index;

  index = spatial_audio_event_queue_index(event);
  if(index >= 0)
    {
      set_spatial_audio_event_family_binding(index, 0);
    }
}


static
int
spatial_audio_event_family_binding_is_current(const SpatialAudioEvent *event)
{
  unsigned int generation;
  int index;

  index = spatial_audio_event_queue_index(event);
  if(index < 0)
    {
      return 0;
    }

  generation = spatial_audio_event_family_generations[index];
  if(generation == 0)
    {
      return 1;
    }
  return family_resource_binding_values_are_current(
    spatial_audio_event_family_selectors[index], generation);
}


int
load_spatial_audio_event_sample_copy(SpatialAudioEvent *event)
{
  int sample_item;

  if(!acquire_family_resource_render_barrier())
    {
      return -1;
    }
  if(spatial_audio_event_family_binding_is_current(event) == 0)
    {
      event->sample_image = 0;
      release_family_resource_render_barrier();
      return -1;
    }

  /* The oracle attaches the family-buffer bytes directly.  Copying the
     transient data preserves the same sample while allowing that reusable
     family group to be replaced as soon as this guarded parse returns. */
  sample_item = load_aiff_sample_image((void *)event->sample_image, 1);
  release_family_resource_render_barrier();
  return sample_item;
}
