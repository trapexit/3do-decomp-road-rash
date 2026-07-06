#include "string.h"

#include "audio_manager_voice_internal.h"

enum AudioManagerInstrumentPathConstant
{
  AUDIO_MANAGER_INSTRUMENT_PATH_CAPACITY = 256
};

char *
strip_audio_manager_instrument_path(char *instrument_path)
{
  char temporary_path[AUDIO_MANAGER_INSTRUMENT_PATH_CAPACITY];
  char *scan;
  int length;

  length = (int)strlen(instrument_path);
  if(length > 0)
    {
      scan = instrument_path + length;
      while(scan > instrument_path)
        {
          scan--;
          if(*scan == '/')
            {
              scan++;
              break;
            }
        }

      strcpy(temporary_path, scan);
      strcpy(instrument_path, temporary_path);
    }

  return instrument_path;
}
