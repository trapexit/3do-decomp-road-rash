#ifndef RESOURCE_MANAGER_RUNTIME_H
#define RESOURCE_MANAGER_RUNTIME_H

#include "byte_file_runtime.h"
#include "memory_manager_runtime.h"

#define RESOURCE_FLAG_RESIDENT 0x00000001U
#define RESOURCE_FLAG_HANDLE_BACKED 0x00000002U
#define RESOURCE_FLAG_PRELOAD 0x00000100U
#define RESOURCE_LOAD_OPTION_KEEP_BUSY 0x00000008U
#define RESOURCE_LOAD_OPTION_EXISTING_ONLY 0x00000010U
#define RESOURCE_LOAD_OPTION_TRANSIENT 0x00000020U
#define RESOURCE_LOAD_OPTION_ALLOW_FAILURE 0x00000001U
#define RESOURCE_LOAD_OPTION_COMPACT_FIRST 0x00000002U
#define RESOURCE_LOAD_OPTION_COMPACT_HEAP_FIRST 0x00000004U
#define RESOURCE_LOAD_MEMORY_VRAM 0x00000040U
#define RESOURCE_LOAD_MEMORY_DRAM 0x00000080U
#define RESOURCE_LOAD_MEMORY_MASK 0x000000C0U
#define RESOURCE_ERROR_NOT_FOUND 0x1103
#define RESOURCE_ERROR_SHORT_READ 0x1104
#define RESOURCE_ERROR_NOT_RESIDENT 0x1105

#define PREPARED_RESOURCE_FILE_HEADER_SIZE 0x30
#define PREPARED_RESOURCE_TABLE_HEADER_SIZE 0x10
#define PREPARED_RESOURCE_MEMORY_DEBUG 1
#define PREPARED_RESOURCE_MEMORY_NONDEBUG 2
#define DEBUG_MEMORY_BLOCK_HEADER_SIZE 0x10
#define NONDEBUG_MEMORY_BLOCK_HEADER_SIZE 8

typedef struct ResourceRecord
{
  uint32 resource_type;
  int32 resource_id;
  int32 file_position;
  int32 byte_count;
  uint32 flags;
  MemoryHandle *resource_handle;
  int32 reserved_18;
  void *attached_data;
} ResourceRecord;

typedef struct ResourceByteFile
{
  ByteFileReader reader;
  Item owner_task;
} ResourceByteFile;

typedef struct ResourceFileRecord
{
  int32 state;
  MemoryHandle *byte_file_handle;
  MemoryHandle *previous_file_handle;
  MemoryHandle *next_file_handle;
  MemoryHandle *resource_tables;
} ResourceFileRecord;

typedef struct PreparedResourceFileHeader
{
  int32 reserved_00[2];
  int32 memory_manager_layout;
  int32 reserved_0C[9];
} PreparedResourceFileHeader;

typedef struct PreparedResourceTableHeader
{
  int32 reserved_00[2];
  int32 resource_count;
  int32 reserved_0C;
  ResourceRecord records[1];
} PreparedResourceTableHeader;

typedef struct ResourceTableBlock
{
  int32 reserved_00[2];
  int32 last_resource_index;
  MemoryHandle *next_table_handle;
} ResourceTableBlock;

typedef struct ResourceByteFilePool
{
  int32 count;
  MemoryHandle *entries_handle;
} ResourceByteFilePool;

typedef char ResourceRecordSizeCheck[
    (sizeof(ResourceRecord) == 0x20) ? 1 : -1];
typedef char ResourceByteFileSizeCheck[
    (sizeof(ResourceByteFile) == 0x3C) ? 1 : -1];
typedef char ResourceFileRecordMinimumSizeCheck[
    (sizeof(ResourceFileRecord) == 0x14) ? 1 : -1];
typedef char PreparedResourceFileHeaderSizeCheck[
    (sizeof(PreparedResourceFileHeader) ==
     PREPARED_RESOURCE_FILE_HEADER_SIZE) ? 1 : -1];
typedef char PreparedResourceTableRecordsOffsetCheck[
    (offsetof(PreparedResourceTableHeader, records) ==
     PREPARED_RESOURCE_TABLE_HEADER_SIZE) ? 1 : -1];
typedef char ResourceTableBlockMinimumSizeCheck[
    (sizeof(ResourceTableBlock) == 0x10) ? 1 : -1];
typedef char ResourceByteFilePoolSizeCheck[
    (sizeof(ResourceByteFilePool) == 8) ? 1 : -1];

ResourceRecord *
SearchRsrcFiles(uint32         resource_type,
                int32          resource_id,
                MemoryHandle **out_file_handle,
                MemoryHandle **out_table_handle);
ResourceRecord *
find_resource_record_in_table(ResourceTableBlock *table,
                              uint32              resource_type,
                              int32               resource_id);
void *
RSRC_StuffResource(uint32 resource_type,
                   int32  resource_id,
                   void  *destination);
int
RSRC_GetResourceSize(int selector,
                     int identifier);
ResourceByteFile *
GetRsrcByteFile(ResourceByteFilePool *pool);
int
LoadResource(ResourceByteFilePool *pool,
             ResourceRecord       *resource,
             void                 *destination);
MemoryHandle *
OpenAndLinkRsrcFile(const char *path);
int
CloseAndUnlinkRsrcFile(MemoryHandle *entry);
int
CleanUpFile(MemoryHandle *root,
            int         (*release_callback)(MemoryHandle *));
void
RsrcShellSort(ResourceRecord **resources,
              int              resource_count);
MemoryHandle *
RSRC_OpenFile(const char *path,
              int         options);
MemoryHandle *
RSRC_LoadFile(const char *path,
              int         options);
int
ShareFile(ResourceByteFilePool *pool);
int
RSRC_ShareFile(MemoryHandle *file_handle);
int
RSRC_LoadAdjacentResources(unsigned int resource_type,
                           unsigned int first_index,
                           unsigned int last_index);
MemoryHandle *
CheckRsrcInMemory(ResourceRecord *resource);
void
decode_resource_load_options(uint32  options,
                             uint32 *out_memory_type,
                             uint32 *out_handle_options);

/*
 * These inactive indexed/group entry points retain their Road Rash stub
 * behavior. Their names and argument counts come from intact APCS traceback
 * records in the sibling EA resource manager. The word-sized parameter types
 * describe the ABI because the historical typedef spellings are unavailable.
 */
void
RSRC_Patch(void *patch_data);
MemoryHandle *
RSRC_GetIndexedResource(uint32 resource_type,
                        int32  resource_id,
                        int32  resource_index,
                        int32  options);
int32
RSRC_GetIndexedResourceSize(uint32 resource_type,
                            int32  resource_id,
                            int32  resource_index);
Err
RSRC_StuffIndexedResource(uint32      resource_type,
                          int32       resource_id,
                          int32       resource_index,
                          const void *resource_data);
Err
RSRC_ReleaseIndexedResource(uint32 resource_type,
                            int32  resource_id,
                            int32  resource_index);
Err
RSRC_OpenGroup(uint32 group_type,
               int32  group_id);
int32
RSRC_OpenGroupSize(uint32 group_type,
                   int32  group_id);
Err
RSRC_CloseGroup(uint32 group_type,
                int32  group_id);
MemoryHandle *
RSRC_LoadGroup(uint32 group_type,
               int32  group_id);
int32
RSRC_LoadGroupSize(uint32 group_type,
                   int32  group_id);

#endif
