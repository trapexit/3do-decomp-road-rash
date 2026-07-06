#include "kernel.h"
#include "string.h"

#include "data_stream_server_runtime.h"
#include "memory_manager_runtime.h"
#include "saudio_subscriber_runtime.h"

int
create_saudio_subscriber_context(SAudioContext **created_context,
                                 DSStreamCBPtr   stream_control,
                                 int             priority_delta)
{
  MemPoolPtr context_pool;
  SAudioContext *context;
  int status;
  int priority;

  context_pool = (MemPoolPtr)saudio_subscriber_context_pool;
  context = (SAudioContext *)AllocPoolMem(context_pool);
  if(context == 0)
    {
      return STREAM_ERROR_NO_MEMORY;
    }

  memset(context, 0, sizeof(*context));
  context->creator_task = CURRENTTASK->t.n_Item;
  context->stream_control = stream_control;

  context->creator_signal = AllocSignal(0);
  if(context->creator_signal <= 0)
    {
      status = STREAM_ERROR_NO_SIGNAL;
      context->creator_signal = 0;
    }
  else
    {
      priority = CURRENTTASK->t.n_Priority + priority_delta;
      status = create_data_stream_thread(
        (void *)saudio_subscriber_thread_main,
        SAUDIO_SUBSCRIBER_THREAD_STACK_BYTES, priority, 0,
        &context->thread_stack_block, 0, context);
      if(status > 0)
        {
          context->thread_item = status;
          status = wait_for_data_stream_thread_startup(
            context->thread_item, context->creator_signal);
          if(status < 0)
            {
              goto fail;
            }

          FreeSignal(context->creator_signal);
          context->creator_signal = 0;
          status = context->creator_status;
          if(status >= 0)
            {
              *created_context = context;
              return status;
            }
        }
    }

fail:
  if(context->creator_signal != 0)
    {
      FreeSignal(context->creator_signal);
      context->creator_signal = 0;
    }
  if(dispose_saudio_subscriber_context(context) < 0)
    {
      *created_context = context;
    }
  return status;
}
