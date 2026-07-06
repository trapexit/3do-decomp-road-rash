#include "audio.h"
#include "item.h"
#include "msgport.h"
#include "platform.h"
#include "filefunctions.h"
#include "stdlib.h"

#include "audio_message_runtime.h"
#include "byte_file_runtime.h"
#include "midi_player_runtime.h"
#include "rw_semantic_data.h"

void
release_midi_player_resources(MidiPlayer *player);

int
process_midi_player_control_message(MidiPlayer *player,
                                    int        *time_offset)
{
  MidiPlayerRuntime *runtime;
  int32 message_type;
  Item message_item;
  int result;
  int reply_result;
  unsigned char received;

  message_type = 0;
  message_item = 0;
  reply_result = 0;
  received = 0;
  runtime = player->runtime;

  result = AM_PollMsg(&received,
                      runtime->control_port, &message_item,
                      &message_type, 0);
  if(result >= 0 && received != 0)
    {
      if(message_type != MIDI_PLAYER_COMMAND_PAUSE)
        {
          reply_result = MIDI_PLAYER_UNEXPECTED_COMMAND_ERROR;
        }
      result = ReplySmallMsg(message_item, reply_result, 0, 0);
      if(result >= 0 && message_type == MIDI_PLAYER_COMMAND_PAUSE)
        {
          result = wait_for_midi_player_resume(player, time_offset);
        }
    }

  if(result >= 0)
    {
      return 0;
    }
  return result;
}


int
run_midi_player_stream(MidiPlayer *player)
{
  MidiStream *replacement_stream;
  MidiPlayerCallback callback;
  Item cue;
  int status;
  int release_result;
  int now;
  int time_offset;
  unsigned int next_time;

  time_offset = 0;
  next_time = 0;

  cue = CreateSizedItem(MIDI_PLAYER_TIMER_ITEM_NODE, 0, 0);
  if(cue < 0)
    {
      status = cue;
      goto cleanup;
    }

  status = claim_midi_player_audio_clock(player);
  if(status < 0)
    {
      goto cleanup;
    }
  now = GetAudioTime();
  next_time = (unsigned int)now;
  OMS_ResetMIDIStream(player->stream, (unsigned int)now);

  do
    {
      SleepUntilTime(cue, (int)next_time + time_offset);
      status = OMS_BumpMIDIStream(player->stream, next_time,
                                  &next_time, &time_offset);

      if(player->callback != 0 &&
         (status == MIDI_STREAM_STATUS_FINISHED ||
          status == MIDI_ALL_SOUND_OFF_CONTROLLER))
        {
          callback = (MidiPlayerCallback)player->callback;
          replacement_stream = callback(status, player->stream,
                                        player->callback_context);
          if(replacement_stream != 0)
            {
              replacement_stream->score_context =
                player->stream->score_context;
              replacement_stream->remaining_repeats =
                player->stream->remaining_repeats;
              player->stream = replacement_stream;
              now = GetAudioTime();
              next_time = (unsigned int)now;
              OMS_ResetMIDIStream(replacement_stream,
                                  (unsigned int)now);
              status = 0;
            }
        }

      now = process_midi_player_control_message(player, &time_offset);
      if(now < 0)
        {
          status = now;
        }
    } while(status >= 0 && status != MIDI_STREAM_STATUS_FINISHED &&
            player->runtime->stop_requested == 0);

cleanup:
  OMS_FlushActiveNotes(player->stream);
  release_result = release_midi_player_audio_clock(player);
  DeleteItem(cue);
  if(status == MIDI_STREAM_STATUS_FINISHED)
    {
      status = 0;
    }
  if(status >= 0 && release_result < 0)
    {
      status = release_result;
    }
  return status;
}


void
midi_player_thread_main(MidiPlayerConfig *config,
                        MidiPlayer      **out_player)
{
  char path[MIDI_PLAYER_WORK_PATH_CAPACITY];
  MidiPlayerRuntime *runtime;
  MidiPlayer *player;
  ByteFileReader *file;
  int result;
  int send_result;
  int32 message_data;
  Item message_item;
  int init_failed;
  int audio_folio_open;
  MsgPort *creator_port;
  Item creator_task;
  uint32 creator_signal;

  runtime = 0;
  player = 0;
  file = 0;
  message_data = 0;
  message_item = 0;
  audio_folio_open = 0;
  if(config != 0)
    {
      runtime = config->runtime_state;
    }

  copy_audio_working_directory(path);
  result = ChangeDirectory(path);
  if(result < 0)
    {
      goto startup_done;
    }

  result = OpenAudioFolio();
  if(result < 0)
    {
      goto startup_done;
    }
  audio_folio_open = 1;
  if(runtime == 0)
    {
      result = -1;
      goto startup_done;
    }

  runtime->control_port = NewMsgPort(0);
  if(runtime->control_port < 0)
    {
      result = -1;
      goto startup_done;
    }

  runtime->control_reply_item = CreateConfiguredMsgItem(runtime->control_port, 1);
  if(runtime->control_reply_item < 0)
    {
      result = -1;
      goto startup_done;
    }

  runtime->ready_signal = AllocSignal(0);
  if(runtime->ready_signal == 0 || out_player == 0)
    {
      result = -1;
      goto startup_done;
    }

  player = create_midi_player(MIDI_SCORE_PROGRAM_COUNT);
  if(player == 0)
    {
      result = score_loader_last_error;
      goto startup_done;
    }

  player->mixer_amplitude = config->mixer_amplitude;
  if(config->score_image != 0)
    {
      result = load_midi_player_score_file(
        player, (const char *)config->score_image);
      if(result < 0)
        {
          goto startup_done;
        }
    }
  else if(config->file_handle != 0)
    {
      file = BF_Share(config->file_handle);
      if(file == 0)
        {
          result = MIDI_ERROR_SCORE_FILE_OPEN;
          goto startup_done;
        }
      result = load_midi_player_score_stream(
        player, file, config->file_offset, config->file_length);
      BF_ShareClose(file);
      file = 0;
      if(result < 0)
        {
          goto startup_done;
        }
    }
  else
    {
      result = MIDI_ERROR_MISSING_SCORE_SOURCE;
    }

  if(result >= 0)
    {
      player->runtime = runtime;
      *out_player = player;
    }

startup_done:
  if(runtime == 0)
    {
      if(audio_folio_open != 0)
        {
          CloseAudioFolio();
        }
      exit(0);
    }

  creator_port = (MsgPort *)LookupItem(runtime->creator_port);
  creator_task = creator_port->mp.n_Owner;
  creator_signal = creator_port->mp_Signal;
  runtime->state = MIDI_PLAYER_STATE_STOPPED;
  init_failed = result < 0;
  message_item = runtime->control_reply_item >= 0 ?
    runtime->control_reply_item : runtime->reply_port;
  send_result = SendSmallMsg(runtime->creator_port, message_item,
                             (uint32)result, 0);
  if(send_result >= 0 && runtime->control_reply_item >= 0)
    {
      send_result = AM_WaitReply(runtime->control_port,
                                  runtime->control_reply_item, 0, 0, 0);
    }

  if(init_failed != 0 || send_result < 0)
    {
      goto cleanup;
    }

  for(;;)
    {
      runtime->state = MIDI_PLAYER_STATE_STOPPED;
      result = AM_WaitMsg(runtime->control_port, &message_item,
                          &message_data, 0);
      if(result < 0)
        {
          break;
        }

      if(message_data == MIDI_PLAYER_COMMAND_START)
        {
          if(player->stream != 0)
            {
              runtime->state = MIDI_PLAYER_STATE_PLAYING;
              result = ReplySmallMsg(message_item, 0, 0, 0);
              if(result >= 0)
                {
                  run_midi_player_stream(player);
                }
            }
        }
      else
        {
          ReplySmallMsg(message_item, 0, 0, 0);
          if(message_data == MIDI_PLAYER_COMMAND_SHUTDOWN)
            {
              break;
            }
        }
    }

cleanup:
  if(init_failed != 0)
    {
      delete_midi_player(player);
    }
  else
    {
      release_midi_player_resources(player);
    }

  if(runtime->control_reply_item >= 0)
    {
      RemoveMsgItem(runtime->control_reply_item);
      runtime->control_reply_item = -1;
    }
  if(runtime->control_port >= 0)
    {
      DeleteItem(runtime->control_port);
      runtime->control_port = -1;
    }
  if(runtime->ready_signal != 0)
    {
      FreeSignal(runtime->ready_signal);
    }
  if(audio_folio_open != 0)
    {
      CloseAudioFolio();
    }
  runtime->state = MIDI_PLAYER_STATE_TERMINATED;
  SendSignal(creator_task, creator_signal);
  exit(0);
}
