#pragma once

/* Raw on-disk AIFF MARK chunk layout offset, extracted verbatim from
 * find_aiff_marker_position_unaligned.c. Single consumer; the value is
 * not proven against any struct and stays offset-oriented until
 * aggregate evidence exists. */

#define AIFF_MARK_FIRST_ENTRY_OFFSET 0x0A
