#include "audio.h"

#include "audio_mixer_runtime.h"

void
unload_audio_mixer_templates(AudioMixerRuntime *mixer)
{
  int template_index;

  template_index = 0;
  while(template_index < AUDIO_MIXER_TEMPLATE_COUNT)
    {
      if(mixer->template_items[template_index] >= 0)
        {
          UnloadInsTemplate(mixer->template_items[template_index]);
          mixer->template_items[template_index] = -1;
        }
      template_index++;
    }
}
