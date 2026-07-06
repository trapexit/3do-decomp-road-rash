#include "family_resource_runtime.h"
#include "rw_semantic_data.h"

int
visit_family_resource_entry(FamilyResourceSelector    selector,
                            int                       family_index,
                            FamilyResourceEntryVisitor visitor,
                            void                      *context)
{
  int group_index;
  int entry_index;
  FamilyResourceLock *lock;
  FamilyResourceTable *table;
  FamilyResourceLevel entries;
  FamilyResourceValue value;

  if(visitor == 0)
    {
      return 0;
    }

  group_index = FAMILY_RESOURCE_GROUP_INDEX(selector);
  entry_index = FAMILY_RESOURCE_ENTRY_INDEX(selector);

  if(group_index < 0 ||
     family_resource_globals.group_count <= group_index)
    {
      return 0;
    }

  lock = &family_resource_globals.group_locks[group_index];
  if(try_acquire_family_resource_lock(
       lock, FAMILY_RESOURCE_LOCK_ACQUIRED) !=
     FAMILY_RESOURCE_LOCK_ACQUIRED)
    {
      return 0;
    }

  table = (FamilyResourceTable *)
          family_resource_globals.group_tables[group_index];
  if(table->entry_count == 0)
    {
      release_family_resource_lock(lock);
      return 0;
    }

  if(family_index >= 0 && table->entry_count <= family_index)
    {
      release_family_resource_lock(lock);
      return 0;
    }

  entries = table->entries[family_index];
  if(entry_index >= 0 &&
     FAMILY_RESOURCE_LEVEL_COUNT(entries) <= entry_index)
    {
      release_family_resource_lock(lock);
      return 0;
    }

  value = entries[entry_index];
  visitor(value, context);
  release_family_resource_lock(lock);
  return 1;
}
