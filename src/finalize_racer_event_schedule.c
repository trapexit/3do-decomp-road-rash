#include "ai_racer_runtime.h"
#include "bss_early_data.h"
#include "division_helpers.h"
#include "rw_semantic_data.h"

#include "stdlib.h"

int
record_racer_finish_result(RacerEntity *racer,
                           int          finish_tick);

enum RacerFinishFinalizationConstant
{
  RACER_FINISH_RESULT_CAPACITY = 4,
  RACER_FINISH_FIRST_TIE_INDEX = 1,
  RACER_FINISH_MAXIMUM_PENDING_COUNT = 15,
  RACER_FINISH_DEFAULT_INTERVAL = 300,
  RACER_FINISH_RANDOM_TIE_SPAN = 3,
  RACER_FINISH_SCHEDULED_FLAG = 0x10
};

void
finalize_racer_event_schedule(int current_tick)
{
  AiRacerObject *racer;
  AiRacerObject *owner;
  PendingRacerEvent *pending;
  PendingRacerEvent *entry;
  SignedDivisionResult division;
  int pending_count;
  int scan_index;
  int finish_interval;
  int finish_tick;
  int insert_index;
  int move_index;
  int result_index;

  pending = gPendingRacerEvents;
  pending_count = race_object_scheduler.pending_racer_count;

  if(pending_count < RACER_FINISH_RESULT_CAPACITY ||
     race_object_scheduler.player_finished == 0)
    {
      scan_index = 0;
      while(scan_index < RACER_FINISH_MAXIMUM_PENDING_COUNT &&
            race_rider_state.rider_count - 1 > scan_index)
        {
          racer = (AiRacerObject *)
                  race_rider_state.rider_states[scan_index];
          if(racer != 0 &&
             (racer->status_flags & RACER_FINISH_SCHEDULED_FLAG) == 0)
            {
              if(pending_count >= RACER_FINISH_MAXIMUM_PENDING_COUNT)
                {
                  pending_count = RACER_FINISH_MAXIMUM_PENDING_COUNT;
                  break;
                }

              owner = racer->owner;
              finish_interval = owner->elapsed_time;
              if(finish_interval > 0)
                {
                  division = signed_divide_with_remainder(
                    (unsigned int)finish_interval,
                    (unsigned int)owner->accumulated_track_delta);
                  finish_interval = division.quotient;
                  if(finish_interval == 0)
                    {
                      finish_interval = RACER_FINISH_DEFAULT_INTERVAL;
                    }
                }
              else
                {
                  finish_interval = RACER_FINISH_DEFAULT_INTERVAL;
                }

              division = signed_divide_with_remainder(
                (unsigned int)finish_interval,
                (unsigned int)update_racer_finish_distance(racer));
              finish_tick = division.quotient + current_tick;

              insert_index = race_object_scheduler.pending_racer_count;
              while(insert_index < pending_count &&
                    pending[insert_index].trigger_time <= finish_tick)
                {
                  insert_index++;
                }

              move_index = pending_count;
              while(insert_index < move_index)
                {
                  pending[move_index] = pending[move_index - 1];
                  move_index--;
                }

              entry = &pending[insert_index];
              pending_count++;
              entry->trigger_time = finish_tick;
              entry->racer = (unsigned int)racer;
            }

          scan_index++;
        }

      result_index = race_object_scheduler.pending_racer_count;
      while(result_index < pending_count)
        {
          entry = &pending[result_index];
          record_racer_finish_result(
            (RacerEntity *)entry->racer, entry->trigger_time);
          result_index++;
        }
    }

  result_index = RACER_FINISH_FIRST_TIE_INDEX;
  do
    {
      if((unsigned int)race_results[result_index].elapsed_ticks <=
         (unsigned int)race_results[result_index - 1].elapsed_ticks)
        {
          division = signed_divide_with_remainder(
            RACER_FINISH_RANDOM_TIE_SPAN, (unsigned int)rand());
          race_results[result_index].elapsed_ticks =
            race_results[result_index - 1].elapsed_ticks +
            division.remainder + 1;
        }
      result_index++;
    } while(result_index < RACER_FINISH_RESULT_CAPACITY);
}
