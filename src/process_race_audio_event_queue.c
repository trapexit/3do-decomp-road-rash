#include "stdlib.h"

#include "aiff_sample_runtime.h"
#include "audio_manager_voice.h"
#include "race_audio_runtime.h"
#include "rw_semantic_data.h"
#include "spatial_audio_event_binding.h"
#include "spatial_audio_event_queue.h"

typedef struct RaceAudioDivisionResult
{
  unsigned int quotient;
  unsigned int remainder;
} RaceAudioDivisionResult;

__value_in_regs RaceAudioDivisionResult
unsigned_divide_with_remainder(unsigned int divisor,
                               unsigned int numerator);
int
unload_sample_image(int sample_item,
                    int flags);

/*
 * Names for events 2 through 7 describe channel preference only. Multiple
 * contact and recovery producers share those values.
 */
enum RaceAudioEventType
{
  RACE_AUDIO_PRIMARY_PREFERRED_A = 2,
  RACE_AUDIO_PRIMARY_PREFERRED_B = 3,
  RACE_AUDIO_PRIMARY_PREFERRED_C = 4,
  RACE_AUDIO_SECONDARY_PREFERRED_A = 5,
  RACE_AUDIO_SECONDARY_PREFERRED_B = 6,
  RACE_AUDIO_SECONDARY_PREFERRED_C = 7,
  RACE_AUDIO_GUARDED_PRIMARY_A = 8,
  RACE_AUDIO_GUARDED_PRIMARY_B = 9,
  RACE_AUDIO_DIRECT_PRIMARY_A = 10,
  RACE_AUDIO_DIRECT_PRIMARY_B = 11,
  RACE_AUDIO_DIRECT_PRIMARY_C = 12,
  RACE_AUDIO_DIRECT_PRIMARY_D = 13,
  RACE_AUDIO_DIRECT_PRIMARY_E = 14,
  RACE_AUDIO_REPEATED_PRIMARY = 15,
  RACE_AUDIO_FIXED_VOLUME_PRIMARY = 16,
  RACE_AUDIO_BOOST_STARTED = 17,
  RACE_AUDIO_SLIDING_UPDATE = 19,
  RACE_AUDIO_SECONDARY_AUXILIARY = 20,
  RACE_AUDIO_PURSUIT_HORN = 21,
  RACE_AUDIO_PURSUIT_SIREN_A = 22,
  RACE_AUDIO_PURSUIT_SIREN_B = 23,
  RACE_AUDIO_PLAYER_BOUNCE = 26,
  RACE_AUDIO_DUAL_CHANNEL_VARIANT = 27,
  RACE_AUDIO_BOOST_STOPPED = 28
};

enum RaceAudioChannelIndex
{
  PRIMARY_EFFECT_CHANNEL = 0,
  SECONDARY_EFFECT_CHANNEL = 1
};

enum RaceAudioSampleIndex
{
  ENGINE_SAMPLE = 0,
  SECONDARY_VARIANT_SAMPLE = 2,
  PRIMARY_VARIANT_SAMPLE = 5,
  GUARDED_PRIMARY_A_SAMPLE = 10,
  GUARDED_PRIMARY_B_SAMPLE = 11,
  REPEATED_PRIMARY_SAMPLE = 15,
  FIXED_VOLUME_PRIMARY_SAMPLE = 16
};

enum RaceAudioModulationIndex
{
  PRIMARY_EFFECT_PITCH_INDEX = 3,
  SECONDARY_EFFECT_PITCH_INDEX = 4,
  PRIMARY_EFFECT_MODULATION_INDEX = 2,
  SECONDARY_EFFECT_MODULATION_INDEX = 3
};

enum RaceAudioPlaybackConstants
{
  CENTER_PAN = 0x40,
  MAX_VOLUME = 0x7FFF,
  FRAME_VARIANT_MASK = 1,
  REPEATED_EVENT_RESET_COUNT = 5,
  TRANSIENT_SAMPLE_OWNS_DATA = 1,

  PRIMARY_PREFERRED_VOLUME_DIVISOR = 0x4001,
  PRIMARY_PREFERRED_VOLUME_BASE = 0x3FFF,
  NARROW_PITCH_DIVISOR = 0x1F41,
  COMMON_PITCH_BASE = 0x62B4,

  STANDARD_VOLUME_DIVISOR = 0x2001,
  STANDARD_VOLUME_BASE = 0x5FFF,
  STANDARD_PITCH_DIVISOR = 0x3A99,

  GUARDED_PRIMARY_A_PITCH_DIVISOR = 0x32C9,
  GUARDED_PRIMARY_A_PITCH_BASE = 0x6A84,

  REPEATED_VOLUME_DIVISOR = 0x04B1,
  REPEATED_VOLUME_BASE = 0x4BC8,
  REPEATED_PITCH_DIVISOR = 0x332D,
  REPEATED_PITCH_BASE = 0x666A,

  BOUNCE_VOLUME_DIVISOR = 0x1334,
  BOUNCE_VOLUME_BASE = 0x1333,
  BOUNCE_PITCH_DIVISOR = 0x2135,

  DYNAMIC_SAMPLE_PITCH_DIVISOR = 0x0A01,
  DYNAMIC_SAMPLE_PITCH_BASE = 0x7B00,

  PRIMARY_FADE_STEP = 0x60,
  SECONDARY_FADE_STEP = 0x70
};

static
unsigned int
random_remainder(unsigned int divisor)
{
  RaceAudioDivisionResult result;

  result = unsigned_divide_with_remainder(
    divisor, (unsigned int)rand());
  return result.remainder;
}


static
int
audio_voice_is_active(const AudioVoiceSlot *voice)
{
  return is_audio_manager_voice_active(voice->voice_item);
}


static
int
play_audio_voice(AudioVoiceSlot *voice,
                 int             sample_item,
                 int             pitch,
                 int             volume,
                 int             pan)
{
  return play_or_update_audio_voice_slot(
    voice, sample_item, pitch, volume, pan);
}


static
int
play_randomized_audio_voice(AudioVoiceSlot *voice,
                            int             sample_item,
                            unsigned int    volume_divisor,
                            int             volume_base,
                            unsigned int    pitch_divisor,
                            int             pitch_base)
{
  int volume;
  int pitch;

  volume = (int)random_remainder(volume_divisor) + volume_base;
  pitch = (int)random_remainder(pitch_divisor) + pitch_base;
  return play_audio_voice(
    voice, sample_item, pitch, volume, CENTER_PAN);
}


static
void
play_primary_preferred_event(const SpatialAudioEvent *event)
{
  AudioVoiceSlot *primary;
  AudioVoiceSlot *secondary;

  /* The original always targets primary after confirming a channel is free. */
  primary =
    &game_audio_state.effect_channels[PRIMARY_EFFECT_CHANNEL].channel;
  secondary =
    &game_audio_state.effect_channels[SECONDARY_EFFECT_CHANNEL].channel;
  if(audio_voice_is_active(primary) != 0)
    {
      if(audio_voice_is_active(secondary) != 0)
        {
          return;
        }
    }

  play_randomized_audio_voice(
    primary, game_audio_state.sound_sample_items[event->event_type],
    PRIMARY_PREFERRED_VOLUME_DIVISOR,
    PRIMARY_PREFERRED_VOLUME_BASE,
    NARROW_PITCH_DIVISOR, COMMON_PITCH_BASE);
}


static
void
play_secondary_preferred_event(const SpatialAudioEvent *event)
{
  AudioVoiceSlot *primary;
  AudioVoiceSlot *secondary;
  AudioVoiceSlot *voice;

  /* Prefer secondary when free, then fall back to an idle primary. */
  primary =
    &game_audio_state.effect_channels[PRIMARY_EFFECT_CHANNEL].channel;
  secondary =
    &game_audio_state.effect_channels[SECONDARY_EFFECT_CHANNEL].channel;
  if(audio_voice_is_active(secondary) == 0)
    {
      voice = secondary;
    }
  else
    {
      if(audio_voice_is_active(primary) != 0)
        {
          return;
        }
      voice = primary;
    }

  play_randomized_audio_voice(
    voice, game_audio_state.sound_sample_items[event->event_type],
    STANDARD_VOLUME_DIVISOR, STANDARD_VOLUME_BASE,
    STANDARD_PITCH_DIVISOR, COMMON_PITCH_BASE);
}


static
void
play_dynamic_spatial_sample(SpatialAudioEvent *event)
{
  AudioEffectChannel *secondary;
  int sample_item;
  int ignored_pitch;
  int volume;
  int pan;
  int pitch;

  secondary =
    &game_audio_state.effect_channels[SECONDARY_EFFECT_CHANNEL];
  if(is_audio_manager_voice_active(
       secondary->auxiliary_voice_item) != 0 ||
     audio_voice_is_active(&secondary->channel) != 0)
    {
      return;
    }

  if(game_audio_state.transient_sample_item > 0)
    {
      unload_sample_image(
        game_audio_state.transient_sample_item,
        TRANSIENT_SAMPLE_OWNS_DATA);
    }

  sample_item = load_spatial_audio_event_sample_copy(event);
  game_audio_state.transient_sample_item = sample_item;
  if(sample_item <= 0)
    {
      return;
    }

  event->sample_image = 0;
  calculate_spatial_audio_parameters(
    0, event, &ignored_pitch, &volume, &pan);
  pitch = (int)random_remainder(DYNAMIC_SAMPLE_PITCH_DIVISOR) +
          DYNAMIC_SAMPLE_PITCH_BASE;
  play_audio_voice(&secondary->channel, sample_item, pitch, volume, pan);
}


static
void
dispatch_race_audio_event(SpatialAudioEvent *event)
{
  AudioEffectChannel *primary_effect;
  AudioEffectChannel *secondary_effect;
  AudioVoiceSlot *primary;
  int previous_countdown;
  int volume;
  int pitch;
  int sample_selector;

  primary_effect =
    &game_audio_state.effect_channels[PRIMARY_EFFECT_CHANNEL];
  secondary_effect =
    &game_audio_state.effect_channels[SECONDARY_EFFECT_CHANNEL];
  primary = &primary_effect->channel;

  switch(event->event_type)
    {
    case RACE_AUDIO_PRIMARY_PREFERRED_A:
    case RACE_AUDIO_PRIMARY_PREFERRED_B:
    case RACE_AUDIO_PRIMARY_PREFERRED_C:
      play_primary_preferred_event(event);
      break;

    case RACE_AUDIO_SECONDARY_PREFERRED_A:
    case RACE_AUDIO_SECONDARY_PREFERRED_B:
    case RACE_AUDIO_SECONDARY_PREFERRED_C:
      play_secondary_preferred_event(event);
      break;

    case RACE_AUDIO_GUARDED_PRIMARY_A:
      if(audio_voice_is_active(primary) == 0 ||
         primary->sample_item !=
         game_audio_state
         .sound_sample_items[GUARDED_PRIMARY_A_SAMPLE])
        {
          pitch = (int)random_remainder(
            GUARDED_PRIMARY_A_PITCH_DIVISOR) +
                  GUARDED_PRIMARY_A_PITCH_BASE;
          play_audio_voice(
            primary,
            game_audio_state.sound_sample_items[event->event_type],
            pitch, MAX_VOLUME, CENTER_PAN);
        }
      break;

    case RACE_AUDIO_GUARDED_PRIMARY_B:
      if(audio_voice_is_active(primary) == 0 ||
         primary->sample_item !=
         game_audio_state
         .sound_sample_items[GUARDED_PRIMARY_B_SAMPLE])
        {
          play_randomized_audio_voice(
            primary,
            game_audio_state.sound_sample_items[event->event_type],
            STANDARD_VOLUME_DIVISOR, STANDARD_VOLUME_BASE,
            NARROW_PITCH_DIVISOR, COMMON_PITCH_BASE);
        }
      break;

    case RACE_AUDIO_DIRECT_PRIMARY_A:
    case RACE_AUDIO_DIRECT_PRIMARY_B:
    case RACE_AUDIO_DIRECT_PRIMARY_C:
    case RACE_AUDIO_DIRECT_PRIMARY_D:
    case RACE_AUDIO_DIRECT_PRIMARY_E:
      play_randomized_audio_voice(
        primary,
        game_audio_state.sound_sample_items[event->event_type],
        STANDARD_VOLUME_DIVISOR, STANDARD_VOLUME_BASE,
        NARROW_PITCH_DIVISOR, COMMON_PITCH_BASE);
      break;

    case RACE_AUDIO_REPEATED_PRIMARY:
      if(audio_voice_is_active(primary) == 0 ||
         primary->sample_item !=
         game_audio_state
         .sound_sample_items[REPEATED_PRIMARY_SAMPLE])
        {
          previous_countdown = game_audio_state.repeated_event_countdown;
          game_audio_state.repeated_event_countdown =
            previous_countdown - 1;
          if(previous_countdown <= 0)
            {
              play_randomized_audio_voice(
                primary,
                game_audio_state
                .sound_sample_items[REPEATED_PRIMARY_SAMPLE],
                REPEATED_VOLUME_DIVISOR, REPEATED_VOLUME_BASE,
                REPEATED_PITCH_DIVISOR, REPEATED_PITCH_BASE);
              game_audio_state.repeated_event_countdown =
                REPEATED_EVENT_RESET_COUNT;
            }
        }
      break;

    case RACE_AUDIO_FIXED_VOLUME_PRIMARY:
      if(primary->sample_item !=
         game_audio_state
         .sound_sample_items[FIXED_VOLUME_PRIMARY_SAMPLE] ||
         audio_voice_is_active(primary) == 0)
        {
          pitch = (int)random_remainder(STANDARD_PITCH_DIVISOR) +
                  COMMON_PITCH_BASE;
          play_audio_voice(
            primary,
            game_audio_state
            .sound_sample_items[FIXED_VOLUME_PRIMARY_SAMPLE],
            pitch, MAX_VOLUME, CENTER_PAN);
        }
      break;

    case RACE_AUDIO_BOOST_STARTED:
      game_audio_state.boost_sound_active = 1;
      break;

    case RACE_AUDIO_SLIDING_UPDATE:
      update_rider_sliding_audio(
        primary_effect->auxiliary_voice_item, event);
      break;

    case RACE_AUDIO_SECONDARY_AUXILIARY:
      if(is_audio_manager_voice_active(
           secondary_effect->auxiliary_voice_item) == 0 &&
         audio_voice_is_active(&secondary_effect->channel) == 0)
        {
          play_offroad_skid_audio(
            secondary_effect->auxiliary_voice_item, event);
        }
      break;

    case RACE_AUDIO_PURSUIT_HORN:
      start_spatial_audio_voice(
        game_audio_state.pursuit_voices.horn_voice_item, event);
      break;

    case RACE_AUDIO_PURSUIT_SIREN_A:
    case RACE_AUDIO_PURSUIT_SIREN_B:
      update_pursuit_siren_audio(
        game_audio_state.pursuit_voices.siren_voice_item,
        event);
      break;

    case RACE_AUDIO_PLAYER_BOUNCE:
      if(audio_voice_is_active(&secondary_effect->channel) == 0)
        {
          play_randomized_audio_voice(
            &secondary_effect->channel,
            game_audio_state
            .sound_sample_items[SECONDARY_VARIANT_SAMPLE],
            BOUNCE_VOLUME_DIVISOR, BOUNCE_VOLUME_BASE,
            BOUNCE_PITCH_DIVISOR, COMMON_PITCH_BASE);
        }
      break;

    case RACE_AUDIO_DUAL_CHANNEL_VARIANT:
      if(audio_voice_is_active(primary) == 0)
        {
          volume = (int)random_remainder(STANDARD_VOLUME_DIVISOR) +
                   STANDARD_VOLUME_BASE;
          pitch = (int)random_remainder(STANDARD_PITCH_DIVISOR) +
                  COMMON_PITCH_BASE;
          sample_selector =
            (int)(frame_tick & FRAME_VARIANT_MASK);
          play_audio_voice(
            primary,
            game_audio_state.sound_sample_items[
              sample_selector + PRIMARY_VARIANT_SAMPLE],
            pitch, volume, CENTER_PAN);
        }
      if(audio_voice_is_active(&secondary_effect->channel) == 0)
        {
          volume = (int)random_remainder(STANDARD_VOLUME_DIVISOR) +
                   STANDARD_VOLUME_BASE;
          pitch = (int)random_remainder(STANDARD_PITCH_DIVISOR) +
                  COMMON_PITCH_BASE;
          sample_selector =
            (int)(frame_tick & FRAME_VARIANT_MASK);
          play_audio_voice(
            &secondary_effect->channel,
            game_audio_state.sound_sample_items[
              sample_selector + SECONDARY_VARIANT_SAMPLE],
            pitch, volume, CENTER_PAN);
        }
      break;

    case RACE_AUDIO_BOOST_STOPPED:
      game_audio_state.boost_sound_active = 0;
      break;

    case SPATIAL_AUDIO_EVENT_TRANSIENT_SAMPLE:
      play_dynamic_spatial_sample(event);
      break;
    }
}


static
int
finish_effect_channel_fades(void)
{
  AudioVoiceSlot *primary;
  AudioVoiceSlot *secondary;
  int remaining_volume;

  primary =
    &game_audio_state.effect_channels[PRIMARY_EFFECT_CHANNEL].channel;
  secondary =
    &game_audio_state.effect_channels[SECONDARY_EFFECT_CHANNEL].channel;

  if(game_audio_state.voice_volume[PRIMARY_EFFECT_MODULATION_INDEX] > 0 &&
     audio_voice_is_active(primary) != 0 &&
     primary->sample_item ==
     game_audio_state.sound_sample_items[ENGINE_SAMPLE])
    {
      remaining_volume =
        game_audio_state
        .voice_volume[PRIMARY_EFFECT_MODULATION_INDEX] -
        PRIMARY_FADE_STEP;
      game_audio_state.voice_volume[PRIMARY_EFFECT_MODULATION_INDEX] =
        remaining_volume;
      if(remaining_volume <= 0)
        {
          stop_audio_manager_voice(primary->voice_item);
        }
      else
        {
          play_audio_voice(
            primary,
            game_audio_state.sound_sample_items[ENGINE_SAMPLE],
            game_audio_state.voice_pitch[PRIMARY_EFFECT_PITCH_INDEX],
            remaining_volume,
            game_audio_state.voice_pan[PRIMARY_EFFECT_MODULATION_INDEX]);
        }
    }

  if(game_audio_state.voice_volume[SECONDARY_EFFECT_MODULATION_INDEX] <=
     0)
    {
      return game_audio_state
             .voice_volume[SECONDARY_EFFECT_MODULATION_INDEX];
    }
  if(audio_voice_is_active(secondary) == 0)
    {
      return 0;
    }
  if(secondary->sample_item !=
     game_audio_state.sound_sample_items[ENGINE_SAMPLE])
    {
      return secondary->sample_item;
    }

  remaining_volume =
    game_audio_state
    .voice_volume[SECONDARY_EFFECT_MODULATION_INDEX] -
    SECONDARY_FADE_STEP;
  game_audio_state.voice_volume[SECONDARY_EFFECT_MODULATION_INDEX] =
    remaining_volume;
  if(remaining_volume <= 0)
    {
      return stop_audio_manager_voice(secondary->voice_item);
    }
  return play_audio_voice(
    secondary, game_audio_state.sound_sample_items[ENGINE_SAMPLE],
    game_audio_state.voice_pitch[SECONDARY_EFFECT_PITCH_INDEX],
    remaining_volume,
    game_audio_state.voice_pan[SECONDARY_EFFECT_MODULATION_INDEX]);
}


int
process_race_audio_event_queue(void)
{
  SpatialAudioEvent *event;

  for(;;)
    {
      event = dequeue_spatial_audio_event();
      if(event == 0)
        {
          return finish_effect_channel_fades();
        }
      dispatch_race_audio_event(event);
      clear_spatial_audio_event_family_binding(event);
    }
}
