#include "score_context_api.h"

#include "audio.h"

#include "rw_semantic_data.h"

#define MIXER_GAIN_NAME_COUNT 12
#define SCORE_MIXER_PRIORITY  0xB4

int
update_score_channel_gains(ScoreContext *score_context,
                           int           voice_index);
int
initialize_score_mixer_from_template(ScoreContext *score_context,
                                     int           mixer_template,
                                     int           voice_count,
                                     int           channel_amplitude)
{
  char **left_names;
  char **right_names;
  int index;
  int result;
  int instrument;

  result = 0;
  index = 0;
  if(score_context->scon_MaxVoices <= 0)
    {
      result = InitScoreDynamics(score_context, voice_count);
      if(result < 0)
        {
          return result;
        }
    }

  score_context->scon_MaxVolume = channel_amplitude;
  instrument = AllocInstrument(mixer_template, SCORE_MIXER_PRIORITY);
  score_context->scon_MixerIns = instrument;
  if(instrument < 0)
    {
      return instrument;
    }

  if(voice_count > 0)
    {
      left_names = (char **)mixer_gain_names;
      right_names =
        (char **)&mixer_gain_names[MIXER_GAIN_NAME_COUNT];
      do
        {
          result = GrabKnob(instrument, left_names[index]);
          score_context->scon_LeftGains[index] = result;
          if(result < 0)
            {
              return result;
            }

          result = GrabKnob(instrument, right_names[index]);
          score_context->scon_RightGains[index] = result;
          if(result < 0)
            {
              return result;
            }

          update_score_channel_gains(score_context, index);
          index++;
        } while(index < voice_count);
    }

  result = StartInstrument(score_context->scon_MixerIns, 0);
  return result;
}
