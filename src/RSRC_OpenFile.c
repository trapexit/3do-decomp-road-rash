#include "item.h"
#include "resource_manager_runtime.h"
#include "rw_semantic_data.h"

#define RESOURCE_LOCK resource_manager_globals.lock_item




#include "memory_manager_runtime.h"
void
PrepRSRCTable(PreparedResourceTableHeader *table);

typedef struct ResourceFileDiskHeader
{
  int32 reserved_00[4];
  int32 resource_data_position;
  int32 resource_data_byte_count;
} ResourceFileDiskHeader;

MemoryHandle *
RSRC_OpenFile(const char *path,
              int         options)
{
  MemoryHandle *entry;
  ResourceFileRecord *file_record;
  MemoryHandle *byte_file_handle;
  ResourceByteFile *byte_file;
  MemoryHandle *resource_data_handle;
  ResourceFileDiskHeader *disk_header;
  volatile int file_data;
  int bytes_read;

  LockItem(RESOURCE_LOCK, 1);
  (void)options;

  entry = OpenAndLinkRsrcFile(path);
  if(entry == 0)
    {
      UnlockItem(RESOURCE_LOCK);
      return 0;
    }

  entry->flags |= MEM_HANDLE_FLAG_BUSY;
  file_record = (ResourceFileRecord *)entry->data;
  byte_file_handle = file_record->byte_file_handle;
  byte_file_handle->flags |= MEM_HANDLE_FLAG_BUSY;
  byte_file = (ResourceByteFile *)byte_file_handle->data;

  disk_header = (ResourceFileDiskHeader *)MEM_NewPointer(
    byte_file->reader.file_status.fs.ds_DeviceBlockSize,
    MEMORY_REQUEST_ANY,
    MEM_POINTER_OPTIONS_DEFAULT);
  if(disk_header == 0)
    {
      CloseAndUnlinkRsrcFile(entry);
      MEM_DisposeHandle(entry);
      resource_system_last_error = memory_system_last_error;
      UnlockItem(RESOURCE_LOCK);
      return 0;
    }

  if(SF_Read(&byte_file->reader, disk_header, 1, 0) != 1)
    {
      MEM_DisposePointer(disk_header);
      CloseAndUnlinkRsrcFile(entry);
      MEM_DisposeHandle(entry);
      resource_system_last_error = RESOURCE_ERROR_SHORT_READ;
      UnlockItem(RESOURCE_LOCK);
      return 0;
    }
  resource_data_handle = MEM_NewHandle(
    disk_header->resource_data_byte_count, MEMORY_REQUEST_ANY,
    MEM_POINTER_OPTIONS_DEFAULT);
  if(resource_data_handle == 0)
    {
      MEM_DisposePointer(disk_header);
      CloseAndUnlinkRsrcFile(entry);
      MEM_DisposeHandle(entry);
      resource_system_last_error = memory_system_last_error;
      UnlockItem(RESOURCE_LOCK);
      return 0;
    }

  resource_data_handle->flags |= MEM_HANDLE_FLAG_BUSY;
  file_data = (int)resource_data_handle->data;
  bytes_read = BF_ReadBytes(
    &byte_file->reader, (void *)file_data,
    disk_header->resource_data_byte_count,
    disk_header->resource_data_position);
  if(bytes_read != disk_header->resource_data_byte_count)
    {
      MEM_DisposeHandle(resource_data_handle);
      MEM_DisposePointer(disk_header);
      CloseAndUnlinkRsrcFile(entry);
      MEM_DisposeHandle(entry);
      resource_system_last_error = RESOURCE_ERROR_SHORT_READ;
      UnlockItem(RESOURCE_LOCK);
      return 0;
    }
  MEM_DisposePointer(disk_header);
  file_record->resource_tables = resource_data_handle;
  PrepRSRCTable((PreparedResourceTableHeader *)file_data);
  resource_data_handle->flags &= ~MEM_HANDLE_FLAG_BUSY;
  byte_file_handle->flags &= ~MEM_HANDLE_FLAG_BUSY;
  entry->flags &= ~MEM_HANDLE_FLAG_BUSY;
  UnlockItem(RESOURCE_LOCK);
  return entry;
}
