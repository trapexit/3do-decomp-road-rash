/* Reconstructed from the original function at 0x00041204. */

#include "division_helpers.h"
#include "dll_runtime.h"
#include "midi_player_runtime.h"
#include "score_context_api.h"
#include "score_program_runtime.h"

#define MIDI_PLAYER_BAD_ARGUMENT (-0x15)
#define MIDI_PLAYER_NO_MEMORY    (-0x17)
#define MAX_SCORE_AMPLITUDE      0x7FFF
#define SCORE_PROGRAM_RATE_DIVIDE_NONE 1

int
initialize_score_mixer_from_template(ScoreContext *score_context,
                                     int           mixer_template,
                                     int           voice_count,
                                     int           channel_amplitude);
int
attach_program_input_samples(MidiPlayer         *player,
                             ScoreProgramRecord *program,
                             int                 sample_info_list);
int
DLL_GetErr(void);
int
DLL_GetItemCount(int list);
int
initialize_midi_player_program_map(MidiPlayer *player,
                                   int         program_list,
                                   int         sample_info_list,
                                   int         mixer_template)
{
  int result;
  DLLIterator *iter;
  ScoreProgramRecord *program;
  unsigned int max_program_number;
  int channel_amplitude;
  ScoreContext *score_context;

  result = 0;
  if(player == 0 || program_list == 0 ||
     DLL_GetItemCount(program_list) <= 0 || mixer_template < 0)
    {
      return MIDI_PLAYER_BAD_ARGUMENT;
    }

  iter = DLL_IterNew((DLLList *)program_list, DLL_ITERATE_FORWARD);
  if(iter == 0)
    {
      return DLL_GetErr();
    }

  max_program_number = 0;
  DLL_IterReset(iter);
  program = (ScoreProgramRecord *)DLL_IterFirst(iter);
  while(DLL_IterValid(iter) != 0)
    {
      if(max_program_number < program->program_number)
        {
          max_program_number = program->program_number;
        }
      program = (ScoreProgramRecord *)DLL_IterNext(iter);
    }

  score_context = (ScoreContext *)player->score_context;
  if(score_context != 0)
    {
      result = DeleteScoreContext(score_context);
      if(result < 0)
        {
          goto cleanup;
        }
    }

  score_context = CreateScoreContext(max_program_number + 1);
  player->score_context = score_context;
  if(score_context == 0)
    {
      result = MIDI_PLAYER_NO_MEMORY;
      goto cleanup;
    }

  player->mixer_voice_count = 0;
  DLL_IterReset(iter);
  program = (ScoreProgramRecord *)DLL_IterFirst(iter);
  while(DLL_IterValid(iter) != 0)
    {
      result = SetPIMapEntry(score_context, program->program_number,
                             program->instrument_template_item,
                             program->max_voices, program->priority);
      /*
       * The V24 score folio repurposed PIMapEntry's former reserved byte as
       * pimp_RateDivide and reads it on the first note. SetPIMapEntry leaves
       * it zero, which wedges the folio; 1 restores the 1.x behavior of no
       * rate division.
       */
      score_context->scon_PIMap[program->program_number].pimp_RateDivide =
        SCORE_PROGRAM_RATE_DIVIDE_NONE;
      if(result < 0)
        {
          goto cleanup;
        }

      player->mixer_voice_count += program->max_voices;
      result = attach_program_input_samples(player, program, sample_info_list);
      if(result < 0)
        {
          goto cleanup;
        }

      program = (ScoreProgramRecord *)DLL_IterNext(iter);
    }

  channel_amplitude = player->mixer_amplitude;
  if(channel_amplitude < 0)
    {
      channel_amplitude = signed_divide_with_remainder(
        max_program_number + 1,
        MAX_SCORE_AMPLITUDE).quotient;
    }
  result = initialize_score_mixer_from_template(score_context, mixer_template,
                                                player->mixer_voice_count, channel_amplitude);

cleanup:
  DLL_IterDelete(iter);
  return result;
}
