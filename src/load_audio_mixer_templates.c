#include "audio.h"

#include "audio_mixer_runtime.h"
#include "iff_runtime.h"
#include "rw_semantic_data.h"

enum AudioMixerTemplateChunk
{
  AUDIO_MIXER_TEMPLATE_CHUNK_2_INPUT = 0x4D783032,
  AUDIO_MIXER_TEMPLATE_CHUNK_4_INPUT = 0x4D783034,
  AUDIO_MIXER_TEMPLATE_CHUNK_8_INPUT = 0x4D783038,
  AUDIO_MIXER_TEMPLATE_CHUNK_12_INPUT = 0x4D783132,
  AUDIO_MIXER_TEMPLATE_CHUNK_MISSING_ERROR = -21
};

static
int
load_audio_mixer_template_from_file(AudioMixerRuntime *mixer,
                                    int                template_index)
{
  int template_item;

  template_item = LoadInsTemplate(
    (char *)audio_mixer_template_defaults.instrument_names[
      template_index],
    0);
  mixer->template_items[template_index] = template_item;
  return template_item;
}


static
int
define_audio_mixer_template_from_image(AudioMixerRuntime *mixer,
                                       int                template_image,
                                       int                template_index,
                                       int                chunk_type)
{
  int chunk_size;
  int chunk_data;
  int template_item;

  chunk_size = 0;
  chunk_data = (int)find_iff_form_in_image(
    (const void *)template_image, (unsigned int)chunk_type,
    &chunk_size);
  if(chunk_data == 0)
    {
      return AUDIO_MIXER_TEMPLATE_CHUNK_MISSING_ERROR;
    }

  template_item = DefineInsTemplate(
    (uint8 *)chunk_data, chunk_size, 0,
    (char *)audio_mixer_template_defaults.instrument_names[
      template_index]);
  mixer->template_items[template_index] = template_item;
  return template_item;
}


int
load_audio_mixer_templates(AudioMixerRuntime *mixer,
                           int                template_image,
                           int                mixer_2_count,
                           int                mixer_4_count,
                           int                mixer_8_count,
                           int                mixer_12_count)
{
  int result;

  result = 0;

  if(template_image == 0)
    {
      if(mixer_2_count > 0)
        {
          result = load_audio_mixer_template_from_file(
            mixer, AUDIO_MIXER_TEMPLATE_2_INPUT);
          if(result < 0)
            {
              return result;
            }
        }
      if(mixer_4_count > 0)
        {
          result = load_audio_mixer_template_from_file(
            mixer, AUDIO_MIXER_TEMPLATE_4_INPUT);
          if(result < 0)
            {
              return result;
            }
        }
      if(mixer_8_count > 0)
        {
          result = load_audio_mixer_template_from_file(
            mixer, AUDIO_MIXER_TEMPLATE_8_INPUT);
          if(result < 0)
            {
              return result;
            }
        }
      if(mixer_12_count > 0)
        {
          result = load_audio_mixer_template_from_file(
            mixer, AUDIO_MIXER_TEMPLATE_12_INPUT);
        }
      return result;
    }

  if(mixer_2_count > 0)
    {
      result = define_audio_mixer_template_from_image(
        mixer, template_image, AUDIO_MIXER_TEMPLATE_2_INPUT,
        AUDIO_MIXER_TEMPLATE_CHUNK_2_INPUT);
      if(result < 0)
        {
          return result;
        }
    }
  if(mixer_4_count > 0)
    {
      result = define_audio_mixer_template_from_image(
        mixer, template_image, AUDIO_MIXER_TEMPLATE_4_INPUT,
        AUDIO_MIXER_TEMPLATE_CHUNK_4_INPUT);
      if(result < 0)
        {
          return result;
        }
    }
  if(mixer_8_count > 0)
    {
      result = define_audio_mixer_template_from_image(
        mixer, template_image, AUDIO_MIXER_TEMPLATE_8_INPUT,
        AUDIO_MIXER_TEMPLATE_CHUNK_8_INPUT);
      if(result < 0)
        {
          return result;
        }
    }
  if(mixer_12_count > 0)
    {
      result = define_audio_mixer_template_from_image(
        mixer, template_image, AUDIO_MIXER_TEMPLATE_12_INPUT,
        AUDIO_MIXER_TEMPLATE_CHUNK_12_INPUT);
      if(result < 0)
        {
          return result;
        }
    }

  return result;
}
