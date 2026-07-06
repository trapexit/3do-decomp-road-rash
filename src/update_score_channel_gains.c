#include "score_context_api.h"

#define SCORE_VOLUME_ONE           0x4000
#define SCORE_VOLUME_FRACTION_BITS 14

int
apply_score_channel_gains_to_note(ScoreContext *score_context,
                                  int           channel_index,
                                  NoteTracker  *note);

int
update_score_channel_gains(ScoreContext *score_context,
                           int           channel_index)
{
  ScoreChannel *channel;
  NoteTracker *note;
  int value;

  channel = &score_context->scon_Channels[channel_index];

  value = (channel->schn_Volume * score_context->scon_MaxVolume) >>
          SCORE_VOLUME_FRACTION_BITS;
  channel->schn_LeftVolume =
    ((SCORE_VOLUME_ONE - channel->schn_Pan) * value) >>
    SCORE_VOLUME_FRACTION_BITS;
  channel->schn_RightVolume =
    (channel->schn_Pan * value) >> SCORE_VOLUME_FRACTION_BITS;

  note = (NoteTracker *)FIRSTNODE(&channel->schn_NoteList);
  while(ISNODE(&channel->schn_NoteList, (Node *)note))
    {
      apply_score_channel_gains_to_note(score_context, channel_index, note);
      note = (NoteTracker *)NEXTNODE(note);
    }

  return 0;
}
