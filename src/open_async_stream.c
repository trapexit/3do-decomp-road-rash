#include "stdio.h"
#include "msgport.h"

#include "string.h"
/* Reconstructed from the original function at 0x00030DE0. */

#include "platform.h"

#include "async_stream_runtime.h"
#include "rw_semantic_data.h"
#include "memory_manager_runtime.h"

int
create_data_stream_thread(void  *thread_entry,
                          int    thread_stack_size,
                          int    priority,
                          char  *thread_name,
                          void **thread_stack,
                          int    initial_r0,
                          void  *initial_r1);
int
dispose_data_stream_thread_stack(void *stack_block);
int
dispose_data_stream_thread(Item thread_item);

static const char async_stream_thread_name_prefix[] = "AS_Subtask(";
static const char async_stream_thread_name_suffix[] = ")";
static const char async_stream_fallback_thread_name[] =
  "AS_Subtask(?" "?)";
static Item retained_async_stream_open_task;
static void *retained_async_stream_open_stack;
static Item retained_async_stream_open_message;
static AsyncStreamOpenRequest *retained_async_stream_open_request;
static AsyncStreamPlaybackStatePrefix *retained_async_stream_open_state;
/* 1: STARTUP is queued; 2: its reply was received but not inspected. */
static int retained_async_stream_startup_pending;

typedef char AsyncStreamFallbackThreadNameSizeCheck[
  (sizeof(async_stream_fallback_thread_name) ==
   ASYNC_STREAM_FALLBACK_THREAD_NAME_BYTES) ? 1 : -1];

static
void
format_async_stream_thread_name(char       *name,
                                unsigned int name_capacity,
                                const char *path)
{
  unsigned int prefix_bytes;
  unsigned int suffix_bytes;
  unsigned int path_bytes;

  prefix_bytes = sizeof(async_stream_thread_name_prefix) - 1;
  suffix_bytes = sizeof(async_stream_thread_name_suffix) - 1;
  if(path != 0)
    {
      path_bytes = strlen(path);
      if(prefix_bytes + suffix_bytes < name_capacity &&
         path_bytes <
         name_capacity - prefix_bytes - suffix_bytes)
        {
          memcpy(name, async_stream_thread_name_prefix,
                 prefix_bytes);
          memcpy(name + prefix_bytes, path, path_bytes);
          memcpy(name + prefix_bytes + path_bytes,
                 async_stream_thread_name_suffix,
                 suffix_bytes + 1);
          return;
        }
    }

  memcpy(name, async_stream_fallback_thread_name,
         sizeof(async_stream_fallback_thread_name));
}

static Item
wait_for_async_stream_startup(Item message_item, Item worker)
{
  MsgPort *port;
  Message *message;
  Item received;
  int signals;

  port = (MsgPort *)LookupItem(async_stream_message_port);
  if(port == 0)
    {
      return -1;
    }
  for(;;)
    {
      received = 0;
      if(message_item == 0)
        {
          received = GetMsg(async_stream_message_port);
        }
      else
        {
          message = (Message *)CheckItem(
            message_item, KERNELNODE, MESSAGENODE);
          if(message == 0)
            {
              return -1;
            }
          if((message->msg.n_Flags & MESSAGE_REPLIED) != 0 &&
             message->msg_MsgPort == async_stream_message_port)
            {
              received = GetThisMsg(message_item);
            }
        }
      if(received != 0)
        {
          return received;
        }
      if(CheckItem(worker, KERNELNODE, TASKNODE) == 0)
        {
          return -1;
        }
      signals = WaitSignal(port->mp_Signal | SIGF_DEADTASK);
      if(signals < 0 || (signals & SIGF_ABORT) != 0)
        {
          return -1;
        }
    }
}
int
release_retained_async_stream_open_resources(void)
{
  int result;
  int stop_result;
  int stop_accepted;
  Item received;
  Message *message;

  stop_result = 0;
  if(retained_async_stream_startup_pending != 0)
    {
      received = retained_async_stream_startup_pending == 2
        ? retained_async_stream_open_message
        : wait_for_async_stream_startup(retained_async_stream_open_message,
                                        retained_async_stream_open_task);
      if(received < 0)
        {
          if(CheckItem(retained_async_stream_open_task,
                       KERNELNODE, TASKNODE) != 0)
            {
              return received;
            }
        }
      else
        {
          retained_async_stream_startup_pending = 2;
          message = (Message *)LookupItem(received);
          if(message == 0)
            {
              return -1;
            }
          if((int)message->msg_Result == ASYNC_STREAM_COMMAND_STARTUP)
            {
              retained_async_stream_open_state =
                (AsyncStreamPlaybackStatePrefix *)message->msg_DataPtr;
            }
        }
      retained_async_stream_startup_pending = 0;
    }

  if(retained_async_stream_open_state != 0)
    {
      stop_result = stop_async_stream(retained_async_stream_open_state,
                                      &stop_accepted);
      if(stop_accepted == 0)
        {
          return stop_result;
        }
      /* Accepted STOP drains the worker and frees its command and stack. */
      retained_async_stream_open_state = 0;
      retained_async_stream_open_task = 0;
      retained_async_stream_open_stack = 0;
      retained_async_stream_open_message = 0;
    }

  if(retained_async_stream_open_task > 0)
    {
      result =
        dispose_data_stream_thread(retained_async_stream_open_task);
      if(result < 0 &&
         CheckItem(retained_async_stream_open_task,
                   KERNELNODE, TASKNODE) != 0)
        {
          return result;
        }
      retained_async_stream_open_task = 0;
    }

  if(retained_async_stream_open_stack != 0)
    {
      result = dispose_data_stream_thread_stack(
        retained_async_stream_open_stack);
      if(result < 0)
        {
          return result;
        }
      retained_async_stream_open_stack = 0;
    }

  if(retained_async_stream_open_message > 0)
    {
      result = DeleteItem(retained_async_stream_open_message);
      if(result < 0)
        {
          return result;
        }
      retained_async_stream_open_message = 0;
    }

  if(retained_async_stream_open_request != 0)
    {
      result = MEM_DisposePointer(retained_async_stream_open_request);
      if(result < 0)
        {
          return result;
        }
      retained_async_stream_open_request = 0;
    }

  return stop_result;
}




int
open_async_stream(AsyncStreamPlaybackStatePrefix **state,
                  const char                      *path,
                  int                              stream_unit_count,
                  const RwTagArgument             *tags)
{
  char name[ASYNC_STREAM_OPEN_THREAD_NAME_BYTES];
  void *thread_stack_block;
  int subtask;
  int msg;
  int received;
  int result;
  int dispose_result;
  const RwTagArgument *entry;
  int port;
  int priority;
  AsyncStreamOpenRequest *request;
  RwTagArgument *owned_tags;
  char *owned_path;
  unsigned int path_bytes;
  unsigned int tag_count;
  unsigned int tag_bytes;
  int startup_pending;
  Message *message;

  msg = -1;
  subtask = -1;
  thread_stack_block = 0;
  request = 0;
  startup_pending = 0;
  if(state == 0 || path == 0)
    {
      return -1;
    }
  if(*state != 0)
    {
      printf("AS_ORPHAN_REFUSED path=%s\n", path);
      return -1;
    }
  *state = 0;
  result = release_retained_async_stream_open_resources();
  if(result < 0)
    {
      return result;
    }
  if(async_stream_message_port == 0)
    {
      result = initialize_async_stream_message_port(0);
      if(result < 0)
        {
          return result;
        }
    }

  path_bytes = strlen(path);
  tag_count = 1;
  if(tags != 0)
    {
      while(tags[tag_count - 1].tag != 0)
        {
          if(tag_count >=
             (0x7FFFFFFFU - sizeof(*request)) / sizeof(*tags))
            {
              return -1;
            }
          tag_count++;
        }
    }
  tag_bytes = tag_count * sizeof(*tags);
  if(path_bytes >= 0x7FFFFFFFU - sizeof(*request) - tag_bytes)
    {
      return -1;
    }
  path_bytes++;
  request = (AsyncStreamOpenRequest *)MEM_NewPointer(
    sizeof(*request) + tag_bytes + path_bytes,
    MEMTYPE_ANY, MEM_POINTER_OPTIONS_DEFAULT);
  if(request == 0)
    {
      return -1;
    }
  owned_tags = (RwTagArgument *)(request + 1);
  if(tags != 0)
    {
      memcpy(owned_tags, tags, tag_bytes);
    }
  else
    {
      owned_tags[0].tag = 0;
      owned_tags[0].argument = 0;
    }
  owned_path = (char *)(owned_tags + tag_count);
  memcpy(owned_path, path, path_bytes);
  request->path = owned_path;
  request->stream_unit_count = stream_unit_count;
  request->tags = owned_tags;

  entry = find_stream_tag_argument(ASYNC_STREAM_TAG_SUBTASK_PRIORITY_OFFSET,
                                   tags,
                                   stream_open_tags);

  format_async_stream_thread_name(
    name, sizeof(name), path);

  priority = CURRENTTASK->t.n_Priority + (int)entry->argument;

  subtask = create_data_stream_thread(
    (void *)async_stream_playback_thread_main,
    ASYNC_STREAM_THREAD_STACK_BYTES,
    priority,
    name,
    &thread_stack_block,
    0,
    (void *)async_stream_message_port);
  if(subtask < 0)
    {
      result = subtask;
      goto cleanup;
    }

  msg = CreateSmallMsg("AS_StreamMsg", ASYNC_STREAM_MESSAGE_PRIORITY,
                       async_stream_message_port);
  if(msg < 0)
    {
      result = msg;
      goto cleanup;
    }

  request->thread_stack_block = thread_stack_block;

  received = wait_for_async_stream_startup(0, subtask);
  if(received < 0)
    {
      result = received;
      goto cleanup;
    }

  message = (Message *)LookupItem(received);
  if(message == 0)
    {
      result = -1;
      goto cleanup;
    }

  port = message->msg_ReplyPort;
  result = ReplyMsg(received, 0, (void *)msg, 0);
  if(result < 0)
    {
      goto cleanup;
    }

  result = SendSmallMsg(port, msg, ASYNC_STREAM_COMMAND_STARTUP,
                        (uint32)request);
  if(result < 0)
    {
      goto cleanup;
    }
  startup_pending = 1;

  received = wait_for_async_stream_startup(msg, subtask);
  if(received < 0)
    {
      result = received;
      goto cleanup;
    }

  startup_pending = 2;
  message = (Message *)LookupItem(received);
  if(message == 0)
    {
      result = -1;
      goto cleanup;
    }

  startup_pending = 0;
  if((int)message->msg_Result != ASYNC_STREAM_COMMAND_STARTUP)
    {
      result = (int)message->msg_Result;
      if(result >= 0)
        {
          result = -1;
        }
      goto cleanup;
    }
  *state = (AsyncStreamPlaybackStatePrefix *)message->msg_DataPtr;
  if(*state != 0)
    {
      dispose_result = MEM_DisposePointer(request);
      if(dispose_result < 0)
        {
          retained_async_stream_open_request = request;
        }
      return 0;
    }

  result = -1;

cleanup:
  retained_async_stream_open_task = subtask;
  retained_async_stream_open_stack = thread_stack_block;
  retained_async_stream_open_message = msg;
  retained_async_stream_open_request = request;
  retained_async_stream_startup_pending = startup_pending;
  release_retained_async_stream_open_resources();
  return result;
}
