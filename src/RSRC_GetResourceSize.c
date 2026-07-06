#include "item.h"

#include "resource_manager_runtime.h"
#include "rw_semantic_data.h"

#define RESOURCE_BUSY 0x01000000U

int
RSRC_GetResourceSize(int selector,
                     int identifier)
{
  MemoryHandle *inner;
  MemoryHandle *outer;
  ResourceRecord *resource;
  int size;

  LockItem(resource_manager_globals.lock_item, 1);
  resource = SearchRsrcFiles(selector, identifier,
                             &outer, &inner);

  size = 0;
  if(resource != 0)
    {
      size = resource->byte_count;
      inner->flags &= ~RESOURCE_BUSY;
      outer->flags &= ~RESOURCE_BUSY;
    }

  UnlockItem(resource_manager_globals.lock_item);
  return size;
}
