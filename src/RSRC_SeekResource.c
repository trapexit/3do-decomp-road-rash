#include "item.h"

#include "resource_manager_runtime.h"
#include "rw_semantic_data.h"

Err
RSRC_SeekResource(uint32 resource_type,
                  int32  resource_id)
{
  ResourceRecord *record;
  MemoryHandle *file_handle;
  MemoryHandle *table_handle;
  ResourceByteFile *task_file;
  int result;

  LockItem(resource_manager_globals.lock_item, 1);
  record = SearchRsrcFiles(resource_type, resource_id,
                           &file_handle, &table_handle);

  if(record == 0)
    {
      UnlockItem(resource_manager_globals.lock_item);
      return resource_system_last_error;
    }

  file_handle->flags |= MEM_HANDLE_FLAG_BUSY;
  task_file = GetRsrcByteFile(
    (ResourceByteFilePool *)file_handle->data);
  if(task_file == 0)
    {
      table_handle->flags &= ~MEM_HANDLE_FLAG_BUSY;
      file_handle->flags &= ~MEM_HANDLE_FLAG_BUSY;
      UnlockItem(resource_manager_globals.lock_item);
      return resource_system_last_error;
    }

  result = BF_SeekHead(&task_file->reader, record->file_position,
              BYTE_FILE_SEEK_SET);
  ((ResourceByteFilePool *)file_handle->data)->
    entries_handle->flags &= ~MEM_HANDLE_FLAG_BUSY;
  table_handle->flags &= ~MEM_HANDLE_FLAG_BUSY;
  file_handle->flags &= ~MEM_HANDLE_FLAG_BUSY;
  UnlockItem(resource_manager_globals.lock_item);
  return result < 0 ? result : 0;
}
