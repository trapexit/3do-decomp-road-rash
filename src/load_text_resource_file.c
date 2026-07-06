#include "filestreamfunctions.h"
#include "limits.h"

#include "text_render_runtime.h"

void *
load_text_resource_file(const char *path,
                        int         memory_flags,
                        int         trailing_bytes,
                        int        *file_size)
{
  void *resource_image;
  Stream *stream;
  int resource_size;
  int read_size;

  resource_image = 0;
  if(path == 0 || file_size == 0 || trailing_bytes < 0)
    {
      return 0;
    }
  *file_size = 0;
  stream = OpenDiskStream((char *)path, 0);
  if(stream != 0)
    {
      resource_size = stream->st_FileLength;
      if(resource_size <= 0 || trailing_bytes > INT_MAX - resource_size)
        {
          CloseDiskStream(stream);
          return 0;
        }
      *file_size = resource_size;
      resource_image = allocate_text_memory(resource_size + trailing_bytes,
                                            memory_flags);
      if(resource_image != 0)
        {
          if(SeekDiskStream(stream, 0, SEEK_SET) < 0)
            {
              dispose_text_memory(resource_image, 0, 0);
              resource_image = 0;
            }
          else
            {
              ReadDiskStream(stream, (char *)resource_image, 0);
              read_size = ReadDiskStream(
                stream, (char *)resource_image, resource_size);
              if(read_size != resource_size)
                {
                  dispose_text_memory(resource_image, 0, 0);
                  resource_image = 0;
                }
            }
        }
      CloseDiskStream(stream);
    }

  return resource_image;
}
