#include "item.h"
#include "string.h"

#include "resource_manager_runtime.h"
#include "rw_semantic_data.h"





static
void
clear_resource_handle_busy(MemoryHandle *handle)
{
  if(handle != 0)
    {
      handle->flags &= ~MEM_HANDLE_FLAG_BUSY;
    }
}


void *
RSRC_StuffResource(uint32 resource_type,
                   int32  resource_id,
                   void  *destination)
{
  MemoryHandle *file_handle;
  MemoryHandle *table_handle;
  MemoryHandle *resident_handle;
  ResourceRecord *resource;
  int owns_destination;
  uint32 resident_busy;

  LockItem(resource_manager_globals.lock_item, 1);

  file_handle = 0;
  table_handle = 0;
  resource = SearchRsrcFiles(
    resource_type, resource_id, &file_handle, &table_handle);
  if(resource == 0)
    {
      resource_system_last_error = RESOURCE_ERROR_NOT_FOUND;
      clear_resource_handle_busy(table_handle);
      clear_resource_handle_busy(file_handle);
      UnlockItem(resource_manager_globals.lock_item);
      return 0;
    }

  owns_destination = destination == 0;
  if(destination == 0)
    {
      destination = MEM_NewPointer(
        resource->byte_count, MEMORY_REQUEST_ANY,
        MEM_POINTER_OPTIONS_DEFAULT);
      if(destination == 0)
        {
          resource_system_last_error = memory_system_last_error;
          clear_resource_handle_busy(table_handle);
          clear_resource_handle_busy(file_handle);
          UnlockItem(resource_manager_globals.lock_item);
          return 0;
        }
    }

  resident_handle = CheckRsrcInMemory(resource);
  if(resident_handle == 0)
    {
      file_handle->flags |= MEM_HANDLE_FLAG_BUSY;
      if(LoadResource(
           (ResourceByteFilePool *)file_handle->data,
           resource, destination) != 0)
        {
          if(owns_destination)
            {
              MEM_DisposePointer(destination);
            }
          clear_resource_handle_busy(table_handle);
          clear_resource_handle_busy(file_handle);
          UnlockItem(resource_manager_globals.lock_item);
          return 0;
        }
    }
  else
    {
      resident_busy = resident_handle->flags & MEM_HANDLE_FLAG_BUSY;
      resident_handle->flags |= MEM_HANDLE_FLAG_BUSY;
      memcpy(destination, resident_handle->data,
             resource->byte_count);
      if(resident_busy == 0)
        {
          resident_handle->flags &= ~MEM_HANDLE_FLAG_BUSY;
        }
    }

  clear_resource_handle_busy(table_handle);
  clear_resource_handle_busy(file_handle);
  UnlockItem(resource_manager_globals.lock_item);
  return destination;
}
