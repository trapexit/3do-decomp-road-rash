#include "byte_file_runtime.h"
#include "rw_semantic_data.h"

ByteFileReader *
BF_Share(ByteFileReader *source_reader)
{
  RwMemoryAllocator allocate_reader;
  RwMemoryReleaser release_reader;
  ByteFileReader *reader;

  BF_GetHooks((int *)&allocate_reader, (int *)&release_reader);
  reader = (ByteFileReader *)allocate_reader(BYTE_FILE_READER_SIZE, 0);
  if(reader == 0)
    {
      return 0;
    }

  reader->position = 0;
  if(SF_Share(source_reader, reader, 0) == 0)
    {
      release_reader((int)reader, BYTE_FILE_READER_SIZE);
      return 0;
    }
  return reader;
}
