#include "filestreamfunctions.h"

#define FILE_STREAM_END_MARKER 0xFF
#define FILE_STREAM_LINE_END   0x0D
#define FILE_TEXT_LINE_FEED    0x0A

char *
File_GetLine(char   *buffer,
             int     capacity,
             Stream *stream)
{
  unsigned char byte;
  int index;

  if(buffer == 0 || stream == 0 || capacity <= 0)
    {
      return 0;
    }
  index = 0;
  if(ReadDiskStream(stream, (char *)&byte, 1) != 1 ||
     byte == FILE_STREAM_END_MARKER)
    {
      return 0;
    }

  while(byte != FILE_STREAM_LINE_END)
    {
      if(index >= capacity - 1)
        {
          break;
        }
      if(byte == FILE_STREAM_END_MARKER)
        {
          break;
        }

      buffer[index] = (char)byte;
      index++;
      if(ReadDiskStream(stream, (char *)&byte, 1) != 1)
        {
          break;
        }
    }

  if(byte == FILE_STREAM_LINE_END && index < capacity - 1)
    {
      buffer[index] = FILE_TEXT_LINE_FEED;
      index++;
    }

  buffer[index] = 0;
  return buffer;
}


#undef FILE_STREAM_END_MARKER
#undef FILE_STREAM_LINE_END
#undef FILE_TEXT_LINE_FEED
