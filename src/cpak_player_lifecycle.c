#include "graphics.h"
#include "stdio.h"

#include "application_runtime.h"
#include "audio_message_runtime.h"
#include "cpak_player_runtime.h"
#include "data_stream_runtime.h"

#define CPAK_STREAM_BLOCK_BYTES 0xC000
#define CPAK_SILENT_BUFFER_COUNT 5
#define CPAK_AUDIO_BUFFER_COUNT 9
#define CPAK_STREAMER_PRIORITY_DELTA 9
#define CPAK_ACQUISITION_PRIORITY_DELTA 8
#define CPAK_SUBSCRIBER_MESSAGE_COUNT 100
#define CPAK_FILM_SUBSCRIBER_PRIORITY 7
#define CPAK_CONTROL_SUBSCRIBER_PRIORITY 11
#define CPAK_AUDIO_SUBSCRIBER_PRIORITY 10
#define CPAK_AUDIO_CHANNEL_CAPACITY 32
#define CPAK_UNKNOWN_SUBSCRIBER_FORMAT_BYTES 68
#define CPAK_AUDIO_TEMPLATE_TAG 0x02020201L

#define ROADRASH_SNDS_CHUNK_TYPE 0x534E4453L

#define CPAK_PLAYER_NO_BUFFER_MEMORY (-1)
#define CPAK_PLAYER_UNKNOWN_SUBSCRIBER (-0x7D3)

enum CpakSubscriberSlot
{
  CPAK_FILM_SUBSCRIBER_SLOT = 0,
  CPAK_CONTROL_SUBSCRIBER_SLOT = 1,
  CPAK_AUDIO_SUBSCRIBER_SLOT = 2,
  CPAK_SUBSCRIBER_TERMINATOR_SLOT = 3,
  CPAK_INCLUDE_AUDIO_BYTE_MASK = 0xFF
};


static const char prepare_cpak_player_unknown_subscriber_format[
  CPAK_UNKNOWN_SUBSCRIBER_FORMAT_BYTES] =
  "PrepareCPakPlayer() - unknown subscriber in stream header: '%.4s'\n";

int32
finish_preparing_cpak_player_audio(RoadRashCPakPlayer *player)
{
  SAudioControlBlock audio_control;
  int32 channel_number;
  int32 status;
  const char *stream_path;

  stream_path = player->acquisition_context != 0 ?
    player->acquisition_context->file_name : 0;

  channel_number = 1;
  do
    {
      if((player->header.enableAudioChan &
          (1U << channel_number)) != 0)
        {
          status = set_legacy_data_stream_channel(
            player->request_message, 0, player->stream_context,
            ROADRASH_SNDS_CHUNK_TYPE, channel_number, CHAN_ENABLED);
          if(status != 0)
            {
              show_application_error("Movie: enable audio channel", status,
                                     stream_path);
              return status;
            }
        }
      channel_number++;
    } while(channel_number < CPAK_AUDIO_CHANNEL_CAPACITY);

  audio_control.clock.channel = player->header.audioClockChan;
  status = control_legacy_data_stream(
    player->request_message, 0, player->stream_context,
    ROADRASH_SNDS_CHUNK_TYPE, SAUDIO_CONTROL_SET_CLOCK_CHANNEL,
    &audio_control);
  if(status != 0)
    {
      show_application_error("Movie: set audio clock channel", status,
                             stream_path);
    }
  return status;
}


int32
prepare_cpak_player(RoadRashCPakPlayer *player,
                    char               *stream_path,
                    uint32              include_audio)
{
  SAudioControlBlock audio_control;
  DSHeaderSubs *subscriber;
  int32 status;
  int32 subscriber_index;
  int32 stream_has_audio;

  include_audio &= CPAK_INCLUDE_AUDIO_BYTE_MASK;
  player->buffer_list = 0;
  player->stream_context = 0;
  player->acquisition_context = 0;
  player->request_message = 0;
  player->end_of_stream_message = 0;
  player->reply_port = 0;
  player->control_context = 0;
  player->audio_context = 0;
  player->cpak_record = 0;
  player->cpak_context = 0;
  player->vbl_io_request = 0;
  player->vram_io_request = 0;

  status = GetVBLIOReq();
  if(status < 0)
    {
      show_application_error("Movie: GetVBLIOReq", status, stream_path);
      return status;
    }
  player->vbl_io_request = status;

  status = GetVRAMIOReq();
  if(status < 0)
    {
      show_application_error("Movie: GetVRAMIOReq", status, stream_path);
      return status;
    }
  player->vram_io_request = status;

  player->header.headerVersion = DS_STREAM_VERSION;
  player->header.streamBlockSize = CPAK_STREAM_BLOCK_BYTES;
  if(include_audio == 0)
    {
      player->header.streamBuffers = CPAK_SILENT_BUFFER_COUNT;
    }
  else
    {
      player->header.streamBuffers = CPAK_AUDIO_BUFFER_COUNT;
    }
  player->header.streamerDeltaPri = CPAK_STREAMER_PRIORITY_DELTA;
  player->header.dataAcqDeltaPri = CPAK_ACQUISITION_PRIORITY_DELTA;
  player->header.numSubsMsgs = CPAK_SUBSCRIBER_MESSAGE_COUNT;

  player->header.audioClockChan = 0;
  player->header.enableAudioChan = CHAN_ENABLED;
  if(include_audio == 0)
    {
      player->header.preloadInstList[0] = 0;
    }
  else
    {
      player->header.preloadInstList[1] = 0;
      player->header.preloadInstList[0] = CPAK_AUDIO_TEMPLATE_TAG;
    }

  subscriber =
    &player->header.subscriberList[CPAK_FILM_SUBSCRIBER_SLOT];
  subscriber->subscriberType = FILM_CHUNK_TYPE;
  subscriber->deltaPriority = CPAK_FILM_SUBSCRIBER_PRIORITY;

  subscriber =
    &player->header.subscriberList[CPAK_CONTROL_SUBSCRIBER_SLOT];
  subscriber->subscriberType = CTRL_CHUNK_TYPE;
  subscriber->deltaPriority = CPAK_CONTROL_SUBSCRIBER_PRIORITY;

  subscriber =
    &player->header.subscriberList[CPAK_AUDIO_SUBSCRIBER_SLOT];
  if(include_audio == 0)
    {
      subscriber->subscriberType = 0;
      subscriber->deltaPriority = 0;
    }
  else
    {
      subscriber->subscriberType = ROADRASH_SNDS_CHUNK_TYPE;
      subscriber->deltaPriority = CPAK_AUDIO_SUBSCRIBER_PRIORITY;
      subscriber = &player->header.subscriberList[
        CPAK_SUBSCRIBER_TERMINATOR_SLOT];
      subscriber->subscriberType = 0;
      subscriber->deltaPriority = 0;
    }

  player->buffer_list = create_data_stream_buffer_list(
    player->header.streamBuffers, player->header.streamBlockSize);
  if(player->buffer_list == 0)
    {
      status = CPAK_PLAYER_NO_BUFFER_MEMORY;
      show_application_error("Movie: allocate stream buffers", status,
                             stream_path);
      return status;
    }

  status = NewMsgPort(0);
  if(status < 0)
    {
      show_application_error("Movie: create reply port", status, stream_path);
      return status;
    }
  player->reply_port = status;

  status = CreateConfiguredMsgItem(player->reply_port, 0);
  if(status < 0)
    {
      show_application_error("Movie: create request message", status,
                             stream_path);
      return status;
    }
  player->request_message = status;

  status = CreateConfiguredMsgItem(player->reply_port, 0);
  if(status < 0)
    {
      show_application_error("Movie: create EOF message", status, stream_path);
      return status;
    }
  player->end_of_stream_message = status;

  status = create_stream_acquisition_context(
    &player->acquisition_context, stream_path,
    player->header.dataAcqDeltaPri);
  if(status != 0)
    {
      show_application_error("Movie: create acquisition worker", status,
                             stream_path);
      return status;
    }

  status = NewDataStream(&player->stream_context,
                         player->buffer_list,
                         player->header.streamBlockSize,
                         player->header.streamerDeltaPri,
                         player->header.numSubsMsgs);
  if(status != 0)
    {
      show_application_error("Movie: NewDataStream", status, stream_path);
      return status;
    }

  status = connect_legacy_data_stream(player->request_message, 0,
                                      player->stream_context,
                                      player->acquisition_context->request_port);
  if(status != 0)
    {
      show_application_error("Movie: connect acquisition", status, stream_path);
      return status;
    }

  stream_has_audio = 0;
  subscriber_index = 0;
  subscriber =
    &player->header.subscriberList[CPAK_FILM_SUBSCRIBER_SLOT];
  if(subscriber->subscriberType != 0)
    {
      do
        {
          subscriber = &player->header.subscriberList[subscriber_index];

          if(subscriber->subscriberType == CTRL_CHUNK_TYPE)
            {
              status = create_control_subscriber(
                &player->control_context, player->stream_context,
                subscriber->deltaPriority);
              if(status != 0)
                {
                  show_application_error("Movie: create control subscriber",
                                         status, stream_path);
                  return status;
                }

              status = subscribe_legacy_data_stream(
                player->request_message, 0, player->stream_context,
                CTRL_CHUNK_TYPE,
                player->control_context->standard.requestPort);
              if(status != 0)
                {
                  show_application_error("Movie: subscribe control", status,
                                         stream_path);
                  return status;
                }
            }
          else if(subscriber->subscriberType == FILM_CHUNK_TYPE)
            {
              status = create_cpak_subscriber(
                &player->cpak_context, 1,
                subscriber->deltaPriority);
              if(status != 0)
                {
                  show_application_error("Movie: create Cinepak subscriber",
                                         status, stream_path);
                  return status;
                }

              status = subscribe_legacy_data_stream(
                player->request_message, 0, player->stream_context,
                FILM_CHUNK_TYPE, player->cpak_context->requestPort);
              if(status != 0)
                {
                  show_application_error("Movie: subscribe Cinepak", status,
                                         stream_path);
                  return status;
                }

              status = initialize_cpak_cel_record(
                player->stream_context, player->cpak_context,
                &player->cpak_record, 0, 1);
              if(status != 0)
                {
                  show_application_error("Movie: initialize Cinepak cel",
                                         status, stream_path);
                  return status;
                }
            }
          else if(subscriber->subscriberType ==
                  ROADRASH_SNDS_CHUNK_TYPE)
            {
              status = create_saudio_subscriber_context(
                &player->audio_context, player->stream_context,
                subscriber->deltaPriority);
              if(status != 0)
                {
                  show_application_error("Movie: create audio subscriber",
                                         status, stream_path);
                  return status;
                }

              status = subscribe_legacy_data_stream(
                player->request_message, 0, player->stream_context,
                ROADRASH_SNDS_CHUNK_TYPE,
                player->audio_context->request_port);
              if(status != 0)
                {
                  show_application_error("Movie: subscribe audio", status,
                                         stream_path);
                  return status;
                }
              stream_has_audio = 1;
            }
          else
            {
              printf(prepare_cpak_player_unknown_subscriber_format,
                     (char *)&subscriber->subscriberType);
              status = CPAK_PLAYER_UNKNOWN_SUBSCRIBER;
              show_application_error("Movie: unknown subscriber", status,
                                     stream_path);
              return status;
            }

          subscriber_index++;
        } while(player->header.subscriberList[subscriber_index]
                .subscriberType != 0);
    }

  if(stream_has_audio == 0)
    {
      return 0;
    }

  audio_control.load_templates.tag_list =
    (long *)player->header.preloadInstList;
  status = control_legacy_data_stream(
    player->request_message, 0, player->stream_context,
    ROADRASH_SNDS_CHUNK_TYPE, SAUDIO_CONTROL_LOAD_TEMPLATES,
    &audio_control);
  if(status != 0)
    {
      show_application_error("Movie: load audio templates", status, stream_path);
      return status;
    }

  return finish_preparing_cpak_player_audio(player);
}


int32
dismantle_cpak_player(RoadRashCPakPlayer *player)
{
  int32 result;
  const char *stream_path;

  stream_path = player->acquisition_context != 0 ?
    player->acquisition_context->file_name : 0;

  /* Return borrowed frames while the subscriber's task-owned locks exist. */
  if(player->cpak_record != 0)
    {
      result = destroy_cpak_cel_record(player->cpak_context,
                                      player->cpak_record,
                                      player->cpak_record->channel);
      if(result < 0)
        {
          show_application_error("Movie: destroy Cinepak cel", result,
                                 stream_path);
          return result;
        }
      player->cpak_record = 0;
    }
  if(player->stream_context != 0)
    {
      result = DisposeDataStream(
        player->request_message, player->stream_context);
      if(result < 0)
        {
          show_application_error("Movie: DisposeDataStream", result,
                                 stream_path);
          return result;
        }
      player->stream_context = 0;
    }
  if(player->acquisition_context != 0)
    {
      result = dispose_stream_acquisition_context(
        player->acquisition_context);
      if(result < 0)
        {
          show_application_error("Movie: dispose acquisition", result,
                                 stream_path);
          return result;
        }
      player->acquisition_context = 0;
    }
  if(player->control_context != 0)
    {
      result = dispose_control_subscriber(player->control_context);
      if(result < 0)
        {
          show_application_error("Movie: dispose control subscriber", result,
                                 stream_path);
          return result;
        }
      player->control_context = 0;
    }
  if(player->cpak_context != 0)
    {
      result = dispose_cpak_subscriber(player->cpak_context);
      if(result < 0)
        {
          show_application_error("Movie: dispose Cinepak subscriber", result,
                                 stream_path);
          return result;
        }
      player->cpak_context = 0;
    }
  if(player->audio_context != 0)
    {
      result = dispose_saudio_subscriber_context(player->audio_context);
      if(result < 0)
        {
          show_application_error("Movie: dispose audio subscriber", result,
                                 stream_path);
          return result;
        }
      player->audio_context = 0;
    }
  if(player->buffer_list != 0)
    {
      result = dispose_data_stream_buffer_list(player->buffer_list);
      if(result < 0)
        {
          show_application_error("Movie: dispose stream buffers", result,
                                 stream_path);
          return result;
        }
      player->buffer_list = 0;
    }
  if(player->request_message > 0)
    {
      result = DeleteItem(player->request_message);
      if(result < 0)
        {
          show_application_error("Movie: delete request message", result,
                                 stream_path);
          return result;
        }
      player->request_message = 0;
    }
  if(player->end_of_stream_message > 0)
    {
      result = DeleteItem(player->end_of_stream_message);
      if(result < 0)
        {
          show_application_error("Movie: delete EOF message", result,
                                 stream_path);
          return result;
        }
      player->end_of_stream_message = 0;
    }
  if(player->reply_port > 0)
    {
      result = DeleteItem(player->reply_port);
      if(result < 0)
        {
          show_application_error("Movie: delete reply port", result,
                                 stream_path);
          return result;
        }
      player->reply_port = 0;
    }
  if(player->vbl_io_request > 0)
    {
      result = DeleteItem(player->vbl_io_request);
      if(result < 0)
        {
          show_application_error("Movie: delete VBL request", result,
                                 stream_path);
          return result;
        }
      player->vbl_io_request = 0;
    }

  result = player->vram_io_request;
  if(result > 0)
    {
      result = DeleteItem(result);
      if(result < 0)
        {
          show_application_error("Movie: delete VRAM request", result,
                                 stream_path);
          return result;
        }
    }
  player->vram_io_request = 0;
  return result;
}
