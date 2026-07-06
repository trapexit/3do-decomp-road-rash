/* Deterministic random-seed advancement used by road rendering. */



#include "rw_semantic_data.h"

#define PARK_MILLER_MULTIPLIER 0x41A7
#define RANDOM_HALFWORD_BITS 16
#define RANDOM_HIGH_PRODUCT_MASK_SHIFT 17
#define RANDOM_HIGH_PRODUCT_CARRY_SHIFT 15
#define RANDOM_LOW_HALF_MASK 0xFFFFU
#define PARK_MILLER_MODULUS 0x7FFFFFFFU

static
int
advance_seed_with_carry(volatile int *seed,
                        volatile int *carry_slot)
{
  int value;
  int high;
  unsigned int low;
  int high_product;
  unsigned int low_product;
  unsigned int mixed;

  value = *seed;
  high = value >> RANDOM_HALFWORD_BITS;
  high_product = high * PARK_MILLER_MULTIPLIER;
  *carry_slot = high_product;

  low = (unsigned int)value & RANDOM_LOW_HALF_MASK;
  low_product = low * PARK_MILLER_MULTIPLIER;
  mixed = low_product +
          ((((unsigned int)high_product << RANDOM_HIGH_PRODUCT_MASK_SHIFT) >>
            RANDOM_HIGH_PRODUCT_MASK_SHIFT) << RANDOM_HALFWORD_BITS);
  mixed +=
    (unsigned int)(high_product >> RANDOM_HIGH_PRODUCT_CARRY_SHIFT);

  *seed = (int)mixed;
  if((int)mixed < 0)
    {
      mixed -= PARK_MILLER_MODULUS;
    }

  *seed = (int)mixed;
  return (int)mixed;
}


int
advance_random_seed(volatile int *seed)
{
  return advance_seed_with_carry(
    seed,
    &display_runtime_globals.random_seed_scratch.seed_high_product);
}


int
advance_global_random_seed(void)
{
  return advance_seed_with_carry(
    &display_runtime_globals.last_present_vbl,
    &display_runtime_globals.random_seed_scratch.global_high_product);
}


void
set_global_random_seed(int seed)
{
  display_runtime_globals.last_present_vbl = seed;
}


#undef PARK_MILLER_MULTIPLIER
#undef RANDOM_HALFWORD_BITS
#undef RANDOM_HIGH_PRODUCT_MASK_SHIFT
#undef RANDOM_HIGH_PRODUCT_CARRY_SHIFT
#undef RANDOM_LOW_HALF_MASK
#undef PARK_MILLER_MODULUS
