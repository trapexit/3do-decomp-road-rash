#ifndef MEMORY_MANAGER_RUNTIME_H
#define MEMORY_MANAGER_RUNTIME_H

#include "mem.h"
#include "mempool.h"

#define MEMORY_REQUEST_DRAM        1U
#define MEMORY_REQUEST_VRAM        2U
#define MEMORY_REQUEST_ANY         4U

#define MEM_POINTER_OPTIONS_DEFAULT       0U
#define MEM_POINTER_OPTION_POINTER_BLOCK  1U
#define MEM_POINTER_OPTION_COMPACT_FIRST  4U
#define MEM_ALLOCATION_OPTION_PURGE_FIRST 8U
#define MEM_POINTER_OPTION_ALLOW_FAILURE  0x10U
#define MEM_ALLOCATION_OPTION_SKIP_PURGE  0x20U
#define MEM_ALLOCATION_OPTION_PRESERVE_SECONDARY_PURGE 0x40U
#define MEM_HANDLE_OPTION_PURGEABLE        2U
#define MEM_RESIZE_OPTION_KEEP_BUSY        2U

#define MEM_POINTER_HEADER_SIZE           8
#define MEM_ALLOCATION_ALIGNMENT          4U
#define MEM_ALLOCATION_ALIGNMENT_MASK     3U
#define MEM_SPLIT_HEADER_OVERHEAD          0x10U
#define MEM_HANDLE_BACK_POINTER_SIZE      4
#define MEM_BLOCK_ALLOCATED_FLAG          0x80000000U
#define MEM_BLOCK_HANDLE_OWNER_FLAG       0x40000000U
#define MEM_BLOCK_VRAM_FLAG               0x20000000U
#define MEM_BLOCK_SIZE_MASK               0x00FFFFFFU
#define MEM_BLOCK_FLAGS_MASK              0xFF000000U
#define MEM_BLOCK_PADDING_MASK            0x0F000000U
#define MEM_BLOCK_PADDING_SHIFT           24
#define MEM_BLOCK_FIXED_LOW_FLAG          0x20000000U
#define MEM_BLOCK_FIXED_HIGH_FLAG         0x10000000U
#define MEM_BLOCK_ALLOCATE_HIGH_FLAG      0x00080000U
#define MEM_BLOCK_ALLOCATE_LOW_FLAG       0x00010000U
#define MEM_HEAP_BASE_ADDRESS_INDEX       1
#define MEM_HEAP_FREE_BYTES_INDEX         7
#define MEM_HEAP_SIZE_INDEX               8
#define MEM_HEAP_NEXT_INDEX               9
#define MEM_BANK_FREE_BLOCK_LIST_INDEX    3
#define MEM_BLOCK_PREVIOUS_INDEX          2

#define MEM_HANDLE_FLAG_BUSY              0x01000000U
#define MEM_HANDLE_FLAG_VALID             0x02000000U
#define MEM_HANDLE_FLAG_RESOURCE          0x04000000U
#define MEM_HANDLE_FLAG_RELEASED          0x10000000U
#define MEM_HANDLE_FLAG_MASK              0xFF000000U
#define MEM_HANDLE_USER_DATA_MASK         0x00FFFFFFU
#define MEM_HANDLE_LOCK_VALUE_MASK        0xFFU
#define MEM_HANDLE_STATE_PRESERVE_MASK    0xEE000000U
#define MEM_HANDLE_FLAG_SECONDARY_MEMORY  0x20000000U
#define MEM_PURGE_LINK_REVERSED_FLAG      0x80000000U
#define MEM_PURGE_LINK_END                MEM_HANDLE_USER_DATA_MASK

#define MEM_BANK_HEADER_SIZE              8
#define MEM_BANK_NEXT_OFFSET              8
#define MEM_BANK_FIRST_HANDLE_OFFSET      0x10

#define MEM_INIT_MEMORY_TYPE_COUNT        2
#define MEM_INIT_MAX_REGIONS              10
#define MEM_INIT_UNSPECIFIED_SIZE         0xFFFFFFFEU
#define MEM_INIT_MINIMUM_REGION_SIZE      0x1034U
#define MEM_INIT_RESTRICTED_ADDRESS_LIMIT 0x00300000U

#define MEM_INIT_TAG_END                  0U
#define MEM_INIT_TAG_MASTER_COUNT         1U
#define MEM_INIT_TAG_DRAM_SIZE            2U
#define MEM_INIT_TAG_VRAM_SIZE            3U
#define MEM_INIT_TAG_DRAM_RESERVE         4U
#define MEM_INIT_TAG_VRAM_RESERVE         5U
#define MEM_INIT_TAG_CONFIGURATION_WORD   7U
#define MEM_INIT_TAG_CONFIGURATION_PAIR   8U
#define MEM_INIT_TAG_ENABLE_OPTION        9U
#define MEM_INIT_TAG_CONFIGURATION_LIMIT  10U
#define MEM_INIT_TAG_RESTRICT_ADDRESS     11U

#define MEM_CONFIGURATION_WORD_INDEX      2
#define MEM_CONFIGURATION_LIMIT_INDEX     3
#define MEM_CONFIGURATION_OPTION_INDEX    4

#define MEM_BLOCK_KIND_HANDLE             2
#define MEM_BLOCK_KIND_POINTER            1
#define MEM_MINIMUM_PAYLOAD_SIZE          8
#define MEM_HEAP_VRAM_FLAG                0x00010000U
#define MEM_MOVE_TOWARD_LOW_ADDRESS       1
#define MEM_MOVE_TOWARD_HIGH_ADDRESS      2
#define MEM_MOVE_RESULT_MOVED             1
#define MEM_MOVE_RESULT_RETRY             2
#define MEM_MOVE_RESULT_DIRECTION_MISMATCH 3
#define MEM_COMPACT_PURGEABLE_HANDLES     2
#define MEM_COMPACT_TARGET_VRAM           1U
#define MEM_COMPACT_TARGET_DRAM           2U
#define MEM_COMPACT_TARGET_MASK           3U
#define MEM_COMPACT_PURGE_HANDLES         4U
#define MEM_HANDLE_POOL_INITIAL_COUNT     200

#define MEM_RECLAIM_RESULT_COMPACTED      1
#define MEM_RECLAIM_RESULT_UNAVAILABLE    2

#define MEM_ERROR_NO_FREE_BLOCK    0x1000
#define MEM_ERROR_INVALID_HANDLE   0x1001
#define MEM_ERROR_FRAGMENTED_FREE_SPACE 0x1003
#define MEM_ERROR_NO_MEMORY_BANK   0x1004
#define MEM_ERROR_BAD_BANK         0x1006
#define MEM_ERROR_BAD_HANDLE       0x1007
#define MEM_ERROR_BAD_ADDRESS      0x1008
#define MEM_ERROR_RECOVER_FAILED   0x1009
#define MEM_ERROR_ALREADY_INITIALIZED 0x100C
#define MEM_ERROR_HANDLE_NOT_QUEUED 0x100D
#define MEM_ERROR_MONITOR_NOT_REGISTERED 0x100E

typedef struct MemoryHandle
{
  void *data;
  uint32 flags;
} MemoryHandle;

typedef struct MemoryBlockHeader
{
  uint32 size_and_flags;
  void *owner;
  struct MemoryBlockHeader *previous;
  struct MemoryBlockHeader *next;
} MemoryBlockHeader;

typedef struct MemoryHeapDescriptor MemoryHeapDescriptor;
struct MemoryHeapDescriptor
{
  uint32 memory_type;
  void *base_address;
  MemoryBlockHeader *first_free_block;
  MemoryBlockHeader *last_free_block;
  MemoryBlockHeader *largest_free_block;
  uint32 purgeable_bytes;
  uint32 secondary_purgeable_bytes;
  uint32 free_bytes;
  uint32 size;
  MemoryHeapDescriptor *next;
};

typedef struct MemoryRegionHeader MemoryRegionHeader;
struct MemoryRegionHeader
{
  MemoryRegionHeader *next;
  void *base_address;
  uint32 size;
  MemoryHeapDescriptor heap;
};

typedef struct MemoryHandleBank MemoryHandleBank;
struct MemoryHandleBank
{
  uint32 size_and_flags;
  void *owner;
  MemoryHandleBank *next;
  MemoryHandleBank *previous;
  MemoryHandle handles[1];
};

typedef struct MemoryInitTag
{
  uint32 tag;
  uint32 value;
} MemoryInitTag;

typedef char MemoryHandleSizeCheck[
    (sizeof(MemoryHandle) == 8) ? 1 : -1];
typedef char MemoryBlockHeaderSizeCheck[
    (sizeof(MemoryBlockHeader) == 0x10) ? 1 : -1];
typedef char MemoryHeapBaseAddressOffsetCheck[
    (offsetof(MemoryHeapDescriptor, base_address) == 0x04) ? 1 : -1];
typedef char MemoryHeapFirstFreeBlockOffsetCheck[
    (offsetof(MemoryHeapDescriptor, first_free_block) == 0x08) ? 1 : -1];
typedef char MemoryHeapLastFreeBlockOffsetCheck[
    (offsetof(MemoryHeapDescriptor, last_free_block) == 0x0C) ? 1 : -1];
typedef char MemoryHeapLargestBlockOffsetCheck[
    (offsetof(MemoryHeapDescriptor, largest_free_block) == 0x10) ? 1 : -1];
typedef char MemoryHeapPurgeableBytesOffsetCheck[
    (offsetof(MemoryHeapDescriptor, purgeable_bytes) == 0x14) ? 1 : -1];
typedef char MemoryHeapSecondaryPurgeableBytesOffsetCheck[
    (offsetof(MemoryHeapDescriptor, secondary_purgeable_bytes) == 0x18) ?
    1 : -1];
typedef char MemoryHeapNextOffsetCheck[
    (offsetof(MemoryHeapDescriptor, next) == 0x24) ? 1 : -1];
typedef char MemoryHeapSizeCheck[
    (sizeof(MemoryHeapDescriptor) == 0x28) ? 1 : -1];
typedef char MemoryRegionHeapOffsetCheck[
    (offsetof(MemoryRegionHeader, heap) == 0x0C) ? 1 : -1];
typedef char MemoryRegionHeaderSizeCheck[
    (sizeof(MemoryRegionHeader) == 0x34) ? 1 : -1];

MemoryBlockHeader *
merge_adjacent_free_memory_blocks(MemoryHeapDescriptor *heap,
                                  MemoryBlockHeader    *left_block,
                                  MemoryBlockHeader    *right_block);
void
coalesce_free_memory_blocks(MemoryHeapDescriptor *heap);
MemoryBlockHeader *
InsertNewFreeBlk(MemoryHeapDescriptor *heap,
                 MemoryBlockHeader    *block);
MemoryBlockHeader *
AllocPtrFromBlock(MemoryBlockHeader *free_block,
                  int                block_byte_count,
                  int                allocation_kind);
int
MoveBlkToFreeSpace(MemoryHeapDescriptor *heap,
                   MemoryBlockHeader   **allocated_block,
                   MemoryBlockHeader   **free_block,
                   int                   move_direction);
int
CompactBank(MemoryHeapDescriptor *heap,
            int                   block_kind);
void
rebuild_memory_heap_free_list(MemoryHeapDescriptor *heap);
MemoryHandle *
GetNextPurgeNode(MemoryHandle *handle);
void
SkipPurgeNode(MemoryHandle *previous,
              MemoryHandle *handle);
void
AppendPurgeList(MemoryHandle *handle);
MemoryBlockHeader *
AddHandleToFreeSpace(MemoryHandle         *handle,
                     MemoryHeapDescriptor *heap);
MemoryHandle *
ReturnHandleToFreeList(MemoryHandle *handle);
MemoryHandle *
PurgeOneHandle(uint32                 requested_memory_flags,
               MemoryHeapDescriptor **heap_ref,
               MemoryHandle          *handle,
               MemoryHandle         **previous_ref,
               int                   *purged_count,
               uint32                 allocation_options);
typedef char MemoryHandleBankNextOffsetCheck[
    (offsetof(MemoryHandleBank, next) == MEM_BANK_NEXT_OFFSET) ? 1 : -1];
typedef char MemoryHandleBankHandlesOffsetCheck[
    (offsetof(MemoryHandleBank, handles) ==
     MEM_BANK_FIRST_HANDLE_OFFSET) ? 1 : -1];

typedef struct MemoryPressureMonitor
{
  struct MemoryPressureMonitor **next_ref;
  int task_item;
  int signal_mask;
  int request_capacity;
  int request_index;
  int dram_free_bytes;
  int vram_free_bytes;
  int dram_purgeable_bytes;
  int vram_purgeable_bytes;
  int dram_secondary_purgeable_bytes;
  int vram_secondary_purgeable_bytes;
  int requested_bytes[1];
} MemoryPressureMonitor;

typedef char MemoryPressureMonitorHeaderSizeCheck[
    (sizeof(MemoryPressureMonitor) == 0x30) ? 1 : -1];

uint32
memory_request_to_memtype_flags(uint32 memory_request);
int
update_memory_pressure_monitor(void);
int
MEM_Init(const MemoryInitTag *tags);
int
MEM_Close(void);
int
CreateMoreMasters(int memory_type,
                  int handle_count);
int
get_largest_free_memory_run(uint32 memory_type);
void
append_memory_region(MemoryRegionHeader *region,
                     void               *base_address,
                     uint32              size);
MemoryBlockHeader *
link_free_memory_block(MemoryBlockHeader    *block,
                       MemoryHeapDescriptor *heap,
                       uint32                size,
                       MemoryBlockHeader    *previous,
                       MemoryBlockHeader    *next);
int
MEM_GetLargestBlockSize(uint32 memory_request);
int
MEM_GetTotalFreeSize(uint32 memory_request);
void *
MEM_LockHandle(MemoryHandle *handle);
int
MEM_UnlockHandle(MemoryHandle *handle);
int
MEM_IsHandleLocked(const MemoryHandle *handle);
void
MEM_SetHandleLocked(MemoryHandle *handle,
                    int           locked);
int
MEM_GetHandleSize(MemoryHandle *handle);
int
MEM_GetPointerSize(void *pointer);
int
MEM_GetHandleUserData(const MemoryHandle *handle);
int
MEM_SetHandleUserData(MemoryHandle *handle,
                      int           user_data);
int
MEM_ValidateAllHandles(void);
int
MEM_ValidateHandle(MemoryHandle *handle);
int
MEM_ValidatePointer(void *pointer);
int
MEM_IsValidAddress(void  *address,
                   uint32 memory_type,
                   uint32 validation_flags);
int
MEM_ValidateMemory(uint32 memory_type,
                   int   *callback);
int
MEM_ValidateMasterPtrs(int *callback);
int
ValidateBank(MemoryHeapDescriptor *heap,
             int                  *callback);
MemoryHeapDescriptor *
find_memory_bank_containing_address(uint32 address);
MemoryHandle *
get_memory_bank_handle_scan_bounds(MemoryHandleBank *bank,
                                   MemoryHandle    **first_handle);
MemoryHandle *
find_next_movable_memory_handle(MemoryHeapDescriptor *heap,
                                MemoryHandle         *cursor,
                                MemoryHandleBank    **bank_state,
                                MemoryHandle        **first_handle,
                                MemoryHandle        **last_handle,
                                int                   block_kind);

MemoryHandle *
MEM_NewHandle(int    byte_count,
              uint32 memory_type,
              uint32 options);
MemoryHandle *
NewHandle(int    byte_count,
          uint32 memtype_flags,
          uint32 options);
int
MEM_ReleaseHandle(MemoryHandle *handle);
MemoryHandle *
MEM_RecoverHandle(MemoryHandle *handle);
int
MEM_DisposeHandle(MemoryHandle *handle);
int
MEM_ResizeHandle(MemoryHandle *handle,
                 int           byte_count,
                 uint32        options);
void *
MEM_ResizePointer(void  *pointer,
                  int    byte_count,
                  uint32 options);
MemoryHeapDescriptor *
GetMaxMemBank(uint32 memory_type);
MemoryBlockHeader *
FindFreeBlockFor(MemoryHeapDescriptor *heap,
                 int                   byte_count,
                 uint32                options,
                 int                   block_kind);
MemoryBlockHeader *
FindFreeBlockForAlloc(int    byte_count,
                      uint32 memory_type,
                      uint32 options);
int
PurgeSomeBlocks(uint32                 memory_type,
                MemoryHeapDescriptor **heap_ref,
                int                    byte_count,
                int                    block_kind,
                uint32                 options);
void
PurgeAllBlksInBank(MemoryHeapDescriptor *heap);
MemoryBlockHeader *
NewBlock(int    byte_count,
         uint32 placement_flags,
         uint32 options);
int
ResizeBlock(MemoryBlockHeader *block,
            int                byte_count);
MemoryHandle *
GetNewMasterPtr(MemoryBlockHeader *block);
int
record_memory_pressure_request(int                     byte_count,
                               uint32                  unused_memtype_flags,
                               uint32                  unused_options,
                               MemoryPressureMonitor **monitor_ref);
int
register_memory_pressure_monitor(MemoryPressureMonitor **monitor_ref);
int
unregister_memory_pressure_monitor(MemoryPressureMonitor **monitor_ref);
void *
MEM_SplitBlock(void *pointer,
               int   byte_count);
MemoryHandle *
MEM_PointerToHandle(void *pointer);
int
MEM_CompactHeap(uint32 options);
int
get_memory_manager_block_header_size(void);

void *
MEM_NewPointer(int    byte_count,
               uint32 memory_type,
               uint32 options);
void *
MEM_NewPointerAsync(int                     byte_count,
                    uint32                  memory_type,
                    uint32                  options,
                    MemoryPressureMonitor **monitor_ref);
void *
NewPointer(int    byte_count,
           uint32 memtype_flags,
           uint32 options);
int
MEM_DisposePointer(void *pointer);
int
DisposePointer(void *pointer);
int
MEM_PrepHandleLock(MemoryHandle *handle);
/*
 * Portfolio's DataStreamer refers to these helpers by their SDK names.
 * Keep the semantic source names while exporting the providers expected by
 * the embedded library objects.
 */
#define create_memory_pool  CreateMemPool
#define destroy_memory_pool DeleteMemPool
MemPoolPtr
create_memory_pool(long count,
                   long entry_size);
void
destroy_memory_pool(MemPoolPtr pool);

#endif
