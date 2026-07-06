#include "platform.h"

#include "resource_manager_runtime.h"
#include "rw_semantic_data.h"

int
CloseAndUnlinkRsrcFile(MemoryHandle *entry)
{
  ResourceFileRecord *file_record;
  MemoryHandle *byte_file_handle;
  ResourceByteFile *byte_file;
  MemoryHandle *previous;
  MemoryHandle *next;
  ResourceFileRecord *linked_record;
  int index;

  resource_system_last_error = 0;
  entry->flags |= MEM_HANDLE_FLAG_BUSY;
  file_record = (ResourceFileRecord *)entry->data;

  byte_file_handle = file_record->byte_file_handle;
  byte_file_handle->flags |= MEM_HANDLE_FLAG_BUSY;
  byte_file = (ResourceByteFile *)byte_file_handle->data;
  for(index = 0; index < file_record->state; index++)
    {
      if(byte_file[index].owner_task == CURRENTTASK->t.n_Item)
        {
          SF_Close(&byte_file[index].reader);
        }
    }

  byte_file_handle->flags &= ~MEM_HANDLE_FLAG_BUSY;
  MEM_DisposeHandle(byte_file_handle);

  previous = file_record->previous_file_handle;
  if(previous == 0)
    {
      resource_manager_globals.resource_list = file_record->next_file_handle;
    }
  else
    {
      previous->flags |= MEM_HANDLE_FLAG_BUSY;
      linked_record = (ResourceFileRecord *)previous->data;
      linked_record->next_file_handle = file_record->next_file_handle;
      previous->flags &= ~MEM_HANDLE_FLAG_BUSY;
    }

  next = file_record->next_file_handle;
  if(next != 0)
    {
      next->flags |= MEM_HANDLE_FLAG_BUSY;
      linked_record = (ResourceFileRecord *)next->data;
      linked_record->previous_file_handle = previous;
      next->flags &= ~MEM_HANDLE_FLAG_BUSY;
    }

  return resource_system_last_error;
}
