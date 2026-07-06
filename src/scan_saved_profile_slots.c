#include "bss_early_data.h"
#include "disk_file_runtime.h"
#include "event.h"
#include "front_end_commands.h"
#include "rw_semantic_data.h"
#include "saved_rider_profile.h"
#include "stdio.h"
#include "string.h"

/* Reconstructed from the original function at 0x000097E8. */



#include "memory_manager_runtime.h"
void
set_text_color(int red,
               int green,
               int blue);
int
draw_text(int bitmap_item,
          int x,
          int y,
          int text,
          int flags,
          int alignment);
int
submit_cel_batch(void);

#define PROFILE_SLOT_LABEL_LENGTH 32
#define SELECTED_TEXT_RED 0xF7
#define SELECTED_TEXT_GREEN 0xD6
#define SELECTED_TEXT_BLUE 0x10
#define NORMAL_TEXT_COMPONENT 0xC6
#define PROFILE_TEXT_ALIGNMENT 2
#define SINGLE_PLAYER_PROFILE_BUTTONS 0x08000000
#define MULTIPLAYER_PROFILE_BUTTONS 0x0E000000
#define PROFILE_SLOT_X 0xA0
#define PROFILE_SLOT_ONE_Y 0x21
#define PROFILE_SLOT_TWO_Y 0x3A
#define PROFILE_SLOT_THREE_Y 0x52
#define PROFILE_SLOT_FOUR_Y 0x6A
#define PROFILE_SLOT_FIVE_Y 0x82
#define PROFILE_SLOT_SIX_Y 0x99
#define PROFILE_SLOT_SEVEN_Y 0xB1
#define PROFILE_SLOT_EXIT_Y 0xC9

static
int
profile_slot_y(int slot)
{
  switch(slot)
    {
    case PROFILE_SLOT_ONE:
      return PROFILE_SLOT_ONE_Y;
    case PROFILE_SLOT_TWO:
      return PROFILE_SLOT_TWO_Y;
    case PROFILE_SLOT_THREE:
      return PROFILE_SLOT_THREE_Y;
    case PROFILE_SLOT_FOUR:
      return PROFILE_SLOT_FOUR_Y;
    case PROFILE_SLOT_FIVE:
      return PROFILE_SLOT_FIVE_Y;
    case PROFILE_SLOT_SIX:
      return PROFILE_SLOT_SIX_Y;
    case PROFILE_SLOT_SEVEN:
      return PROFILE_SLOT_SEVEN_Y;
    default:
      return PROFILE_SLOT_EXIT_Y;
    }
}


int
scan_saved_profile_slots(void)
{
  char path[SAVED_PROFILE_PATH_BUFFER_LENGTH] = SAVED_PROFILE_PATH;
  char *resource;
  SavedRiderProfile *saved_profile;
  int index;
  int resource_capacity;
  int loaded_size;

  resource = 0;
  resource_capacity = 0;
  for(index = 0; index < PROFILE_SLOT_COUNT; index++)
    {
      profile_slot_rider_types[index] = -1;
      path[SAVED_PROFILE_PATH_SLOT_OFFSET] = (char)('0' + index);
      if(disk_file_exists(path) != 0)
        {
          loaded_size = load_disk_file(
            path, &resource, &resource_capacity);
          saved_profile = (SavedRiderProfile *)resource;
          if(saved_rider_profile_is_valid(
               saved_profile, loaded_size) != 0)
            {
              profile_slot_rider_types[index] = saved_profile->rider_type;
            }
        }
    }

  if(resource != 0)
    {
      return MEM_DisposePointer(resource);
    }
  return 0;
}


void
front_end_noop(void)
{
}


void
draw_saved_profile_slots(int selected_slot)
{
  char buffer[PROFILE_SLOT_LABEL_LENGTH];
  int index;
  int rider_type;

  for(index = 0; index < PROFILE_SLOT_COUNT; index++)
    {
      if(selected_slot == index)
        {
          set_text_color(SELECTED_TEXT_RED, SELECTED_TEXT_GREEN,
                         SELECTED_TEXT_BLUE);
        }
      else
        {
          set_text_color(NORMAL_TEXT_COMPONENT, NORMAL_TEXT_COMPONENT,
                         NORMAL_TEXT_COMPONENT);
        }

      if(index == PROFILE_SLOT_EXIT)
        {
          sprintf(buffer, "Exit");
        }
      else
        {
          rider_type = profile_slot_rider_types[index];
          if(rider_type == -1)
            {
              sprintf(buffer, "Empty");
            }
          else
            {
              sprintf(buffer, "%s %ld", rider_names[rider_type], index + 1);
            }
        }

      draw_text(display_runtime_globals.screen_items[0],
                PROFILE_SLOT_X, profile_slot_y(index),
                (int)buffer, 0, PROFILE_TEXT_ALIGNMENT);
      submit_cel_batch();
    }
}


int
wait_for_profile_slot_input(int player_count)
{
  int mask;
  int32 result;
  ControlPadEventData event;

  mask = SINGLE_PLAYER_PROFILE_BUTTONS;
  if(player_count > 1)
    {
      mask = MULTIPLAYER_PROFILE_BUTTONS;
    }

  event.cped_ButtonBits = 0;
  for(;;)
    {
      result = GetControlPad(1, 1, &event);
      if(result <= 0)
        {
          return 0;
        }
      if((event.cped_ButtonBits & mask) != 0)
        {
          break;
        }
    }
  return (int)(event.cped_ButtonBits & (unsigned int)mask);
}


int
save_rider_profile_slot(int profile_slot,
                        int player_index)
{
  char path[SAVED_PROFILE_PATH_BUFFER_LENGTH] = SAVED_PROFILE_PATH;
  SavedRiderProfile *saved_profile;
  RiderProfile *profile;
  int rider_type;
  int result;
  int row;
  int column;

  result = 0;
  if(race_mode_state.game_mode != 0 ||
     race_mode_state.outcome_mode == 0)
    {
      return result;
    }

  path[SAVED_PROFILE_PATH_SLOT_OFFSET] = (char)('0' + profile_slot);
  saved_profile = (SavedRiderProfile *)
                  MEM_NewPointer(sizeof(*saved_profile), MEMORY_REQUEST_ANY,
                                 MEM_POINTER_OPTIONS_DEFAULT);
  if(saved_profile == 0)
    {
      return result;
    }

  memset(saved_profile, 0, sizeof(*saved_profile));
  saved_profile->signature = SAVED_PROFILE_SIGNATURE;
  saved_profile->music_enabled = race_mode_state.music_enabled;
  saved_profile->engine_audio_enabled = race_mode_state.engine_audio_enabled;

  profile = &player_profiles[player_index];
  rider_type = profile->rider_type;
  saved_profile->rider_type = (signed char)rider_type;
  saved_profile->current_variant = profile->current_variant;
  saved_profile->level = profile->level;
  saved_profile->completed_courses = profile->completed_courses;
  saved_profile->balance = profile->balance;
  saved_profile->rider_style = profile->rider_style;

  for(row = 0; row < SAVED_PROFILE_PROGRESSION_ROW_COUNT; row++)
    {
      saved_profile->progression[row].status = 0;
      for(column = 0;
          column < SAVED_PROFILE_PROGRESSION_VALUE_COUNT; column++)
        {
          saved_profile->progression[row].values[column] =
            gRiderProgressionProfiles[player_index][row]
            .words[rider_type *
                   SAVED_PROFILE_PROGRESSION_VALUE_COUNT + column];
        }
    }

  result = write_disk_file(path, (char *)saved_profile,
                           sizeof(*saved_profile));
  MEM_DisposePointer((char *)saved_profile);
  return result;
}


#undef PROFILE_SLOT_LABEL_LENGTH
#undef SELECTED_TEXT_RED
#undef SELECTED_TEXT_GREEN
#undef SELECTED_TEXT_BLUE
#undef NORMAL_TEXT_COMPONENT
#undef PROFILE_TEXT_ALIGNMENT
#undef SINGLE_PLAYER_PROFILE_BUTTONS
#undef MULTIPLAYER_PROFILE_BUTTONS
#undef PROFILE_SLOT_X
#undef PROFILE_SLOT_ONE_Y
#undef PROFILE_SLOT_TWO_Y
#undef PROFILE_SLOT_THREE_Y
#undef PROFILE_SLOT_FOUR_Y
#undef PROFILE_SLOT_FIVE_Y
#undef PROFILE_SLOT_SIX_Y
#undef PROFILE_SLOT_SEVEN_Y
#undef PROFILE_SLOT_EXIT_Y
