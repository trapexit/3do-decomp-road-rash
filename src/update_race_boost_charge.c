#include "racer_runtime.h"

enum RaceBoostChargeConstant
{
  RACE_BOOST_CHARGE_FRACTION_BITS = 8,
  RACE_BOOST_CHARGE_ROUNDING_BIAS = 0xFF,
  RACE_BOOST_ACTIVE_FLAG = 0x20
};

void
deactivate_race_boost(RacerEntity *rider);

void
update_race_boost_charge(RacerEntity *rider)
{
  int charge;

  charge = race_boost_config.charge_8_8;
  if((rider->status_flags & RACE_BOOST_ACTIVE_FLAG) != 0)
    {
      charge -= rider->time_step << RACE_BOOST_CHARGE_FRACTION_BITS;
      race_boost_config.charge_8_8 = charge;
      if(charge <= 0)
        {
          deactivate_race_boost(rider);
        }
      return;
    }

  if(charge < 0)
    {
      charge += RACE_BOOST_CHARGE_ROUNDING_BIAS;
    }
  if((charge >> RACE_BOOST_CHARGE_FRACTION_BITS) >=
     race_boost_config.charge_capacity)
    {
      return;
    }

  race_boost_config.charge_8_8 +=
    race_boost_config.recharge_step * rider->time_step;
}
