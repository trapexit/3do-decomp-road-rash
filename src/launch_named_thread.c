#include "stdio.h"

#include "operror.h"
#include "task.h"

#include "thread_launch.h"

enum ThreadLaunchConstant
{
  THREAD_ERROR_MESSAGE_CAPACITY = 256,
  THREAD_ERROR_MESSAGE_MAXIMUM = THREAD_ERROR_MESSAGE_CAPACITY - 1
};

int
launch_named_thread(ThreadEntryPoint entry_point,
                    int              stack_size,
                    int              priority,
                    char            *name)
{
  char error_message[THREAD_ERROR_MESSAGE_CAPACITY];
  int result;

  result = CreateThread(name, (uint8)priority, entry_point, stack_size);
  if(result < 0)
    {
      error_message[0] = 0;
      GetSysErr(error_message, THREAD_ERROR_MESSAGE_MAXIMUM, result);
      error_message[THREAD_ERROR_MESSAGE_MAXIMUM] = 0;
      printf("Launch of '%s' failed\n", name);
      printf("Err #%d :%s\n", result, error_message);
    }

  return result;
}
