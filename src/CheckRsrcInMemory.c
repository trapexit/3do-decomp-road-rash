#include "resource_manager_runtime.h"

MemoryHandle *
CheckRsrcInMemory(ResourceRecord *resource)
{
  MemoryHandle *resource_handle;
  MemoryHandle *recovered_handle;

  resource_handle = resource->resource_handle;
  if(resource_handle == 0)
    {
      return 0;
    }

  if((resource_handle->flags & MEM_HANDLE_FLAG_VALID) == 0 ||
     resource_handle->data == 0)
    {
      MEM_DisposeHandle(resource_handle);
      resource->resource_handle = 0;
      return 0;
    }

  if((resource_handle->flags & MEM_HANDLE_FLAG_RELEASED) != 0)
    {
      recovered_handle = MEM_RecoverHandle(resource_handle);
      resource->resource_handle = recovered_handle;
      if(recovered_handle != 0)
        {
          resource_handle = recovered_handle;
        }
      else
        {
          MEM_DisposeHandle(resource_handle);
          return 0;
        }
    }

  return resource_handle;
}
