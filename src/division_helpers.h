#ifndef ROADRASH_DIVISION_HELPERS_H
#define ROADRASH_DIVISION_HELPERS_H

#ifndef offsetof
  #include "stddef.h"
#endif

typedef struct SignedDivisionResult
{
  int quotient;
  int remainder;
} SignedDivisionResult;

typedef struct UnsignedDivisionResult
{
  unsigned int quotient;
  unsigned int remainder;
} UnsignedDivisionResult;

enum DivisionBitPosition
{
  DIVISION_BIT_0 = 0,
  DIVISION_BIT_1 = 1,
  DIVISION_BIT_2 = 2,
  DIVISION_BIT_3 = 3,
  DIVISION_BIT_4 = 4,
  DIVISION_BIT_5 = 5,
  DIVISION_BIT_6 = 6,
  DIVISION_BIT_7 = 7,
  DIVISION_BIT_8 = 8,
  DIVISION_BIT_9 = 9,
  DIVISION_BIT_10 = 10,
  DIVISION_BIT_11 = 11,
  DIVISION_BIT_12 = 12,
  DIVISION_BIT_13 = 13,
  DIVISION_BIT_14 = 14,
  DIVISION_BIT_15 = 15,
  DIVISION_BIT_16 = 16,
  DIVISION_BIT_17 = 17,
  DIVISION_BIT_18 = 18,
  DIVISION_BIT_19 = 19,
  DIVISION_BIT_20 = 20,
  DIVISION_BIT_21 = 21,
  DIVISION_BIT_22 = 22,
  DIVISION_BIT_23 = 23,
  DIVISION_BIT_24 = 24,
  DIVISION_BIT_25 = 25,
  DIVISION_BIT_26 = 26,
  DIVISION_BIT_27 = 27,
  DIVISION_BIT_28 = 28,
  DIVISION_BIT_29 = 29,
  DIVISION_BIT_30 = 30,
  DIVISION_BIT_31 = 31
};

#define DIVISION_SIGN_BIT_MASK 0x80000000U
#define DIVIDE_BY_TEN_REFINE_SHIFT_4 4
#define DIVIDE_BY_TEN_REFINE_SHIFT_8 8
#define DIVIDE_BY_TEN_REFINE_SHIFT_16 16
#define DIVIDE_BY_TEN_NORMALIZE_SHIFT 3
#define DIVIDE_BY_TEN_QUINTUPLE_SHIFT 2
#define DIVIDE_BY_TEN_DIVISOR 10U

__value_in_regs SignedDivisionResult
signed_divide_with_remainder(unsigned int divisor,
                             unsigned int numerator);
__value_in_regs UnsignedDivisionResult
unsigned_divide_with_remainder(unsigned int divisor,
                               unsigned int numerator);
__value_in_regs SignedDivisionResult
signed_divide_by_ten_with_remainder(int value);
__value_in_regs UnsignedDivisionResult
unsigned_divide_by_ten_with_remainder(unsigned int value);

/* The track's lanes sit TRACK_OBJECT_LANE_SPACING (250) units apart, and a
   lateral offset is carried in 8.8 fixed point, so recovering a lane index
   means dividing by 250 << 8 == 0xFA00.  Both callers only want the
   truncated quotient, and both can spend the general shift-subtract routine
   only rarely. */
enum DivisionLaneSpacingValue
{
  DIVISION_LANE_SPACING_FIXED = 0xFA00,
  DIVISION_LANE_SPACING_FAST_LIMIT = 0x800000,
  DIVISION_LANE_SPACING_RECIPROCAL = 33555,
  DIVISION_LANE_SPACING_RECIPROCAL_SHIFT = 23
};

/* Truncated quotient of a 8.8 fixed lateral offset by the lane spacing.
   Offsets below the guard are divided through their whole units, where
   floor(whole / 250) == floor(offset / 0xFA00), and 33555 / 2^23 is an exact
   reciprocal of 250 for every whole unit below 2^15 (the largest the guard
   admits).  Offsets at or beyond the guard keep the general routine, so the
   result is exact for every 32-bit offset.  Checked exhaustively for every
   offset in +-2^25 and at the guard and integer limits. */
static __inline int
divide_lane_offset_by_spacing(int offset)
{
  int magnitude;
  int quotient;

  if(offset <= -DIVISION_LANE_SPACING_FAST_LIMIT ||
     offset >= DIVISION_LANE_SPACING_FAST_LIMIT)
    {
      return signed_divide_with_remainder(
        DIVISION_LANE_SPACING_FIXED, (unsigned int)offset).quotient;
    }

  magnitude = (offset < 0) ? -offset : offset;
  quotient = (int)(((unsigned int)(magnitude >> 8) *
                    DIVISION_LANE_SPACING_RECIPROCAL) >>
                   DIVISION_LANE_SPACING_RECIPROCAL_SHIFT);
  return (offset < 0) ? -quotient : quotient;
}

#define DIVISION_LAYOUT_ASSERT(name, expression) \
        typedef char name[(expression) ? 1 : -1]

DIVISION_LAYOUT_ASSERT(SignedDivisionQuotientOffsetIs0,
                       offsetof(SignedDivisionResult, quotient) == 0);
DIVISION_LAYOUT_ASSERT(SignedDivisionRemainderOffsetIs4,
                       offsetof(SignedDivisionResult, remainder) == 4);
DIVISION_LAYOUT_ASSERT(SignedDivisionResultSizeIs8,
                       sizeof(SignedDivisionResult) == 8);
DIVISION_LAYOUT_ASSERT(UnsignedDivisionQuotientOffsetIs0,
                       offsetof(UnsignedDivisionResult, quotient) == 0);
DIVISION_LAYOUT_ASSERT(UnsignedDivisionRemainderOffsetIs4,
                       offsetof(UnsignedDivisionResult, remainder) == 4);
DIVISION_LAYOUT_ASSERT(UnsignedDivisionResultSizeIs8,
                       sizeof(UnsignedDivisionResult) == 8);

#undef DIVISION_LAYOUT_ASSERT

#endif
