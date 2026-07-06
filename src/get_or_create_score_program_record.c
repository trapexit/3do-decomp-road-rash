#include "debug.h"
#include "stdio.h"

#include "dll_runtime.h"
#include "rw_semantic_data.h"
#include "score_program_runtime.h"

int
DLL_GetErr(void);
int
DLL_AddTail(DLLList *list,
            int      value);

ScoreProgramRecord *
get_or_create_score_program_record(DLLList     *program_records,
                                   unsigned int program_number)
{
  RwMemoryAllocator allocate;
  DLLIterator *iterator;
  ScoreProgramRecord *record;
  ScoreProgramRecord *match;
  unsigned char wanted_program;
  int error;

  wanted_program = (unsigned char)(program_number &
                                   SCORE_PROGRAM_SAMPLE_INDEX_MASK);
  match = 0;
  iterator = DLL_IterNew(program_records, DLL_ITERATE_FORWARD);
  if(iterator == 0)
    {
      error = DLL_GetErr();
      kprintf("GetOrCreateScoreProgRec:DLL_IterNew failed, error %ld\n",
              error);
      fflush(stdout);
    }
  else
    {
      record = (ScoreProgramRecord *)DLL_IterFirst(iterator);
      while(DLL_IterValid(iterator) != 0)
        {
          if(record->program_number == wanted_program)
            {
              match = record;
            }
          record = (ScoreProgramRecord *)DLL_IterNext(iterator);
        }

      if(match == 0)
        {
          allocate = music_library_globals.allocate;
          match = (ScoreProgramRecord *)allocate(
            sizeof(ScoreProgramRecord), 0);
          if(match != 0)
            {
              match->program_number = wanted_program;
              match->priority = 0;
              match->max_voices = 1;
              match->reserved_03 = 0;
              match->reserved_04 = 0;
              match->instrument_template_item = -1;
              match->sample_index_list = 0;
              error = DLL_AddTail(program_records, (int)match);
              if(error < 0)
                {
                  music_library_globals.release(
                    (int)match, sizeof(ScoreProgramRecord));
                  match = 0;
                }
            }
        }
    }

  DLL_IterDelete(iterator);
  return match;
}
