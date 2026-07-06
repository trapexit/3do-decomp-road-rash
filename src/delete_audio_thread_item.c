#include "audio_message_runtime.h"

int32
delete_audio_thread_item(Item thread_item)
{
  int32 result;

  /* A completed shutdown can remove the task before its creator joins it. */
  if(CheckItem(thread_item, KERNELNODE, TASKNODE) == 0)
    {
      return 0;
    }
  result = DeleteItem(thread_item);
  if(result < 0 && CheckItem(thread_item, KERNELNODE, TASKNODE) == 0)
    {
      return 0;
    }
  return result;
}
