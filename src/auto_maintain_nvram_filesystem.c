#include "stdio.h"

#include "audio.h"
#include "audio_manager_voice.h"
#include "disk_file_runtime.h"
#include "filesystem.h"
#include "memory_manager_runtime.h"
#include "rw_semantic_data.h"
#include "filefunctions.h"
#include "stream_audio_playback.h"
#include "string.h"
#include "task.h"

typedef struct UnsignedDivisionResult
{
  unsigned int quotient;
  unsigned int remainder;
} UnsignedDivisionResult;

enum StreamAudioConfiguration
{
  STREAM_AUDIO_CHUNK_BYTES = 0x3F58,
  STREAM_AUDIO_BUFFER_BYTES = 0x7EB0,
  STREAM_AUDIO_SAMPLE_WIDTH_BYTES = 1,
  STREAM_AUDIO_SAMPLE_WIDTH_BITS = 16,
  STREAM_AUDIO_SAMPLE_RATE_HZ = 22050,
  STREAM_AUDIO_SAMPLE_RATE_16_16 = STREAM_AUDIO_SAMPLE_RATE_HZ << 16,
  STREAM_AUDIO_POSITIVE_BYTE_COUNT_MASK = 0x7FFFFFFF,
  STREAM_AUDIO_PLAYBACK_RATE = 0x4000,
  STREAM_AUDIO_UPDATE_TAG_COUNT = 2
};

enum StreamAudioPlaybackItem
{
  STREAM_AUDIO_INVALID_VOICE_ID = -1
};

enum StreamAudioSampleTagIndex
{
  STREAM_AUDIO_TAG_ADDRESS = 0,
  STREAM_AUDIO_TAG_NUMBYTES,
  STREAM_AUDIO_TAG_CHANNELS,
  STREAM_AUDIO_TAG_WIDTH,
  STREAM_AUDIO_TAG_NUMBITS,
  STREAM_AUDIO_TAG_COMPRESSION_TYPE,
  STREAM_AUDIO_TAG_SAMPLE_RATE,
  STREAM_AUDIO_TAG_SUSTAIN_BEGIN,
  STREAM_AUDIO_TAG_SUSTAIN_END,
  STREAM_AUDIO_TAG_END,
  STREAM_AUDIO_TAG_COUNT
};

__value_in_regs UnsignedDivisionResult
unsigned_divide_with_remainder(unsigned int divisor,
                               unsigned int numerator);

int
auto_maintain_nvram_filesystem(void)
{
  if(LoadProgram("lmadm -a ram 3 0 NVRAM") >= 0)
    {
      WaitSignal(SIGF_DEADTASK);
      return 1;
    }

  printf("lmadm could not be run");
  return 0;
}


void
merge_tag_values(TagArg       *destination_tags,
                 const TagArg *source_tags)
{
  int destination_index;
  int source_index;

  if(destination_tags == 0 || source_tags == 0 ||
     source_tags[0].ta_Tag == TAG_END)
    {
      return;
    }

  source_index = 0;
  do
    {
      destination_index = 0;
      while(destination_tags[destination_index].ta_Tag != TAG_END)
        {
          if(destination_tags[destination_index].ta_Tag ==
             source_tags[source_index].ta_Tag)
            {
              destination_tags[destination_index].ta_Arg =
                source_tags[source_index].ta_Arg;
              break;
            }
          destination_index++;
        }
      source_index++;
    } while(source_tags[source_index].ta_Tag != TAG_END);
}


int
retain_stream_audio_playback(void)
{
  AudioPlaybackState *state;
  int count;
  int zero;

  count = stream_audio_reference_count;
  if(count > 0)
    {
      stream_audio_reference_count = count + 1;
      return 0;
    }

  state = &audio_playback_state;
  if(state->active.value != 0)
    {
      return -1;
    }

  stream_audio_reference_count = 1;

  zero = 0;
  state->channel_count = zero;
  state->sample_item = zero;
  state->playback_voice_id = STREAM_AUDIO_INVALID_VOICE_ID;
  state->buffer_bytes = zero;
  state->buffer_a = 0;
  state->buffer_b = 0;
  state->chunk_bytes = zero;
  state->active_buffer_index = -1;
  state->amplitude = MAXDSPAMPLITUDE;

  return 0;
}


int
initialize_stream_audio_playback(const SAudioHeaderChunk *header)
{
  AudioPlaybackState *state;
  const AudioSourceDescriptor *source;
  char *buffer;
  UnsignedDivisionResult divmod;
  TagArg tags[STREAM_AUDIO_TAG_COUNT];
  int zero;

  state = &audio_playback_state;
  if(state->active.value != 0)
    {
      return 0;
    }

  source = (const AudioSourceDescriptor *)header;
  if(source == 0)
    {
      source = &default_audio_source;
    }
  if(source->channel_count <= 0)
    {
      return -1;
    }

  state->chunk_bytes = STREAM_AUDIO_CHUNK_BYTES;
  state->buffer_bytes = STREAM_AUDIO_BUFFER_BYTES;

  buffer = MEM_NewPointer(STREAM_AUDIO_BUFFER_BYTES, MEMTYPE_ANY,
                          MEM_POINTER_OPTIONS_DEFAULT);
  state->buffer_a = buffer;
  if(buffer == 0)
    {
      return -1;
    }

  zero = 0;
  state->channel_count = source->channel_count;
  tags[STREAM_AUDIO_TAG_ADDRESS].ta_Tag = AF_TAG_ADDRESS;
  tags[STREAM_AUDIO_TAG_ADDRESS].ta_Arg = buffer;
  tags[STREAM_AUDIO_TAG_NUMBYTES].ta_Tag = AF_TAG_NUMBYTES;
  tags[STREAM_AUDIO_TAG_NUMBYTES].ta_Arg = (void *)state->buffer_bytes;
  tags[STREAM_AUDIO_TAG_CHANNELS].ta_Tag = AF_TAG_CHANNELS;
  tags[STREAM_AUDIO_TAG_CHANNELS].ta_Arg =
    (void *)source->channel_count;
  tags[STREAM_AUDIO_TAG_WIDTH].ta_Tag = AF_TAG_WIDTH;
  tags[STREAM_AUDIO_TAG_WIDTH].ta_Arg =
    (void *)STREAM_AUDIO_SAMPLE_WIDTH_BYTES;
  tags[STREAM_AUDIO_TAG_NUMBITS].ta_Tag = AF_TAG_NUMBITS;
  tags[STREAM_AUDIO_TAG_NUMBITS].ta_Arg =
    (void *)STREAM_AUDIO_SAMPLE_WIDTH_BITS;
  tags[STREAM_AUDIO_TAG_COMPRESSION_TYPE].ta_Tag =
    AF_TAG_COMPRESSIONTYPE;
  tags[STREAM_AUDIO_TAG_COMPRESSION_TYPE].ta_Arg = (void *)ID_SDX2;
  tags[STREAM_AUDIO_TAG_SAMPLE_RATE].ta_Tag = AF_TAG_SAMPLE_RATE;
  tags[STREAM_AUDIO_TAG_SAMPLE_RATE].ta_Arg =
    (void *)STREAM_AUDIO_SAMPLE_RATE_16_16;
  tags[STREAM_AUDIO_TAG_SUSTAIN_BEGIN].ta_Tag = AF_TAG_SUSTAINBEGIN;
  tags[STREAM_AUDIO_TAG_SUSTAIN_BEGIN].ta_Arg = (void *)zero;
  tags[STREAM_AUDIO_TAG_SUSTAIN_END].ta_Tag = AF_TAG_SUSTAINEND;

  divmod = unsigned_divide_with_remainder((unsigned int)state->channel_count,
                                          (unsigned int)state->chunk_bytes &
                                          STREAM_AUDIO_POSITIVE_BYTE_COUNT_MASK);
  tags[STREAM_AUDIO_TAG_SUSTAIN_END].ta_Arg = (void *)divmod.quotient;
  tags[STREAM_AUDIO_TAG_END].ta_Tag = TAG_END;
  tags[STREAM_AUDIO_TAG_END].ta_Arg = (void *)zero;

  state->sample_item = CreateSample(tags);
  if(state->sample_item <= 0)
    {
      int result;

      result = state->sample_item;
      MEM_DisposePointer(state->buffer_a);
      state->buffer_a = 0;
      state->buffer_b = 0;
      state->sample_item = 0;
      if(result < 0)
        {
          return result;
        }
      return -1;
    }

  state->buffer_b = buffer + state->chunk_bytes;
  state->active_buffer_index = -1;
  state->active.value = 1;

  return 0;
}


void
release_stream_audio_playback(void)
{
  AudioPlaybackState *state;
  int count;

  count = stream_audio_reference_count;
  if(count <= 0)
    {
      stream_audio_reference_count = 0;
      return;
    }
  count--;
  stream_audio_reference_count = count;
  if(count > 0)
    {
      return;
    }

  state = &audio_playback_state;
  if(state->playback_voice_id >= 0)
    {
      stop_audio_manager_voice(state->playback_voice_id);
      disown_audio_manager_sample_voice(state->playback_voice_id);
    }
  if(state->sample_item > 0)
    {
      UnloadSample(state->sample_item);
    }
  if(state->buffer_a != 0)
    {
      MEM_DisposePointer(state->buffer_a);
    }

  state->buffer_a = 0;
  state->buffer_b = 0;
  state->playback_voice_id = STREAM_AUDIO_INVALID_VOICE_ID;
  state->sample_item = 0;
  state->active_buffer_index = -1;
  state->active.value = 0;
}


void
submit_stream_audio_chunk(const unsigned char *sample_data,
                          unsigned int         sample_byte_count)
{
  AudioPlaybackState *state;
  char *buffer;
  unsigned int chunk_size;
  int active_buffer;
  UnsignedDivisionResult divmod;
  TagArg tags[STREAM_AUDIO_UPDATE_TAG_COUNT];
  int result;

  state = &audio_playback_state;
  if(state->active.value == 0)
    {
      result = initialize_stream_audio_playback(0);
      if(result < 0 || state->active.value == 0)
        {
          return;
        }
    }

  active_buffer = state->active_buffer_index;
  if(active_buffer < 0 && sample_byte_count == 0)
    {
      return;
    }
  if(state->buffer_a == 0)
    {
      return;
    }
  if(sample_byte_count > 0 && sample_data == 0)
    {
      return;
    }

  if(active_buffer < 0)
    {
      if(sample_byte_count != (unsigned int)state->chunk_bytes)
        {
          if((unsigned int)state->buffer_bytes >=
             (sample_byte_count << 1))
            {
              state->chunk_bytes = (int)sample_byte_count;
            }
          else
            {
              state->chunk_bytes = state->buffer_bytes >> 1;
            }

          state->buffer_b =
            (char *)state->buffer_a + state->chunk_bytes;

          tags[0].ta_Tag = AF_TAG_SUSTAINEND;
          divmod = unsigned_divide_with_remainder((unsigned int)state->channel_count,
                                                  (unsigned int)state->chunk_bytes &
                                                  STREAM_AUDIO_POSITIVE_BYTE_COUNT_MASK);
          tags[0].ta_Arg = (void *)divmod.quotient;
          tags[1].ta_Tag = TAG_END;
          tags[1].ta_Arg = 0;
          result = SetAudioItemInfo(state->sample_item, tags);
          if(result < 0)
            {
              return;
            }
        }
    }

  if(state->active_buffer_index == 1)
    {
      buffer = (char *)state->buffer_b;
    }
  else
    {
      buffer = (char *)state->buffer_a;
    }

  chunk_size = (unsigned int)state->chunk_bytes;
  if(sample_byte_count < chunk_size)
    {
      memset(buffer + sample_byte_count, 0,
             chunk_size - sample_byte_count);
    }
  if(sample_byte_count > chunk_size)
    {
      sample_byte_count = chunk_size;
    }
  if(sample_byte_count > 0)
    {
      memcpy(buffer, sample_data, sample_byte_count);
    }

  if(state->active_buffer_index < 0)
    {
      state->playback_voice_id =
        own_audio_manager_sample_voice(state->sample_item, 1);
      if(state->playback_voice_id < 0)
        {
          return;
        }
      result = start_audio_manager_voice(state->playback_voice_id,
                                         STREAM_AUDIO_PLAYBACK_RATE,
                                         state->amplitude, -1);
      if(result < 0)
        {
          disown_audio_manager_sample_voice(state->playback_voice_id);
          state->playback_voice_id = STREAM_AUDIO_INVALID_VOICE_ID;
          return;
        }
      result = set_audio_manager_voice_stereo_spread(
        state->playback_voice_id,
        AUDIO_MANAGER_VOICE_MAXIMUM_STEREO_SPREAD);
      if(result < 0)
        {
          stop_audio_manager_voice(state->playback_voice_id);
          disown_audio_manager_sample_voice(state->playback_voice_id);
          state->playback_voice_id = STREAM_AUDIO_INVALID_VOICE_ID;
          return;
        }
      state->active_buffer_index = 1;
    }
  else
    {
      state->active_buffer_index ^= 1;
    }
}
