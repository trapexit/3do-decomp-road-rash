#include "string.h"

#include "bss_late_data.h"

char *
copy_audio_working_directory(char *path)
{
  char *result;

  result = path;
  if(path != 0)
    {
      strcpy(path, gAudioWorkingDirectory);
    }
  return result;
}
