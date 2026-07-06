#include "dll_runtime.h"
#include "midi_player_runtime.h"
#include "runtime_data_types.h"
#include "score_context_api.h"

int
DLL_Delete(DLLList *list);

int
unload_midi_player_score(MidiPlayer *player)
{
  ScoreContext *score_context;
  DLLNode *node;
  MidiSampleAllocation *allocation;

  if(player == 0)
    {
      return MIDI_ERROR_BAD_POINTER;
    }

  player->score_loaded = 0;
  score_context = (ScoreContext *)player->score_context;
  if(score_context != 0)
    {
      unload_positive_score_instrument_templates(score_context);
      TermScoreMixer(score_context);
    }
  if(player->score_resource_list != 0)
    {
      node = ((DLLList *)player->score_resource_list)->head;
      while(node != 0)
        {
          allocation = (MidiSampleAllocation *)node->value;
          music_library_globals.release(allocation->address, allocation->byte_count);
          music_library_globals.release((int)allocation, sizeof(MidiSampleAllocation));
          node = node->next;
        }
      DLL_Delete((DLLList *)player->score_resource_list);
      player->score_resource_list = 0;
    }
  return 0;
}
