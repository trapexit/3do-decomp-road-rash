#include "division_helpers.h"
#include "racer_runtime.h"
#include "rw_semantic_data.h"

enum RacerFinishResultConstant
{
  RACER_FINISH_TICKS_PER_SECOND = 60,
  RACER_FINISH_HUNDREDTHS_PER_SECOND = 100,
  RACER_FINISH_RESULT_CAPACITY = 4,
  RACER_FINISH_LAST_RESULT_INDEX = 3,
  RACER_FINISH_COUNT_SHIFT = 24
};

#define RACER_FINISH_STATUS_LOW_MASK 0x00FFFFFFU

int
record_racer_finish_result(RacerEntity *racer,
                           int          finish_tick)
{
  RaceResultEntry *result_entry;
  UnsignedDivisionResult division;
  int elapsed_hundredths;
  int result_count;
  int rider_name_index;

  division = unsigned_divide_with_remainder(
    RACER_FINISH_TICKS_PER_SECOND,
    (unsigned int)(finish_tick * RACER_FINISH_HUNDREDTHS_PER_SECOND));
  elapsed_hundredths = (int)division.quotient;

  if(racer->runtime != 0)
    {
      racer->runtime->race_score = finish_tick;
    }

  result_count = race_object_scheduler.pending_racer_count;
  if(result_count < RACER_FINISH_RESULT_CAPACITY)
    {
      result_entry = &race_results[result_count];
      result_entry->elapsed_ticks = elapsed_hundredths;

      if(racer->runtime == 0)
        {
          rider_name_index = -1;
        }
      else
        {
          rider_name_index = racer->interaction_data->metric_index;
        }
      result_entry->rider_name_index = rider_name_index;
    }

  result_count++;
  race_object_scheduler.pending_racer_count = result_count;

  if(racer->owner != race_rider_state.player_object)
    {
      return elapsed_hundredths;
    }

  race_object_scheduler.player_finished = 1;
  race_rider_state.status =
    (int)(((unsigned int)race_rider_state.status &
           RACER_FINISH_STATUS_LOW_MASK) |
          ((unsigned int)(unsigned char)result_count <<
           RACER_FINISH_COUNT_SHIFT));
  race_mode_state.finish_position = (signed char)(result_count - 1);

  if(result_count > RACER_FINISH_RESULT_CAPACITY)
    {
      race_results[RACER_FINISH_LAST_RESULT_INDEX].elapsed_ticks =
        elapsed_hundredths;
    }

  return elapsed_hundredths;
}


#undef RACER_FINISH_STATUS_LOW_MASK
