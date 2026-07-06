#include "family_resource_binding.h"
#include "family_resource_runtime.h"
#include "rw_semantic_data.h"

enum FamilyResourceLookupDepth
{
  FAMILY_RESOURCE_LOOKUP_ENTRY = 0,
  FAMILY_RESOURCE_LOOKUP_CHILD = 1,
  FAMILY_RESOURCE_LOOKUP_GRANDCHILD = 2,
  FAMILY_RESOURCE_LOOKUP_DESCENDANT = 3
};

static
FamilyResourceValue
lookup_family_resource_child_internal(FamilyResourceSelector         selector,
                                      int                            family_index,
                                      int                            child_index,
                                      FamilyResourceBindingIdentity *identity)
{
  int group_index;
  int entry_index;
  int family_id;
  unsigned int generation;
  FamilyResourceLock *lock;
  FamilyResourceTable *table;
  FamilyResourceLevel entries;
  FamilyResourceLevel children;
  FamilyResourceValue result;

  if(identity != 0)
    {
      identity->selector = 0;
      identity->generation = 0;
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

  children = (FamilyResourceLevel)entries[entry_index];
  if(child_index >= 0 &&
     FAMILY_RESOURCE_LEVEL_COUNT(children) <= child_index)
    {
      release_family_resource_lock(lock);
      return 0;
    }

  result = children[child_index];
  if(identity != 0)
    {
      family_id =
        family_resource_globals.resident_family_ids[group_index];
      generation = family_resource_generations[group_index];
      if(family_id <= 0 || generation == 0)
        {
          result = 0;
        }
      else
        {
          identity->selector = (unsigned int)selector;
          identity->generation = generation;
        }
    }
  release_family_resource_lock(lock);
  return result;
}


FamilyResourceValue
lookup_family_resource_child(FamilyResourceSelector selector,
                             int                    family_index,
                             int                    child_index)
{
  return lookup_family_resource_child_internal(
    selector, family_index, child_index, 0);
}


FamilyResourceValue
lookup_family_resource_child_with_identity(FamilyResourceSelector         selector,
                                           int                            family_index,
                                           int                            child_index,
                                           FamilyResourceBindingIdentity *identity)
{
  if(identity == 0)
    {
      return 0;
    }
  return lookup_family_resource_child_internal(
    selector, family_index, child_index, identity);
}


int
try_acquire_current_family_resource_binding(const FamilyResourceBindingIdentity *identity)
{
  FamilyResourceTable *table;
  unsigned int group_index;
  int family_id;
  unsigned int generation;
  FamilyResourceLock *lock;

  if(identity == 0 || identity->generation == 0)
    {
      return 0;
    }

  group_index = FAMILY_RESOURCE_GROUP_INDEX(identity->selector);
  if(group_index >=
     (unsigned int)family_resource_globals.group_count)
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

  family_id = family_resource_globals.resident_family_ids[group_index];
  generation = family_resource_generations[group_index];
  table = (FamilyResourceTable *)
          family_resource_globals.group_tables[group_index];
  if(family_id <= 0 || generation != identity->generation ||
     table == 0 || table->entry_count <= 0)
    {
      release_family_resource_lock(lock);
      return 0;
    }
  return 1;
}


void
release_current_family_resource_binding(const FamilyResourceBindingIdentity *identity)
{
  unsigned int group_index;

  if(identity == 0)
    {
      return;
    }
  group_index = FAMILY_RESOURCE_GROUP_INDEX(identity->selector);
  if(group_index <
     (unsigned int)family_resource_globals.group_count)
    {
      release_family_resource_lock(
        &family_resource_globals.group_locks[group_index]);
    }
}


int
family_resource_binding_values_are_current(FamilyResourceSelector selector,
                                           unsigned int generation)
{
  FamilyResourceTable *table;
  unsigned int group_index;
  unsigned int current_generation;
  int family_id;

  if(generation == 0)
    {
      return 0;
    }

  group_index = FAMILY_RESOURCE_GROUP_INDEX(selector);
  if(group_index >=
     (unsigned int)family_resource_globals.group_count)
    {
      return 0;
    }

  /* The caller owns the global render barrier, so a worker that reserved
     this group's lock cannot mutate or publish the table until the caller
     finishes using the borrowed resource.  Taking the group lock here
     would therefore reject a still-current binding just because that
     worker is waiting for the same global barrier. */
  family_id = family_resource_globals.resident_family_ids[group_index];
  current_generation = family_resource_generations[group_index];
  table = (FamilyResourceTable *)
          family_resource_globals.group_tables[group_index];
  return family_id > 0 && current_generation == generation &&
         table != 0 && table->entry_count > 0;
}


int
family_resource_binding_identity_is_current(const FamilyResourceBindingIdentity *identity)
{
  if(identity == 0)
    {
      return 0;
    }
  return family_resource_binding_values_are_current(
    identity->selector, identity->generation);
}


static
FamilyResourceValue
lookup_family_resource_for_render(FamilyResourceSelector selector,
                                  int                    family_index,
                                  int                    child_index,
                                  int                    grandchild_index,
                                  int                    descendant_index,
                                  int                    depth)
{
  FamilyResourceTable *table;
  FamilyResourceLevel level;
  FamilyResourceValue value;
  int entry_index;
  int group_index;

  group_index = FAMILY_RESOURCE_GROUP_INDEX(selector);
  entry_index = FAMILY_RESOURCE_ENTRY_INDEX(selector);
  if(group_index < 0 ||
     family_resource_globals.group_count <= group_index ||
     family_resource_globals.resident_family_ids[group_index] <= 0 ||
     family_index < 0 || entry_index < 0)
    {
      return 0;
    }

  table = (FamilyResourceTable *)
          family_resource_globals.group_tables[group_index];
  if(table == 0 || table->entry_count <= family_index)
    {
      return 0;
    }

  level = table->entries[family_index];
  if(level == 0 || FAMILY_RESOURCE_LEVEL_COUNT(level) <= entry_index)
    {
      return 0;
    }
  value = level[entry_index];
  if(depth == FAMILY_RESOURCE_LOOKUP_ENTRY)
    {
      return value;
    }

  level = (FamilyResourceLevel)value;
  if(level == 0 || child_index < 0 ||
     FAMILY_RESOURCE_LEVEL_COUNT(level) <= child_index)
    {
      return 0;
    }
  value = level[child_index];
  if(depth == FAMILY_RESOURCE_LOOKUP_CHILD)
    {
      return value;
    }

  level = (FamilyResourceLevel)value;
  if(level == 0 || grandchild_index < 0 ||
     FAMILY_RESOURCE_LEVEL_COUNT(level) <= grandchild_index)
    {
      return 0;
    }
  value = level[grandchild_index];
  if(depth == FAMILY_RESOURCE_LOOKUP_GRANDCHILD)
    {
      return value;
    }

  level = (FamilyResourceLevel)value;
  if(level == 0 || descendant_index < 0 ||
     FAMILY_RESOURCE_LEVEL_COUNT(level) <= descendant_index)
    {
      return 0;
    }
  return level[descendant_index];
}


FamilyResourceValue
lookup_family_resource_entry_for_render(FamilyResourceSelector selector,
                                        int                    family_index)
{
  return lookup_family_resource_for_render(
    selector, family_index, 0, 0, 0,
    FAMILY_RESOURCE_LOOKUP_ENTRY);
}


FamilyResourceValue
lookup_family_resource_child_for_render(FamilyResourceSelector selector,
                                        int                    family_index,
                                        int                    child_index)
{
  return lookup_family_resource_for_render(
    selector, family_index, child_index, 0, 0,
    FAMILY_RESOURCE_LOOKUP_CHILD);
}


FamilyResourceValue
lookup_family_resource_child_for_render_with_identity(FamilyResourceSelector         selector,
                                                      int                            family_index,
                                                      int                            child_index,
                                                      FamilyResourceBindingIdentity *identity)
{
  FamilyResourceValue value;
  unsigned int generation;
  int group_index;

  if(identity == 0)
    {
      return 0;
    }
  identity->selector = 0;
  identity->generation = 0;
  value = lookup_family_resource_child_for_render(
    selector, family_index, child_index);
  if(value == 0)
    {
      return 0;
    }

  /* The render barrier makes this publication stable even when its loader
     has reserved the narrower group lock while waiting to replace it. */
  group_index = FAMILY_RESOURCE_GROUP_INDEX(selector);
  generation = family_resource_generations[group_index];
  if(family_resource_globals.resident_family_ids[group_index] <= 0 ||
     generation == 0)
    {
      return 0;
    }
  identity->selector = (unsigned int)selector;
  identity->generation = generation;
  return value;
}


FamilyResourceValue
lookup_family_resource_grandchild_for_render(FamilyResourceSelector selector,
                                             int                    family_index,
                                             int                    child_index,
                                             int                    grandchild_index)
{
  return lookup_family_resource_for_render(
    selector, family_index, child_index, grandchild_index, 0,
    FAMILY_RESOURCE_LOOKUP_GRANDCHILD);
}


FamilyResourceValue
lookup_family_resource_descendant_for_render(FamilyResourceSelector selector,
                                             int                    family_index,
                                             int                    child_index,
                                             int                    grandchild_index,
                                             int                    descendant_index)
{
  return lookup_family_resource_for_render(
    selector, family_index, child_index, grandchild_index,
    descendant_index, FAMILY_RESOURCE_LOOKUP_DESCENDANT);
}
