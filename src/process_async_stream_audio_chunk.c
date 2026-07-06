#include "audio.h"

#include "async_stream_playback_helpers.h"
#include "division_helpers.h"
#include "saudio_subscriber_runtime.h"

enum
{
  ASYNC_STREAM_AUDIO_CLOCK_TICKS_PER_SECOND = 240
};

typedef union AsyncStreamAudioChunk
{
  AsyncStreamChunkHeader common;
  SAudioHeaderChunk header;
  SAudioSampleChunk sample;
} AsyncStreamAudioChunk;

static
int
signed_quotient(int          divisor,
                unsigned int numerator)
{
  SignedDivisionResult division;

  division = signed_divide_with_remainder(
    (unsigned int)divisor, numerator);
  return (int)division.quotient;
}


/* Reconstructed from the original function at 0x000324D4. */
void
process_async_stream_audio_chunk(const AsyncStreamChunkHeader    *chunk,
                                 AsyncStreamAudioPlaybackContext *audio)
{
  const AsyncStreamAudioChunk *audio_chunk;
  const unsigned char *sample_data;
  unsigned int sample_byte_count;
  unsigned int sample_duration_numerator;
  int sample_duration_ticks;
  unsigned int scheduled_start_time;
  int lateness;
  int result;

  audio_chunk = (const AsyncStreamAudioChunk *)chunk;

  if(chunk->subchunk_type == SAUDIO_HEADER_CHUNK_TYPE)
    {
      if(audio->initialize_callback != 0)
        {
          result = audio->initialize_callback(&audio_chunk->header);
          if(result < 0)
            {
              audio->state->block_stream->last_error = result;
              return;
            }
        }
      audio->channel_count = audio_chunk->header.sample.channel_count;
      return;
    }

  if(chunk->subchunk_type != SAUDIO_SAMPLE_CHUNK_TYPE)
    {
      return;
    }

  if(audio->awaiting_first_sample != 0)
    {
      audio->awaiting_first_sample = 0;
    }

  sample_duration_numerator =
    (unsigned int)audio_chunk->sample.sample_bytes *
    (unsigned int)ASYNC_STREAM_AUDIO_CLOCK_TICKS_PER_SECOND;
  sample_duration_ticks = signed_quotient(
    SAUDIO_SAMPLE_RATE_22KHZ,
    sample_duration_numerator);
  sample_duration_ticks = signed_quotient(
    audio->channel_count, sample_duration_ticks);

  scheduled_start_time =
    audio->state->audio_clock_origin + chunk->stream_time;
  lateness = (int)((unsigned int)GetAudioTime() - scheduled_start_time);
  if(lateness <= sample_duration_ticks)
    {
      sample_data = audio_chunk->sample.samples;
      sample_byte_count =
        (unsigned int)audio_chunk->sample.sample_bytes;
    }
  else
    {
      sample_data = 0;
      sample_byte_count = 0;
    }

  result = SleepUntilTime(audio->state->audio_cue_item, scheduled_start_time);
  if(result < 0)
    {
      audio->state->block_stream->last_error = result;
      return;
    }

  if(audio->submit_callback != 0)
    {
      audio->submit_callback(sample_data, sample_byte_count);
    }

  audio->scheduled_end_time =
    audio->state->audio_clock_origin + chunk->stream_time +
    sample_duration_ticks;
}
