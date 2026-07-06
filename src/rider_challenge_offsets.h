#pragma once

/* Raw struct-layout offsets for the rider challenge/pacing update path.
 * Most of the former offset block in update_rider_animation_and_challenge_state.c
 * is now expressed as typed RacerEntity / RiderInteractionRuntime /
 * RiderRuntimeSlot / RiderAnimationRuntime accesses. Only the entries below
 * stay raw: RACER_DATA_ADJUSTMENT_STEP_A/B/C (bytes 0x12C/0x130/0x134) fall
 * inside RiderInteractionRuntime.response_deltas, but treating them as that
 * row is unproven, so they keep offset-oriented reads. WORD_BYTES scales the
 * remaining raw table steps and the handler-entry define in the .c file.
 *
 * Scope note: the RACER_DATA_ names here are specific to that translation
 * unit. Other files reuse some of these names with different values for
 * different structs (e.g. is_rider_close_on_left.c defines
 * RIDER_TRACK_POSITION_OFFSET as 0x28, not 0x1C), so do NOT include this
 * header broadly until those names are disambiguated. Values must agree
 * with RiderInteractionRuntime in racer_runtime.h, which carries the
 * offsetof static asserts. */

#define WORD_BYTES 4

#define RACER_DATA_ADJUSTMENT_STEP_A_OFFSET 0x12C
#define RACER_DATA_ADJUSTMENT_STEP_B_OFFSET 0x130
#define RACER_DATA_ADJUSTMENT_STEP_C_OFFSET 0x134
