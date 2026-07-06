#include "racer_runtime.h"
#include "spatial_audio_event_queue.h"

enum RaceBoostStateConstant
{
  RACE_BOOST_FIXED_FRACTION_BITS = 8,
  RACE_BOOST_ROUNDING_BIAS = 0xFF,
  RACE_BOOST_ACTIVE_FLAG = 0x20,
  RACE_BOOST_ACTIVATION_AUDIO_EVENT = 0x11
};

int
initialize_race_boost_state(const RaceBoostConfig *configuration)
{
  unsigned int initial_charge;

  if(configuration != 0)
    {
      race_boost_config.recharge_step = configuration->recharge_step;
      race_boost_config.active_speed_scale_8_8 =
        configuration->active_speed_scale_8_8;
      race_boost_config.charge_capacity =
        configuration->charge_capacity;
      race_boost_config.remaining_charge_units =
        configuration->remaining_charge_units;
      race_boost_config.activation_threshold =
        configuration->activation_threshold;
      race_boost_config.activation_cooldown_ticks =
        configuration->activation_cooldown_ticks;
    }

  initial_charge =
    (unsigned int)race_boost_config.charge_capacity <<
      RACE_BOOST_FIXED_FRACTION_BITS;
  race_boost_config.charge_8_8 = (int)initial_charge;

  return (int)&race_boost_config.charge_8_8;
}


void
activate_race_boost_if_ready(RacerEntity *rider)
{
  int charge;

  if((rider->status_flags & RACE_BOOST_ACTIVE_FLAG) != 0)
    {
      return;
    }
  if(race_boost_config.remaining_charge_units <= 0)
    {
      return;
    }

  charge = race_boost_config.charge_8_8;
  if(charge < 0)
    {
      charge += RACE_BOOST_ROUNDING_BIAS;
    }
  if((charge >> RACE_BOOST_FIXED_FRACTION_BITS) <
     race_boost_config.activation_threshold)
    {
      return;
    }

  race_boost_config.remaining_charge_units--;
  rider->status_flags |= RACE_BOOST_ACTIVE_FLAG;
  queue_spatial_audio_event(
    RACE_BOOST_ACTIVATION_AUDIO_EVENT,
    &rider->position_z, &rider->velocity.forward);
}
