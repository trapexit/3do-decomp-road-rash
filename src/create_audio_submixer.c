#include "audio.h"

#include "audio_mixer_runtime.h"
#include "rw_semantic_data.h"

enum AudioSubmixerCreationConstant
{
  AUDIO_SUBMIXER_INSTRUMENT_PRIORITY = 0x64,
  AUDIO_MIXER_NO_MEMORY_ERROR = -27
};

AudioSubmixerRuntime *
create_audio_submixer(int input_count,
                      int template_item)
{
  AudioSubmixerRuntime *submixer;
  int input_index;
  int instrument_item;

  input_index = 0;
  music_library_globals.last_error = 0;

  submixer = (AudioSubmixerRuntime *)music_library_globals.allocate(
    sizeof(AudioSubmixerRuntime), 0);
  if(submixer == 0)
    {
      music_library_globals.last_error = AUDIO_MIXER_NO_MEMORY_ERROR;
    }
  else
    {
      initialize_audio_submixer(submixer);

      instrument_item = AllocInstrument(
        template_item, AUDIO_SUBMIXER_INSTRUMENT_PRIORITY);
      if(instrument_item < 0)
        {
          music_library_globals.last_error = instrument_item;
        }
      else
        {
          submixer->input_count = input_count;
          submixer->maximum_amplitude = 0;
          submixer->instrument_item = instrument_item;

          while(input_index < input_count)
            {
              music_library_globals.last_error =
                bind_audio_submixer_input_knobs(submixer, input_index);
              if(music_library_globals.last_error < 0)
                {
                  break;
                }
              input_index++;
            }

          if(music_library_globals.last_error >= 0)
            {
              music_library_globals.last_error =
                StartInstrument(instrument_item, 0);
              submixer->valid_left_amplitudes = 0;
              submixer->valid_right_amplitudes = 0;
            }
        }
    }

  if(music_library_globals.last_error == 0)
    {
      return submixer;
    }

  destroy_audio_submixer(submixer);
  return 0;
}
