#ifndef ROADRASH_AUDIO_MIXER_RUNTIME_H
#define ROADRASH_AUDIO_MIXER_RUNTIME_H

#include "runtime_data_types.h"

#include "types.h"

#ifndef offsetof
  #include "stddef.h"
#endif

enum AudioMixerLayoutConstant
{
  AUDIO_SUBMIXER_INPUT_CAPACITY = 16,
  AUDIO_MIXER_TEMPLATE_COUNT = 4,
  AUDIO_MIXER_SUBMIXER_CAPACITY = 4,
  AUDIO_MIXER_CHANNEL_SOURCE_NAME_CAPACITY = 0x10,
  AUDIO_MIXER_CHANNEL_STATE_SIZE = 0x18,
  AUDIO_SUBMIXER_RUNTIME_SIZE =
    ((0xAC + ((2 * AUDIO_SUBMIXER_INPUT_CAPACITY) * sizeof(s32))) +
     (2 * sizeof(u32))),
  AUDIO_MIXER_RUNTIME_SIZE = 0x38,
  AUDIO_MIXER_ENABLE_BYTE_MASK = 0xFF,
  AUDIO_MIXER_FULL_SCALE_AMPLITUDE = 0x7FFF
};

enum AudioMixerTemplateIndex
{
  AUDIO_MIXER_TEMPLATE_2_INPUT = 0,
  AUDIO_MIXER_TEMPLATE_4_INPUT = 1,
  AUDIO_MIXER_TEMPLATE_8_INPUT = 2,
  AUDIO_MIXER_TEMPLATE_12_INPUT = 3
};

typedef struct AudioSubmixerRuntime
{
  int instrument_item;
  int input_count;
  int maximum_amplitude;
  int left_gain_knobs[AUDIO_SUBMIXER_INPUT_CAPACITY];
  int right_gain_knobs[AUDIO_SUBMIXER_INPUT_CAPACITY];
  unsigned char left_gain_levels[AUDIO_SUBMIXER_INPUT_CAPACITY];
  unsigned char right_gain_levels[AUDIO_SUBMIXER_INPUT_CAPACITY];
  // Runtime-only state; validity means the last write to this knob returned zero.
  s32 applied_left_amplitudes[AUDIO_SUBMIXER_INPUT_CAPACITY];
  s32 applied_right_amplitudes[AUDIO_SUBMIXER_INPUT_CAPACITY];
  u32 valid_left_amplitudes;
  u32 valid_right_amplitudes;
} AudioSubmixerRuntime;

typedef struct AudioMixerChannelState
{
  int connection_key;
  int source_instrument;
  char source_name[AUDIO_MIXER_CHANNEL_SOURCE_NAME_CAPACITY];
} AudioMixerChannelState;

struct AudioMixerRuntime
{
  int input_count;
  int template_items[AUDIO_MIXER_TEMPLATE_COUNT];
  AudioSubmixerRuntime *submixers[AUDIO_MIXER_SUBMIXER_CAPACITY];
  int submixer_count;
  AudioMixerChannelState *channels;
  int connection_count;
  int maximum_amplitude;
  unsigned char enabled;
  unsigned char enabled_alignment[3];
};

typedef char AudioSubmixerInstrumentOffsetCheck[
    (offsetof(AudioSubmixerRuntime, instrument_item) == 0x00) ? 1 : -1];
typedef char AudioSubmixerLeftKnobOffsetCheck[
    (offsetof(AudioSubmixerRuntime, left_gain_knobs) == 0x0C) ? 1 : -1];
typedef char AudioSubmixerRightKnobOffsetCheck[
    (offsetof(AudioSubmixerRuntime, right_gain_knobs) == 0x4C) ? 1 : -1];
typedef char AudioSubmixerLeftLevelOffsetCheck[
    (offsetof(AudioSubmixerRuntime, left_gain_levels) == 0x8C) ? 1 : -1];
typedef char AudioSubmixerRightLevelOffsetCheck[
    (offsetof(AudioSubmixerRuntime, right_gain_levels) == 0x9C) ? 1 : -1];
typedef char AudioSubmixerSizeCheck[
    (sizeof(AudioSubmixerRuntime) == AUDIO_SUBMIXER_RUNTIME_SIZE) ? 1 : -1];
typedef char AudioMixerChannelStateSizeCheck[
    (sizeof(AudioMixerChannelState) == AUDIO_MIXER_CHANNEL_STATE_SIZE)
        ? 1 : -1];
typedef char AudioMixerTemplatesOffsetCheck[
    (offsetof(AudioMixerRuntime, template_items) == 0x04) ? 1 : -1];
typedef char AudioMixerSubmixersOffsetCheck[
    (offsetof(AudioMixerRuntime, submixers) == 0x14) ? 1 : -1];
typedef char AudioMixerChannelTableOffsetCheck[
    (offsetof(AudioMixerRuntime, channels) == 0x28) ? 1 : -1];
typedef char AudioMixerEnabledOffsetCheck[
    (offsetof(AudioMixerRuntime, enabled) == 0x34) ? 1 : -1];
typedef char AudioMixerSizeCheck[
    (sizeof(AudioMixerRuntime) == AUDIO_MIXER_RUNTIME_SIZE) ? 1 : -1];

AudioSubmixerRuntime *
create_audio_submixer(int input_count,
                      int template_item);
void
initialize_audio_submixer(AudioSubmixerRuntime *submixer);
void
destroy_audio_submixer(AudioSubmixerRuntime *submixer);
int
bind_audio_submixer_input_knobs(AudioSubmixerRuntime *submixer,
                                int                   input_index);
int
connect_audio_submixer_input(AudioSubmixerRuntime *submixer,
                             int                   source_instrument,
                             char                 *source_name);
int
disconnect_audio_submixer_input(AudioSubmixerRuntime *submixer,
                                int                   source_instrument,
                                char                 *source_name,
                                int                   input_index);
int
set_audio_submixer_input_pan(AudioSubmixerRuntime *submixer,
                             int                   input_index,
                             int                   pan);
void
set_audio_submixer_maximum_amplitude(AudioSubmixerRuntime *submixer,
                                     int                   maximum_amplitude);
void
apply_audio_submixer_input_pans(AudioSubmixerRuntime *submixer);
void
initialize_audio_mixer(AudioMixerRuntime *mixer,
                       int                input_count);
AudioMixerRuntime *
create_audio_mixer(int input_count,
                   int template_image);
void
destroy_audio_mixer(AudioMixerRuntime *mixer);
void
unload_audio_mixer_templates(AudioMixerRuntime *mixer);
int
disconnect_all_audio_mixer_channels(AudioMixerRuntime *mixer);
int
set_audio_mixer_enabled(AudioMixerRuntime *mixer,
                        int                enabled);
int
get_audio_mixer_maximum_amplitude(AudioMixerRuntime *mixer);
int
connect_audio_mixer_source(AudioMixerRuntime *mixer,
                           int                source_instrument,
                           char              *source_name);
int
disconnect_audio_mixer_source(AudioMixerRuntime *mixer,
                              int                connection_key);
int
register_audio_mixer_connection(AudioMixerRuntime *mixer,
                                int                connection_key,
                                int                source_instrument,
                                char              *source_name);
void
unregister_audio_mixer_connection(AudioMixerRuntime *mixer,
                                  int                connection_key);
AudioMixerChannelState *
find_audio_mixer_connection(AudioMixerRuntime *mixer,
                            int                connection_key);
int
set_audio_mixer_connection_pan(AudioMixerRuntime *mixer,
                               int                connection_key,
                               int                pan);
int
set_audio_mixer_maximum_amplitude(AudioMixerRuntime *mixer,
                                  int                maximum_amplitude);
int
apply_audio_mixer_gain_levels(AudioMixerRuntime *mixer);
int
load_audio_mixer_templates(AudioMixerRuntime *mixer,
                           int                template_image,
                           int                mixer_2_count,
                           int                mixer_4_count,
                           int                mixer_8_count,
                           int                mixer_12_count);
int
configure_global_audio_mixer(int input_count,
                             int template_image);
int
get_global_audio_mixer_maximum_amplitude(void);
int
set_global_audio_mixer_maximum_amplitude(int maximum_amplitude);
int
set_global_audio_mixer_enabled(int enabled);

#endif
