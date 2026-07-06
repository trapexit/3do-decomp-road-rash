#include "debug.h"
#include "stdio.h"

#include "iff_runtime.h"
#include "rw_semantic_data.h"
#include "score_program_runtime.h"

void
get_music_memory_vectors(RwMemoryAllocator *out_allocator,
                         RwMemoryReleaser  *out_releaser);
DLLList *
DLL_New(RwMemoryAllocator allocator,
        RwMemoryReleaser  releaser);
int
DLL_GetErr(void);
int
DLL_AddTail(DLLList *list,
            int      value);

int
load_score_program_sample_refs_chunk(struct MidiPlayer    *player,
                                     ByteFileReader       *reader,
                                     const IffChunkHeader *chunk_header,
                                     ScoreProgramRecord   *program)
{
  RwMemoryAllocator allocator;
  RwMemoryReleaser releaser;
  DLLList *sample_indexes;
  unsigned char sample_index;
  int index;
  int result;

  sample_index = 0;
  index = 0;
  allocator = 0;
  releaser = 0;

  get_music_memory_vectors(&allocator, &releaser);
  sample_indexes = DLL_New(allocator, releaser);
  if(sample_indexes == 0)
    {
      result = DLL_GetErr();
      kprintf(
        "DLL_New failed in OMStPreamProgSampsRefListChunk(), error %ld\n\n",
        result);
      fflush(stdout);
      return result;
    }

  program->sample_index_list = sample_indexes;
  result = enter_score_iff_chunk_payload(reader);
  if(result < 0)
    {
      return result;
    }

  if(chunk_header->chunk_size > 0)
    {
      do
        {
          result = read_score_chunk_bytes(
            reader, &sample_index, 1, 0);
          if(result < 0)
            {
              return result;
            }

          result = DLL_AddTail(sample_indexes, sample_index);
          if(result != 0)
            {
              return result;
            }

          index++;
        } while(index < chunk_header->chunk_size);
    }

  return IFF_AlignReader(reader);
}
