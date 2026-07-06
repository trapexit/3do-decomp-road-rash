#include "audio_message_runtime.h"
#include "rw_semantic_data.h"

#define AUDIO_THREAD_TAG_COUNT             9
#define AUDIO_THREAD_STACK_ALIGNMENT_MASK  3
#define AUDIO_THREAD_PRIORITY_TAG_INDEX    0
#define AUDIO_THREAD_PC_TAG_INDEX          2
#define AUDIO_THREAD_STACK_SIZE_TAG_INDEX  3
#define AUDIO_THREAD_STACK_PTR_TAG_INDEX   4
#define AUDIO_THREAD_ARG0_TAG_INDEX        5
#define AUDIO_THREAD_ARG1_TAG_INDEX        6
#define AUDIO_THREAD_NAME_TAG_INDEX        7

Item
AM_NewThread(AudioThreadEntry entry_point,
             int             *stack_size,
             int32            priority,
             const char      *thread_name,
             void           **out_stack_memory,
             void            *initial_argument,
             void            *secondary_argument)
{
  RwMemoryAllocator allocate_stack;
  RwMemoryReleaser release_stack;
  RwTagArgument *template_tags;
  TagArg task_tags[AUDIO_THREAD_TAG_COUNT];
  int stack_memory;
  int aligned_stack_size;
  Item thread_item;
  int index;

  aligned_stack_size = (*stack_size + AUDIO_THREAD_STACK_ALIGNMENT_MASK) &
                       ~AUDIO_THREAD_STACK_ALIGNMENT_MASK;
  template_tags = audio_manager_thread_defaults.tags;
  for(index = 0; index < AUDIO_THREAD_TAG_COUNT; index++)
    {
      task_tags[index].ta_Tag = template_tags[index].tag;
      task_tags[index].ta_Arg = (void *)template_tags[index].argument;
    }

  if(thread_name == 0)
    {
      thread_name = next_audio_thread_name();
    }
  task_tags[AUDIO_THREAD_NAME_TAG_INDEX].ta_Arg = (void *)thread_name;

  *out_stack_memory = 0;
  *stack_size = 0;

  allocate_stack = music_library_globals.allocate;
  stack_memory = allocate_stack(aligned_stack_size, 0);
  if(stack_memory == 0)
    {
      return 0;
    }

  *out_stack_memory = (void *)stack_memory;
  *stack_size = aligned_stack_size;

  task_tags[AUDIO_THREAD_PRIORITY_TAG_INDEX].ta_Arg = (void *)priority;
  task_tags[AUDIO_THREAD_PC_TAG_INDEX].ta_Arg = (void *)entry_point;
  task_tags[AUDIO_THREAD_STACK_SIZE_TAG_INDEX].ta_Arg =
    (void *)aligned_stack_size;
  task_tags[AUDIO_THREAD_STACK_PTR_TAG_INDEX].ta_Arg =
    (void *)(stack_memory + aligned_stack_size);
  task_tags[AUDIO_THREAD_ARG0_TAG_INDEX].ta_Arg = initial_argument;
  task_tags[AUDIO_THREAD_ARG1_TAG_INDEX].ta_Arg = secondary_argument;

  thread_item = CreateSizedItem(MKNODEID(KERNELNODE, TASKNODE),
                                task_tags, 0);
  if(thread_item < 0)
    {
      release_stack = music_library_globals.release;
      release_stack(stack_memory, aligned_stack_size);
      *out_stack_memory = 0;
    }

  return thread_item;
}
