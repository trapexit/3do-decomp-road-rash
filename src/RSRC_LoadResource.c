#include "item.h"
#include "resource_manager_runtime.h"
#include "rw_semantic_data.h"

#define RESOURCE_LOCK resource_manager_globals.lock_item
static void
clear_resource_search_busy(MemoryHandle *handle)
{
  if(handle != 0)
    {
      handle->flags &= ~MEM_HANDLE_FLAG_BUSY;
    }
}


MemoryHandle *
RSRC_LoadResource(uint32 resource_type,
                  int32  resource_id,
                  uint32 options)
{
  MemoryHandle *file_handle;
  MemoryHandle *table_handle;
  ResourceRecord *resource;
  MemoryHandle *resource_handle;
  uint32 memory_type;
  uint32 handle_options;

  LockItem(RESOURCE_LOCK, 1);

  resource = SearchRsrcFiles(
    resource_type, resource_id, &file_handle, &table_handle);
  if(resource == 0)
    {
      resource_system_last_error = RESOURCE_ERROR_NOT_FOUND;
      clear_resource_search_busy(file_handle);
      clear_resource_search_busy(table_handle);
      UnlockItem(RESOURCE_LOCK);
      return 0;
    }

  resource_handle = CheckRsrcInMemory(resource);
  if(resource_handle == 0)
    {
      if((options & RESOURCE_LOAD_OPTION_EXISTING_ONLY) != 0)
        {
          resource_system_last_error = RESOURCE_ERROR_NOT_RESIDENT;
          clear_resource_search_busy(file_handle);
          clear_resource_search_busy(table_handle);
          UnlockItem(RESOURCE_LOCK);
          return 0;
        }

      decode_resource_load_options(
        options, &memory_type, &handle_options);
      resource_handle = MEM_NewHandle(
        resource->byte_count, memory_type, handle_options);
      if(resource_handle == 0)
        {
          resource_system_last_error = memory_system_last_error;
          clear_resource_search_busy(file_handle);
          clear_resource_search_busy(table_handle);
          UnlockItem(RESOURCE_LOCK);
          return 0;
        }

      resource_handle->flags |= MEM_HANDLE_FLAG_BUSY;
      file_handle->flags |= MEM_HANDLE_FLAG_BUSY;
      if(LoadResource(
           (ResourceByteFilePool *)file_handle->data, resource,
           resource_handle->data) != 0)
        {
          clear_resource_search_busy(file_handle);
          clear_resource_search_busy(table_handle);
          MEM_DisposeHandle(resource_handle);
          UnlockItem(RESOURCE_LOCK);
          return 0;
        }
      resource_handle->flags &= ~MEM_HANDLE_FLAG_BUSY;

      if((options & RESOURCE_LOAD_OPTION_TRANSIENT) == 0)
        {
          resource->resource_handle = resource_handle;
          resource->flags |= RESOURCE_FLAG_RESIDENT;
        }
    }

  file_handle->flags &= ~MEM_HANDLE_FLAG_BUSY;
  table_handle->flags &= ~MEM_HANDLE_FLAG_BUSY;
  if((options & RESOURCE_LOAD_OPTION_KEEP_BUSY) != 0)
    {
      resource_handle->flags |= MEM_HANDLE_FLAG_BUSY;
    }
  resource_handle->flags |= MEM_HANDLE_FLAG_RESOURCE;

  UnlockItem(RESOURCE_LOCK);
  return resource_handle;
}
