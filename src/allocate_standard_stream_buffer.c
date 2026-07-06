#include "mem.h"
#include "stdio.h"

uint8 *
allocate_standard_stream_buffer(FILE *stream)
{
  uint8 *buffer;

  if(stream == 0)
    {
      return 0;
    }

  buffer = (uint8 *)AllocMem(FCB_BUFSIZE, MEMTYPE_ANY);
  stream->fcb_buffer = buffer;
  if(buffer != 0)
    {
      stream->fcb_cp = buffer;
      stream->fcb_buffsize = FCB_BUFSIZE;
    }
  return buffer;
}
