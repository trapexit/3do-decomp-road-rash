#include "bss_late_data.h"
#include "racer_creation_runtime.h"
#include "rw_semantic_data.h"

#include "string.h"

enum RaceRiderInitializationConstant
{
  RIDER_RESOURCE_INDEX_SHIFT = 8,
  RIDER_RESOURCE_INDEX_MASK = 0x7F,
  RIDER_RESOURCE_LANE_INDEX = 2,
  RIDER_STYLE_SHIFT = 16,
  RIDER_STYLE_MASK = 0xFF,
  RACE_MODE_SINGLE_PLAYER = 0,
  OPPONENT_ALTERNATE_REFERENCE_MIN_LEVEL = 4,
  OPPONENT_NAME_DESTINATION_BASE = 10,
  PLAYER_RIDER_TYPE_COUNT = 10
};

void
load_bike_specs_resource(void);
void
initialize_race_object_scheduler(void);
void
initialize_racer_object_pool(void);
void
initialize_racer_state_pool(void);
int
initialize_race_boost_state(const RaceBoostConfig *configuration);

static
RiderProfile *
selected_rider_profile(void)
{
  return &player_profiles[race_mode_state.active_player_index];
}


static
RoadSegmentLaneRuntime *
rider_resource_lane(unsigned int packed_resource_reference)
{
  unsigned int resource_index;
  RoadSegmentRuntime *segment;

  resource_index =
    (packed_resource_reference >> RIDER_RESOURCE_INDEX_SHIFT) &
    RIDER_RESOURCE_INDEX_MASK;
  segment = gRoadSegmentEffectWorkspace.segment_index
            .by_track_index[resource_index];
  if(segment == 0)
    {
      return 0;
    }
  return segment->selected_lanes[RIDER_RESOURCE_LANE_INDEX];
}


static
unsigned char
runtime_rider_style(const RiderRuntimeSlot *runtime)
{
  return (unsigned char)(
    (runtime->rider_flags_and_style >> RIDER_STYLE_SHIFT) &
    RIDER_STYLE_MASK);
}


static
AiRacerObject *
create_configured_rider(const RacerCreationTemplate *object_template,
                        const RacerCreationTemplate *state_template,
                        const ResourceRefPair       *reference,
                        RacerRole                    role,
                        int                          rider_slot)
{
  if(reference == 0)
    {
      return 0;
    }
  return create_rider_pair(
    object_template, state_template,
    rider_resource_lane(reference->packed_resource_ref),
    reference->object_offset, role, rider_slot);
}


int
initialize_race_rider_runtime(void)
{
  RiderProfile *profile;
  ResourceRefPair *reference;
  AiRacerObject *player;
  AiRacerObject *created;
  RacerEntity *player_state;
  RiderBikeSpecificationData *bike_specification;
  int reference_group;
  int rider_index;
  int name_index;
  int style_index;

  load_bike_specs_resource();
  if(race_rider_state.bike_specs_resource == 0)
    {
      return 0;
    }
  initialize_race_object_scheduler();
  initialize_racer_object_pool();
  initialize_racer_state_pool();

  race_rider_state.challenge_object = 0;
  race_rider_state.rider_objects[RACE_RIDER_COUNT - 1] = 0;
  race_rider_state.rider_states[RACE_RIDER_COUNT - 1] = 0;
  race_rider_state.active_challenge_index = 0;
  race_rider_state.pacing_update_deadline = 0;

  if(race_mode_state.active_player_index < 0 ||
     race_mode_state.active_player_index >= PLAYER_PROFILE_COUNT ||
     race_rider_state.rider_count <= 1 ||
     race_rider_state.rider_count > RACE_RIDER_COUNT)
    {
      return 0;
    }
  profile = selected_rider_profile();
  if(profile->rider_type < 0 ||
     profile->rider_type >= PLAYER_RIDER_TYPE_COUNT ||
     profile->level < 0 ||
     profile->level >= RIDER_PROGRESSION_LEVEL_COUNT ||
     profile->rider_style < 0 ||
     profile->rider_style >= RIDER_BIKE_STYLE_COUNT ||
     profile->current_variant < 0 ||
     profile->current_variant >= OPPONENT_NAME_LEVELS)
    {
      return 0;
    }
  reference = &rider_resource_references[0][0];
  player = create_configured_rider(
    &player_object_template, &player_state_template,
    reference, RACER_ROLE_PLAYER, profile->rider_type);
  race_rider_state.player_object = player;
  if(player == 0)
    {
      return 0;
    }

  player_state = (RacerEntity *)player->paired_state;
  style_index = runtime_rider_style(player_state->runtime);
  if(style_index < 0 || style_index >= BIKE_SPEC_CACHE_COUNT)
    {
      return 0;
    }
  bike_specification = (RiderBikeSpecificationData *)
                       race_rider_state.bike_spec_cache[style_index];
  if(bike_specification == 0)
    {
      return 0;
    }
  initialize_race_boost_state(
    &bike_specification->boost_configuration);

  race_rider_state.rider_objects[0] = player;
  race_rider_state.rider_states[0] = player->paired_state;

  for(rider_index = 1;
      rider_index < race_rider_state.rider_count - 1;
      rider_index++)
    {
      if(race_mode_state.game_mode == RACE_MODE_SINGLE_PLAYER &&
         profile->rider_type == rider_index)
        {
          race_rider_state.rider_objects[rider_index] = 0;
          race_rider_state.rider_states[rider_index] = 0;
        }
      else
        {
          reference_group =
            profile->level < OPPONENT_ALTERNATE_REFERENCE_MIN_LEVEL
                    ? 0 : 1;
          reference =
            &rider_resource_references[reference_group][rider_index];
          created = create_configured_rider(
            &opponent_object_template, &opponent_state_template,
            reference, RACER_ROLE_OPPONENT, rider_index);
          race_rider_state.rider_objects[rider_index] = created;
          if(created == 0)
            {
              race_rider_state.rider_states[rider_index] = 0;
              return 0;
            }
          race_rider_state.rider_states[rider_index] =
            created->paired_state;
        }
    }

  for(name_index = 0;
      name_index < OPPONENTS_PER_LEVEL;
      name_index++)
    {
      strcpy(rider_names[OPPONENT_NAME_DESTINATION_BASE + name_index],
             opponent_names[profile->current_variant][name_index]);
    }
  return 1;
}
