#include "audio.h"
#include "debug.h"
#include "msgport.h"
#include "sdk_audio_compat.h"
#include "stdlib.h"
#include "string.h"

#include "audio_message_runtime.h"
#include "memory_manager_runtime.h"
#include "saudio_subscriber_runtime.h"

int
process_saudio_data_message(SAudioContext           *context,
                            SAudioSubscriberMessage *message);
int
get_saudio_channel_status(SAudioContext           *context,
                          SAudioSubscriberMessage *message);
int
set_saudio_channel_subscription(SAudioContext           *context,
                                SAudioSubscriberMessage *message);
int
dispatch_saudio_control_command(SAudioContext           *context,
                                SAudioSubscriberMessage *message);
int
synchronize_saudio_stream_channels(SAudioContext           *context,
                                   SAudioSubscriberMessage *message);
int
start_saudio_stream_channels(SAudioContext           *context,
                             SAudioSubscriberMessage *message);
int
stop_saudio_stream_channels(SAudioContext           *context,
                            SAudioSubscriberMessage *message);
int
acknowledge_saudio_stream_open(void);
int
close_saudio_stream_channels(SAudioContext *context);
int
acknowledge_saudio_end_of_file(void);
int
abort_saudio_stream(SAudioContext *context);

static const char saudio_stream_abort_message[] = "kStreamOpAbort\n";

int
initialize_saudio_subscriber_thread(SAudioContext *context)
{
  SAudioChannel *channel;
  int template_bytes;
  int channel_number;
  int status;
  int signal_status;

  context->creator_status = 0;
  context->request_port = 0;
  context->all_buffer_signals = 0;

  status = OpenAudioFolio();
  if(status < 0)
    {
      context->creator_status = status;
      goto signal_creator;
    }

  template_bytes = saudio_template_count * sizeof(SAudioTemplateRecord);
  context->template_array =
    MEM_NewPointer(template_bytes, 0, MEM_POINTER_OPTIONS_DEFAULT);
  if(context->template_array == 0)
    {
      context->creator_status = STREAM_ERROR_NO_MEMORY;
      goto signal_creator;
    }
  memcpy(context->template_array, saudio_initial_templates, template_bytes);

  status = LoadInsTemplate(SAUDIO_OUTPUT_INSTRUMENT_NAME, 0);
  context->output_template_item = status;
  if(status < 0)
    {
      context->creator_status = status;
      goto signal_creator;
    }

  status = LoadInsTemplate(SAUDIO_ENVELOPE_INSTRUMENT_NAME, 0);
  context->envelope_template_item = status;
  if(status < 0)
    {
      context->creator_status = status;
      goto signal_creator;
    }

  context->clock_channel = 0;
  for(channel_number = 0;
      channel_number < SAUDIO_SUBSCRIBER_CHANNEL_COUNT;
      channel_number++)
    {
      channel = &context->channels[channel_number];
      channel->status = 0;
      channel->buffer_count = 0;
      channel->buffer_pool = 0;
      channel->instrument = 0;
      channel->instrument_started = 0;
      channel->attachments_running = 0;
      channel->signal_mask = 0;
      channel->pending_messages.head = 0;
      channel->pending_messages.tail = 0;
      channel->in_use_count = 0;
      channel->in_use_queue_head = 0;
      channel->in_use_queue_tail = 0;
      channel->output.instrument = 0;
      channel->output.channel_count = 0;
      channel->output.left_envelope = 0;
      channel->output.left_envelope_target_knob = 0;
      channel->output.right_envelope = 0;
      channel->output.right_envelope_target_knob = 0;
      channel->output.current_amplitude = -1;
      channel->output.saved_amplitude = 0;
      channel->output.muted = 0;
      channel->output.external_mute = 0;
      channel->output.current_pan = 0;
    }

  status = NewMsgPort((uint32 *)&context->request_port_signal);
  if(status > 0)
    {
      context->request_port = status;
    }
  else
    {
      context->creator_status = status;
    }

signal_creator:
  signal_status = SendSignal(context->creator_task, context->creator_signal);
  if(context->creator_status < 0 || signal_status < 0)
    {
      return -1;
    }
  return 0;
}


void
saudio_subscriber_thread_main(int            ignored_argument,
                              SAudioContext *context)
{
  SAudioSubscriberMessage *message;
  unsigned int awaited_signals;
  unsigned int received_signals;
  int32 message_poll_status;
  int running;
  int status;
  int reply_status;

  (void)ignored_argument;

  status = initialize_saudio_subscriber_thread(context);
  if(status != 0)
    {
      exit(0);
    }

  running = 1;
  while(running != 0)
    {
      awaited_signals =
        context->all_buffer_signals | context->request_port_signal;
      received_signals = WaitSignal(awaited_signals);

      if((received_signals & context->request_port_signal) != 0)
        {
          while(PollForMsg(context->request_port, 0, 0,
                           (void **)&message,
                           &message_poll_status) != 0)
            {
              status = message_poll_status;
              switch(message->operation)
                {
                case SAUDIO_STREAM_OP_DATA:
                  status = process_saudio_data_message(context, message);
                  break;
                case SAUDIO_STREAM_OP_GET_CHANNEL:
                  status = get_saudio_channel_status(context, message);
                  break;
                case SAUDIO_STREAM_OP_SET_CHANNEL:
                  status =
                    set_saudio_channel_subscription(context, message);
                  break;
                case SAUDIO_STREAM_OP_CONTROL:
                  status = dispatch_saudio_control_command(context, message);
                  break;
                case SAUDIO_STREAM_OP_SYNCHRONIZE:
                  status =
                    synchronize_saudio_stream_channels(context, message);
                  break;
                case SAUDIO_STREAM_OP_START:
                  status = start_saudio_stream_channels(context, message);
                  break;
                case SAUDIO_STREAM_OP_STOP:
                  status = stop_saudio_stream_channels(context, message);
                  break;
                case SAUDIO_STREAM_OP_OPENING:
                  status = acknowledge_saudio_stream_open();
                  break;
                case SAUDIO_STREAM_OP_CLOSING:
                  status = close_saudio_stream_channels(context);
                  running = 0;
                  break;
                case SAUDIO_STREAM_OP_END_OF_FILE:
                  status = acknowledge_saudio_end_of_file();
                  break;
                case SAUDIO_STREAM_OP_ABORT:
                  status = abort_saudio_stream(context);
                  running = 0;
                  kprintf(saudio_stream_abort_message);
                  break;
                }

              if(message->operation != SAUDIO_STREAM_OP_DATA)
                {
                  reply_status = ReplyMsg(
                    message->message_item, status, message,
                    sizeof(SAudioSubscriberMessage));
                  if(status >= 0)
                    {
                      status = reply_status;
                    }
                  if(reply_status < 0)
                    {
                      running = 0;
                      break;
                    }
                }
            }
        }

      if((received_signals & context->all_buffer_signals) != 0)
        {
          HandleCompletedBuffers(context, received_signals);
        }
    }


  exit(0);
}
