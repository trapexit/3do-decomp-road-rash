#include "resource_manager_runtime.h"

int
cleanup_resource_file_without_callback(MemoryHandle *root)
{
  return CleanUpFile(root, 0);
}


int
RSRC_DisposeFileResources(MemoryHandle *root)
{
  return CleanUpFile(root, MEM_DisposeHandle);
}


int
RSRC_ReleaseFileResources(MemoryHandle *root)
{
  return CleanUpFile(root, MEM_ReleaseHandle);
}
