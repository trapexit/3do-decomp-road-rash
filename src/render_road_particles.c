#include "stdlib.h"

#include "bss_late_data.h"
#include "display_cel_runtime.h"
#include "division_helpers.h"
#include "rw_semantic_data.h"

enum RoadParticleRenderConstant
{
  ROAD_PARTICLE_TRACK_POSITION_STATE_INDEX = 0,
  ROAD_PARTICLE_SCROLL_PHASE_STATE_INDEX = 1,
  ROAD_PARTICLE_FRAME_TICK_STATE_INDEX = 2,
  ROAD_PARTICLE_MAXIMUM_FRAME_DELTA = 10,
  ROAD_PARTICLE_FALLBACK_FRAME_DELTA = 3,
  ROAD_PARTICLE_CLIP_FRACTION_SHIFT = 16,
  ROAD_PARTICLE_VERTICAL_STEP_SHIFT = 14,
  ROAD_PARTICLE_SCROLL_DELTA_SHIFT = 5,
  ROAD_PARTICLE_VELOCITY_TICK_SHIFT = 17,
  ROAD_PARTICLE_VELOCITY_POSITION_SHIFT = 12,
  ROAD_PARTICLE_PHASE_LIMIT = 0x3FFFFFFF,
  ROAD_PARTICLE_PHASE_SPAN = 0x7FFFFFFE,
  ROAD_PARTICLE_DEPTH_ORIGIN = 0x20FF,
  ROAD_PARTICLE_PHASE_COORDINATE_SHIFT = 17,
  ROAD_PARTICLE_SCREEN_COORDINATE_SHIFT = 19,
  ROAD_PARTICLE_SCROLL_SCALE_SHIFT = 2,
  ROAD_PARTICLE_RECIPROCAL_INDEX_SHIFT = 1,
  ROAD_PARTICLE_RECIPROCAL_OFFSET = 0x100,
  ROAD_PARTICLE_RECIPROCAL_LIMIT = 0x2200,
  ROAD_PARTICLE_VERTICAL_SCALE_BIAS = 0x400,
  ROAD_PARTICLE_SCREEN_CENTER_X = 0x960000,
  ROAD_PARTICLE_SCREEN_HORIZON_Y = 0x5A0000
};

__inline
static
int
reset_out_of_range_particle_velocity(int velocity)
{
  if(velocity < -ROAD_PARTICLE_PHASE_LIMIT ||
     velocity > ROAD_PARTICLE_PHASE_LIMIT)
    {
      return 0;
    }
  return velocity;
}


__inline
static
int
wrap_road_particle_phase(int phase)
{
  if(phase < -ROAD_PARTICLE_PHASE_LIMIT ||
     phase > ROAD_PARTICLE_PHASE_LIMIT)
    {
      if(phase < 0)
        {
          phase += ROAD_PARTICLE_PHASE_SPAN;
        }
      else
        {
          phase -= ROAD_PARTICLE_PHASE_SPAN;
        }
    }
  return phase;
}


void
render_road_particles(const DisplayRectangle    *clip_bounds,
                      const RendererMotionState *motion)
{
  DisplayRectangle fixed_bounds;
  DisplayQuad quad;
  int *reciprocal_table;
  int current_tick;
  int elapsed_ticks;
  int velocity;
  int vertical_step;
  int scroll_delta;
  int remaining;
  int random_bits;
  int reciprocal_depth;
  int reciprocal_index;
  int vertical_phase;
  int vertical_screen_offset;
  int horizontal_phase;
  int horizontal_coordinate;
  int lower_vertical_offset;
  int far_reciprocal;
  int near_reciprocal;
  int right_y;
  int left_y;
  SignedDivisionResult division;
  RoadParticleSlot *slot;

  fixed_bounds.left =
    clip_bounds->left << ROAD_PARTICLE_CLIP_FRACTION_SHIFT;
  fixed_bounds.top =
    clip_bounds->top << ROAD_PARTICLE_CLIP_FRACTION_SHIFT;
  fixed_bounds.right =
    clip_bounds->right << ROAD_PARTICLE_CLIP_FRACTION_SHIFT;
  fixed_bounds.bottom =
    clip_bounds->bottom << ROAD_PARTICLE_CLIP_FRACTION_SHIFT;

  current_tick = (int)frame_tick;
  elapsed_ticks =
    current_tick -
    particle_resource_globals
    .render_state[ROAD_PARTICLE_FRAME_TICK_STATE_INDEX];
  particle_resource_globals
  .render_state[ROAD_PARTICLE_FRAME_TICK_STATE_INDEX] = current_tick;
  if(elapsed_ticks > ROAD_PARTICLE_MAXIMUM_FRAME_DELTA)
    {
      elapsed_ticks = ROAD_PARTICLE_FALLBACK_FRAME_DELTA;
    }

  vertical_step =
    elapsed_ticks << ROAD_PARTICLE_VERTICAL_STEP_SHIFT;
  scroll_delta =
    (particle_resource_globals
     .render_state[ROAD_PARTICLE_SCROLL_PHASE_STATE_INDEX] -
     motion->road_scroll_phase) >>
    ROAD_PARTICLE_SCROLL_DELTA_SHIFT;
  particle_resource_globals
  .render_state[ROAD_PARTICLE_SCROLL_PHASE_STATE_INDEX] =
    motion->road_scroll_phase;

  velocity =
    particle_resource_globals
    .render_state[ROAD_PARTICLE_TRACK_POSITION_STATE_INDEX];
  particle_resource_globals
  .render_state[ROAD_PARTICLE_TRACK_POSITION_STATE_INDEX] =
    motion->packed_track_position;
  velocity =
    (elapsed_ticks << ROAD_PARTICLE_VELOCITY_TICK_SHIFT) +
    ((motion->packed_track_position - velocity) <<
      ROAD_PARTICLE_VELOCITY_POSITION_SHIFT);
  velocity = reset_out_of_range_particle_velocity(velocity);

  slot = gRoadSegmentEffectWorkspace.particle_effects.particle_slots;
  remaining = particle_resource_globals.active_count;
  if(remaining <= 0)
    {
      return;
    }

  /* The table pointer is fixed for the whole frame; reading it back out of
     road_renderer_state for every particle costs two loads per particle. */
  reciprocal_table = road_renderer_state.reciprocal_table;

  do
    {
      vertical_phase = slot->vertical_phase + velocity;
      if(vertical_phase < 0 ||
         vertical_phase > ROAD_PARTICLE_PHASE_LIMIT)
        {
          if(vertical_phase < 0)
            {
              vertical_phase += ROAD_PARTICLE_PHASE_LIMIT;
            }
          else
            {
              vertical_phase -= ROAD_PARTICLE_PHASE_LIMIT;
            }

          random_bits = rand();
          division = signed_divide_with_remainder(
            PARTICLE_RESOURCE_COUNT, (unsigned int)random_bits);
          slot->cel =
            particle_resource_globals.cel_pointers[division.remainder];

          division = signed_divide_with_remainder(
            ROAD_PARTICLE_PHASE_SPAN, (unsigned int)random_bits);
          slot->horizontal_phase =
            division.remainder - ROAD_PARTICLE_PHASE_LIMIT;
        }

      slot->vertical_phase = vertical_phase;
      reciprocal_depth =
        ROAD_PARTICLE_DEPTH_ORIGIN -
        (vertical_phase >> ROAD_PARTICLE_PHASE_COORDINATE_SHIFT);
      vertical_screen_offset =
        vertical_phase >> ROAD_PARTICLE_SCREEN_COORDINATE_SHIFT;
      horizontal_phase =
        slot->horizontal_phase + vertical_step +
        ((scroll_delta * reciprocal_depth) >>
         ROAD_PARTICLE_SCROLL_SCALE_SHIFT);
      horizontal_phase = wrap_road_particle_phase(horizontal_phase);
      slot->horizontal_phase = horizontal_phase;

      horizontal_coordinate =
        horizontal_phase >> ROAD_PARTICLE_PHASE_COORDINATE_SHIFT;
      if(reciprocal_depth <= ROAD_PARTICLE_RECIPROCAL_LIMIT)
        {
          reciprocal_index =
            (reciprocal_depth - ROAD_PARTICLE_RECIPROCAL_OFFSET) >>
            ROAD_PARTICLE_RECIPROCAL_INDEX_SHIFT;
          far_reciprocal = reciprocal_table[reciprocal_index];

          quad.top_right.x =
            far_reciprocal * horizontal_coordinate +
            ROAD_PARTICLE_SCREEN_CENTER_X;
          if(quad.top_right.x < fixed_bounds.right)
            {
              right_y =
                (-vertical_screen_offset) *
                (far_reciprocal -
                 ROAD_PARTICLE_VERTICAL_SCALE_BIAS) +
                ROAD_PARTICLE_SCREEN_HORIZON_Y;
              quad.top_right.y = right_y;
              if(right_y < fixed_bounds.bottom)
                {
                  horizontal_coordinate += slot->half_height;
                  quad.bottom_right.x =
                    far_reciprocal * horizontal_coordinate +
                    ROAD_PARTICLE_SCREEN_CENTER_X;
                  if(quad.bottom_right.x > fixed_bounds.left)
                    {
                      reciprocal_index =
                        (reciprocal_depth +
                         ROAD_PARTICLE_RECIPROCAL_OFFSET) >>
                        ROAD_PARTICLE_RECIPROCAL_INDEX_SHIFT;
                      near_reciprocal =
                        reciprocal_table[reciprocal_index];
                      lower_vertical_offset =
                        slot->half_width - vertical_screen_offset;
                      quad.bottom_right.y = right_y;
                      quad.top_left.x =
                        near_reciprocal *
                        (slot->horizontal_phase >>
                         ROAD_PARTICLE_PHASE_COORDINATE_SHIFT) +
                        ROAD_PARTICLE_SCREEN_CENTER_X;
                      left_y =
                        (near_reciprocal -
                         ROAD_PARTICLE_VERTICAL_SCALE_BIAS) *
                        lower_vertical_offset +
                        ROAD_PARTICLE_SCREEN_HORIZON_Y;
                      quad.top_left.y = left_y;
                      if(left_y > fixed_bounds.top)
                        {
                          quad.bottom_left.x =
                            near_reciprocal * horizontal_coordinate +
                            ROAD_PARTICLE_SCREEN_CENTER_X;
                          quad.bottom_left.y = left_y;
                          append_mapped_quad_cel(
                            (const CCB *)slot->cel, &quad);
                        }
                    }
                }
            }
        }

      slot++;
      remaining--;
    } while(remaining > 0);
}
