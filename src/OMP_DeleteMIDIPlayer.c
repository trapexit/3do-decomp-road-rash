#include "audio_message_runtime.h"
#include "midi_player_runtime.h"
#include "rw_semantic_data.h"

int
OMP_DeleteMIDIPlayer(MidiPlayerRuntime *runtime)
{
  void (*release_runtime)(int,
                          int);
  int result;

  if(runtime == 0)
    {
      return 0;
    }

  if(runtime->thread_item >= 0)
    {
      if(runtime->state != MIDI_PLAYER_STATE_TERMINATED)
        {
          result = SendSmallMsg(runtime->control_port, runtime->reply_port,
                                 MIDI_PLAYER_COMMAND_SHUTDOWN, 0);
          if(result < 0)
            {
              music_library_globals.last_error = result;
              return result;
            }
          result = AM_WaitReply(runtime->creator_port, runtime->reply_port,
                                0, 0, 0);
          if(result < 0)
            {
              music_library_globals.last_error = result;
              return result;
            }
          while(runtime->state != MIDI_PLAYER_STATE_TERMINATED)
            {
            }
        }
      result = delete_audio_thread_item(runtime->thread_item);
      if(result < 0)
        {
          music_library_globals.last_error = result;
          return result;
        }
      runtime->thread_item = -1;
    }

  release_runtime = music_library_globals.release;
  if(runtime->stack_memory != 0)
    {
      release_runtime((int)runtime->stack_memory, runtime->stack_size);
    }
  if(runtime->reply_port >= 0)
    {
      RemoveMsgItem(runtime->reply_port);
    }
  if(runtime->creator_port >= 0)
    {
      DeleteItem(runtime->creator_port);
    }
  release_runtime((int)runtime, MIDI_PLAYER_RUNTIME_ALLOCATION_SIZE);
  return 0;
}
