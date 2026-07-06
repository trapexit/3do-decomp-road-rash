#include "stdlib.h"

#include "bss_early_data.h"
#include "rw_semantic_data.h"

#define PROGRESSION_COPY_CHUNKS       39
#define PROGRESSION_COPY_WORDS        3
#define CHALLENGE_RANDOM_WORD_START   111
#define CHALLENGE_RANDOM_WORD_COUNT   5
#define CHALLENGE_RANDOM_RANGE        13
#define CHALLENGE_RANDOM_CENTER       6

typedef struct DivModResult
{
  unsigned int quotient;
  unsigned int remainder;
} DivModResult;
__value_in_regs DivModResult
signed_divide_with_remainder(unsigned int divisor,
                             unsigned int numerator);

static
void
copy_progression_record(int *destination,
                              int *source)
{
  int chunks_remaining;

  chunks_remaining = PROGRESSION_COPY_CHUNKS;
  do
    {
      destination[0] = source[0];
      destination[1] = source[1];
      destination[PROGRESSION_COPY_WORDS - 1] =
        source[PROGRESSION_COPY_WORDS - 1];
      source += PROGRESSION_COPY_WORDS;
      destination += PROGRESSION_COPY_WORDS;
      chunks_remaining--;
    } while(chunks_remaining != 0);

  destination[0] = source[0];
}


void
initialize_rider_progression_profiles(void)
{
  int row;
  int value;
  int *dst;
  int *src;
  int sample;

  row = 0;
  do
    {
      src = (int *)&rider_progression_defaults[row];
      dst = (int *)&gRiderProgressionProfiles[0][row];

      copy_progression_record(dst, src);
      copy_progression_record(
        (int *)&gRiderProgressionProfiles[1][row], src);

      row++;
    } while(row < RIDER_TYPE_COUNT);

  row = 0;
  do
    {
      dst = (int *)&gChallengeProgressionProfiles[row];
      copy_progression_record(
        dst, (int *)&rider_progression_defaults[0]);

      sample = 0;
      do
        {
          value = signed_divide_with_remainder(
            CHALLENGE_RANDOM_RANGE, rand()).remainder;
          dst[sample + CHALLENGE_RANDOM_WORD_START] +=
            value - CHALLENGE_RANDOM_CENTER;
          sample = (signed char)(sample + 1);
        } while(sample < CHALLENGE_RANDOM_WORD_COUNT);

      row++;
    } while(row < CHALLENGE_PROFILE_COUNT);
}
