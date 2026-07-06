#include "string.h"

#include "audio_message_runtime.h"
#include "midi_player_runtime.h"
#include "rw_semantic_data.h"

MidiPlayer *
OMP_CreateMIDIPlayer(MidiPlayerConfig *config)
{
  MidiPlayerRuntime *(*allocate_runtime)(int size,
                                         int flags);
  MidiPlayerRuntime *runtime;
  int error;
  int32 startup_message_result;
  Item startup_message_item;
  MidiPlayer *player;
  unsigned char received;
  uint32 startup_signal;

  runtime = 0;
  startup_message_result = 0;
  startup_message_item = 0;
  player = 0;

  if(config == 0)
    {
      error = MIDI_ERROR_BAD_POINTER;
      goto fail;
    }

  allocate_runtime = (MidiPlayerRuntime * (*)(int, int))
                     music_library_globals.allocate;
  runtime = allocate_runtime(MIDI_PLAYER_RUNTIME_ALLOCATION_SIZE, 0);
  if(runtime == 0)
    {
      error = MIDI_ERROR_NO_MEMORY;
      goto fail;
    }

  memset(runtime, 0, sizeof(*runtime));
  runtime->creator_port = -1;
  runtime->reply_port = -1;
  runtime->control_port = -1;
  runtime->control_reply_item = -1;
  runtime->thread_item = -1;
  runtime->thread_priority = config->thread_priority;

  error = NewMsgPort(0);
  runtime->creator_port = error;
  if(error < 0)
    {
      goto fail;
    }

  error = CreateConfiguredMsgItem(error, 1);
  runtime->reply_port = error;
  if(error < 0)
    {
      goto fail;
    }

  config->runtime_state = runtime;
  runtime->stack_size = MIDI_PLAYER_THREAD_STACK_SIZE;

  error = AM_NewThread((AudioThreadEntry)midi_player_thread_main,
                       &runtime->stack_size, runtime->thread_priority, 0,
                       &runtime->stack_memory, config, &player);
  runtime->thread_item = error;
  if(error < 0)
    {
      goto fail;
    }

  received = 0;
  startup_signal = GetMsgPortSignal(runtime->creator_port);
  while(received == 0)
    {
      error = AM_PollMsg(&received, runtime->creator_port,
                         &startup_message_item, &startup_message_result, 0);
      if(error < 0)
        {
          goto fail;
        }
      if(received == 0)
        {
          if(runtime->state == MIDI_PLAYER_STATE_TERMINATED)
            {
              error = MIDI_ERROR_BAD_POINTER;
              goto fail;
            }
          WaitSignal(startup_signal);
        }
    }

  if(startup_message_item != runtime->control_reply_item)
    {
      error = startup_message_result < 0 ?
        startup_message_result : MIDI_ERROR_BAD_POINTER;
      while(runtime->state != MIDI_PLAYER_STATE_TERMINATED)
        {
        }
      goto fail;
    }

  error = ReplySmallMsg(startup_message_item, 0, 0, 0);
  if(error < 0)
    {
      if(runtime->thread_item >= 0 &&
         delete_audio_thread_item(runtime->thread_item) >= 0)
        {
          runtime->thread_item = -1;
          runtime->state = MIDI_PLAYER_STATE_TERMINATED;
        }
      if(startup_message_result < 0)
        {
          error = startup_message_result;
        }
      goto fail;
    }

  if(startup_message_result < 0)
    {
      error = startup_message_result;
      while(runtime->state != MIDI_PLAYER_STATE_TERMINATED)
        {
        }
      goto fail;
    }

  if(player == 0)
    {
      error = MIDI_ERROR_BAD_POINTER;
      goto fail;
    }

  return player;

fail:
  score_loader_last_error = error;
  if(OMP_DeleteMIDIPlayer(runtime) >= 0)
    {
      if(player != 0)
        {
          delete_midi_player(player);
        }
      if(config != 0)
        {
          config->runtime_state = 0;
        }
    }
  player = 0;
  music_library_globals.last_error = error;
  return player;
}
