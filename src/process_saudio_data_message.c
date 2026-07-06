#include "msgport.h"
#include "audio.h"
#include "stdio.h"

#include "saudio_subscriber_runtime.h"

static const char saudio_channel_initialization_error[] =
  "Initializaion for SAudio channel %ld failed; error = %ld\n";
static const char saudio_channel_close_notice[] =
  "Closing channel %ld, other channel(s) will still function\n";

int
process_saudio_data_message(SAudioContext           *context,
                            SAudioSubscriberMessage *message)
{
  SAudioSampleChunk *stream_chunk;
  SAudioSubscriberMessage *failed_message;
  SAudioChannel *channel;
  SAudioBuffer *buffer;
  int channel_number;
  int status;
  int stop_status;

  stream_chunk = (SAudioSampleChunk *)message->payload.data.buffer;
  channel_number = stream_chunk->channel;
  if(!SAUDIO_CHANNEL_NUMBER_IS_VALID(channel_number))
    {
      return ReplyMsg(message->message_item,
                      SAUDIO_ERROR_CHANNEL_OUT_OF_RANGE,
                      message, sizeof(SAudioSubscriberMessage));
    }

  if(stream_chunk->subchunk_type == SAUDIO_SAMPLE_CHUNK_TYPE)
    {
      QueueNewAudioBuffer(context, message);
      status =
        begin_saudio_playback_if_ready(context, channel_number);
      if(status < 0)
        {
          channel = &context->channels[channel_number];
          buffer = channel->in_use_queue_head;
          if(buffer == 0)
            {
              return status;
            }
          stop_status = StopAttachment(buffer->attachment, 0);
          if(stop_status < 0)
            {
              return stop_status;
            }
          GetNextBuffer(channel);
          channel->attachments_running = 0;

          failed_message = buffer->pending_message;
          buffer->pending_message = 0;
          context->all_buffer_signals &= ~buffer->signal;
          FreeBufferFromAudioFolio(buffer);
          ReturnPoolMem(channel->buffer_pool, buffer);
          if(failed_message == 0)
            {
              return status;
            }

          return ReplyMsg(failed_message->message_item, status,
                          failed_message,
                          sizeof(SAudioSubscriberMessage));
        }
      return status;
    }

  if(stream_chunk->subchunk_type != SAUDIO_HEADER_CHUNK_TYPE)
    {
      return ReplyMsg(message->message_item, 0, message,
                      sizeof(SAudioSubscriberMessage));
    }

  status = InitChannel(context, (SAudioHeaderChunk *)stream_chunk);
  if(status < 0)
    {
      printf(saudio_channel_initialization_error, channel_number, status);
      printf(saudio_channel_close_notice, channel_number);
      CloseChannel(context, channel_number);
    }

  return ReplyMsg(message->message_item, status, message,
                  sizeof(SAudioSubscriberMessage));
}
