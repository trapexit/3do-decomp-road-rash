#include "resource_manager_runtime.h"
#include "rw_semantic_data.h"





ResourceRecord *
SearchRsrcFiles(uint32         resource_type,
                int32          resource_id,
                MemoryHandle **out_file_handle,
                MemoryHandle **out_table_handle)
{
  MemoryHandle *file_handle;
  MemoryHandle *table_handle;
  ResourceFileRecord *file;
  ResourceTableBlock *table;
  ResourceRecord *record;

  record = 0;
  table_handle = 0;
  file_handle = (MemoryHandle *)resource_manager_globals.resource_list;

  while(file_handle != 0 && record == 0)
    {
      file_handle->flags |= MEM_HANDLE_FLAG_BUSY;
      file = (ResourceFileRecord *)file_handle->data;
      table_handle = file->resource_tables;

      while(table_handle != 0 && record == 0)
        {
          table_handle->flags |= MEM_HANDLE_FLAG_BUSY;
          table = (ResourceTableBlock *)table_handle->data;
          record = find_resource_record_in_table(
            table, resource_type, resource_id);
          if(record == 0)
            {
              table_handle->flags &= ~MEM_HANDLE_FLAG_BUSY;
              table_handle = table->next_table_handle;
            }
        }

      if(record == 0)
        {
          file_handle->flags &= ~MEM_HANDLE_FLAG_BUSY;
          file_handle = file->next_file_handle;
        }
    }

  *out_file_handle = file_handle;
  *out_table_handle = table_handle;
  return record;
}
