#include "audio_manager_voice.h"
#include "race_audio_runtime.h"
#include "rw_semantic_data.h"

enum RiderSkidAudioEventConstant
{
  RIDER_ROAD_SKID_EVENT = 0x10,
  RIDER_OFFROAD_SKID_EVENT = 0x14,
  RIDER_ROAD_SKID_SAMPLE_INDEX = 16,
  RIDER_SKID_SLIP_THRESHOLD = 0x32,
  RIDER_NORMAL_MODE = 0,
  RIDER_ROAD_SURFACE_DRAG_INDEX = 1,
  PRIMARY_EFFECT_CHANNEL = 0,
  SECONDARY_EFFECT_CHANNEL = 1
};

int
update_rider_skid_audio_events(const RacerEntity *rider)
{
  AudioEffectChannel *primary_effect;
  AudioEffectChannel *secondary_effect;
  int road_skid_sample_item;
  signed char road_surface_drag_limit;

  primary_effect =
    &game_audio_state.effect_channels[PRIMARY_EFFECT_CHANNEL];
  secondary_effect =
    &game_audio_state.effect_channels[SECONDARY_EFFECT_CHANNEL];

  if(rider->surface_contact_scale_8_8 != 0 &&
     rider->world_velocity.forward > 0 &&
     rider->slip_amount > RIDER_SKID_SLIP_THRESHOLD &&
     rider->rider_mode == RIDER_NORMAL_MODE)
    {
      road_surface_drag_limit =
        rider_physics_tuning
        .surface_drag[RIDER_ROAD_SURFACE_DRAG_INDEX];

      if(road_surface_drag_limit < rider->surface_drag)
        {
          queue_spatial_audio_event(
            RIDER_OFFROAD_SKID_EVENT,
            &rider->position_z, &rider->velocity.forward);

          road_skid_sample_item =
            game_audio_state
            .sound_sample_items[RIDER_ROAD_SKID_SAMPLE_INDEX];
          if(primary_effect->channel.sample_item !=
             road_skid_sample_item)
            {
              return road_skid_sample_item;
            }
          if(is_audio_manager_voice_active(
               primary_effect->channel.voice_item) != 0)
            {
              return stop_audio_manager_voice(
                primary_effect->channel.voice_item);
            }
          return 0;
        }

      queue_spatial_audio_event(
        RIDER_ROAD_SKID_EVENT,
        &rider->position_z, &rider->velocity.forward);
      if(is_audio_manager_voice_active(
           secondary_effect->auxiliary_voice_item) != 0)
        {
          return stop_audio_manager_voice(
            secondary_effect->auxiliary_voice_item);
        }
      return 0;
    }

  road_skid_sample_item =
    game_audio_state
    .sound_sample_items[RIDER_ROAD_SKID_SAMPLE_INDEX];
  if(primary_effect->channel.sample_item == road_skid_sample_item &&
     is_audio_manager_voice_active(
       primary_effect->channel.voice_item) != 0)
    {
      stop_audio_manager_voice(primary_effect->channel.voice_item);
    }

  if(is_audio_manager_voice_active(
       secondary_effect->auxiliary_voice_item) != 0)
    {
      /* This policy bypasses the race-level stop veneer. */
      return stop_audio_manager_voice(
        secondary_effect->auxiliary_voice_item);
    }
  return 0;
}
