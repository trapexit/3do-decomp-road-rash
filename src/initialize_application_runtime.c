

#include "platform.h"

#include "display_system_runtime.h"
#include "memory_manager_runtime.h"
#include "rw_semantic_data.h"

enum ApplicationMemoryConfiguration
{
  APPLICATION_MEMORY_CONFIGURATION_WORD = 0x20,
  /* Keep 16K of VRAM pages outside the MEM heaps so post-race movie
     decode (Cinepak CreateCodec) and thread stacks still fit after the
     race-audio voice resources take their share. */
  APPLICATION_VRAM_RESERVE_BYTES = 0x583A8,
  APPLICATION_MEMORY_CONFIGURATION_LIMIT = 0x0E,
  APPLICATION_MEMORY_TAG_CAPACITY = 9,
  APPLICATION_MEMORY_CALLBACK_SLOT = 8,
  APPLICATION_RESTRICTED_TAG_COUNT = 2
};

void
application_memory_callback(void);

int
initialize_application_runtime(void)
{
  MemoryInitTag tags[APPLICATION_MEMORY_TAG_CAPACITY];
  int index;
  int result;

  tags[0].tag = MEM_INIT_TAG_CONFIGURATION_WORD;
  tags[0].value = APPLICATION_MEMORY_CONFIGURATION_WORD;
  index = 1;

  if(runtime_feature_flags.streaming_audio_enabled.value == 0)
    {
      tags[1].tag = MEM_INIT_TAG_RESTRICT_ADDRESS;
      tags[1].value = 0;
      index = APPLICATION_RESTRICTED_TAG_COUNT;
    }

  tags[index].tag = MEM_INIT_TAG_DRAM_RESERVE;
  tags[index].value = (uint32)frame_time_scale;
  index++;

  tags[index].tag = MEM_INIT_TAG_VRAM_RESERVE;
  tags[index].value = APPLICATION_VRAM_RESERVE_BYTES;
  index++;

  tags[index].tag = MEM_INIT_TAG_CONFIGURATION_LIMIT;
  tags[index].value = APPLICATION_MEMORY_CONFIGURATION_LIMIT;
  index++;

  tags[index].tag = MEM_INIT_TAG_END;
  tags[index].value = 0;
  tags[APPLICATION_MEMORY_CALLBACK_SLOT].tag =
    (uint32)application_memory_callback;
  tags[APPLICATION_MEMORY_CALLBACK_SLOT].value = 0;

  result = initialize_platform_runtime(tags);
  return result;
}


void
application_memory_callback(void)
{
}
