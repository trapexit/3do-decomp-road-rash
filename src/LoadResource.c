#include "resource_manager_runtime.h"
#include "rw_semantic_data.h"





int
LoadResource(ResourceByteFilePool *pool,
             ResourceRecord       *resource,
             void                 *destination)
{
  ResourceByteFile *task_file;
  int bytes_read;

  task_file = GetRsrcByteFile(pool);
  if(task_file == 0)
    {
      return resource_system_last_error;
    }

  bytes_read = BF_ReadBytes(&task_file->reader, destination,
                            resource->byte_count,
                            resource->file_position);
  pool->entries_handle->flags &= ~MEM_HANDLE_FLAG_BUSY;
  if(bytes_read < resource->byte_count)
    {
      resource_system_last_error = RESOURCE_ERROR_SHORT_READ;
      return RESOURCE_ERROR_SHORT_READ;
    }

  return 0;
}
