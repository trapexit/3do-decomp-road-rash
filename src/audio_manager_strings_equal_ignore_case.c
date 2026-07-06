#include "strings.h"

#include "audio_manager_voice_internal.h"

int
audio_manager_strings_equal_ignore_case(const char *left,
                                        const char *right)
{
  int result;

  result = 0;
  if(left == 0)
    {
      if(right == 0)
        {
          result = 1;
        }
    }
  else if(right != 0)
    {
      if(strcasecmp(left, right) == 0)
        {
          result = 1;
        }
    }

  return result;
}
