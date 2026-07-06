#include "memory_manager_runtime.h"
#include "road_rash_item_pool.h"

ItemPoolPtr
RoadRashCreateItemPool(long          item_count,
                       CreateProcPtr create_item,
                       void         *create_argument)
{
  ItemPoolPtr item_pool;
  ItemDescPtr descriptor;
  long remaining;
  Item item;

  if(item_count < 0 || create_item == 0 ||
     (uint32)item_count >
     (MEM_BLOCK_SIZE_MASK - MEM_POINTER_HEADER_SIZE -
      MEM_ALLOCATION_ALIGNMENT_MASK) / sizeof(ItemDesc))
    {
      return 0;
    }

  remaining = item_count;
  item_pool = (ItemPoolPtr)MEM_NewPointer(
    sizeof(ItemPool), MEMTYPE_ANY, MEM_POINTER_OPTIONS_DEFAULT);
  if(item_pool != 0)
    {
      item_pool->inUseList = 0;
      item_pool->numItemsInPool = remaining;
      item_pool->availList = 0;

      descriptor = (ItemDescPtr)MEM_NewPointer(
        (int)(remaining * sizeof(ItemDesc)), MEMTYPE_ANY,
        MEM_POINTER_OPTIONS_DEFAULT);
      item_pool->itemDescBlockPtr = descriptor;
      if(descriptor == 0)
        {
          MEM_DisposePointer(item_pool);
          return 0;
        }
      while(remaining > 0)
        {
          remaining--;
          item = create_item(create_argument);
          descriptor->item = item;
          if(item <= 0)
            {
              RoadRashDeleteItemPool(item_pool);
              return 0;
            }

          descriptor->next = item_pool->availList;
          item_pool->availList = descriptor;
          descriptor++;
        }
    }

  return item_pool;
}


Item
RoadRashAllocPoolItem(ItemPoolPtr item_pool)
{
  ItemDescPtr descriptor;
  Item item;

  item = 0;
  descriptor = item_pool->availList;
  if(descriptor != 0)
    {
      item = descriptor->item;
      item_pool->availList = descriptor->next;
      descriptor->next = item_pool->inUseList;
      item_pool->inUseList = descriptor;
    }

  return item;
}


void
RoadRashReturnPoolItem(ItemPoolPtr item_pool,
                       Item        item)
{
  ItemDescPtr descriptor;
  ItemDescPtr previous;

  previous = 0;
  descriptor = item_pool->inUseList;
  while(descriptor != 0)
    {
      if(descriptor->item == item)
        {
          if(previous == 0)
            {
              item_pool->inUseList = descriptor->next;
            }
          else
            {
              previous->next = descriptor->next;
            }

          descriptor->next = item_pool->availList;
          item_pool->availList = descriptor;
          return;
        }

      previous = descriptor;
      descriptor = descriptor->next;
    }
}


void
RoadRashDeleteItemPool(ItemPoolPtr item_pool)
{
  ItemDescPtr descriptor;

  if(item_pool == 0)
    {
      return;
    }

  descriptor = item_pool->availList;
  while(descriptor != 0)
    {
      DeleteItem(descriptor->item);
      descriptor = descriptor->next;
    }

  MEM_DisposePointer(item_pool->itemDescBlockPtr);
  MEM_DisposePointer(item_pool);
}
