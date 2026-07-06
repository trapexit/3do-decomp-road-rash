#include "rw_semantic_data.h"
#include "filefunctions.h"

/* Original address: 0x0003C9E8. */

int
change_music_library_directory(char *path)
{
  int result;

  if(music_library_globals.suppress_directory_changes != 0)
    {
      return 0;
    }

  result = ChangeDirectory(path);
  if(result < 0)
    {
      return result;
    }

  return 0;
}
