#ifndef ROADRASH_SPATIAL_AUDIO_EVENT_QUEUE_H
#define ROADRASH_SPATIAL_AUDIO_EVENT_QUEUE_H

#include "bss_early_data.h"

enum SpatialAudioSourcePositionWord
{
  SPATIAL_AUDIO_SOURCE_POSITION_Z = 0,
  SPATIAL_AUDIO_SOURCE_POSITION_Y = 1,
  SPATIAL_AUDIO_SOURCE_POSITION_X = 3
};

enum SpatialAudioEventType
{
  SPATIAL_AUDIO_EVENT_TRANSIENT_SAMPLE = 30
};

/* The position argument starts at a racer's position_z word. */
void
queue_spatial_audio_event(int        event_type,
                          const int *source_position,
                          const int *source_forward_velocity);
/* The image is borrowed; its sample data must outlive playback. */
void
queue_transient_spatial_sample_event(const void *sample_image,
                                     const int  *source_position);
/* Returns nonzero when the oracle's selector-scoped sample lookup succeeds.
   The borrowed family pointer is generation-checked and copied at dispatch. */
int
queue_family_transient_spatial_sample_event(int        selector,
                                            int        family_index,
                                            int        child_index,
                                            const int *source_position);
/* The returned slot remains owned by the ring and is for immediate dispatch. */
SpatialAudioEvent *
dequeue_spatial_audio_event(void);

#endif
