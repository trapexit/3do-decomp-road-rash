#include "audio.h"

#include "dll_runtime.h"
#include "midi_player_runtime.h"
#include "rw_semantic_data.h"
#include "score_program_runtime.h"

int
DLL_Delete(DLLList *list);

int
cleanup_score_program_records(MidiPlayer *player,
                              DLLList *program_records,
                              int resources_transferred)
{
  DLLNode *node;
  ScoreProgramRecord *program;
  int result;
  int release_result;

  result = 0;
  if(program_records == 0)
    {
      return result;
    }
  node = program_records->head;
  while(node != 0)
    {
      program = (ScoreProgramRecord *)node->value;
      if((resources_transferred & SCORE_RESOURCE_TRANSFER_FLAG_MASK) == 0 &&
         program->instrument_template_item >= 0)
        {
          release_result = UnloadInsTemplate(program->instrument_template_item);
          if(release_result < 0 && result >= 0)
            {
              result = release_result;
            }
        }
      if(program->sample_index_list != 0)
        {
          DLL_Delete(program->sample_index_list);
        }
      music_library_globals.release((int)program, sizeof(ScoreProgramRecord));
      node = node->next;
    }
  return result;
}
