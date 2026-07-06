#include "platform.h"

#include "rw_semantic_data.h"

enum
{
  AUDIO_THREAD_NAME_TENS_INDEX = 6,
  AUDIO_THREAD_NAME_ONES_INDEX = 7
};

char *
next_audio_thread_name(void)
{
  char *thread_name;
  unsigned int digit;

  thread_name = audio_manager_thread_defaults.next_thread_name;
  digit = (unsigned char)(
    thread_name[AUDIO_THREAD_NAME_ONES_INDEX] + 1);
  thread_name[AUDIO_THREAD_NAME_ONES_INDEX] = (char)digit;
  if(digit > '9')
    {
      thread_name = audio_manager_thread_defaults.next_thread_name;
      thread_name[AUDIO_THREAD_NAME_ONES_INDEX] = '0';
      thread_name = audio_manager_thread_defaults.next_thread_name;
      thread_name[AUDIO_THREAD_NAME_TENS_INDEX]++;
    }

  return audio_manager_thread_defaults.next_thread_name;
}


int
block_file_allocate_memory(int byte_count,
                           int memory_type)
{
  return (int)AllocMem(byte_count, memory_type);
}


void
block_file_free_memory(int memory,
                       int byte_count)
{
  FreeMem((void *)memory, byte_count);
}
