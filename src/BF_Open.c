#include "byte_file_runtime.h"
#include "rw_semantic_data.h"

ByteFileReader *
BF_Open(const char *path,
        int         sharing_flags)
{
  RwMemoryAllocator allocate_reader;
  RwMemoryReleaser release_reader;
  ByteFileReader *reader;

  (void)sharing_flags;
  if(path == 0)
    {
      return 0;
    }

  if(block_file_sector_cache.sector_buffer == 0)
    {
      if(BF_InitReader(0, 0, 0) < 0)
        {
          return 0;
        }
    }

  BF_GetHooks((int *)&allocate_reader, (int *)&release_reader);
  reader = (ByteFileReader *)allocate_reader(BYTE_FILE_READER_SIZE, 0);
  if(reader == 0)
    {
      return 0;
    }

  if(SF_Open(path, reader, 0) == 0)
    {
      release_reader((int)reader, BYTE_FILE_READER_SIZE);
      return 0;
    }

  reader->position = 0;
  return reader;
}
