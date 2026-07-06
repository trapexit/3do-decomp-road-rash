#include "rw_semantic_data.h"

enum TrackSimulationScaleConstant
{
  TRACK_SIMULATION_DEFAULT_SCALE = 6,
  TRACK_SIMULATION_BASE_MULTIPLIER = 100,
  TRACK_SIMULATION_DERIVED_MULTIPLIER = 60,
  TRACK_SIMULATION_FIXED_SHIFT = 8,
  TRACK_SIMULATION_FIXED_ONE = 0x100,
  TRACK_SIMULATION_RESULT_SHIFT = 24,
  TRACK_SIMULATION_SECOND_WORKSPACE = 2,
  TRACK_SIMULATION_BASE_PAIR_WORKSPACE = 3,
  TRACK_SIMULATION_FINAL_WORKSPACE = 4
};

void
configure_track_simulation_scale(int scale)
{
  int signed_scale;
  int scaled_value;

  signed_scale = (signed char)scale;
  if(signed_scale < 0)
    {
      signed_scale = -signed_scale;
      track_simulation_globals.packed_divisor =
        (unsigned char)signed_scale;
      signed_scale =
        (signed char)track_simulation_globals.packed_divisor;
      scaled_value =
        signed_scale * TRACK_SIMULATION_BASE_MULTIPLIER;
      track_simulation_globals.active_workspace[0] = scaled_value;
      track_simulation_globals.active_workspace[
        TRACK_SIMULATION_BASE_PAIR_WORKSPACE] = scaled_value;
      return;
    }

  if((signed_scale & (TRACK_SIMULATION_FIXED_ONE - 1)) == 0)
    {
      signed_scale = TRACK_SIMULATION_DEFAULT_SCALE;
    }

  track_simulation_globals.last_result =
    (int)((unsigned int)(unsigned char)signed_scale <<
          TRACK_SIMULATION_RESULT_SHIFT);
  signed_scale = (signed char)signed_scale;

  scaled_value = signed_scale *
                 track_simulation_globals.active_workspace[0];
  scaled_value *= TRACK_SIMULATION_DERIVED_MULTIPLIER;
  if(scaled_value < 0)
    {
      scaled_value += TRACK_SIMULATION_FIXED_ONE - 1;
    }
  scaled_value >>= TRACK_SIMULATION_FIXED_SHIFT;
  track_simulation_globals.active_workspace[1] = scaled_value;
  track_simulation_globals.active_workspace[
    TRACK_SIMULATION_SECOND_WORKSPACE] = scaled_value;

  scaled_value = signed_scale *
                 track_simulation_globals.active_workspace[
    TRACK_SIMULATION_BASE_PAIR_WORKSPACE];
  scaled_value *= TRACK_SIMULATION_DERIVED_MULTIPLIER;
  if(scaled_value < 0)
    {
      scaled_value += TRACK_SIMULATION_FIXED_ONE - 1;
    }
  scaled_value >>= TRACK_SIMULATION_FIXED_SHIFT;
  track_simulation_globals.active_workspace[TRACK_SIMULATION_FINAL_WORKSPACE] =
    scaled_value;
}
