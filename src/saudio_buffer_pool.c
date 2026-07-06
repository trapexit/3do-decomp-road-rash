#include "audio.h"
#include "item.h"

#include "memory_manager_runtime.h"
#include "saudio_subscriber_runtime.h"
#include "sdk_audio_compat.h"

static int
audio_buffer_stage_result(int result)
{
  if(result < 0)
    {
      return result;
    }
  return SAUDIO_ERROR_INITIALIZE_BUFFER;
}

static void
release_audio_buffer_items(SAudioChannel *channel,
                           SAudioBuffer  *buffer)
{
  if(buffer->sample > 0)
    {
      UnloadSample(buffer->sample);
      buffer->sample = 0;
      buffer->attachment = 0;
    }
  if(buffer->cue > 0)
    {
      DeleteItem(buffer->cue);
      buffer->cue = 0;
    }
  channel->signal_mask &= ~buffer->signal;
  buffer->signal = 0;
}

boolean
InitAudioBuffer(void *visitor_context,
                void *pool_entry)
{
  SAudioBufferInitState *buffer_init;
  SAudioHeaderChunk *header;
  SAudioChannel *channel;
  SAudioBuffer *buffer;
  TagArg tags[SAUDIO_SAMPLE_FORMAT_TAG_COUNT];
  int status;

  buffer_init = (SAudioBufferInitState *)visitor_context;
  buffer = (SAudioBuffer *)pool_entry;
  channel = buffer_init->parameters.channel;
  header = buffer_init->parameters.header;
  buffer->sample = 0;
  buffer->attachment = 0;
  buffer->cue = 0;
  buffer->signal = 0;
  buffer->pending_message = 0;
  buffer->link = 0;

  tags[SAUDIO_SAMPLE_CHANNEL_TAG_INDEX].ta_Tag =
    SAUDIO_AUDIO_TAG_CHANNEL_COUNT;
  tags[SAUDIO_SAMPLE_CHANNEL_TAG_INDEX].ta_Arg =
    (void *)header->sample.channel_count;
  tags[SAUDIO_SAMPLE_WIDTH_TAG_INDEX].ta_Tag =
    SAUDIO_AUDIO_TAG_SAMPLE_WIDTH;
  tags[SAUDIO_SAMPLE_WIDTH_TAG_INDEX].ta_Arg =
    (void *)header->sample.sample_size;
  tags[SAUDIO_SAMPLE_RATE_TAG_INDEX].ta_Tag =
    SAUDIO_AUDIO_TAG_SAMPLE_RATE;
  tags[SAUDIO_SAMPLE_RATE_TAG_INDEX].ta_Arg =
    (void *)header->sample.sample_rate;
  tags[SAUDIO_SAMPLE_RATIO_TAG_INDEX].ta_Tag =
    SAUDIO_AUDIO_TAG_COMPRESSION_RATIO;
  tags[SAUDIO_SAMPLE_RATIO_TAG_INDEX].ta_Arg =
    (void *)header->sample.compression_ratio;
  tags[SAUDIO_SAMPLE_COMPRESSION_TAG_INDEX].ta_Tag =
    SAUDIO_AUDIO_TAG_COMPRESSION_TYPE;
  tags[SAUDIO_SAMPLE_COMPRESSION_TAG_INDEX].ta_Arg =
    (void *)header->sample.compression_type;
  tags[SAUDIO_SAMPLE_END_TAG_INDEX].ta_Tag = TAG_END;

  status = MakeSample(0, tags);
  if(status <= 0)
    {
      buffer_init->status = audio_buffer_stage_result(status);
      return false;
    }
  buffer->sample = status;

  status = AttachSample(channel->instrument, buffer->sample, 0);
  if(status <= 0)
    {
      buffer_init->status = audio_buffer_stage_result(status);
      release_audio_buffer_items(channel, buffer);
      return false;
    }
  buffer->attachment = status;

  tags[SAUDIO_SAMPLE_CHANNEL_TAG_INDEX].ta_Tag =
    SAUDIO_AUDIO_TAG_ATTACHMENT_FLAGS;
  tags[SAUDIO_SAMPLE_CHANNEL_TAG_INDEX].ta_Arg =
    (void *)SAUDIO_ATTACHMENT_NO_AUTOSTART;
  tags[SAUDIO_SAMPLE_WIDTH_TAG_INDEX].ta_Tag = TAG_END;
  status = SetAudioItemInfo(buffer->attachment, tags);
  if(status < 0)
    {
      buffer_init->status = status;
      release_audio_buffer_items(channel, buffer);
      return false;
    }

  status = CreateSizedItem(SAUDIO_CUE_ITEM_NODE, 0, 0);
  if(status <= 0)
    {
      buffer_init->status = audio_buffer_stage_result(status);
      release_audio_buffer_items(channel, buffer);
      return false;
    }
  buffer->cue = status;

  status = MonitorAttachment(buffer->attachment, buffer->cue,
                             SAUDIO_ATTACHMENT_MONITOR_AT_END);
  if(status < 0)
    {
      buffer_init->status = status;
      release_audio_buffer_items(channel, buffer);
      return false;
    }

  status = GetCueSignal(buffer->cue);
  if(status <= 0)
    {
      buffer_init->status = audio_buffer_stage_result(status);
      release_audio_buffer_items(channel, buffer);
      return false;
    }
  buffer->signal = (unsigned int)status;
  channel->signal_mask |= buffer->signal;
  buffer->link = buffer_init->initialized_head;
  buffer_init->initialized_head = buffer;
  return true;
}


boolean
FreeAudioBuffer(void *visitor_context,
                void *pool_entry)
{
  SAudioBufferInitBlock *buffer_init;
  SAudioChannel *channel;
  SAudioBuffer *buffer;

  buffer_init = (SAudioBufferInitBlock *)visitor_context;
  channel = buffer_init->channel;
  buffer = (SAudioBuffer *)pool_entry;
  release_audio_buffer_items(channel, buffer);
  return true;
}


int
initialize_saudio_buffer_pool(SAudioChannel     *channel,
                              SAudioHeaderChunk *header)
{
  SAudioBufferInitState buffer_init;
  SAudioBuffer *initialized_buffer;
  SAudioBuffer *next_initialized_buffer;

  if(header->buffer_count <= 0)
    {
      return SAUDIO_ERROR_ALLOC_BUFFER_POOL;
    }

  channel->buffer_pool =
    create_memory_pool(header->buffer_count, sizeof(SAudioBuffer));
  if(channel->buffer_pool == 0)
    {
      return SAUDIO_ERROR_ALLOC_BUFFER_POOL;
    }

  buffer_init.parameters.channel = channel;
  buffer_init.parameters.header = header;
  buffer_init.initialized_head = 0;
  buffer_init.status = 0;
  if(ForEachFreePoolMember(channel->buffer_pool, InitAudioBuffer,
                           &buffer_init))
    {
      return 0;
    }

  initialized_buffer = buffer_init.initialized_head;
  while(initialized_buffer != 0)
    {
      next_initialized_buffer = initialized_buffer->link;
      FreeAudioBuffer(&buffer_init.parameters, initialized_buffer);
      initialized_buffer = next_initialized_buffer;
    }
  destroy_memory_pool(channel->buffer_pool);
  channel->buffer_pool = 0;
  return buffer_init.status;
}
