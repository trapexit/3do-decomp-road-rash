#ifndef ROADRASH_RACER_CREATION_RUNTIME_H
#define ROADRASH_RACER_CREATION_RUNTIME_H

#include "ai_racer_runtime.h"

enum RiderBikeSpecificationValue
{
  RIDER_BIKE_CONFIGURATION_WORD_COUNT = 76
};

typedef struct RiderBikeSpecification
{
  unsigned char collision_profile_id;
  unsigned char profile_alignment[3];
  int minimum_acceleration;
  int contact_threshold;
  int bounce_scale;
  int impact_strength;
  int impact_scale;
  int tuning_base;
  unsigned int
    racer_configuration[RIDER_BIKE_CONFIGURATION_WORD_COUNT];
} RiderBikeSpecification;

typedef struct RiderBikeSpecificationResource
{
  RiderBikeSpecification *specification;
  unsigned int flags;
} RiderBikeSpecificationResource;

typedef struct RiderBikeSpecificationData
{
  RiderBikeSpecification specification;
  RaceBoostConfig boost_configuration;
} RiderBikeSpecificationData;

typedef struct AiOpponentTimingView
{
  unsigned char reserved_000[0x1CC];
  int lane_change_interval;
  unsigned char reserved_1d0[0x0C];
  int attack_interval;
  unsigned char reserved_1e0[0x0C];
  int recovery_interval;
  unsigned char reserved_1f0[0x0C];
  int pursuit_interval;
} AiOpponentTimingView;

AiRacerObject *
create_rider_pair(const RacerCreationTemplate *object_template,
                  const RacerCreationTemplate *state_template,
                  RoadSegmentLaneRuntime      *lane,
                  int                          position_offset,
                  RacerRole                    role,
                  int                          rider_slot);
int
initialize_race_rider_runtime(void);
void
initialize_ai_racer_state(AiRacerObject *racer);
int
apply_rider_bike_specification(AiRacerObject *racer,
                               int            rider_rating,
                               int            specification_index);
int
initialize_opponent_progression_record(OpponentProgressionRecord **destination,
                                       int                         selection_kind,
                                       int                         selected_reward,
                                       int                         rider_slot);
int *
spawn_challenge_rider_at_lane(RoadSegmentLaneRuntime *track_lane);

typedef char RiderBikeSpecificationConfigurationOffsetCheck[
    offsetof(RiderBikeSpecification, racer_configuration) == 0x1C
        ? 1 : -1];
typedef char RiderBikeSpecificationMinimumAccelerationOffsetCheck[
    offsetof(RiderBikeSpecification, minimum_acceleration) == 0x04
        ? 1 : -1];
typedef char RiderBikeSpecificationContactThresholdOffsetCheck[
    offsetof(RiderBikeSpecification, contact_threshold) == 0x08
        ? 1 : -1];
typedef char RiderBikeSpecificationBounceScaleOffsetCheck[
    offsetof(RiderBikeSpecification, bounce_scale) == 0x0C
        ? 1 : -1];
typedef char RiderBikeSpecificationImpactStrengthOffsetCheck[
    offsetof(RiderBikeSpecification, impact_strength) == 0x10
        ? 1 : -1];
typedef char RiderBikeSpecificationImpactScaleOffsetCheck[
    offsetof(RiderBikeSpecification, impact_scale) == 0x14
        ? 1 : -1];
typedef char RiderBikeSpecificationSizeCheck[
    sizeof(RiderBikeSpecification) == 0x14C ? 1 : -1];
typedef char RiderBikeSpecificationBoostOffsetCheck[
    offsetof(RiderBikeSpecificationData, boost_configuration) == 0x14C
        ? 1 : -1];

#endif
