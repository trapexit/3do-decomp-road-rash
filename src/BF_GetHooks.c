#include "rw_semantic_data.h"

void
BF_GetHooks(int *out_allocate,
            int *out_release)
{
  if(out_allocate != 0)
    {
      *out_allocate = (int)byte_file_memory_vectors.allocate;
    }
  if(out_release != 0)
    {
      *out_release = (int)byte_file_memory_vectors.release;
    }
}
