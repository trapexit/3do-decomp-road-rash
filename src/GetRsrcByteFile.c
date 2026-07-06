#include "platform.h"

#include "resource_manager_runtime.h"
#include "rw_semantic_data.h"

#define RESOURCE_ERROR_TASK_FILE_UNAVAILABLE 0x1109





static
Item
get_current_task_item(void)
{
  return CURRENTTASK->t.n_Item;
}


ResourceByteFile *
GetRsrcByteFile(ResourceByteFilePool *pool)
{
  MemoryHandle *entries_handle;
  ResourceByteFile *entries;
  int i;
  int count;
  Item task_item;

  entries_handle = pool->entries_handle;
  entries_handle->flags |= MEM_HANDLE_FLAG_BUSY;

  entries = (ResourceByteFile *)entries_handle->data;
  count = pool->count;
  task_item = get_current_task_item();
  for(i = 0; i < count; i++)
    {
      if(entries[i].owner_task == task_item)
        {
          return &entries[i];
        }
    }

  entries_handle->flags &= ~MEM_HANDLE_FLAG_BUSY;
  if(ShareFile(pool) != 0)
    {
      return 0;
    }

  entries_handle = pool->entries_handle;
  entries_handle->flags |= MEM_HANDLE_FLAG_BUSY;

  entries = (ResourceByteFile *)entries_handle->data;
  count = pool->count;
  task_item = get_current_task_item();
  for(i = 0; i < count; i++)
    {
      if(entries[i].owner_task == task_item)
        {
          return &entries[i];
        }
    }

  entries_handle->flags &= ~MEM_HANDLE_FLAG_BUSY;
  resource_system_last_error = RESOURCE_ERROR_TASK_FILE_UNAVAILABLE;
  return 0;
}


#undef RESOURCE_ERROR_TASK_FILE_UNAVAILABLE
