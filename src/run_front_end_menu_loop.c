#include "io.h"

#include "event.h"
#include "graphics.h"
#include "stdlib.h"
/* Reconstructed from the original function at 0x00007520 using Ghidra and assembly. */



#include "platform.h"

#include "front_end_commands.h"
#include "rw_semantic_data.h"
#include "stream_input_runtime.h"

typedef struct DivModResult
{
  unsigned int quotient;
  unsigned int remainder;
} DivModResult;

#define INPUT_LATCH (*(unsigned char *)(int)&(race_input_latch_state))
#define FRONT_END_TIMEOUT_ENABLED \
        (runtime_feature_flags.streaming_audio_enabled.value)
#define FRONT_END_STREAM_PATHS ((int *)(int)&(front_end_stream_paths))
#define FRONT_END_CONFIRM_BUTTONS (ControlA | ControlB | ControlC)
#define FRONT_END_VBL_WAIT_COUNT 2
#define FRONT_END_IDLE_TIMEOUT_VBLS 0xE10U
#define FRONT_END_ATTRACT_MOVIE_COUNT 3
#define MOVIE_PLAYBACK_FADE_IN 4

int
initialize_front_end_assets(void);
void
draw_profile_slot_menu(int menu_id,
                       int initial_selection);
int
finish_front_end_session(void);
void
execute_front_end_commands(FrontEndCommandHeader **commands);
FrontEndMenu *
find_front_end_menu(int menu_id);
int
queue_fullscreen_clear(void);
FrontEndAction *
find_front_end_action(int action_id);
int
service_background_audio(int flags);
int
stop_front_end_menu_audio(void);
int
stop_background_audio_stream(void);
int
initialize_movie_subscribers(void);
__value_in_regs DivModResult
unsigned_divide_with_remainder(unsigned int divisor,
                               unsigned int numerator);
void
play_movie_stream(char *stream_path,
                  int   playback_flags);
int
dispose_movie_subscribers(void);
int
start_next_background_audio_stream(void);
int
start_front_end_menu_audio(void);
int
play_front_end_menu_audio(void);
void
update_profile_slot_menu_selection(int command);
void
handle_front_end_command(int command);

int
run_front_end_menu_loop(void)
{
  FrontEndMenu *menu;
  FrontEndAction *action;
  int item;
  int current_menu;
  int next_menu;
  int selected_command;
  int buttons;
  int callback_result;
  unsigned int start_time;
  unsigned int elapsed;
  DivModResult random_index;

  menu = 0;
  next_menu = 0;
  current_menu = 0;

  item = GetVBLIOReq();
  if(item < 0)
    {
      return item;
    }
  callback_result = initialize_front_end_assets();
  if(callback_result < 0)
    {
      DeleteItem(item);
      return callback_result;
    }

  for(;;)
    {
      if(next_menu != -1)
        {
          draw_profile_slot_menu(next_menu, 1);
          current_menu = next_menu;

          if(next_menu == FRONT_END_MENU_EXIT_APPLICATION)
            {
              callback_result = finish_front_end_session();
              DeleteItem(item);
              return callback_result;
            }
          if(next_menu == FRONT_END_MENU_FINISH_SESSION)
            {
              callback_result = finish_front_end_session();
              if(callback_result < 0)
                {
                  DeleteItem(item);
                  return callback_result;
                }
            }

          if(menu != 0)
            {
              execute_front_end_commands(menu->exit_commands);
            }

          do
            {
              menu = find_front_end_menu(current_menu);
              if(menu == 0)
                {
                  current_menu = 0;
                }
            } while(menu == 0);

          queue_fullscreen_clear();
          execute_front_end_commands(menu->enter_commands);
          draw_profile_slot_menu(next_menu, 0);
        }

      do
        {
          action = find_front_end_action(menu->current_action_id);
          if(action == 0)
            {
              callback_result = finish_front_end_session();
              DeleteItem(item);
              return callback_result;
            }

          front_end_asset_state.action_navigation_mask = 0;
          if(action->up_action_id != -1)
            {
              front_end_asset_state.action_navigation_mask |= ControlUp;
            }
          if(action->down_action_id != -1)
            {
              front_end_asset_state.action_navigation_mask |= ControlDown;
            }
          if(action->left_action_id != -1)
            {
              front_end_asset_state.action_navigation_mask |= ControlLeft;
            }
          if(action->right_action_id != -1)
            {
              front_end_asset_state.action_navigation_mask |= ControlRight;
            }

          execute_front_end_commands(action->enter_commands);
          front_end_asset_state.action_navigation_mask = 0;
          selected_command = -1;
          next_menu = -1;

          for(;;)
            {
              start_time =
                GrafBase->gf_VBLNumber;

              for(;;)
                {
                  callback_result =
                    WaitVBLDefer(item, FRONT_END_VBL_WAIT_COUNT);
                  if(callback_result >= 0)
                    {
                      callback_result = WaitIO(item);
                    }
                  if(callback_result < 0)
                    {
                      goto failure;
                    }
                  buttons = get_stream_input_buttons(1);

                  if(INPUT_LATCH != 0 &&
                     (buttons & FRONT_END_CONFIRM_BUTTONS) != 0)
                    {
                      buttons = 0;
                      INPUT_LATCH = 0;
                      do
                        {
                          sample_stream_input_pads();
                        } while(get_stream_input_buttons(1) != 0);
                    }

                  sample_stream_input_pads();
                  front_end_preview_state.operation_busy = 0;

                  elapsed =
                    GrafBase->gf_VBLNumber -
                    start_time;
                  if(FRONT_END_TIMEOUT_ENABLED != 0 &&
                     elapsed > FRONT_END_IDLE_TIMEOUT_VBLS)
                    {
                      stop_front_end_menu_audio();
                      stop_background_audio_stream();
                      if(background_audio_globals.stream_state != 0 ||
                         front_end_asset_state.menu_audio_item != -1)
                        {
                          DeleteItem(item);
                          return -1;
                        }
                      callback_result = initialize_movie_subscribers();
                      if(callback_result < 0)
                        {
                          DeleteItem(item);
                          return callback_result;
                        }
                      random_index = unsigned_divide_with_remainder(
                        FRONT_END_ATTRACT_MOVIE_COUNT,
                        (unsigned int)rand());
                      callback_result = (int)random_index.remainder + 1;
                      if(callback_result >= FRONT_END_ATTRACT_MOVIE_COUNT)
                        {
                          callback_result = 0;
                        }
                      play_movie_stream(
                        (char *)FRONT_END_STREAM_PATHS[callback_result],
                        MOVIE_PLAYBACK_FADE_IN);
                      callback_result = dispose_movie_subscribers();
                      if(callback_result < 0)
                        {
                          DeleteItem(item);
                          return callback_result;
                        }
                      callback_result = start_next_background_audio_stream();
                      if(callback_result != 0)
                        {
                          goto failure;
                        }
                      callback_result = start_front_end_menu_audio();
                      if(callback_result != 0)
                        {
                          goto failure;
                        }
                      front_end_preview_state.operation_busy = 1;
                      break;
                    }

                  callback_result = service_background_audio(buttons);
                  if(callback_result != 0)
                    {
                      goto failure;
                    }
                  if(buttons != 0 || front_end_preview_state.operation_busy != 0 ||
                     menu->force_selection != 0 || INPUT_LATCH != 0)
                    {
                      break;
                    }
                }

              do
                {
                  sample_stream_input_pads();
                  callback_result =
                    service_background_audio(get_stream_input_buttons(1));
                  if(callback_result != 0)
                    {
                      goto failure;
                    }
                } while(get_stream_input_buttons(1) != 0);
              callback_result = service_background_audio(buttons);
              if(callback_result != 0)
                {
                  goto failure;
                }

              if(front_end_preview_state.operation_busy != 0)
                {
                  next_menu = current_menu;
                }
              else if((buttons & ControlUp) != 0)
                {
                  selected_command = action->up_action_id;
                }
              else if((buttons & (int)ControlDown) != 0)
                {
                  selected_command = action->down_action_id;
                }
              else if((buttons & ControlLeft) != 0)
                {
                  selected_command = action->left_action_id;
                }
              else if((buttons & ControlRight) != 0)
                {
                  selected_command = action->right_action_id;
                }
              else
                {
                  if((buttons & FRONT_END_CONFIRM_BUTTONS) == 0 &&
                     menu->force_selection == 0)
                    {
                      if(INPUT_LATCH == 0)
                        {
                          goto selection_done;
                        }
                    }
                  else if(menu->force_selection != 0)
                    {
                      action = find_front_end_action(menu->current_action_id);
                    }

                  if(action->condition == 0)
                    {
                      next_menu = action->success_menu_id;
                    }
                  else
                    {
                      callback_result =
                        action->condition(action->condition_first_argument,
                                          action->condition_second_argument);
                      if(callback_result == 0)
                        {
                          next_menu = action->failure_menu_id;
                        }
                      else if(
                        menu->current_action_id ==
                        FRONT_END_COMMAND_COPY_MENU_OVERRIDE_C ||
                        menu->current_action_id ==
                        FRONT_END_COMMAND_COPY_MENU_OVERRIDE_B ||
                        menu->current_action_id ==
                        FRONT_END_COMMAND_COPY_MENU_OVERRIDE_A)
                        {
                          next_menu = front_end_preview_state.next_menu_override_id;
                        }
                      else
                        {
                          next_menu = action->success_menu_id;
                        }
                    }
                }

selection_done:
              if(selected_command != -1 || next_menu != -1)
                {
                  break;
                }
            }

          if(buttons != 0)
            {
              play_front_end_menu_audio();
            }

          if(selected_command != -1)
            {
              execute_front_end_commands(action->selection_exit_commands);
              update_profile_slot_menu_selection(selected_command);
              menu->current_action_id = selected_command;
            }
          else
            {
              handle_front_end_command(menu->current_action_id);
            }
        } while(next_menu == -1);
    }
failure:
  DeleteItem(item);
  return callback_result > 0 ? -callback_result : callback_result;
}
