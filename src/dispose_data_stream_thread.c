#include "debug.h"
#include "operror.h"
#include "platform.h"

#include "memory_manager_runtime.h"

static const char dispose_thread_error_message[] = "DisposeThread error:";

enum DataStreamThreadDisposalConstant
{
  DATA_STREAM_THREAD_GRACE_YIELDS = 64
};

int
dispose_data_stream_thread_stack(void *stack_block)
{
  return MEM_DisposePointer(stack_block);
}


int
dispose_data_stream_thread(Item thread_item)
{
  int remaining_yields;
  int result;

  remaining_yields = DATA_STREAM_THREAD_GRACE_YIELDS;
  while(CheckItem(thread_item, KERNELNODE, TASKNODE) != 0 &&
        remaining_yields > 0)
    {
      Yield();
      remaining_yields--;
    }
  if(CheckItem(thread_item, KERNELNODE, TASKNODE) == 0)
    {
      return 0;
    }

  result = DeleteItem(thread_item);
  if(result < 0)
    {
      if(CheckItem(thread_item, KERNELNODE, TASKNODE) == 0)
        {
          return 0;
        }
      kprintf(dispose_thread_error_message);
      PrintfSysErr(result);
    }
  return result;
}
