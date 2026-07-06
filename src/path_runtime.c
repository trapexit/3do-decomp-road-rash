#include "string.h"

#include "path_runtime.h"

int
copy_path_with_capacity(char       *destination,
                        unsigned int capacity,
                        const char *source)
{
  unsigned int source_bytes;

  if(destination == 0 || source == 0 || capacity == 0)
    {
      return PATH_RUNTIME_ERROR_TOO_LONG;
    }
  source_bytes = strlen(source);
  if(source_bytes >= capacity)
    {
      destination[0] = 0;
      return PATH_RUNTIME_ERROR_TOO_LONG;
    }

  memcpy(destination, source, source_bytes + 1);
  return 0;
}


int
append_path_with_capacity(char       *destination,
                          unsigned int capacity,
                          const char *suffix)
{
  unsigned int destination_bytes;
  unsigned int suffix_bytes;

  if(destination == 0 || suffix == 0 || capacity == 0)
    {
      return PATH_RUNTIME_ERROR_TOO_LONG;
    }
  destination_bytes = strlen(destination);
  suffix_bytes = strlen(suffix);
  if(destination_bytes >= capacity ||
     suffix_bytes >= capacity - destination_bytes)
    {
      return PATH_RUNTIME_ERROR_TOO_LONG;
    }

  memcpy(destination + destination_bytes, suffix, suffix_bytes + 1);
  return 0;
}
