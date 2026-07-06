#include "family_resource_runtime.h"
#include "rw_semantic_data.h"

FamilyResourceValue
lookup_family_resource_descendant(FamilyResourceSelector selector,
                                  int                    family_index,
                                  int                    child_index,
                                  int                    grandchild_index,
                                  int                    descendant_index)
{
  int group_index;
  int entry_index;
  FamilyResourceLock *lock;
  FamilyResourceTable *table;
  FamilyResourceLevel entries;
  FamilyResourceLevel children;
  FamilyResourceLevel grandchildren;
  FamilyResourceLevel descendants;
  FamilyResourceValue result;

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

  children = (FamilyResourceLevel)entries[entry_index];
  if(child_index >= 0 &&
     FAMILY_RESOURCE_LEVEL_COUNT(children) <= child_index)
    {
      release_family_resource_lock(lock);
      return 0;
    }

  grandchildren = (FamilyResourceLevel)children[child_index];
  if(grandchild_index >= 0 &&
     FAMILY_RESOURCE_LEVEL_COUNT(grandchildren) <= grandchild_index)
    {
      release_family_resource_lock(lock);
      return 0;
    }

  descendants = (FamilyResourceLevel)grandchildren[grandchild_index];
  if(descendant_index >= 0 &&
     FAMILY_RESOURCE_LEVEL_COUNT(descendants) <= descendant_index)
    {
      release_family_resource_lock(lock);
      return 0;
    }

  result = descendants[descendant_index];
  release_family_resource_lock(lock);
  return result;
}
