#include "item.h"
#include "makename.h"
/* create_data_stream_thread.c - reconstructed from create_data_stream_thread.s.bak */

#include "data_stream_runtime.h"
#include "data_stream_server_runtime.h"
#include "memory_manager_runtime.h"
int
dispose_data_stream_thread_stack(void *stack_block);

#define STREAM_THREAD_TAG_WORD_COUNT       17
#define STREAM_THREAD_STACK_ALIGNMENT_MASK 3
#define STREAM_THREAD_NAME_BUFFER_SIZE     0x20
#define STREAM_THREAD_STACK_LIMIT          0x4000

#define STREAM_THREAD_PRIORITY_TAG_WORD    0
#define STREAM_THREAD_PRIORITY_VALUE_WORD  1
#define STREAM_THREAD_ENTRY_TAG_WORD       2
#define STREAM_THREAD_ENTRY_VALUE_WORD     3
#define STREAM_THREAD_SIZE_TAG_WORD        4
#define STREAM_THREAD_SIZE_VALUE_WORD      5
#define STREAM_THREAD_STACK_TAG_WORD       6
#define STREAM_THREAD_STACK_VALUE_WORD     7
#define STREAM_THREAD_ARG0_TAG_WORD        8
#define STREAM_THREAD_ARG0_VALUE_WORD      9
#define STREAM_THREAD_ARG1_TAG_WORD        10
#define STREAM_THREAD_ARG1_VALUE_WORD      11
#define STREAM_THREAD_LIMIT_TAG_WORD       12
#define STREAM_THREAD_LIMIT_VALUE_WORD     13
#define STREAM_THREAD_NAME_TAG_WORD        14
#define STREAM_THREAD_NAME_VALUE_WORD      15
#define STREAM_THREAD_END_TAG_WORD         16

#define STREAM_THREAD_PRIORITY_TAG 2
#define STREAM_THREAD_ENTRY_TAG    0x0A
#define STREAM_THREAD_SIZE_TAG     0x0C
#define STREAM_THREAD_STACK_TAG    0x0F
#define STREAM_THREAD_ARG0_TAG     0x0D
#define STREAM_THREAD_ARG1_TAG     0x0E
#define STREAM_THREAD_LIMIT_TAG    0x0B
#define STREAM_THREAD_NAME_TAG     1

int
create_data_stream_thread(void  *thread_entry,
                          int    thread_stack_size,
                          int    priority,
                          char  *thread_name,
                          void **thread_stack,
                          int    initial_r0,
                          void  *initial_r1)
{
  char generated_name[STREAM_THREAD_NAME_BUFFER_SIZE];
  int tags[STREAM_THREAD_TAG_WORD_COUNT];
  char *stack;
  int aligned_size;
  int result;

  aligned_size =
    (thread_stack_size + STREAM_THREAD_STACK_ALIGNMENT_MASK) &
    ~STREAM_THREAD_STACK_ALIGNMENT_MASK;
  *thread_stack = 0;

  stack = MEM_NewPointer(aligned_size, MEMTYPE_ANY,
                         MEM_POINTER_OPTIONS_DEFAULT);
  if(stack == 0)
    {
      return ROADRASH_DS_NO_MEMORY_ERROR;
    }

  *thread_stack = stack;

  tags[STREAM_THREAD_PRIORITY_TAG_WORD] = STREAM_THREAD_PRIORITY_TAG;
  tags[STREAM_THREAD_PRIORITY_VALUE_WORD] = priority;
  tags[STREAM_THREAD_ENTRY_TAG_WORD] = STREAM_THREAD_ENTRY_TAG;
  tags[STREAM_THREAD_ENTRY_VALUE_WORD] = (int)thread_entry;
  tags[STREAM_THREAD_SIZE_TAG_WORD] = STREAM_THREAD_SIZE_TAG;
  tags[STREAM_THREAD_SIZE_VALUE_WORD] = aligned_size;
  tags[STREAM_THREAD_STACK_TAG_WORD] = STREAM_THREAD_STACK_TAG;
  tags[STREAM_THREAD_STACK_VALUE_WORD] = (int)(stack + aligned_size);
  tags[STREAM_THREAD_ARG0_TAG_WORD] = STREAM_THREAD_ARG0_TAG;
  tags[STREAM_THREAD_ARG0_VALUE_WORD] = initial_r0;
  tags[STREAM_THREAD_ARG1_TAG_WORD] = STREAM_THREAD_ARG1_TAG;
  tags[STREAM_THREAD_ARG1_VALUE_WORD] = (int)initial_r1;
  tags[STREAM_THREAD_LIMIT_TAG_WORD] = STREAM_THREAD_LIMIT_TAG;
  tags[STREAM_THREAD_LIMIT_VALUE_WORD] = STREAM_THREAD_STACK_LIMIT;
  tags[STREAM_THREAD_NAME_TAG_WORD] = STREAM_THREAD_NAME_TAG;
  if(thread_name == 0)
    {
      tags[STREAM_THREAD_NAME_VALUE_WORD] =
        (int)MakeName(generated_name, STREAM_THREAD_NAME_BUFFER_SIZE,
                      "Thread", (int)stack);
    }
  else
    {
      tags[STREAM_THREAD_NAME_VALUE_WORD] = (int)thread_name;
    }
  tags[STREAM_THREAD_END_TAG_WORD] = 0;

  result = CreateSizedItem(MKNODEID(KERNELNODE, TASKNODE),
                           (TagArg *)tags, 0);
  if(result < 0)
    {
      dispose_data_stream_thread_stack(stack);
      *thread_stack = 0;
    }
  return result;
}


int
wait_for_data_stream_thread_startup(Item thread_item, uint32 startup_signal)
{
  int32 received;

  for(;;)
    {
      if(CheckItem(thread_item, KERNELNODE, TASKNODE) == 0)
        {
          return ROADRASH_DS_INITIALIZATION_ERROR;
        }
      received = WaitSignal(startup_signal | SIGF_DEADTASK);
      if(received < 0)
        {
          return received;
        }
      if((received & startup_signal) != 0)
        {
          return 0;
        }
      if((received & SIGF_ABORT) != 0)
        {
          return ROADRASH_DS_SIGNAL_ERROR;
        }
    }
}
