#include "saudio_subscriber_runtime.h"

boolean
AddBufferToTail(SAudioChannel *channel,
                SAudioBuffer  *buffer)
{
  boolean was_empty;

  buffer->link = 0;
  if(channel->in_use_queue_head != 0)
    {
      channel->in_use_queue_tail->link = buffer;
      channel->in_use_queue_tail = buffer;
      was_empty = false;
    }
  else
    {
      channel->in_use_queue_head = buffer;
      channel->in_use_queue_tail = buffer;
      was_empty = true;
    }

  channel->in_use_count++;
  return was_empty;
}


SAudioBuffer *
GetNextBuffer(SAudioChannel *channel)
{
  SAudioBuffer *buffer;

  buffer = channel->in_use_queue_head;
  if(buffer != 0)
    {
      channel->in_use_queue_head = buffer->link;
      if(channel->in_use_queue_tail == buffer)
        {
          channel->in_use_queue_tail = 0;
        }
      channel->in_use_count--;
    }
  return buffer;
}
