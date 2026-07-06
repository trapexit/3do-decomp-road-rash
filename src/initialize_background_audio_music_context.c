#include "audio_mixer_runtime.h"
#include "rw_semantic_data.h"

int
initialize_music_context_from_tags(int tag_list);

enum BackgroundAudioMusicContextEntryKind
{
  BACKGROUND_AUDIO_CONTEXT_NAMED_INSTRUMENT = 6,
  BACKGROUND_AUDIO_CONTEXT_END = 10
};

enum BackgroundAudioMusicContextEntryIndex
{
  BACKGROUND_AUDIO_CONTEXT_DECODER_ENTRY = 0,
  BACKGROUND_AUDIO_CONTEXT_SAMPLER_ENTRY,
  BACKGROUND_AUDIO_CONTEXT_END_ENTRY,
  BACKGROUND_AUDIO_CONTEXT_ENTRY_COUNT
};

typedef struct BackgroundAudioNamedInstrument
{
  int voice_count;
  const char *instrument_name;
} BackgroundAudioNamedInstrument;

typedef struct BackgroundAudioMusicContext
{
  RwTagArgument entries[BACKGROUND_AUDIO_CONTEXT_ENTRY_COUNT];
  unsigned int terminator;
  unsigned int reserved_alignment;
  BackgroundAudioNamedInstrument sampler;
  BackgroundAudioNamedInstrument decoder;
} BackgroundAudioMusicContext;

int
initialize_background_audio_music_context(void)
{
  BackgroundAudioMusicContext context;
  int result;

  context.decoder.voice_count = 1;
  context.decoder.instrument_name = "dcsqxdhalfstereo.dsp";
  context.entries[BACKGROUND_AUDIO_CONTEXT_DECODER_ENTRY].tag =
    BACKGROUND_AUDIO_CONTEXT_NAMED_INSTRUMENT;
  context.entries[BACKGROUND_AUDIO_CONTEXT_DECODER_ENTRY].argument =
    (unsigned int)&context.decoder;

  context.sampler.voice_count = 1;
  context.sampler.instrument_name = "sampler.dsp";
  context.entries[BACKGROUND_AUDIO_CONTEXT_SAMPLER_ENTRY].tag =
    BACKGROUND_AUDIO_CONTEXT_NAMED_INSTRUMENT;
  context.entries[BACKGROUND_AUDIO_CONTEXT_SAMPLER_ENTRY].argument =
    (unsigned int)&context.sampler;

  context.entries[BACKGROUND_AUDIO_CONTEXT_END_ENTRY].tag =
    BACKGROUND_AUDIO_CONTEXT_END;
  context.entries[BACKGROUND_AUDIO_CONTEXT_END_ENTRY].argument = 0;
  context.terminator = 0;

  result = initialize_music_context_from_tags((int)&context);
  if(result < 0)
    {
      return result;
    }
  set_global_audio_mixer_maximum_amplitude(
    AUDIO_MIXER_FULL_SCALE_AMPLITUDE);
  return 0;
}
