#include "byte_file_runtime.h"
#include "rw_semantic_data.h"

void
BF_Close(ByteFileReader *reader)
{
  RwMemoryAllocator allocate_reader;
  RwMemoryReleaser release_reader;

  if(reader != 0)
    {
      SF_Close(reader);
      BF_GetHooks((int *)&allocate_reader, (int *)&release_reader);
      release_reader((int)reader, BYTE_FILE_READER_SIZE);
    }
}
