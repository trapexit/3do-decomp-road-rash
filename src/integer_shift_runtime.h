#ifndef INTEGER_SHIFT_RUNTIME_H
#define INTEGER_SHIFT_RUNTIME_H

/*
 * Shift a 32-bit signed value with ARM's two's-complement bit behavior.
 * The shift count must be less than 32.
 */
int
shift_left_wrapping_32(int          value,
                       unsigned int shift_count);
int
add_wrapping_32(int left,
                int right);
int
negate_wrapping_32(int value);

#endif
