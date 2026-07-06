#include "division_helpers.h"
#include "racer_creation_runtime.h"

#define SIGNED_QUOTIENT(numerator, divisor) \
        ((int)(numerator) / (int)(divisor))

RiderBikeSpecificationResource *
RSRC_LoadResource(int resource_type,
                  int resource_index,
                  int flags);

enum RiderBikeApplicationValue
{
  BIKE_SPEC_RESOURCE_TYPE = 0x53504543,
  BIKE_SPEC_LOAD_FLAGS = 8,
  BIKE_SPEC_RETAIN_FLAG = 0x01000000,
  BIKE_SPEC_BASELINE_RATING = 75,
  BIKE_SPEC_FIXED_SHIFT = 8,
  BIKE_SPEC_STEERING_LIMIT = 0x2E00,
  BIKE_SPEC_ENGINE_RESPONSE_INTERVAL = 20,
  DIVIDE_BY_FOUR_BIAS = 3,
  DIVIDE_BY_FOUR_SHIFT = 2,
  DIVIDE_BY_EIGHT_BIAS = 7,
  DIVIDE_BY_EIGHT_SHIFT = 3,
  BIKE_SPEC_TUNING_LIMIT = 15,
  BIKE_SPEC_COPY_WORDS_PER_CHUNK = 3,
  BIKE_SPEC_COPY_CHUNK_COUNT =
    (RIDER_BIKE_CONFIGURATION_WORD_COUNT - 1) /
    BIKE_SPEC_COPY_WORDS_PER_CHUNK
};

typedef struct RiderBikeConfigurationChunk
{
  unsigned int words[BIKE_SPEC_COPY_WORDS_PER_CHUNK];
} RiderBikeConfigurationChunk;

int
apply_rider_bike_specification(AiRacerObject *racer,
                               int            rider_rating,
                               int            specification_index)
{
  RacerEntity *object;
  RiderBikeSpecification *specification;
  RiderBikeSpecificationResource *resource;
  const RiderBikeConfigurationChunk *source_chunk;
  RiderBikeConfigurationChunk *destination_chunk;
  int tuning_base;
  int tuning_factor;
  int scaled_acceleration;
  int scaled_braking;
  int divisor;
  int primary_response;
  int secondary_response;
  int chunks_remaining;
  int adjusted_rating;

  object = (RacerEntity *)racer;
  specification = (RiderBikeSpecification *)
                  race_rider_state.bike_spec_cache[specification_index];
  if(specification == 0)
    {
      resource = RSRC_LoadResource(
        BIKE_SPEC_RESOURCE_TYPE, specification_index + 1,
        BIKE_SPEC_LOAD_FLAGS);
      if(resource == 0)
        {
          return 0;
        }
      resource->flags |= BIKE_SPEC_RETAIN_FLAG;
      specification = resource->specification;
      race_rider_state.bike_spec_cache[specification_index] =
        specification;
      if(specification == 0)
        {
          return (int)resource;
        }
    }

  object->collision_profile_id = specification->collision_profile_id;
  object->minimum_acceleration = specification->minimum_acceleration;
  object->contact_threshold = specification->contact_threshold;
  object->bounce_scale = specification->bounce_scale;
  object->impact_strength = specification->impact_strength;
  object->impact_scale = specification->impact_scale;

  tuning_base = specification->tuning_base;
  source_chunk = (const RiderBikeConfigurationChunk *)
                 specification->racer_configuration;
  destination_chunk = (RiderBikeConfigurationChunk *)&object->owner;
  chunks_remaining = BIKE_SPEC_COPY_CHUNK_COUNT;
  do
    {
      *destination_chunk++ = *source_chunk++;
      chunks_remaining--;
    }
  while(chunks_remaining != 0);
  *(unsigned int *)destination_chunk = *(const unsigned int *)source_chunk;

  tuning_factor = tuning_base + BIKE_SPEC_BASELINE_RATING;
  scaled_acceleration =
    (int)(((unsigned int)object->forward_speed_factor *
           (unsigned int)tuning_factor) <<
          BIKE_SPEC_FIXED_SHIFT);
  object->forward_speed_factor = scaled_acceleration;
  scaled_braking =
    (int)(((unsigned int)object->acceleration_curve_a *
           (unsigned int)tuning_factor) <<
          BIKE_SPEC_FIXED_SHIFT);
  object->acceleration_curve_a = scaled_braking;

  adjusted_rating = rider_rating;
  if(adjusted_rating < 0)
    {
      adjusted_rating += DIVIDE_BY_FOUR_BIAS;
    }
  divisor = tuning_base + (adjusted_rating >> DIVIDE_BY_FOUR_SHIFT);
  primary_response = SIGNED_QUOTIENT(
    (unsigned int)object->primary_steering_response *
      (unsigned int)tuning_factor,
    divisor);
  object->primary_steering_response = primary_response;

  secondary_response = SIGNED_QUOTIENT(
    (unsigned int)object->secondary_steering_response *
      (unsigned int)tuning_factor,
    divisor);
  object->secondary_steering_response = secondary_response;

  object->steering_limit = BIKE_SPEC_STEERING_LIMIT;
  object->primary_steering_response_step = SIGNED_QUOTIENT(
    BIKE_SPEC_STEERING_LIMIT, primary_response);
  object->secondary_steering_response_step = SIGNED_QUOTIENT(
    BIKE_SPEC_STEERING_LIMIT, secondary_response);

  adjusted_rating = rider_rating - BIKE_SPEC_BASELINE_RATING;
  if(adjusted_rating < 0)
    {
      adjusted_rating += DIVIDE_BY_EIGHT_BIAS;
    }
  divisor = tuning_base + BIKE_SPEC_BASELINE_RATING +
            (adjusted_rating >> DIVIDE_BY_EIGHT_SHIFT);
  primary_response = SIGNED_QUOTIENT(scaled_acceleration, divisor);
  object->acceleration_limit = primary_response;
  secondary_response = SIGNED_QUOTIENT(scaled_braking, divisor);
  object->braking_target = secondary_response;

  divisor = object->forward_acceleration_divisor;
  if(divisor >= BIKE_SPEC_TUNING_LIMIT)
    {
      divisor = BIKE_SPEC_TUNING_LIMIT;
    }
  object->acceleration_rise_step =
    SIGNED_QUOTIENT(primary_response, divisor);

  divisor = object->forward_deceleration_divisor;
  if(divisor >= BIKE_SPEC_TUNING_LIMIT)
    {
      divisor = BIKE_SPEC_TUNING_LIMIT;
    }
  object->acceleration_fall_step =
    SIGNED_QUOTIENT(primary_response, divisor);

  divisor = object->acceleration_curve_b;
  if(divisor >= BIKE_SPEC_TUNING_LIMIT)
    {
      divisor = BIKE_SPEC_TUNING_LIMIT;
    }
  object->braking_rise_step =
    SIGNED_QUOTIENT(secondary_response, divisor);

  divisor = object->acceleration_curve_c;
  if(divisor >= BIKE_SPEC_TUNING_LIMIT)
    {
      divisor = BIKE_SPEC_TUNING_LIMIT;
    }
  object->braking_fall_step =
    SIGNED_QUOTIENT(secondary_response, divisor);

  object->engine_response_interval =
    BIKE_SPEC_ENGINE_RESPONSE_INTERVAL;
  object->engine_pitch = object->base_engine_pitch;
  return object->base_engine_pitch;
}


#undef SIGNED_QUOTIENT
