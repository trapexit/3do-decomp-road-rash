#include "item.h"

#include "memory_manager_runtime.h"
#include "resource_manager_runtime.h"
#include "rw_semantic_data.h"

#define RESOURCE_BUSY 0x01000000U
#define RESOURCE_CALLBACK_OWNED 0x02000000U
#define CLEANUP_FILE_BAD_POINTER 0x1102

typedef struct CleanupResourceNode CleanupResourceNode;
typedef struct CleanupResourceRecord CleanupResourceRecord;

typedef struct ResourceEntry
{
  int unused[5];
  CleanupResourceNode *child;
  int reserved[2];
} ResourceEntry;

struct CleanupResourceRecord
{
  int reserved[2];
  int entry_count;
  CleanupResourceNode *next;
  ResourceEntry entries[1];
};

struct CleanupResourceNode
{
  CleanupResourceRecord *record;
  unsigned int flags;
};

typedef struct ResourceRootData
{
  int reserved[4];
  CleanupResourceNode *children;
} ResourceRootData;

#define RESOURCE_STATUS resource_system_last_error

int
CloseAndUnlinkRsrcFile(MemoryHandle *entry);

int
CleanUpFile(MemoryHandle *root,
            int         (*release_callback)(MemoryHandle *))
{
  ResourceEntry *entry;
  CleanupResourceNode *child;
  CleanupResourceNode *next;
  CleanupResourceNode *node;
  CleanupResourceNode *root_node;
  CleanupResourceRecord *record;
  int index;

  if(root == 0)
    {
      RESOURCE_STATUS = CLEANUP_FILE_BAD_POINTER;
      return RESOURCE_STATUS;
    }

  LockItem(resource_manager_globals.lock_item, 1);
  if(CloseAndUnlinkRsrcFile(root) != 0)
    {
      UnlockItem(resource_manager_globals.lock_item);
      return RESOURCE_STATUS;
    }

  root_node = (CleanupResourceNode *)root;
  root_node->flags |= RESOURCE_BUSY;
  node = ((ResourceRootData *)root_node->record)->children;
  while(node != 0)
    {
      node->flags |= RESOURCE_BUSY;
      record = node->record;

      for(index = 0; index < record->entry_count; index++)
        {
          entry = &record->entries[index];
          child = entry->child;
          if(child == 0)
            {
              continue;
            }

          if((child->flags & RESOURCE_CALLBACK_OWNED) == 0 ||
             child->record == 0)
            {
              MEM_DisposeHandle((MemoryHandle *)child);
            }
          else if(release_callback != 0)
            {
              child->flags &= ~RESOURCE_BUSY;
              release_callback((MemoryHandle *)child);
            }
          entry->child = 0;
        }

      next = record->next;
      node->flags &= ~RESOURCE_BUSY;
      MEM_DisposeHandle((MemoryHandle *)node);
      node = next;
    }

  root_node->flags &= ~RESOURCE_BUSY;
  MEM_DisposeHandle(root);
  UnlockItem(resource_manager_globals.lock_item);
  return RESOURCE_STATUS;
}
