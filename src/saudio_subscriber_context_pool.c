#include "memory_manager_runtime.h"
#include "saudio_subscriber_runtime.h"

#define SAUDIO_SUBSCRIBER_CONTEXT_COUNT 4

int
initialize_saudio_subscriber_context_pool(void)
{
  saudio_subscriber_context_pool =
    create_memory_pool(SAUDIO_SUBSCRIBER_CONTEXT_COUNT,
                       sizeof(SAudioContext));
  if(saudio_subscriber_context_pool != 0)
    {
      return 0;
    }
  return SAUDIO_ERROR_ALLOC_CONTEXT_POOL;
}


int
dispose_saudio_subscriber_context_pool(void)
{
  destroy_memory_pool(saudio_subscriber_context_pool);
  saudio_subscriber_context_pool = 0;
  return 0;
}


#undef SAUDIO_SUBSCRIBER_CONTEXT_COUNT
