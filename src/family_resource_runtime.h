#ifndef FAMILY_RESOURCE_RUNTIME_H
#define FAMILY_RESOURCE_RUNTIME_H

#define FAMILY_RESOURCE_SELECTOR_GROUP_SHIFT 6
#define FAMILY_RESOURCE_SELECTOR_GROUP_MASK  3
#define FAMILY_RESOURCE_SELECTOR_ENTRY_MASK  0x3F

#define FAMILY_RESOURCE_LOCK_FREE      0
#define FAMILY_RESOURCE_LOCK_ACQUIRED  1

#define FAMILY_RESOURCE_PREFETCH_MODE        1
#define FAMILY_RESOURCE_LOAD_AND_FIXUP_MODE  3
#define FAMILY_RESOURCE_ID_MASK              0xFFFFU
#define FAMILY_RESOURCE_TYPE_TAG             0x46414D20U
#define FAMILY_RESOURCE_SYNC_SEEK_FAILED     2
#define FAMILY_RESOURCE_TABLE_KIB_SHIFT      10
#define FAMILY_RESOURCE_PATH_BYTES           52
#define FAMILY_RESOURCE_FILE_LOAD_FLAGS      0x800
#define FAMILY_RESOURCE_CURVED_ROAD_CATEGORY 3
#define FAMILY_RESOURCE_CLGP_TAG              0x434C4750U
#define FAMILY_RESOURCE_CHUNK_HEADER_BYTES    8

enum FamilyResourceWorkerStartupStatus
{
  FAMILY_RESOURCE_WORKER_NOT_READY = 0,
  FAMILY_RESOURCE_WORKER_STARTING = 1,
  FAMILY_RESOURCE_WORKER_READY = 2,
  FAMILY_RESOURCE_WORKER_FAILED = -1
};
enum FamilyResourceLoadFailure
{
  FAMILY_RESOURCE_LOAD_FAILURE_NONE = 0,
  FAMILY_RESOURCE_LOAD_FAILURE_SEEK,
  FAMILY_RESOURCE_LOAD_FAILURE_NOT_FOUND,
  FAMILY_RESOURCE_LOAD_FAILURE_SIZE,
  FAMILY_RESOURCE_LOAD_FAILURE_STUFF
};


typedef int FamilyResourceSelector;
typedef int FamilyResourceLock;
typedef int FamilyResourceValue;
typedef FamilyResourceValue *FamilyResourceLevel;
typedef void (*FamilyResourceEntryVisitor)(FamilyResourceValue value,
                                           void               *context);

typedef struct FamilyResourceWorkerRequest
{
  int reserved_00;
  int family_id;
  int group_index;
  int mode;
} FamilyResourceWorkerRequest;

struct FamilyResourceRequest;
struct RoadFamilyScheduleResource;

/* Reconstruction-only epochs and state for reusable family resources. */
extern unsigned int family_resource_generations[];
extern unsigned int family_resource_loaded_bytes[];
extern int family_resource_failure_codes[];
extern int family_resource_failure_values[];
extern volatile int family_resource_worker_request_pending;
extern int family_resource_request_count;
extern volatile int family_resource_worker_startup_status;
extern volatile int family_resource_worker_startup_signal;
extern volatile int family_resource_worker_startup_task_item;
extern volatile int family_resource_worker_shutdown_requested;

typedef struct FamilyResourceTable
{
  int entry_count;
  FamilyResourceLevel entries[1];
} FamilyResourceTable;

#define FAMILY_RESOURCE_GROUP_INDEX(selector) \
        (((selector) >> FAMILY_RESOURCE_SELECTOR_GROUP_SHIFT) & \
         FAMILY_RESOURCE_SELECTOR_GROUP_MASK)
#define FAMILY_RESOURCE_ENTRY_INDEX(selector) \
        ((selector) & FAMILY_RESOURCE_SELECTOR_ENTRY_MASK)
#define FAMILY_RESOURCE_LEVEL_COUNT(level) ((level)[-1])
#define FAMILY_RESOURCE_CHUNK_TAG(value) \
        (*(unsigned int *)((char *)(value) - FAMILY_RESOURCE_CHUNK_HEADER_BYTES))

int
try_acquire_family_resource_lock(FamilyResourceLock *lock,
                                 int                 requested_state);
int
release_family_resource_lock(FamilyResourceLock *lock);
int
initialize_family_resource_render_barrier(void);
int
dispose_family_resource_render_barrier(void);
int
try_acquire_family_resource_render_barrier(void);
int
acquire_family_resource_render_barrier(void);
void
release_family_resource_render_barrier(void);
int
family_resource_worker_is_ready(void);
void
mark_family_resource_group_unavailable(int group_index,
                                       int family_id);
int
initialize_family_resource_groups(struct RoadFamilyScheduleResource *schedule_resource);
int
initialize_family_resource_runtime(struct RoadFamilyScheduleResource *schedule_resource);
int
shutdown_family_resource_runtime(void);

void *
load_family_resource_chunk_into_group(int   family_id,
                                      void *group_destination);
int
seek_family_resource(const FamilyResourceWorkerRequest *request);
void *
relocate_family_resource_value_tree(void *value);
void *
load_and_fixup_family_resource_group(const FamilyResourceWorkerRequest *request);
int
submit_family_resource_worker_request(const struct FamilyResourceRequest *request);
int
load_family_resource_synchronously(int family_id,
                                   int group_index);
int
enqueue_family_resource_request(int family_id,
                                int group_index,
                                int mode);
void
dispatch_pending_family_resource_request(void);

int
visit_family_resource_entry(FamilyResourceSelector    selector,
                            int                       family_index,
                            FamilyResourceEntryVisitor visitor,
                            void                      *context);
FamilyResourceValue
lookup_family_resource_child(FamilyResourceSelector selector,
                             int                    family_index,
                             int                    child_index);
FamilyResourceValue
lookup_family_resource_grandchild(FamilyResourceSelector selector,
                                  int                    family_index,
                                  int                    child_index,
                                  int                    grandchild_index);
FamilyResourceValue
lookup_family_resource_descendant(FamilyResourceSelector selector,
                                  int                    family_index,
                                  int                    child_index,
                                  int                    grandchild_index,
                                  int                    descendant_index);

/*
 * Render-only lookups do not take a group reservation.  The caller must hold
 * the family-resource render barrier for the complete lookup and borrowed
 * pointer lifetime.  A loader can reserve the group before waiting for that
 * barrier, but it cannot replace the published table until the caller exits.
 */
FamilyResourceValue
lookup_family_resource_entry_for_render(FamilyResourceSelector selector,
                                        int                    family_index);
FamilyResourceValue
lookup_family_resource_child_for_render(FamilyResourceSelector selector,
                                        int                    family_index,
                                        int                    child_index);
FamilyResourceValue
lookup_family_resource_grandchild_for_render(FamilyResourceSelector selector,
                                             int                    family_index,
                                             int                    child_index,
                                             int                    grandchild_index);
FamilyResourceValue
lookup_family_resource_descendant_for_render(FamilyResourceSelector selector,
                                             int                    family_index,
                                             int                    child_index,
                                             int                    grandchild_index,
                                             int                    descendant_index);

#endif
