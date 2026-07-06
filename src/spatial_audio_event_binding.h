#ifndef ROADRASH_SPATIAL_AUDIO_EVENT_BINDING_H
#define ROADRASH_SPATIAL_AUDIO_EVENT_BINDING_H

#include "family_resource_binding.h"
#include "spatial_audio_event_queue.h"

void
reset_all_spatial_audio_event_family_bindings(void);
void
set_spatial_audio_event_family_binding(int                                  queue_index,
                                       const FamilyResourceBindingIdentity *identity);
void
clear_spatial_audio_event_family_binding(const SpatialAudioEvent *event);
int
load_spatial_audio_event_sample_copy(SpatialAudioEvent *event);

#endif
