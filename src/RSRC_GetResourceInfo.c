#include "item.h"
#include "string.h"

#include "resource_manager_runtime.h"
#include "rw_semantic_data.h"

#define RESOURCE_MANAGER_LOCK resource_manager_globals.lock_item
#define RESOURCE_LAST_ERROR (&resource_system_last_error)
#define MEMORY_LAST_ERROR (&memory_system_last_error)


static
void
clear_resource_lookup_busy_flags(MemoryHandle *file_handle,
                                 MemoryHandle *table_handle)
{
  if(table_handle != 0)
    {
      table_handle->flags &= ~MEM_HANDLE_FLAG_BUSY;
    }
  if(file_handle != 0)
    {
      file_handle->flags &= ~MEM_HANDLE_FLAG_BUSY;
    }
}


MemoryHandle *
RSRC_GetResourceInfo(uint32           resource_type,
                     int32            resource_id,
                     ByteFileReader **out_byte_file)
{
  ResourceRecord *record;
  MemoryHandle *file_handle;
  MemoryHandle *table_handle;
  MemoryHandle *info_handle;
  ResourceByteFile *task_file;

  LockItem(RESOURCE_MANAGER_LOCK, 1);

  file_handle = 0;
  table_handle = 0;
  info_handle = 0;
  if(out_byte_file != 0)
    {
      *out_byte_file = 0;
    }
  record = SearchRsrcFiles(resource_type, resource_id,
                           &file_handle, &table_handle);
  if(record == 0)
    {
      goto done;
    }

  if(out_byte_file != 0)
    {
      *out_byte_file = 0;
      file_handle->flags |= MEM_HANDLE_FLAG_BUSY;
      task_file = GetRsrcByteFile(
        (ResourceByteFilePool *)file_handle->data);
      if(task_file == 0)
        {
          goto cleanup;
        }

      *out_byte_file = (ByteFileReader *)MEM_NewPointer(
        sizeof(ByteFileReader), MEMTYPE_ANY,
        MEM_POINTER_OPTIONS_DEFAULT);
      if(*out_byte_file == 0)
        {
          goto cleanup;
        }
      memcpy(*out_byte_file, &task_file->reader,
             sizeof(ByteFileReader));
    }

  info_handle = MEM_NewHandle(sizeof(ResourceRecord),
                              MEMORY_REQUEST_ANY, 0);
  if(info_handle != 0)
    {
      info_handle->flags |= MEM_HANDLE_FLAG_BUSY;
      memcpy(info_handle->data, record, sizeof(ResourceRecord));
      info_handle->flags &= ~MEM_HANDLE_FLAG_BUSY;
    }

cleanup:
  if(out_byte_file != 0)
    {
      ((ResourceByteFilePool *)file_handle->data)->
        entries_handle->flags &= ~MEM_HANDLE_FLAG_BUSY;
      if(info_handle == 0 && *out_byte_file != 0)
        {
          MEM_DisposePointer(*out_byte_file);
          *out_byte_file = 0;
        }
    }
  clear_resource_lookup_busy_flags(file_handle, table_handle);

done:
  UnlockItem(RESOURCE_MANAGER_LOCK);
  return info_handle;
}


Err
RSRC_AttachData(uint32 resource_type,
                int32  resource_id,
                void  *attached_data)
{
  ResourceRecord *record;
  MemoryHandle *file_handle;
  MemoryHandle *table_handle;
  Err result;

  LockItem(RESOURCE_MANAGER_LOCK, 1);
  file_handle = 0;
  table_handle = 0;
  record = SearchRsrcFiles(resource_type, resource_id,
                           &file_handle, &table_handle);
  if(record != 0)
    {
      record->attached_data = attached_data;
      clear_resource_lookup_busy_flags(file_handle, table_handle);
    }
  UnlockItem(RESOURCE_MANAGER_LOCK);

  if(record == 0)
    {
      result = *RESOURCE_LAST_ERROR;
    }
  else
    {
      result = 0;
    }
  return result;
}


void *
RSRC_GetData(uint32 resource_type,
             int32  resource_id)
{
  ResourceRecord *record;
  MemoryHandle *file_handle;
  MemoryHandle *table_handle;
  void *attached_data;

  *RESOURCE_LAST_ERROR = 0;
  attached_data = 0;
  LockItem(RESOURCE_MANAGER_LOCK, 1);
  file_handle = 0;
  table_handle = 0;
  record = SearchRsrcFiles(resource_type, resource_id,
                           &file_handle, &table_handle);
  if(record != 0)
    {
      attached_data = record->attached_data;
      clear_resource_lookup_busy_flags(file_handle, table_handle);
    }
  UnlockItem(RESOURCE_MANAGER_LOCK);
  return attached_data;
}


Err
RSRC_Detach(uint32 resource_type,
            int32  resource_id)
{
  ResourceRecord *record;
  MemoryHandle *file_handle;
  MemoryHandle *table_handle;

  LockItem(RESOURCE_MANAGER_LOCK, 1);
  file_handle = 0;
  table_handle = 0;
  record = SearchRsrcFiles(resource_type, resource_id,
                           &file_handle, &table_handle);
  if(record != 0)
    {
      record->resource_handle = 0;
      record->flags &= ~RESOURCE_FLAG_RESIDENT;
      clear_resource_lookup_busy_flags(file_handle, table_handle);
    }
  UnlockItem(RESOURCE_MANAGER_LOCK);

  if(record == 0)
    {
      return *RESOURCE_LAST_ERROR;
    }
  return 0;
}


Err
RSRC_ReleaseResource(uint32 resource_type,
                     int32  resource_id)
{
  ResourceRecord *record;
  MemoryHandle *file_handle;
  MemoryHandle *table_handle;
  MemoryHandle *resource_handle;
  Err result;

  result = 0;
  LockItem(RESOURCE_MANAGER_LOCK, 1);
  file_handle = 0;
  table_handle = 0;
  record = SearchRsrcFiles(resource_type, resource_id,
                           &file_handle, &table_handle);
  if(record != 0)
    {
      resource_handle = record->resource_handle;
      if(resource_handle != 0)
        {
          resource_handle->flags &= ~MEM_HANDLE_FLAG_BUSY;
          result = MEM_ReleaseHandle(resource_handle);
          if(result != 0)
            {
              result = *MEMORY_LAST_ERROR;
              *RESOURCE_LAST_ERROR = result;
            }
          record->flags &= ~RESOURCE_FLAG_RESIDENT;
        }
      clear_resource_lookup_busy_flags(file_handle, table_handle);
    }
  UnlockItem(RESOURCE_MANAGER_LOCK);

  if(record == 0)
    {
      return *RESOURCE_LAST_ERROR;
    }
  return result;
}


Err
RSRC_DisposeResource(uint32 resource_type,
                     int32  resource_id)
{
  ResourceRecord *record;
  MemoryHandle *file_handle;
  MemoryHandle *table_handle;
  MemoryHandle *resource_handle;
  Err result;

  result = 0;
  LockItem(RESOURCE_MANAGER_LOCK, 1);
  file_handle = 0;
  table_handle = 0;
  record = SearchRsrcFiles(resource_type, resource_id,
                           &file_handle, &table_handle);
  if(record != 0)
    {
      resource_handle = record->resource_handle;
      if(resource_handle != 0)
        {
          resource_handle->flags &= ~MEM_HANDLE_FLAG_BUSY;
          result = MEM_DisposeHandle(resource_handle);
          if(result != 0)
            {
              result = *MEMORY_LAST_ERROR;
              *RESOURCE_LAST_ERROR = result;
            }
        }
      record->flags &= ~RESOURCE_FLAG_RESIDENT;
      record->resource_handle = 0;
      clear_resource_lookup_busy_flags(file_handle, table_handle);
    }
  UnlockItem(RESOURCE_MANAGER_LOCK);

  if(record == 0)
    {
      return *RESOURCE_LAST_ERROR;
    }
  return result;
}

















#undef RESOURCE_MANAGER_LOCK
#undef RESOURCE_LAST_ERROR
#undef MEMORY_LAST_ERROR
