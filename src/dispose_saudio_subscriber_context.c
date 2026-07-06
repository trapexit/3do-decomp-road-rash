#include "data_stream_server_runtime.h"
#include "memory_manager_runtime.h"
#include "saudio_subscriber_runtime.h"

int
dispose_saudio_subscriber_context(SAudioContext *context)
{
  int channel_number;
  int status;
  if(context != 0)
    {
      if(context->thread_item > 0)
        {
          status = dispose_data_stream_thread(context->thread_item);
          if(status < 0)
            {
              return status;
            }
          context->thread_item = 0;
        }
      if(context->thread_stack_block != 0)
        {
          dispose_data_stream_thread_stack(context->thread_stack_block);
          context->thread_stack_block = 0;
        }
      /* Task deletion releases worker-owned audio Items and port signals.
       * Only shared heap allocations survive the joined worker. */
      for(channel_number = 0;
          channel_number < SAUDIO_SUBSCRIBER_CHANNEL_COUNT;
          channel_number++)
        {
          if(context->channels[channel_number].buffer_pool != 0)
            {
              destroy_memory_pool(
                context->channels[channel_number].buffer_pool);
              context->channels[channel_number].buffer_pool = 0;
            }
        }
      if(context->template_array != 0)
        {
          MEM_DisposePointer(context->template_array);
          context->template_array = 0;
        }

      ReturnPoolMem((MemPoolPtr)saudio_subscriber_context_pool, context);
    }

  return 0;
}
