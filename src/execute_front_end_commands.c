/* Reconstructed from the original function at 0x000078D4. */



#include "display_system_runtime.h"
#include "front_end_cel_runtime.h"
#include "rw_semantic_data.h"
#include "stream_input_runtime.h"

#define FRONT_END_STREAM_BUFFERING_THRESHOLD 0x780
#define FRONT_END_COMMAND_STREAM_FLAGS 0x45

void
present_display_frame(int wait_for_vbl);
void
set_background_audio_buffering(int enabled,
                               int threshold);
int
initialize_movie_subscribers(void);
void
play_movie_stream(char *stream_path,
                  int   playback_flags);
int
dispose_movie_subscribers(void);

void
execute_front_end_commands(FrontEndCommandHeader **commands)
{
  FrontEndCommandHeader *command;
  FrontEndWaitCommand *wait_command;
  FrontEndShowCelsCommand *show_cels_command;
  FrontEndPlayStreamCommand *play_stream_command;
  FrontEndCallbackCommand *callback_command;
  unsigned int target;
  int current_buffer;

  if(commands == 0)
    {
      return;
    }

  for(;;)
    {
      command = *commands;
      if(command == 0)
        {
          return;
        }

      switch(command->type)
        {
        case FRONT_END_COMMAND_END:
          return;

        case FRONT_END_COMMAND_WAIT:
          wait_command = (FrontEndWaitCommand *)command;
          target = GrafBase->gf_VBLNumber;
          target += wait_command->ticks;
          while(GrafBase->gf_VBLNumber < target)
            {
            }
          break;

        case FRONT_END_COMMAND_SHOW_CELS:
          show_cels_command = (FrontEndShowCelsCommand *)command;
          apply_front_end_cel_descriptors(show_cels_command->descriptors);
          break;

        case FRONT_END_COMMAND_PRESENT:
          present_display_frame(1);
          current_buffer = display_runtime_globals.active_bitmap_index;
          copy_display_bitmap(current_buffer, current_buffer ^ 1);
          sample_stream_input_pads();
          if((get_stream_input_buttons(1) &
              front_end_asset_state.action_navigation_mask) != 0)
            {
              return;
            }
          break;

        case FRONT_END_COMMAND_RESERVED_THREE:
        case FRONT_END_COMMAND_RESERVED_FIVE:
        case FRONT_END_COMMAND_RESERVED_SIX:
          break;

        case FRONT_END_COMMAND_PLAY_STREAM:
          if(front_end_preview_state.operation_busy == 0)
            {
              play_stream_command = (FrontEndPlayStreamCommand *)command;
              set_background_audio_buffering(
                0, FRONT_END_STREAM_BUFFERING_THRESHOLD);
              if(initialize_movie_subscribers() == 0)
                {
                  play_movie_stream(
                    (char *)play_stream_command->stream_path,
                    FRONT_END_COMMAND_STREAM_FLAGS);
                  if(dispose_movie_subscribers() < 0)
                    {
                      return;
                    }
                }
              set_background_audio_buffering(
                1, FRONT_END_STREAM_BUFFERING_THRESHOLD);
            }
          break;

        case FRONT_END_COMMAND_CALLBACK:
          callback_command = (FrontEndCallbackCommand *)command;
          callback_command->callback(callback_command->first_argument,
                                     callback_command->second_argument);
          break;
        }

      commands++;
    }
}


#undef FRONT_END_STREAM_BUFFERING_THRESHOLD
#undef FRONT_END_COMMAND_STREAM_FLAGS
