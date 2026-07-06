#ifndef ROADRASH_ITEM_POOL_H
#define ROADRASH_ITEM_POOL_H

#include "itempool.h"

ItemPoolPtr
RoadRashCreateItemPool(long          item_count,
                       CreateProcPtr create_item,
                       void         *create_argument);
Item
RoadRashAllocPoolItem(ItemPoolPtr item_pool);
void
RoadRashReturnPoolItem(ItemPoolPtr item_pool,
                       Item        item);
void
RoadRashDeleteItemPool(ItemPoolPtr item_pool);

#endif
