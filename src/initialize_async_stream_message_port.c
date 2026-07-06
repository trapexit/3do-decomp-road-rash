#include "operror.h"

#include "stdio.h"

#include "platform.h"

#include "async_stream_runtime.h"
#include "rw_semantic_data.h"

int
initialize_async_stream_message_port(const RwTagArgument *tag_overrides)
{
  Item port;

  if(async_stream_message_port != 0)
    {
      return -1;
    }

  apply_stream_tag_overrides(stream_open_tags,
                             tag_overrides);

  /* A zero signal lets the port own and release its task-local signal. */
  port = CreateMsgPort("AS_MsgPort", 0, 0);
  if(port < 0)
    {
      return port;
    }
  async_stream_message_port = port;

  return 0;
}
