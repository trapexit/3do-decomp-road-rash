#include "platform.h"

#include "resource_manager_runtime.h"
#include "rw_semantic_data.h"
static
Item
get_share_file_task_item(void)
{
  return CURRENTTASK->t.n_Item;
}


int
ShareFile(ResourceByteFilePool *pool)
{
  MemoryHandle *entries_handle;
  ResourceByteFile *entries;
  ResourceByteFile *new_entry;
  int count;

  count = pool->count;
  entries_handle = pool->entries_handle;
  if(MEM_ResizeHandle(
       entries_handle, (count + 1) * sizeof(ResourceByteFile),
       MEM_POINTER_OPTIONS_DEFAULT) != 0)
    {
      resource_system_last_error = memory_system_last_error;
      return resource_system_last_error;
    }

  resource_system_last_error = 0;
  entries_handle->flags |= MEM_HANDLE_FLAG_BUSY;

  entries = (ResourceByteFile *)entries_handle->data;
  new_entry = &entries[count];
  new_entry->owner_task = get_share_file_task_item();

  if(SF_Share(&entries[0].reader, &new_entry->reader, 0) == 0)
    {
      resource_system_last_error = block_file_last_error;
      entries_handle->flags &= ~MEM_HANDLE_FLAG_BUSY;
      return resource_system_last_error;
    }

  entries_handle->flags &= ~MEM_HANDLE_FLAG_BUSY;
  pool->count = count + 1;
  return resource_system_last_error;
}
