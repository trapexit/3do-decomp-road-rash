#include "item.h"
#include "kernelnodes.h"
#include "nodes.h"
#include "platform.h"
#include "semaphore.h"
#include "string.h"

#include "division_helpers.h"
#include "memory_manager_runtime.h"
#include "rw_semantic_data.h"

typedef struct MemoryInitConfigurationPair
{
  int first;
  int second;
} MemoryInitConfigurationPair;

int
MEM_Init(const MemoryInitTag *tags)
{
  MemoryAllocatorGlobals *state;
  const MemoryInitTag *tag;
  const MemoryInitConfigurationPair *configuration_pair;
  SignedDivisionResult division_result;
  MemInfo memory_info;
  uint32 requested_bytes[MEM_INIT_MEMORY_TYPE_COUNT];
  uint32 reserved_bytes[MEM_INIT_MEMORY_TYPE_COUNT];
  uint32 region_memory_types[MEM_INIT_MAX_REGIONS];
  uint32 region_sizes[MEM_INIT_MAX_REGIONS];
  char *regions[MEM_INIT_MAX_REGIONS];
  uint32 memory_types[MEM_INIT_MEMORY_TYPE_COUNT];
  uint32 size;
  uint32 reservation;
  uint32 page_size;
  uint32 remaining;
  uint32 usable_size;
  int requested_master_count;
  int restrict_address;
  int memory_type_index;
  int region_count;
  int first_region;
  int lock_item;
  int result;
  int index;
  char *region_base;
  MemoryRegionHeader *region_header;
  MemoryHeapDescriptor *heap;
  MemoryHeapDescriptor *heap_tail;
  MemoryBlockHeader *free_block;

  state = &memory_allocator_globals;
  memory_types[0] = (uint32)state->primary_memory_type;
  memory_types[1] = (uint32)state->secondary_memory_type;
  if(state->heap_list_head != 0)
    {
      memory_system_last_error = MEM_ERROR_ALREADY_INITIALIZED;
      return MEM_ERROR_ALREADY_INITIALIZED;
    }

  bzero(state, offsetof(MemoryAllocatorGlobals, primary_memory_type));
  lock_item = CreateSizedItem(
    MKNODEID(KERNELNODE, SEMAPHORENODE), 0, 0);
  state->lock_item = lock_item;
  if(lock_item < 0)
    {
      state->lock_item = 0;
      memory_system_last_error = MEM_ERROR_NO_MEMORY_BANK;
      return MEM_ERROR_NO_MEMORY_BANK;
    }
  if(LockItem(lock_item, 1) < 0)
    {
      DeleteItem(lock_item);
      state->lock_item = 0;
      memory_system_last_error = MEM_ERROR_NO_MEMORY_BANK;
      return MEM_ERROR_NO_MEMORY_BANK;
    }

  requested_master_count = 0;
  restrict_address = 0;
  for(index = 0; index < MEM_INIT_MEMORY_TYPE_COUNT; index++)
    {
      requested_bytes[index] = MEM_INIT_UNSPECIFIED_SIZE;
      reserved_bytes[index] = MEM_INIT_UNSPECIFIED_SIZE;
    }
  for(index = 0; index < MEM_INIT_MAX_REGIONS; index++)
    {
      region_memory_types[index] = 0;
      region_sizes[index] = 0;
      regions[index] = 0;
    }

  tag = tags;
  if(tag != 0)
    {
      while(tag->tag != MEM_INIT_TAG_END)
        {
          switch(tag->tag)
            {
            case MEM_INIT_TAG_MASTER_COUNT:
              requested_master_count = (int)tag->value;
              if(requested_master_count < 0)
                {
                  requested_master_count = (int)(0U - tag->value);
                }
              break;
            case MEM_INIT_TAG_DRAM_SIZE:
              requested_bytes[0] = tag->value;
              if((int)requested_bytes[0] <= 0)
                {
                  requested_bytes[0] = 0U - requested_bytes[0];
                }
              break;
            case MEM_INIT_TAG_VRAM_SIZE:
              requested_bytes[1] = tag->value;
              if((int)requested_bytes[1] <= 0)
                {
                  requested_bytes[1] = 0U - requested_bytes[1];
                }
              break;
            case MEM_INIT_TAG_DRAM_RESERVE:
              reserved_bytes[0] = tag->value;
              if((int)reserved_bytes[0] <= 0)
                {
                  reserved_bytes[0] = 0U - reserved_bytes[0];
                }
              break;
            case MEM_INIT_TAG_VRAM_RESERVE:
              reserved_bytes[1] = tag->value;
              if((int)reserved_bytes[1] <= 0)
                {
                  reserved_bytes[1] = 0U - reserved_bytes[1];
                }
              break;
            case MEM_INIT_TAG_RESTRICT_ADDRESS:
              restrict_address = 1;
              break;
            }
          tag++;
        }
    }

  region_count = 0;
  for(memory_type_index = 0;
      memory_type_index < MEM_INIT_MEMORY_TYPE_COUNT;
      memory_type_index++)
    {
      if(region_count == MEM_INIT_MAX_REGIONS)
        {
          break;
        }
      size = requested_bytes[memory_type_index];
      if(size != MEM_INIT_UNSPECIFIED_SIZE)
        {
          if(size < MEM_INIT_MINIMUM_REGION_SIZE)
            {
              size = MEM_INIT_MINIMUM_REGION_SIZE;
            }

          availMem(&memory_info, memory_types[memory_type_index]);
          if(memory_info.minfo_SysLargest > size)
            {
              regions[region_count] = (char *)AllocMemFromMemLists(
                CURRENTTASK->t_FreeMemoryLists, size,
                memory_types[memory_type_index]);
              if(regions[region_count] == 0)
                {
                  continue;
                }
              region_memory_types[region_count] =
                memory_types[memory_type_index];
              region_sizes[region_count] = size;
              region_count++;
              continue;
            }

          if(memory_info.minfo_SysLargest +
             memory_info.minfo_TaskLargest <= size)
            {
              result = MEM_ERROR_NO_FREE_BLOCK;
              goto failed;
            }

          regions[region_count] = (char *)AllocMemFromMemLists(
            CURRENTTASK->t_FreeMemoryLists,
            memory_info.minfo_TaskLargest,
            memory_types[memory_type_index]);
          if(regions[region_count] != 0)
            {
              region_memory_types[region_count] =
                memory_types[memory_type_index];
              region_sizes[region_count] = memory_info.minfo_TaskLargest;
              size -= memory_info.minfo_TaskLargest;
              region_count++;
            }

          if(region_count == MEM_INIT_MAX_REGIONS)
            {
              break;
            }

          regions[region_count] = (char *)AllocMemFromMemLists(
            CURRENTTASK->t_FreeMemoryLists, size,
            memory_types[memory_type_index]);
          if(regions[region_count] != 0)
            {
              region_memory_types[region_count] =
                memory_types[memory_type_index];
              region_sizes[region_count] = size;
              region_count++;
            }
          continue;
        }

      reservation = 0;
      if(reserved_bytes[memory_type_index] !=
         MEM_INIT_UNSPECIFIED_SIZE)
        {
          page_size = GetPageSize(memory_types[memory_type_index]);
          division_result = signed_divide_with_remainder(
            page_size,
            reserved_bytes[memory_type_index] + page_size - 1);
          reservation = page_size * (uint32)division_result.quotient;
        }

      first_region = region_count;
      while(region_count < MEM_INIT_MAX_REGIONS)
        {
          availMem(&memory_info, memory_types[memory_type_index]);
          size = memory_info.minfo_TaskLargest;
          if(memory_info.minfo_SysLargest > size)
            {
              size = memory_info.minfo_SysLargest;
            }

          regions[region_count] = 0;
          if(size >= MEM_INIT_MINIMUM_REGION_SIZE)
            {
              regions[region_count] = (char *)AllocMemFromMemLists(
                CURRENTTASK->t_FreeMemoryLists, size,
                memory_types[memory_type_index]);
            }

          if(size < MEM_INIT_MINIMUM_REGION_SIZE ||
             regions[region_count] == 0)
            {
              size = get_largest_free_memory_run(
                memory_types[memory_type_index]);
              if(size < MEM_INIT_MINIMUM_REGION_SIZE)
                {
                  break;
                }
              regions[region_count] = (char *)AllocMemFromMemLists(
                CURRENTTASK->t_FreeMemoryLists, size,
                memory_types[memory_type_index]);
            }

          if(regions[region_count] == 0)
            {
              break;
            }

          region_memory_types[region_count] =
            memory_types[memory_type_index];
          region_sizes[region_count] = size;
          if(restrict_address != 0 &&
             (uint32)regions[region_count] >=
             MEM_INIT_RESTRICTED_ADDRESS_LIMIT)
            {
              FreeMemToMemLists(
                CURRENTTASK->t_FreeMemoryLists,
                regions[region_count], region_sizes[region_count]);
              regions[region_count] = 0;
              region_sizes[region_count] = 0;
              region_memory_types[region_count] = 0;
              break;
            }
          region_count++;
        }

      if(reservation > 0 && first_region < region_count)
        {
          if(reservation < region_sizes[first_region] &&
             region_sizes[first_region] - reservation >=
             MEM_INIT_MINIMUM_REGION_SIZE)
            {
              remaining = region_sizes[first_region] - reservation;
              FreeMemToMemLists(
                CURRENTTASK->t_FreeMemoryLists,
                regions[first_region], region_sizes[first_region]);
              region_sizes[first_region] = remaining;
              regions[first_region] = (char *)AllocMemFromMemLists(
                CURRENTTASK->t_FreeMemoryLists, remaining,
                memory_types[memory_type_index]);
            }
          else
            {
              FreeMemToMemLists(
                CURRENTTASK->t_FreeMemoryLists,
                regions[first_region], region_sizes[first_region]);
              regions[first_region] = 0;
            }
        }
    }

  ScavengeMem();
  for(index = 0; index < region_count; index++)
    {
      if(regions[index] == 0)
        {
          continue;
        }
      region_base = regions[index];
      size = region_sizes[index];
      if(size < sizeof(MemoryRegionHeader) + sizeof(MemoryBlockHeader))
        {
          FreeMemToMemLists(
            CURRENTTASK->t_FreeMemoryLists, regions[index], size);
          regions[index] = 0;
          continue;
        }
      region_header = (MemoryRegionHeader *)region_base;
      append_memory_region(region_header, region_base, size);

      heap = &region_header->heap;
      free_block = (MemoryBlockHeader *)(region_header + 1);
      usable_size = size - sizeof(MemoryRegionHeader);
      free_block->size_and_flags = 0;
      free_block->owner = 0;

      heap_tail = (MemoryHeapDescriptor *)state->heap_list_head;
      if(heap_tail == 0)
        {
          state->heap_list_head = heap;
        }
      else
        {
          while(heap_tail->next != 0)
            {
              heap_tail = heap_tail->next;
            }
          heap_tail->next = heap;
        }

      heap->base_address = free_block;
      heap->last_free_block = free_block;
      heap->first_free_block = free_block;
      heap->largest_free_block = free_block;
      heap->free_bytes = usable_size;
      heap->purgeable_bytes = 0;
      heap->secondary_purgeable_bytes = 0;
      heap->next = 0;
      heap->size = usable_size;
      heap->memory_type = (uint32)GetMemType(heap);

      if(region_memory_types[index] == MEMTYPE_VRAM)
        {
          free_block->size_and_flags |= MEM_BLOCK_VRAM_FLAG;
        }
      link_free_memory_block(free_block, heap, usable_size, 0, 0);
    }

  result = CreateMoreMasters(1, requested_master_count);
  if(result != 0)
    {
      goto failed;
    }

  state->reserved_state[MEM_CONFIGURATION_LIMIT_INDEX] = -1;
  tag = tags;
  if(tag != 0)
    {
      while(tag->tag != MEM_INIT_TAG_END)
        {
          switch(tag->tag)
            {
            case MEM_INIT_TAG_CONFIGURATION_WORD:
              state->reserved_state[MEM_CONFIGURATION_WORD_INDEX] =
                (int)tag->value;
              break;
            case MEM_INIT_TAG_CONFIGURATION_PAIR:
              configuration_pair =
                (const MemoryInitConfigurationPair *)tag->value;
              state->reserved_state[0] = configuration_pair->first;
              state->reserved_state[1] = configuration_pair->second;
              break;
            case MEM_INIT_TAG_ENABLE_OPTION:
              *(unsigned char *)&state->reserved_state[
                MEM_CONFIGURATION_OPTION_INDEX] = 1;
              break;
            case MEM_INIT_TAG_CONFIGURATION_LIMIT:
              state->reserved_state[MEM_CONFIGURATION_LIMIT_INDEX] =
                (int)tag->value;
              break;
            }
          tag++;
        }
    }

  UnlockItem(state->lock_item);
  return 0;

failed:
  for(index = 0; index < region_count; index++)
    {
      if(regions[index] != 0)
        {
          FreeMemToMemLists(
            CURRENTTASK->t_FreeMemoryLists, regions[index],
            region_sizes[index]);
        }
    }
  bzero(state, offsetof(MemoryAllocatorGlobals, primary_memory_type));
  UnlockItem(lock_item);
  DeleteItem(lock_item);
  memory_system_last_error = result;
  return result;
}
