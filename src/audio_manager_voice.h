#ifndef ROADRASH_AUDIO_MANAGER_VOICE_H
#define ROADRASH_AUDIO_MANAGER_VOICE_H

typedef struct AudioManagerVoice AudioManagerVoice;

enum AudioManagerVoiceControlValue
{
  AUDIO_MANAGER_VOICE_MAXIMUM_STEREO_SPREAD = 0x7F
};

#define AUDIO_MANAGER_INVALID_VOICE_ERROR (-17)

AudioManagerVoice *
resolve_audio_manager_voice(int voice_item);
int
get_audio_manager_voice_status(AudioManagerVoice *voice);
int
is_audio_manager_voice_active(int voice_item);
int
select_audio_manager_voice_sample(int voice_item,
                                  int sample_item,
                                  int sample_mode);
int
add_audio_manager_voice_sample(int voice_id,
                               int sample_item,
                               int sample_mode);
int
select_audio_manager_voice_sample_chain(int  voice_id,
                                        int *sample_items,
                                        int  sample_count,
                                        int  sample_mode);
int
own_audio_manager_sample_voice(int sample_item,
                               int sample_mode);
int
play_audio_manager_sample_chain(int *sample_items,
                                int  sample_count,
                                int  playback_rate,
                                int  amplitude,
                                int  pan,
                                int  requested_sort_key,
                                int  sample_chain_option);
int
play_audio_manager_sample(int sample_item,
                          int playback_rate,
                          int amplitude,
                          int pan,
                          int requested_sort_key);
int
disown_audio_manager_sample_voice(int voice_item);
int
own_audio_manager_patch_voice(const char *instrument_name);
int
disown_audio_manager_voice(int voice_item);
int
allocate_audio_manager_voice_instrument(int voice_item,
                                        int lock_state_on_failure);
int
get_audio_manager_voice_instrument_item(int voice_item);
int
release_audio_manager_voice_instrument(int voice_item);
int
lock_audio_manager_voice(int voice_item);
int
unlock_audio_manager_voice(int voice_item);
int
get_audio_manager_voice_sample_signal(int voice_item,
                                      int sample_selector);
int
set_audio_manager_voice_stereo_spread(int voice_item,
                                      int spread);
int
set_audio_manager_voice_group_sort_key(int voice_item,
                                       int sort_key);
int
start_audio_manager_voice(int voice_item,
                          int playback_rate,
                          int amplitude,
                          int pan);
int
stop_audio_manager_voice(int voice_item);
int
pause_audio_manager_voice(int voice_item);
int
resume_audio_manager_voice(int voice_item);
int
release_audio_manager_voice(int voice_item);
int
tweak_audio_manager_voice(int voice_item,
                          int frequency,
                          int amplitude,
                          int pan);
int
stop_race_audio_voice(int voice_item);

#endif
