#include "bss_early_data.h"
#include "front_end_cel_runtime.h"
#include "resource_manager_runtime.h"
#include "rw_semantic_data.h"

#define FRONT_END_CEL_RESOURCE_TYPE 0x43454C20U
#define AACL_CHUNK_TYPE 0x4141434CU
#define FRONT_END_CEL_RESOURCE_ID_OFFSET 1
#define FRONT_END_CEL_LOAD_FLAGS 2
#define BACKGROUND_AUDIO_BUFFERING_LEVEL 0xF0
#define AACL_HEADER_WORDS 4

int
load_front_end_cel_resource(int *,
                            int *);
MemoryHandle *
RSRC_GetResourceInfo(uint32,
                     int32,
                     ByteFileReader **);
void
set_background_audio_buffering(int,
                               int);
MemoryHandle *
RSRC_LoadResource(uint32 resource_type,
                  int32  resource_id,
                  uint32 options);
int
finalize_front_end_cel_resource(int,
                                int);

int
load_front_end_cel_resource(int *descriptor_words,
                            int *resource_info_words)
{
  const FrontEndCelDescriptor *descriptor;
  FrontEndCelResourceInfo *resource_info;
  FrontEndCelResourceInfo parent_info;
  ResourceRecord *record;
  MemoryHandle *resource_handle;
  MemoryHandle *info_handle;
  int *data;
  FrontEndCelOffset *offset;
  int child;
  int parent_acquired;
  int resource_acquired;

  if(descriptor_words == 0 || resource_info_words == 0)
    {
      return 0;
    }

  descriptor = (const FrontEndCelDescriptor *)descriptor_words;
  resource_info = (FrontEndCelResourceInfo *)resource_info_words;
  parent_acquired = 0;
  resource_acquired = 0;

  if(descriptor->parent_index != -1)
    {
      if(load_front_end_cel_resource(
           (int *)&gFrontEndCelHierarchy[descriptor->parent_index],
           (int *)&parent_info) == 0)
        {
          return 0;
        }
      parent_acquired = 1;
    }

  info_handle = RSRC_GetResourceInfo(
    FRONT_END_CEL_RESOURCE_TYPE,
    descriptor->resource_id + FRONT_END_CEL_RESOURCE_ID_OFFSET, 0);
  resource_handle = 0;
  if(info_handle != 0)
    {
      info_handle->flags |= MEM_HANDLE_FLAG_BUSY;
      record = (ResourceRecord *)info_handle->data;
      if(record != 0 && record->resource_handle != 0 &&
         record->resource_handle->data != 0)
        {
          resource_handle = record->resource_handle;
        }
      info_handle->flags &= ~MEM_HANDLE_FLAG_BUSY;
      MEM_DisposeHandle(info_handle);
    }

  if(resource_handle == 0)
    {
      set_background_audio_buffering(
        0, BACKGROUND_AUDIO_BUFFERING_LEVEL);
      resource_handle = RSRC_LoadResource(
        FRONT_END_CEL_RESOURCE_TYPE,
        descriptor->resource_id + FRONT_END_CEL_RESOURCE_ID_OFFSET,
        FRONT_END_CEL_LOAD_FLAGS);
      set_background_audio_buffering(
        1, BACKGROUND_AUDIO_BUFFERING_LEVEL);
    }

  if(resource_handle == 0)
    {
      goto fail;
    }

  resource_handle->flags |= MEM_HANDLE_FLAG_BUSY;
  resource_acquired = 1;
  data = (int *)resource_handle->data;
  if(data == 0)
    {
      goto fail;
    }
  if(data[0] == AACL_CHUNK_TYPE)
    {
      data += AACL_HEADER_WORDS;
    }
  if(data[0] == 0)
    {
      goto fail;
    }

  parse_cans_animation_metadata(
    (CansAnimationData *)data, &resource_info->animation);

  resource_info->x = -resource_info->animation.center_x;
  resource_info->y = -resource_info->animation.center_y;
  if(resource_info->animation.hotspot_count > 0)
    {
      offset = resource_info->animation.hotspots;
      resource_info->x -= offset->x;
      resource_info->y -= offset->y;
    }

  if(descriptor->parent_index != -1)
    {
      child = descriptor->child_selector;
      if(child != 0 &&
         child < parent_info.animation.hotspot_count)
        {
          offset = &parent_info.animation.hotspots[child];
          resource_info->x += offset->x + parent_info.x;
          resource_info->y += offset->y + parent_info.y;
        }
      finalize_front_end_cel_resource(descriptor->parent_index, 0);
      parent_acquired = 0;
    }

  return 1;

fail:
  if(resource_acquired != 0)
    {
      finalize_front_end_cel_resource(descriptor->resource_id, 0);
    }
  if(parent_acquired != 0)
    {
      finalize_front_end_cel_resource(descriptor->parent_index, 0);
    }
  return 0;
}


#undef FRONT_END_CEL_RESOURCE_TYPE
#undef AACL_CHUNK_TYPE
#undef FRONT_END_CEL_RESOURCE_ID_OFFSET
#undef FRONT_END_CEL_LOAD_FLAGS
#undef BACKGROUND_AUDIO_BUFFERING_LEVEL
#undef AACL_HEADER_WORDS
