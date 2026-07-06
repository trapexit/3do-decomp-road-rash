#ifndef ROADRASH_RACE_AUDIO_RUNTIME_H
#define ROADRASH_RACE_AUDIO_RUNTIME_H

#include "racer_runtime.h"
#include "spatial_audio_event_queue.h"

enum RaceAudioVoiceSlot
{
  RACE_AUDIO_PRIMARY_ENGINE_VOICE = 0,
  RACE_AUDIO_FIRST_AUXILIARY_ENGINE_VOICE = 1,
  RACE_AUDIO_SECOND_AUXILIARY_ENGINE_VOICE = 2,
  RACE_AUDIO_ENGINE_VOICE_COUNT = 3,
  RACE_AUDIO_SLIDING_EFFECT_CHANNEL = 0,
  RACE_AUDIO_SKID_EFFECT_CHANNEL = 1,
  RACE_AUDIO_EFFECT_CHANNEL_COUNT = 2
};

enum RaceAudioVoiceValue
{
  RACE_AUDIO_UNASSIGNED_ITEM = -1,
  RACE_AUDIO_NO_VOICE_GROUP = 0,
  RACE_AUDIO_ENGINE_SAMPLE_MODE = 1,
  RACE_AUDIO_MAXIMUM_ACTIVE_GROUP_VOICES = 1,
  RACE_AUDIO_ENGINE_START_RATE = 0x2000,
  RACE_AUDIO_ENGINE_START_AMPLITUDE = 0x2AAA,
  RACE_AUDIO_CENTER_PAN_REQUEST = -1
};

int
load_race_audio_samples(void);
int
unload_race_audio_samples(void);
int
load_race_audio_collection(void);
int
initialize_race_audio(void);
int
shutdown_race_audio(void);
int
initialize_race_audio_voice_state(void);
int
release_race_audio_voice_resources(void);
int
claim_race_patch_voices_and_groups(void);
int
claim_race_engine_voices(void);
int
release_race_engine_voices(void);
int
start_race_engine_voices(void);
int
start_race_engine_audio(void);
int
calculate_track_relative_lateral_offset(const RacerEntity *racer);
void
calculate_spatial_audio_parameters(const RacerEntity       *source_object,
                                   const SpatialAudioEvent *event,
                                   int                     *out_pitch,
                                   int                     *out_volume,
                                   int                     *out_pan);
int
play_or_update_audio_voice_slot(AudioVoiceSlot *voice,
                                int             sample_item,
                                int             pitch,
                                int             volume,
                                int             pan);
void
update_rider_sliding_audio(int                      sliding_voice_item,
                           const SpatialAudioEvent *event);
void
play_offroad_skid_audio(int                      skid_voice_item,
                        const SpatialAudioEvent *event);
void
update_pursuit_siren_audio(int                      siren_voice_item,
                           const SpatialAudioEvent *event);
int
start_spatial_audio_voice(int                      voice_item,
                          const SpatialAudioEvent *event);
void
update_racer_engine_audio_spatial_state(RacerEntity *racer);
void
update_rider_engine_pitch(RacerEntity *rider);
int
update_rider_engine_audio_voice(const RacerEntity *rider,
                                AudioVoiceSlot    *voice);
int
update_traffic_engine_audio_voice(const RacerEntity *vehicle,
                                  AudioVoiceSlot    *voice);
void
update_race_object_engine_audio(RacerEntity    *object,
                                AudioVoiceSlot *voice);
int
update_rider_skid_audio_events(const RacerEntity *rider);
void
update_player_rider_audio_events(void);
void
update_engine_audio_modulation(void);
void
find_nearest_engine_audio_objects(RacerEntity **nearest_object,
                                  RacerEntity **second_nearest_object);
void
update_race_engine_audio_sources(void);
int
process_race_audio_event_queue(void);
int
service_race_audio_events(void);

#endif
