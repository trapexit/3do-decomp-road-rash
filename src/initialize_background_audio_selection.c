#include "audio.h"
#include "filesystem.h"
#include "stdlib.h"
/* Reconstructed from the original function at 0x00033028. */



#include "async_stream_runtime.h"
#include "rw_semantic_data.h"

#define BYTE_FLAG_MASK                       0xFF
#define BACKGROUND_AUDIO_INITIAL_GAIN  0x38D7

typedef struct DivResult
{
  unsigned int quotient;
  unsigned int remainder;
} DivResult;

__value_in_regs DivResult
unsigned_divide_by_ten_with_remainder(unsigned int numerator);
__value_in_regs DivResult
signed_divide_with_remainder(unsigned int divisor,
                             unsigned int numerator);
int
stop_background_audio_stream(void);

int
set_async_block_stream_buffering(AsyncBlockStream *stream,
                                 int               buffering_enabled,
                                 int               wait_for_buffer)
{
  AsyncStreamBuffer *buffers;
  int buffer_count;
  int buffer_index;
  int ready_buffer_count;
  int result;

  if(stream == 0)
    {
      return -1;
    }

  buffering_enabled &= BYTE_FLAG_MASK;
  wait_for_buffer &= BYTE_FLAG_MASK;

  if(buffering_enabled != 0)
    {
      stream->buffering_enabled = 1;
    }

  if(wait_for_buffer != 0)
    {
      result = queue_async_block_stream_reads(stream, wait_for_buffer);
      if(result < 0)
        {
          return result;
        }
    }

  stream->buffering_enabled = (unsigned char)buffering_enabled;
  result = service_async_block_stream_reads(stream);
  if(result < 0)
    {
      return result;
    }

  ready_buffer_count = 0;
  buffer_index = 0;
  buffer_count = stream->buffer_count;
  if(buffer_count <= 0)
    {
      return ready_buffer_count;
    }

  buffers = stream->buffers;
  do
    {
      if(buffers[buffer_index].state == ASYNC_STREAM_BUFFER_READY)
        {
          ready_buffer_count++;
        }
      buffer_index++;
    } while(buffer_index < buffer_count);

  return ready_buffer_count;
}


void
initialize_background_audio_selection(void)
{
  DivResult time_value;
  DivResult random_value;
  unsigned int index;

  if(background_audio_globals.selection_state != 0)
    {
      return;
    }

  background_audio_globals.selection_state = 1;
  race_mode_state.previous_background_audio_index = -1;
  time_value = unsigned_divide_by_ten_with_remainder(
    (unsigned int)GetAudioTime());
  if(background_audio_globals.name_count <= BACKGROUND_AUDIO_NO_TRACKS)
    {
      race_mode_state.background_audio_index = -1;
      goto selection_ready;
    }
  if(background_audio_globals.name_count == BACKGROUND_AUDIO_SINGLE_TRACK)
    {
      race_mode_state.background_audio_index = 0;
      goto selection_ready;
    }

  for(;;)
    {
      index = 0;
      while(time_value.remainder > index)
        {
          random_value = signed_divide_with_remainder(
            (unsigned int)background_audio_globals.name_count,
            (unsigned int)rand());
          race_mode_state.background_audio_index =
            (signed char)random_value.remainder;
          index++;
          time_value = unsigned_divide_by_ten_with_remainder(
            (unsigned int)GetAudioTime());
        }
      if(race_mode_state.previous_background_audio_index !=
         race_mode_state.background_audio_index)
        {
          break;
        }
      time_value = unsigned_divide_by_ten_with_remainder(
        (unsigned int)GetAudioTime());
    }

selection_ready:
  initialize_async_stream_message_port(0);
  background_audio_globals.current_gain =
    BACKGROUND_AUDIO_INITIAL_GAIN;
}


int
stop_background_audio_stream_if_active(void)
{
  if(background_audio_globals.stream_state != 0)
    {
      return stop_background_audio_stream();
    }
  return 0;
}
