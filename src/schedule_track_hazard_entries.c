#include "stdlib.h"
#include "track_segment_definition.h"
/* Reconstructed from the original function at 0x00020444. */

#include "rw_semantic_data.h"

#define HAZARD_WEIGHT_SHIFT 24
#define HAZARD_WEIGHT_MASK 0x7FU
#define HAZARD_SPREAD_SHIFT 15
#define HAZARD_SPREAD_MASK 0x1EU
#define HAZARD_DIFFICULTY_SHIFT 20
#define HAZARD_DIFFICULTY_MASK 0x0FU
#define HAZARD_EFFECT_FLAG 0x80000000U
#define HAZARD_TYPE_MASK 0x0FU
#define HAZARD_HARD_TYPE_MIN 0x0B
#define HAZARD_HARD_TYPE_PENALTY 4
#define HAZARD_CONTROL_PRESERVE_MASK 0x8000FFFFU
#define HAZARD_FRAME_MASK 0x7FFFU
#define HAZARD_FRAME_SHIFT 16
#define HAZARD_MODE_SHIFT 12
#define HAZARD_MODE_MASK 7U
#define HAZARD_DISABLED_MASK 0x3F00U

typedef struct DivModResult
{
  unsigned int quotient;
  unsigned int remainder;
} DivModResult;
__value_in_regs DivModResult
signed_divide_with_remainder(unsigned int divisor,
                             unsigned int numerator);
__value_in_regs DivModResult
signed_divide_by_ten_with_remainder(int value);

static
int
current_table_value(void)
{
  int selector;

  selector = race_mode_state.active_player_index;
  return player_profiles[selector].level;
}


void
schedule_track_hazard_entries(TrackHazardTable *table)
{
  int i;
  int accum;
  int entry_count;
  int threshold;
  int mode;
  unsigned int word;
  TrackHazardEntry *entry;
  DivModResult div_result;

  accum = 0;
  i = 0;
  if(table == 0)
    {
      return;
    }
  entry_count = get_track_hazard_entry_count(table);
  if(entry_count <= 0)
    {
      return;
    }

  do
    {
      entry = &table->entries[i];
      word = entry->control_word;
      accum += (word >> HAZARD_WEIGHT_SHIFT) &
               HAZARD_WEIGHT_MASK;

      div_result = signed_divide_with_remainder(
        ((word >> HAZARD_SPREAD_SHIFT) &
         HAZARD_SPREAD_MASK) + 1,
        rand());

      word = entry->control_word;
      threshold = current_table_value() +
                  (int)((word >> HAZARD_DIFFICULTY_SHIFT) &
                        HAZARD_DIFFICULTY_MASK);
      if((word & HAZARD_EFFECT_FLAG) != 0 &&
         (word & HAZARD_TYPE_MASK) >= HAZARD_HARD_TYPE_MIN)
        {
          threshold -= HAZARD_HARD_TYPE_PENALTY;
        }

      entry->control_word =
        (word & HAZARD_CONTROL_PRESERVE_MASK) |
        ((div_result.remainder + accum) & HAZARD_FRAME_MASK)
          << HAZARD_FRAME_SHIFT;

      mode = (entry->detail_word >> HAZARD_MODE_SHIFT) &
             HAZARD_MODE_MASK;
      if(mode == 0 || mode - 1 == current_table_value())
        {
          div_result = signed_divide_by_ten_with_remainder(rand());
          if(threshold >= (int)div_result.remainder)
            {
              goto next_entry;
            }
        }

      entry->control_word |= HAZARD_DISABLED_MASK;

next_entry:
      i++;
    } while(i < entry_count);
}


#undef HAZARD_DISABLED_MASK
#undef HAZARD_MODE_MASK
#undef HAZARD_MODE_SHIFT
#undef HAZARD_FRAME_SHIFT
#undef HAZARD_FRAME_MASK
#undef HAZARD_CONTROL_PRESERVE_MASK
#undef HAZARD_HARD_TYPE_PENALTY
#undef HAZARD_HARD_TYPE_MIN
#undef HAZARD_TYPE_MASK
#undef HAZARD_EFFECT_FLAG
#undef HAZARD_DIFFICULTY_MASK
#undef HAZARD_DIFFICULTY_SHIFT
#undef HAZARD_SPREAD_MASK
#undef HAZARD_SPREAD_SHIFT
#undef HAZARD_WEIGHT_MASK
#undef HAZARD_WEIGHT_SHIFT
