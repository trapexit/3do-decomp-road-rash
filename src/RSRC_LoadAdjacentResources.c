#include "stdio.h"

#include "item.h"
#include "resource_manager_runtime.h"
#include "rw_semantic_data.h"

#define RESOURCE_LOCK resource_manager_globals.lock_item

enum AdjacentResourceConstant
{
  ADJACENT_RESOURCE_SECOND_GAP_SPAN = 2
};




#include "memory_manager_runtime.h"

static const char adjacent_gap_error[] =
  "ERROR: RSRC_LoadAdjacentResources() - Resource file gap spacing does "
  "not match this version of rsrc mgr and memory mgr!\n";

int
RSRC_LoadAdjacentResources(unsigned int resource_type,
                           unsigned int first_index,
                           unsigned int last_index)
{
  ResourceRecord *entry;
  ResourceRecord *last;
  ResourceRecord *resource;
  MemoryHandle *outer;
  MemoryHandle *inner;
  ResourceByteFilePool *owner;
  ResourceRecord **resources;
  char *buffer;
  char *segment;
  char *next;
  char *old_buffer;
  int span;
  int count;
  int loaded_count;
  int gap;
  int next_gap;
  int alignment;
  int total_size;
  ResourceByteFile *task_file;
  int read_result;
  MemoryHandle *shared;
  int i;
  unsigned int current_offset;
  unsigned int delta;

  if(first_index >= last_index)
    {
      return 0;
    }

  LockItem(RESOURCE_LOCK, 1);

  outer = 0;
  inner = 0;
  loaded_count = 0;
  entry = SearchRsrcFiles(
    resource_type, first_index, &outer, &inner);
  if(entry == 0)
    {
      goto unlock;
    }

  span = (int)(last_index - first_index);
  count = span + 1;
  resources = (ResourceRecord **)MEM_NewPointer(
    count * sizeof(*resources), MEMTYPE_ANY,
    MEM_POINTER_OPTIONS_DEFAULT);
  if(resources == 0)
    {
      inner->flags &= ~MEM_HANDLE_FLAG_BUSY;
      outer->flags &= ~MEM_HANDLE_FLAG_BUSY;
      goto unlock;
    }

  for(i = 0; i <= span; i++)
    {
      resources[i] = entry + i;
    }
  RsrcShellSort(resources, count);

  last = entry + span;
  if(last->resource_type != resource_type ||
     (unsigned int)last->resource_id != last_index)
    {
      goto cleanup;
    }

  gap = resources[1]->file_position -
        (entry->file_position + entry->byte_count);
  alignment = get_memory_manager_block_header_size();
  if(span >= ADJACENT_RESOURCE_SECOND_GAP_SPAN)
    {
      next_gap =
        resources[ADJACENT_RESOURCE_SECOND_GAP_SPAN]->file_position -
        (resources[1]->file_position +
         resources[1]->byte_count);
      if(next_gap != gap)
        {
          goto cleanup;
        }
    }
  if(alignment > gap)
    {
      printf(adjacent_gap_error);
      goto cleanup;
    }

  last = resources[span];
  total_size = last->file_position + last->byte_count -
               entry->file_position;

  for(i = 0; i <= span; i++)
    {
      if(CheckRsrcInMemory(resources[i]) != 0)
        {
          loaded_count++;
        }
    }
  if(loaded_count == count)
    {
      goto cleanup;
    }

  outer->flags |= MEM_HANDLE_FLAG_BUSY;
  owner = (ResourceByteFilePool *)outer->data;
  task_file = GetRsrcByteFile(owner);
  if(task_file == 0)
    {
      goto cleanup;
    }

  buffer = MEM_NewPointer(total_size, MEMTYPE_ANY,
                          MEM_POINTER_OPTIONS_DEFAULT);
  if(buffer == 0)
    {
      owner->entries_handle->flags &= ~MEM_HANDLE_FLAG_BUSY;
      goto cleanup;
    }

  read_result = BF_ReadBytes(
    &task_file->reader, buffer, total_size, entry->file_position);
  owner->entries_handle->flags &= ~MEM_HANDLE_FLAG_BUSY;
  if(read_result < total_size)
    {
      MEM_DisposePointer(buffer);
      goto cleanup;
    }

  current_offset = (unsigned int)entry->file_position;

  for(i = 0; buffer != 0 && i <= span; i++)
    {
      resource = resources[i];
      segment = buffer;

      if((unsigned int)resource->file_position != current_offset)
        {
          delta = (unsigned int)resource->file_position - current_offset;
          if(delta <= (unsigned int)alignment)
            {
              current_offset = (unsigned int)resource->file_position;
            }
          else
            {
              old_buffer = segment;
              segment = (char *)MEM_SplitBlock(
                segment, (int)delta - alignment);
              if(segment == 0)
                {
                  MEM_DisposePointer(old_buffer);
                  goto cleanup;
                }
              MEM_DisposePointer(old_buffer);
            }
          current_offset = (unsigned int)resource->file_position;
        }

      if(i < span)
        {
          next = (char *)MEM_SplitBlock(
            segment, resource->byte_count + gap - alignment);
          if(next == 0)
            {
              MEM_DisposePointer(segment);
              goto cleanup;
            }
          current_offset +=
            (unsigned int)(resource->byte_count + gap);
        }
      else
        {
          next = 0;
        }

      if(CheckRsrcInMemory(resource) != 0)
        {
          MEM_DisposePointer(segment);
        }
      else
        {
          shared = MEM_PointerToHandle(segment);
          if(shared != 0)
            {
              resource->flags = RESOURCE_FLAG_HANDLE_BACKED;
              resource->resource_handle = shared;
              shared->flags |= MEM_HANDLE_FLAG_RESOURCE;
            }
          else
            {
              MEM_DisposePointer(segment);
            }
        }

      buffer = next;
    }

cleanup:
  MEM_DisposePointer((char *)resources);
  inner->flags &= ~MEM_HANDLE_FLAG_BUSY;
  outer->flags &= ~MEM_HANDLE_FLAG_BUSY;

unlock:
  UnlockItem(RESOURCE_LOCK);
  return loaded_count;
}


int
RSRC_ShareFile(MemoryHandle *file_handle)
{
  int result;

  file_handle->flags |= MEM_HANDLE_FLAG_BUSY;
  result = ShareFile((ResourceByteFilePool *)file_handle->data);
  file_handle->flags &= ~MEM_HANDLE_FLAG_BUSY;
  return result;
}

#undef RESOURCE_LOCK
