#include "audio.h"
#include "debug.h"
#include "stdio.h"

#include "dll_runtime.h"
#include "midi_player_runtime.h"
#include "rw_semantic_data.h"

#define SAMPLE_INFO_TAG_COUNT 3
#define SAMPLE_ADDRESS_TAG   0
#define SAMPLE_BYTES_TAG     1
#define SAMPLE_END_TAG       2

typedef struct SampleDataRange
{
  int address;
  int byte_count;
} SampleDataRange;

void
get_music_memory_vectors(int *allocator,
                         int *releaser);
int *
DLL_New(RwMemoryAllocator allocator,
        RwMemoryReleaser  releaser);
int
DLL_GetErr(void);
int
DLL_AddTail(int *list,
            int  entry);
int
register_program_sample_data(MidiPlayer *player,
                             int         sample_item)
{
  int releaser;
  int allocator;
  int skip_allocation;
  TagArg tags[SAMPLE_INFO_TAG_COUNT];
  DLLIterator *iter;
  SampleDataRange *entry;
  SampleDataRange *new_range;
  int result;
  int sample_address;
  int sample_bytes;

  skip_allocation = 0;
  allocator = 0;
  releaser = 0;
  get_music_memory_vectors(&allocator, &releaser);

  if(player->score_resource_list == 0)
    {
      player->score_resource_list =
        DLL_New((RwMemoryAllocator)allocator,
                (RwMemoryReleaser)releaser);
      if(player->score_resource_list == 0)
        {
          result = DLL_GetErr();
          return result;
        }
    }

  tags[SAMPLE_ADDRESS_TAG].ta_Tag = AF_TAG_ADDRESS;
  tags[SAMPLE_ADDRESS_TAG].ta_Arg = 0;
  tags[SAMPLE_BYTES_TAG].ta_Tag = AF_TAG_NUMBYTES;
  tags[SAMPLE_BYTES_TAG].ta_Arg = 0;
  tags[SAMPLE_END_TAG].ta_Tag = TAG_END;
  tags[SAMPLE_END_TAG].ta_Arg = 0;

  result = GetAudioItemInfo(sample_item, tags);
  if(result < 0)
    {
      return result;
    }
  if(tags[SAMPLE_ADDRESS_TAG].ta_Arg == 0)
    {
      kprintf("OMP_AttachPrgInSampels: sample data address == 0!!\n");
      fflush(stdout);
      return result;
    }

  sample_address = (int)tags[SAMPLE_ADDRESS_TAG].ta_Arg;
  sample_bytes = (int)tags[SAMPLE_BYTES_TAG].ta_Arg;
  iter = DLL_IterNew((DLLList *)player->score_resource_list, DLL_ITERATE_FORWARD);
  if(iter == 0)
    {
      result = DLL_GetErr();
    }
  else
    {
      entry = (SampleDataRange *)DLL_IterFirst(iter);
      while(DLL_IterValid(iter) != 0)
        {
          if(entry->address == sample_address)
            {
              break;
            }
          entry = (SampleDataRange *)DLL_IterNext(iter);
        }

      if(DLL_IterValid(iter) == 0 && skip_allocation == 0)
        {
          new_range = (SampleDataRange *)music_library_globals.allocate(
            sizeof(SampleDataRange), 0);
          if(new_range != 0)
            {
              new_range->address = sample_address;
              new_range->byte_count = sample_bytes;
              result = DLL_AddTail((int *)player->score_resource_list,
                                   (int)new_range);
              if(result < 0)
                {
                  music_library_globals.release(
                    (int)new_range, sizeof(SampleDataRange));
                }
            }
          else
            {
              result = MIDI_ERROR_NO_MEMORY;
            }
        }
    }

  if(iter != 0)
    {
      DLL_IterDelete(iter);
    }
  return result;
}
