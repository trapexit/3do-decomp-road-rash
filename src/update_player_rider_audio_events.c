#include "audio_manager_voice.h"
#include "race_audio_runtime.h"
#include "road_geometry.h"
#include "rw_semantic_data.h"
#include "spatial_audio_event_queue.h"

enum PlayerRiderAudioConstant
{
  PLAYER_RIDER_MODE_NORMAL = 0,
  PLAYER_RIDER_MODE_RECOVERY = 1,
  PLAYER_RIDER_MODE_STOPPED_OR_INTERACTION_ALIGN = 2,
  PLAYER_RIDER_MODE_INTERACTION_FINISH = 3,
  PLAYER_RIDER_SLIDING_EVENT = 19,
  PLAYER_RIDER_REPEATED_EVENT = 15,
  PLAYER_RIDER_SLIDING_MINIMUM_SPEED = 0x320,
  PLAYER_RIDER_SLIDING_MAXIMUM_HEIGHT = 0x190,
  PLAYER_RIDER_REPEATED_EVENT_MINIMUM_SPEED = 5
};

void
update_player_rider_audio_events(void)
{
  RacerEntity *player;
  RacerEntity *paired_state;
  int rider_mode;

  player = (RacerEntity *)race_rider_state.player_object;
  update_rider_skid_audio_events(player);

  player = (RacerEntity *)race_rider_state.player_object;
  game_audio_state.alternate_engine_sound_active =
    (unsigned char)(player->primary_track->geometry_mode ==
                    ROAD_GEOMETRY_MODE_CURVED);

  rider_mode = player->rider_mode;

  if(rider_mode == PLAYER_RIDER_MODE_NORMAL)
    {
      if(is_audio_manager_voice_active(
           game_audio_state.effect_channels[0]
           .auxiliary_voice_item) != 0)
        {
          stop_race_audio_voice(
            game_audio_state.effect_channels[0]
            .auxiliary_voice_item);
        }
    }
  else if(rider_mode == PLAYER_RIDER_MODE_RECOVERY)
    {
      paired_state = player->paired_state;
      if(paired_state->velocity.forward <=
         PLAYER_RIDER_SLIDING_MINIMUM_SPEED ||
         paired_state->position_y >=
         PLAYER_RIDER_SLIDING_MAXIMUM_HEIGHT)
        {
          if(is_audio_manager_voice_active(
               game_audio_state.effect_channels[0]
               .auxiliary_voice_item) != 0)
            {
              stop_race_audio_voice(
                game_audio_state.effect_channels[0]
                .auxiliary_voice_item);
            }
        }
      else
        {
          queue_spatial_audio_event(
            PLAYER_RIDER_SLIDING_EVENT,
            &paired_state->position_z,
            &paired_state->velocity.forward);
        }
    }
  else if(rider_mode ==
          PLAYER_RIDER_MODE_STOPPED_OR_INTERACTION_ALIGN)
    {
      if(is_audio_manager_voice_active(
           game_audio_state.effect_channels[0]
           .auxiliary_voice_item) != 0)
        {
          stop_race_audio_voice(
            game_audio_state.effect_channels[0]
            .auxiliary_voice_item);
        }

      player = (RacerEntity *)race_rider_state.player_object;
      paired_state = player->paired_state;
      if(paired_state->velocity.forward >
         PLAYER_RIDER_REPEATED_EVENT_MINIMUM_SPEED)
        {
          queue_spatial_audio_event(
            PLAYER_RIDER_REPEATED_EVENT,
            &paired_state->position_z,
            &paired_state->velocity.forward);
        }
    }
  else if(rider_mode == PLAYER_RIDER_MODE_INTERACTION_FINISH)
    {
      if(is_audio_manager_voice_active(
           game_audio_state.effect_channels[0]
           .auxiliary_voice_item) != 0)
        {
          stop_race_audio_voice(
            game_audio_state.effect_channels[0]
            .auxiliary_voice_item);
        }
    }

  player = (RacerEntity *)race_rider_state.player_object;
  game_audio_state.last_player_mode = player->rider_mode;
}
