/* OpenAndLinkRsrcFile - reconstructed from the 0x000389FC routine. */

#include "platform.h"

#include "resource_manager_runtime.h"
#include "rw_semantic_data.h"

MemoryHandle *
OpenAndLinkRsrcFile(const char *path)
{
  MemoryHandle *file_handle;
  MemoryHandle *byte_file_handle;
  ResourceFileRecord *file_record;
  ResourceByteFile *byte_file;
  MemoryHandle *old_head;
  ResourceFileRecord *old_record;

  if(resource_manager_globals.lock_item <= 0)
    {
      resource_manager_globals.lock_item = CreateSizedItem(
        MKNODEID(KERNELNODE, SEMAPHORENODE), 0, 0);
      if(resource_manager_globals.lock_item < 0)
        {
          resource_system_last_error = resource_manager_globals.lock_item;
          resource_manager_globals.lock_item = 0;
          return 0;
        }
      if(BF_InitReader(0, 0, 0) < 0)
        {
          resource_system_last_error = block_file_last_error;
          DeleteItem(resource_manager_globals.lock_item);
          resource_manager_globals.lock_item = 0;
          return 0;
        }
      LockItem(resource_manager_globals.lock_item, 1);
    }

  file_handle = MEM_NewHandle(
    sizeof(ResourceFileRecord), MEMORY_REQUEST_ANY,
    MEM_POINTER_OPTIONS_DEFAULT);
  if(file_handle == 0)
    {
      resource_system_last_error = memory_system_last_error;
      return 0;
    }

  byte_file_handle = MEM_NewHandle(
    sizeof(ResourceByteFile), MEMORY_REQUEST_ANY,
    MEM_POINTER_OPTIONS_DEFAULT);
  if(byte_file_handle == 0)
    {
      resource_system_last_error = memory_system_last_error;
      MEM_DisposeHandle(file_handle);
      return 0;
    }

  file_handle->flags |= MEM_HANDLE_FLAG_BUSY;
  file_record = (ResourceFileRecord *)file_handle->data;
  file_record->state = 1;
  file_record->byte_file_handle = byte_file_handle;
  file_record->previous_file_handle = 0;
  file_record->resource_tables = 0;

  byte_file_handle->flags |= MEM_HANDLE_FLAG_BUSY;
  byte_file = (ResourceByteFile *)byte_file_handle->data;
  byte_file->owner_task = CURRENTTASK->t.n_Item;

  if(SF_Open(path, &byte_file->reader, 0) == 0)
    {
      resource_system_last_error = block_file_last_error;
      MEM_DisposeHandle(byte_file_handle);
      MEM_DisposeHandle(file_handle);
      return 0;
    }

  old_head = (MemoryHandle *)resource_manager_globals.resource_list;
  file_record->next_file_handle = old_head;
  if(old_head != 0)
    {
      old_head->flags |= MEM_HANDLE_FLAG_BUSY;
      old_record = (ResourceFileRecord *)old_head->data;
      old_record->previous_file_handle = file_handle;
      old_head->flags &= ~MEM_HANDLE_FLAG_BUSY;
    }

  resource_manager_globals.resource_list = file_handle;
  byte_file_handle->flags &= ~MEM_HANDLE_FLAG_BUSY;
  file_handle->flags &= ~MEM_HANDLE_FLAG_BUSY;
  return file_handle;
}
