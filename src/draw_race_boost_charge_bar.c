#include "display_cel_runtime.h"
#include "hud_runtime.h"
#include "racer_runtime.h"
#include "rw_semantic_data.h"

enum RaceBoostHudConstant
{
  RACE_BOOST_FIXED_FRACTION_SHIFT = 8,
  RACE_BOOST_FIXED_ROUNDING_BIAS = 0xFF,
  RACE_BOOST_ACTIVE_FLAG = 0x20,
  RACE_BOOST_MAXIMUM_DISPLAY_UNITS = 10,
  RACE_BOOST_PLUT_WORD_OFFSET = 1,
  RACE_BOOST_UNITS_PER_PLUT_WORD = 2,
  RACE_BOOST_BLINK_TICK_MASK = 0x20,
  RACE_BOOST_FULL_WORD = 0x1CE71CE7,
  RACE_BOOST_EMPTY_WORD = 0x0BE00BE0,
  RACE_BOOST_HALF_WORD = 0x0BE01CE7,
  RACE_BOOST_COORDINATE_BASE = 21
};

static
int
race_boost_integer_charge(void)
{
  int charge;

  charge = race_boost_config.charge_8_8;
  if(charge < 0)
    {
      charge += RACE_BOOST_FIXED_ROUNDING_BIAS;
    }
  return charge >> RACE_BOOST_FIXED_FRACTION_SHIFT;
}


static
int
race_boost_is_active(const RacerEntity *player)
{
  return (player->status_flags & RACE_BOOST_ACTIVE_FLAG) != 0;
}


void
draw_race_boost_charge_bar(void)
{
  RacerEntity *player;
  CCB *packet;
  HudCoordinate *position;
  unsigned int *plut_words;
  unsigned int color;
  int unit;
  int visible_units;
  int layout;
  int charging;

  if(race_boost_config.activation_threshold < 1)
    {
      return;
    }

  player = (RacerEntity *)race_rider_state.player_object;
  packet = append_unmapped_cel(hud_globals.boost_bar_ccb);
  visible_units = race_boost_config.remaining_charge_units;
  if(visible_units > RACE_BOOST_MAXIMUM_DISPLAY_UNITS)
    {
      visible_units = RACE_BOOST_MAXIMUM_DISPLAY_UNITS;
    }
  plut_words = (unsigned int *)packet->ccb_PLUTPtr +
               RACE_BOOST_PLUT_WORD_OFFSET;
  charging = race_boost_integer_charge() <
             race_boost_config.charge_capacity &&
             !race_boost_is_active(player);

  unit = 0;
  while(unit < visible_units)
    {
      if(charging)
        {
          color = RACE_BOOST_FULL_WORD;
          if((frame_tick & RACE_BOOST_BLINK_TICK_MASK) != 0)
            {
              color = RACE_BOOST_EMPTY_WORD;
            }
          plut_words[unit / RACE_BOOST_UNITS_PER_PLUT_WORD] = color;
        }
      else
        {
          plut_words[unit / RACE_BOOST_UNITS_PER_PLUT_WORD] =
            RACE_BOOST_EMPTY_WORD;
        }
      unit++;
    }

  if((unit & 1) != 0)
    {
      if(charging)
        {
          color = RACE_BOOST_HALF_WORD;
          if((frame_tick & RACE_BOOST_BLINK_TICK_MASK) == 0)
            {
              color = RACE_BOOST_FULL_WORD;
            }
          plut_words[unit / RACE_BOOST_UNITS_PER_PLUT_WORD] = color;
        }
      else
        {
          plut_words[unit / RACE_BOOST_UNITS_PER_PLUT_WORD] =
            RACE_BOOST_HALF_WORD;
        }
      unit++;
    }

  while(unit < RACE_BOOST_MAXIMUM_DISPLAY_UNITS)
    {
      plut_words[unit / RACE_BOOST_UNITS_PER_PLUT_WORD] =
        RACE_BOOST_FULL_WORD;
      unit++;
    }

  layout = runtime_feature_flags.alternate_hud_layout.value;
  position = &hud_globals
             .coordinates[RACE_BOOST_COORDINATE_BASE + layout];
  map_cel_at_native_scale(packet, position->x, position->y);
}
