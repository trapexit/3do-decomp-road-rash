#include "event.h"
#include "rw_semantic_data.h"

#include "async_stream_runtime.h"
#include "memory_manager_runtime.h"
#include "stream_input_runtime.h"
#include "application_runtime.h"
#include "display_system_runtime.h"
#include "audio_system.h"

int release_movie_playback_resources(void);
int dispose_front_end_assets(void);
void dispose_text_render_manager(void);
int dispose_display_runtime(void);
int shutdown_platform_runtime(void);

enum
{
  CONTROL_PAD_NUMBER_MASK = 0xFF
};

int
shutdown_application_runtime(void)
{
  int result;

  result = dispose_front_end_assets();
  if(result < 0)
    {
      return result;
    }
  result = release_movie_playback_resources();
  if(result < 0)
    {
      return result;
    }
  result = release_retained_async_stream_open_resources();
  if(result < 0)
    {
      return result;
    }
  result = unload_score();
  if(result < 0)
    {
      return result;
    }
  result = shutdown_audio_system();
  if(result < 0)
    {
      return result;
    }
  if(async_stream_message_port > 0)
    {
      result = DeleteItem(async_stream_message_port);
      if(result < 0)
        {
          return result;
        }
      async_stream_message_port = 0;
    }
  result = dispose_display_runtime();
  if(result < 0)
    {
      return result;
    }
  dispose_text_render_manager();
  result = shutdown_platform_runtime();
  if(result < 0)
    {
      return result;
    }
  return MEM_Close();
}


int
read_control_pad_button_edges(int pad_number,
                              int repeat_mask)
{
  int index;
  int value;
  int previous;

  index = (pad_number & CONTROL_PAD_NUMBER_MASK) - 1;
  value = read_control_pad_buttons(
    pad_number & CONTROL_PAD_NUMBER_MASK);
  previous = previous_control_pad_state[index];
  previous_control_pad_state[index] = value;
  return value & (repeat_mask | (previous ^ value));
}
