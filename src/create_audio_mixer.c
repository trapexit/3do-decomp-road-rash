#include "audio_mixer_runtime.h"
#include "rw_semantic_data.h"

enum AudioMixerCreationConstant
{
  AUDIO_MIXER_NO_MEMORY_ERROR = -27,
  AUDIO_MIXER_CREATION_ERROR = -31,
  AUDIO_MIXER_2_INPUTS = 2,
  AUDIO_MIXER_4_INPUTS = 4,
  AUDIO_MIXER_8_INPUTS = 8,
  AUDIO_MIXER_12_INPUTS = 12,
  AUDIO_MIXER_14_INPUT_LIMIT = 14,
  AUDIO_MIXER_16_INPUT_LIMIT = 16,
  AUDIO_MIXER_20_INPUT_LIMIT = 20,
  AUDIO_MIXER_24_INPUT_LIMIT = 24,
  AUDIO_MIXER_26_INPUT_LIMIT = 26,
  AUDIO_MIXER_28_INPUT_LIMIT = 28,
  AUDIO_MIXER_32_INPUT_LIMIT = 32,
  AUDIO_MIXER_DOUBLE_SUBMIXER_COUNT = 2
};

static
AudioSubmixerRuntime *
append_audio_submixer(AudioMixerRuntime *mixer,
                      int                input_count,
                      int                template_item)
{
  AudioSubmixerRuntime *submixer;
  int index;

  submixer = create_audio_submixer(input_count, template_item);
  if(submixer != 0)
    {
      index = mixer->submixer_count;
      mixer->submixer_count = index + 1;
      mixer->submixers[index] = submixer;
    }
  return submixer;
}


static
int
append_audio_submixers(AudioMixerRuntime *mixer,
                       int                submixer_count,
                       int                input_count,
                       int                template_item)
{
  int index;
  int result;

  index = 0;
  while(index < submixer_count)
    {
      if(append_audio_submixer(mixer, input_count, template_item) == 0)
        {
          result = music_library_globals.last_error;
          if(result >= 0)
            {
              result = AUDIO_MIXER_CREATION_ERROR;
            }
          return result;
        }
      index++;
    }
  return 0;
}


AudioMixerRuntime *
create_audio_mixer(int input_count,
                   int template_image)
{
  AudioMixerRuntime *mixer;
  int mixer_2_count;
  int mixer_4_count;
  int mixer_8_count;
  int mixer_12_count;
  int result;

  mixer_2_count = 0;
  mixer_4_count = 0;
  mixer_8_count = 0;
  mixer_12_count = 0;
  result = 0;

  if(input_count <= 0 || input_count > AUDIO_MIXER_32_INPUT_LIMIT)
    {
      music_library_globals.last_error = AUDIO_MIXER_CREATION_ERROR;
      return 0;
    }

  mixer = (AudioMixerRuntime *)music_library_globals.allocate(
    sizeof(AudioMixerRuntime), 0);
  if(mixer == 0)
    {
      music_library_globals.last_error = AUDIO_MIXER_NO_MEMORY_ERROR;
      return 0;
    }

  mixer->channels =
    (AudioMixerChannelState *)music_library_globals.allocate(
      input_count * sizeof(AudioMixerChannelState), 0);
  if(mixer->channels == 0)
    {
      music_library_globals.last_error = AUDIO_MIXER_NO_MEMORY_ERROR;
      music_library_globals.release((int)mixer, sizeof(AudioMixerRuntime));
      return 0;
    }

  mixer->input_count = input_count;
  initialize_audio_mixer(mixer, input_count);

  if(input_count <= AUDIO_MIXER_2_INPUTS)
    {
      mixer_2_count = 1;
    }
  else if(input_count <= AUDIO_MIXER_4_INPUTS)
    {
      mixer_4_count = 1;
    }
  else if(input_count <= AUDIO_MIXER_8_INPUTS)
    {
      mixer_8_count = 1;
    }
  else if(input_count <= AUDIO_MIXER_12_INPUTS)
    {
      mixer_12_count = 1;
    }
  else if(input_count <= AUDIO_MIXER_14_INPUT_LIMIT)
    {
      mixer_2_count = 1;
      mixer_12_count = 1;
    }
  else if(input_count <= AUDIO_MIXER_16_INPUT_LIMIT)
    {
      mixer_8_count = AUDIO_MIXER_DOUBLE_SUBMIXER_COUNT;
    }
  else if(input_count <= AUDIO_MIXER_20_INPUT_LIMIT)
    {
      mixer_8_count = 1;
      mixer_12_count = 1;
    }
  else if(input_count <= AUDIO_MIXER_24_INPUT_LIMIT)
    {
      mixer_12_count = AUDIO_MIXER_DOUBLE_SUBMIXER_COUNT;
    }
  else if(input_count <= AUDIO_MIXER_26_INPUT_LIMIT)
    {
      mixer_2_count = 1;
      mixer_12_count = AUDIO_MIXER_DOUBLE_SUBMIXER_COUNT;
    }
  else if(input_count <= AUDIO_MIXER_28_INPUT_LIMIT)
    {
      mixer_4_count = 1;
      mixer_12_count = AUDIO_MIXER_DOUBLE_SUBMIXER_COUNT;
    }
  else if(input_count <= AUDIO_MIXER_32_INPUT_LIMIT)
    {
      mixer_8_count = 1;
      mixer_12_count = AUDIO_MIXER_DOUBLE_SUBMIXER_COUNT;
    }

  result = load_audio_mixer_templates(
    mixer, template_image, mixer_2_count, mixer_4_count,
    mixer_8_count, mixer_12_count);
  if(result < 0)
    {
      music_library_globals.last_error = result;
    }
  else
    {
      result = append_audio_submixers(
        mixer, mixer_2_count, AUDIO_MIXER_2_INPUTS,
        mixer->template_items[AUDIO_MIXER_TEMPLATE_2_INPUT]);
      if(result >= 0)
        {
          result = append_audio_submixers(
            mixer, mixer_4_count, AUDIO_MIXER_4_INPUTS,
            mixer->template_items[AUDIO_MIXER_TEMPLATE_4_INPUT]);
        }
      if(result >= 0)
        {
          result = append_audio_submixers(
            mixer, mixer_8_count, AUDIO_MIXER_8_INPUTS,
            mixer->template_items[AUDIO_MIXER_TEMPLATE_8_INPUT]);
        }
      if(result >= 0)
        {
          result = append_audio_submixers(
            mixer, mixer_12_count, AUDIO_MIXER_12_INPUTS,
            mixer->template_items[AUDIO_MIXER_TEMPLATE_12_INPUT]);
        }
      if(result < 0)
        {
          music_library_globals.last_error = result;
        }
    }

  if(result < 0)
    {
      destroy_audio_mixer(mixer);
      mixer = 0;
    }

  return mixer;
}
