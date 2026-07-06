#include "bss_late_data.h"
#include "rw_semantic_data.h"
#include "filefunctions.h"
#include "string.h"

/* Original ranges: 0x0003C938-0x0003C9E8. */

#define MUSIC_DIRECTORY_CAPACITY 0x100

int
change_music_library_directory(char *path);

int
enter_audio_working_directory(char *saved_path,
                              int   capacity)
{
  char current_dir[MUSIC_DIRECTORY_CAPACITY];
  char saved_dir[MUSIC_DIRECTORY_CAPACITY];
  int result;
  int changed;

  result = 0;
  if(music_library_globals.suppress_directory_changes == 0)
    {
      result = GetDirectory(saved_dir, MUSIC_DIRECTORY_CAPACITY);
      if(result >= 0)
        {
          changed = change_music_library_directory(gAudioWorkingDirectory);
          if(changed < 0)
            {
              change_music_library_directory(saved_dir);
            }
          else
            {
              if(saved_path != 0)
                {
                  strncpy(saved_path, saved_dir, capacity);
                }
              GetDirectory(current_dir, MUSIC_DIRECTORY_CAPACITY);
            }
        }
    }
  return result;
}


int
restore_music_library_directory(char *path)
{
  if(music_library_globals.suppress_directory_changes == 0)
    {
      return change_music_library_directory(path);
    }
  return (int)path;
}
