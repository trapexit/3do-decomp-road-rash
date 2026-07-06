#include "audio.h"
#include "score_context_api.h"
/* Reconstructed from the original function at 0x000402DC. */

int
apply_score_channel_gains_to_note(ScoreContext *score_context,
                                  int           channel_index,
                                  NoteTracker  *note)
{
  ScoreChannel *channel;
  int mixer_channel;
  int result;

  mixer_channel = note->nttr_MixerChannel;
  channel = &score_context->scon_Channels[channel_index];

  result = TweakKnob(score_context->scon_LeftGains[mixer_channel],
                     channel->schn_LeftVolume);
  if(result >= 0)
    {
      result = TweakKnob(score_context->scon_RightGains[mixer_channel],
                         channel->schn_RightVolume);
    }

  return result;
}
