#include "mempool.h"

#include "data_stream_server_runtime.h"
#include "memory_manager_runtime.h"

void
tear_down_data_stream_context(DSStreamCBPtr stream)
{
  if(stream->subsMsgPool != 0)
    {
      destroy_memory_pool(stream->subsMsgPool);
      stream->subsMsgPool = 0;
    }
  if(stream->dataMsgPool != 0)
    {
      destroy_memory_pool(stream->dataMsgPool);
      stream->dataMsgPool = 0;
    }
}
