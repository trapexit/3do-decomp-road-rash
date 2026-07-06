#include "audio_system.h"

int
shutdown_music_library(void);

int
shutdown_audio_system(void)
{
  return shutdown_music_library();
}
