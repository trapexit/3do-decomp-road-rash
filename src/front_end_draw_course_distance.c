#include "stdio.h"

#include "display_cel_runtime.h"
#include "front_end_cel_runtime.h"
#include "front_end_commands.h"
#include "rw_semantic_data.h"
#include "text_render_runtime.h"

enum FrontEndCourseDistanceConstant
{
  FRONT_END_COURSE_DISTANCE_TEXT_BYTES = 12,
  FRONT_END_COURSE_DISTANCE_COLOR = 0xC6,
  FRONT_END_COURSE_DISTANCE_FRACTION_SHIFT = 4,
  FRONT_END_COURSE_DISTANCE_FRACTION_MASK =
    (1 << FRONT_END_COURSE_DISTANCE_FRACTION_SHIFT) - 1,
  FRONT_END_COURSE_DISTANCE_X = 0x4A,
  FRONT_END_COURSE_DISTANCE_Y = 0x8C,
  FRONT_END_COURSE_DISTANCE_STYLE = 2
};

void
front_end_draw_course_distance(unsigned int index)
{
  char text[FRONT_END_COURSE_DISTANCE_TEXT_BYTES];
  signed char race_index;
  signed char type;
  unsigned char mask;
  unsigned int value;
  RiderProfile *profile;

  race_index = race_mode_state.active_player_index;
  profile = &player_profiles[race_index];
  mask = profile->completed_courses;
  if((mask & (1U << (index & FRONT_END_SELECTION_MASK))) != 0)
    {
      apply_front_end_cel_descriptors(front_end_cel_group_013);
    }

  type = profile->level;
  set_text_color(FRONT_END_COURSE_DISTANCE_COLOR,
                 FRONT_END_COURSE_DISTANCE_COLOR,
                 FRONT_END_COURSE_DISTANCE_COLOR);
  value = course_distance_tenths[index][type];
  sprintf(text, "%lx.%lx miles",
          value >> FRONT_END_COURSE_DISTANCE_FRACTION_SHIFT,
          value & FRONT_END_COURSE_DISTANCE_FRACTION_MASK);
  draw_text(display_runtime_globals.screen_items[0],
            FRONT_END_COURSE_DISTANCE_X,
            FRONT_END_COURSE_DISTANCE_Y,
            (int)text, 0, FRONT_END_COURSE_DISTANCE_STYLE);
  submit_cel_batch();
}
