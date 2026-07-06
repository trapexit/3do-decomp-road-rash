#include "stdio.h"

#include "item.h"

#include "resource_manager_runtime.h"
#include "rw_semantic_data.h"

static
void
abandon_prepared_resource_file(MemoryHandle *file_handle)
{
  CloseAndUnlinkRsrcFile(file_handle);
  MEM_DisposeHandle(file_handle);
}


MemoryHandle *
RSRC_LoadFile(const char *path,
              int         options)
{
  MemoryHandle *file_handle;
  ResourceFileRecord *file_record;
  MemoryHandle *byte_file_handle;
  ResourceByteFile *byte_file;
  MemoryHandle *image_handle;
  PreparedResourceFileHeader *prepared_header;
  PreparedResourceTableHeader *table;
  ResourceRecord **sorted_resources;
  ResourceRecord *resource;
  unsigned char *file_image;
  unsigned char *payload;
  unsigned char *old_payload;
  MemoryHandle *block_handle;
  int file_size;
  int resource_count;
  int allocator_header_size;
  int payload_offset;
  int gap_size;
  int resource_index;
  int loaded_count;
  int failure;

  image_handle = 0;
  table = 0;
  sorted_resources = 0;
  payload = 0;
  loaded_count = 0;
  failure = RESOURCE_ERROR_SHORT_READ;

  (void)options;

  LockItem(resource_manager_globals.lock_item, 1);

  file_handle = OpenAndLinkRsrcFile(path);
  if(file_handle == 0)
    {
      UnlockItem(resource_manager_globals.lock_item);
      return 0;
    }

  file_handle->flags |= MEM_HANDLE_FLAG_BUSY;
  file_record = (ResourceFileRecord *)file_handle->data;
  byte_file_handle = file_record->byte_file_handle;
  byte_file_handle->flags |= MEM_HANDLE_FLAG_BUSY;
  byte_file = (ResourceByteFile *)byte_file_handle->data;

  file_size = byte_file->reader.file_status.fs_ByteCount;
  image_handle = MEM_NewHandle(
    file_size, MEMORY_REQUEST_ANY, MEM_POINTER_OPTIONS_DEFAULT);
  if(image_handle == 0)
    {
      failure = memory_system_last_error;
      goto failure;
    }

  image_handle->flags |= MEM_HANDLE_FLAG_BUSY;
  file_image = (unsigned char *)image_handle->data;
  if(file_size < sizeof(PreparedResourceFileHeader) ||
     BF_ReadBytes(&byte_file->reader, file_image, file_size, 0) != file_size)
    {
      goto failure;
    }
  prepared_header = (PreparedResourceFileHeader *)file_image;

  if(prepared_header->memory_manager_layout == 0)
    {
      printf(
        "File %s is not prepared to be used with RSRC_LoadFile()!, Aborting open.\n",
        path);
      goto failure;
    }

  allocator_header_size = get_memory_manager_block_header_size();
  if(!((prepared_header->memory_manager_layout ==
        PREPARED_RESOURCE_MEMORY_DEBUG &&
        allocator_header_size == DEBUG_MEMORY_BLOCK_HEADER_SIZE) ||
       (prepared_header->memory_manager_layout ==
        PREPARED_RESOURCE_MEMORY_NONDEBUG &&
        allocator_header_size == NONDEBUG_MEMORY_BLOCK_HEADER_SIZE)))
    {
      printf(
        "File %s is prepared for %s version of memory manager,\n"
        "  but you're running the %s memory manager\n",
        path,
        (prepared_header->memory_manager_layout ==
         PREPARED_RESOURCE_MEMORY_DEBUG) ? "Debug" : "Non-Debug",
        (get_memory_manager_block_header_size() ==
         DEBUG_MEMORY_BLOCK_HEADER_SIZE) ? "Debug" : "Non-Debug");
      goto failure;
    }

  table = (PreparedResourceTableHeader *)MEM_SplitBlock(
    file_image, PREPARED_RESOURCE_FILE_HEADER_SIZE);
  if(table == 0)
    {
      goto failure;
    }
  resource_count = table->resource_count;
  sorted_resources = (ResourceRecord **)MEM_NewPointer(
    resource_count * sizeof(ResourceRecord *), MEMTYPE_ANY,
    MEM_POINTER_OPTIONS_DEFAULT);
  if(sorted_resources == 0)
    {
      failure = memory_system_last_error;
      goto failure;
    }

  for(resource_index = 0;
      resource_index < resource_count;
      resource_index++)
    {
      sorted_resources[resource_index] = &table->records[resource_index];
    }

  RsrcShellSort(sorted_resources, resource_count);

  payload = (unsigned char *)MEM_SplitBlock(
    table, PREPARED_RESOURCE_TABLE_HEADER_SIZE +
    resource_count * sizeof(ResourceRecord));
  if(payload == 0 && resource_count > 0)
    {
      goto failure;
    }
  payload_offset = payload != 0 ? payload - file_image : 0;

  for(resource_index = 0;
      payload != 0 && resource_index < resource_count;
      resource_index++)
    {
      resource = sorted_resources[resource_index];
      if(resource->file_position != payload_offset)
        {
          gap_size = resource->file_position - payload_offset;
          if(gap_size <= allocator_header_size)
            {
              payload_offset = resource->file_position;
            }
          else
            {
              old_payload = payload;
              payload = (unsigned char *)MEM_SplitBlock(
                payload, gap_size - allocator_header_size);
              if(payload == 0)
                {
                  payload = old_payload;
                  goto failure;
                }
              MEM_DisposePointer(old_payload);
            }
          payload_offset = resource->file_position;
        }

      block_handle = MEM_PointerToHandle(payload);
      if(block_handle == 0)
        {
          failure = memory_system_last_error;
          goto failure;
        }
      resource->resource_handle = block_handle;
      resource->flags = RESOURCE_FLAG_HANDLE_BACKED;
      block_handle->flags |= MEM_HANDLE_FLAG_RESOURCE;
      loaded_count++;

      if(resource_index >= resource_count - 1)
        {
          payload = 0;
        }
      else
        {
          payload = (unsigned char *)MEM_SplitBlock(
            payload, resource->byte_count);
          if(payload == 0)
            {
              goto failure;
            }
          payload_offset += resource->byte_count + allocator_header_size;
        }
    }

  if(resource_count == 0 && payload != 0)
    {
      MEM_DisposePointer(payload);
      payload = 0;
    }
  block_handle = MEM_PointerToHandle(table);
  if(block_handle == 0)
    {
      failure = memory_system_last_error;
      goto failure;
    }
  file_record->resource_tables = block_handle;
  MEM_DisposePointer(sorted_resources);

  image_handle->flags &= ~MEM_HANDLE_FLAG_BUSY;
  MEM_DisposeHandle(image_handle);
  block_handle->flags &= ~MEM_HANDLE_FLAG_BUSY;
  byte_file_handle->flags &= ~MEM_HANDLE_FLAG_BUSY;
  file_handle->flags &= ~MEM_HANDLE_FLAG_BUSY;

  MEM_CompactHeap(MEM_COMPACT_TARGET_DRAM | MEM_COMPACT_TARGET_VRAM);
  UnlockItem(resource_manager_globals.lock_item);
  return file_handle;

failure:
  if(payload != 0)
    {
      MEM_DisposePointer(payload);
    }
  for(resource_index = 0; resource_index < loaded_count; resource_index++)
    {
      MEM_DisposeHandle(sorted_resources[resource_index]->resource_handle);
    }
  if(sorted_resources != 0)
    {
      MEM_DisposePointer(sorted_resources);
    }
  if(table != 0)
    {
      MEM_DisposePointer(table);
    }
  if(image_handle != 0)
    {
      MEM_DisposeHandle(image_handle);
    }
  abandon_prepared_resource_file(file_handle);
  resource_system_last_error = failure;
  UnlockItem(resource_manager_globals.lock_item);
  return 0;
}
