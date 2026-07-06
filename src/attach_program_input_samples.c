#include "audio.h"
#include "dll_runtime.h"
#include "midi_player_runtime.h"
#include "score_program_runtime.h"

int
DLL_GetItemCount(DLLList *list);
int
DLL_GetErr(void);
int
get_indexed_sample_info_record(int sample_info_list,
                               int sample_index);
int
create_audio_attachment(int instrument_item,
                        int sample_item,
                        int hook_name,
                        int attachment_flags);
int
register_program_sample_data(MidiPlayer *player,
                             int         sample_item);

int
attach_program_input_samples(MidiPlayer         *player,
                             ScoreProgramRecord *program,
                             int                 sample_info_list)
{
  DLLIterator *iter;
  int sample_index;
  int sample_item;
  int result;
  DLLList *sample_index_list;

  result = 0;
  sample_index_list = program->sample_index_list;
  if(sample_index_list == 0)
    {
      return result;
    }
  if(DLL_GetItemCount(sample_index_list) <= 0)
    {
      return result;
    }

  iter = DLL_IterNew(sample_index_list, DLL_ITERATE_FORWARD);
  if(iter == 0)
    {
      result = DLL_GetErr();
    }
  else
    {
      sample_index = DLL_IterFirst(iter) &
                     SCORE_PROGRAM_SAMPLE_INDEX_MASK;
      while(DLL_IterValid(iter) != 0)
        {
          sample_item = get_indexed_sample_info_record(sample_info_list, sample_index);
          if(sample_item < 0)
            {
              result = sample_item;
              break;
            }

          result = create_audio_attachment(
            program->instrument_template_item,
            sample_item, 0,
            AF_ATTF_FATLADYSINGS);
          if(result < 0)
            {
              break;
            }

          result = register_program_sample_data(player, sample_item);
          if(result < 0)
            {
              break;
            }

          sample_index = DLL_IterNext(iter) &
                         SCORE_PROGRAM_SAMPLE_INDEX_MASK;
        }
    }

  if(iter != 0)
    {
      DLL_IterDelete(iter);
    }
  return result;
}
