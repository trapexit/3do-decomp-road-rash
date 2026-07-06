#include "platform.h"

#include "rw_semantic_data.h"
#include "racer_creation_runtime.h"
#include "segment_topology_runtime.h"
#include "thread_launch.h"
#include "track_effect_runtime.h"

void
initialize_race_object_scheduler(void);
int
initialize_track_effect_pools(void);
IntrusiveListStorage *
initialize_rider_visual_pool(void);
void
initialize_track_object_runtime(void);
void
initialize_road_track_runtime(RoadTrackGraphNode *track_root,
                              int                 initial_track_position);
void
track_simulation_worker_main(void);
void
dispose_track_simulation_resources(void);
void
shutdown_track_simulation(void);

volatile int track_simulation_startup_task_item;
volatile int track_simulation_startup_signal;

int
release_scheduled_race_object_nodes(void);
int
destroy_rider_visual_pool(void);
void
destroy_race_rider_runtime(void);
void
destroy_track_object_runtime(void);
int
destroy_road_track_runtime(void);
void
destroy_race_object_scheduler(void);

enum TrackSimulationInitializationConstant
{
  TRACK_SIMULATION_INITIAL_ROAD_POSITION = 0x3200,
  TRACK_SIMULATION_INITIAL_VIEW_POSITION = 0x3100,
  TRACK_SIMULATION_INITIAL_PROJECTION_HEIGHT = 150,
  TRACK_SIMULATION_THREAD_STACK_BYTES = 0x0C00,
  TRACK_SIMULATION_THREAD_PRIORITY = 0x33
};

int
initialize_track_simulation(SegmentTopologyNode *session)
{
  int launched;
  int startup_signal;
  int received_signals;
  track_simulation_globals.update_signal = 0;
  track_simulation_globals.completion_signal = 0;
  track_simulation_globals.worker_running = 0;
  track_simulation_globals.worker_task_item = 0;

  track_simulation_globals.motion_source.source_position_y =
    TRACK_SIMULATION_INITIAL_ROAD_POSITION;
  track_simulation_globals.motion_source.source_position_z = 0;
  track_simulation_globals.motion_source.projection_origin_y =
    TRACK_SIMULATION_INITIAL_PROJECTION_HEIGHT;
  track_simulation_globals.motion_source.projection_origin_x = 0;
  track_simulation_globals.motion_source.projection_origin_z = 0;
  track_simulation_globals.motion_source.packed_track_position =
    TRACK_SIMULATION_INITIAL_VIEW_POSITION;
  track_simulation_globals.motion_source.road_scroll_phase = 0;
  track_simulation_globals.motion_source.camera_velocity = 0;
  track_simulation_globals.motion_source.camera_acceleration = 0;
  track_simulation_globals.motion_source.periodic_event_flags.value = 0;

  initialize_race_object_scheduler();
  if(race_object_scheduler.object_list == 0)
    {
      goto fail;
    }
  if(initialize_track_effect_pools() == 0 ||
     initialize_rider_visual_pool() == 0)
    {
      goto fail;
    }
  initialize_track_object_runtime();
  if(runtime_feature_flags.track_objects_enabled.value != 0 &&
     track_simulation_globals.active_object == 0)
    {
      goto fail;
    }
  initialize_road_track_runtime(
    (RoadTrackGraphNode *)session,
    track_simulation_globals.motion_source.source_position_y);
  if(track_runtime_globals.initialized == 0)
    {
      goto fail;
    }
  if(!initialize_race_rider_runtime())
    {
      goto fail;
    }

  track_simulation_globals.applied_track_position =
    TRACK_SIMULATION_INITIAL_VIEW_POSITION;
  startup_signal = AllocSignal(0);
  if(startup_signal <= 0)
    {
      goto fail;
    }
  track_simulation_startup_task_item = CURRENTTASK->t.n_Item;
  track_simulation_startup_signal = startup_signal;
  /* The worker publishes 1 only after both task-local signals exist. */
  track_simulation_globals.worker_running = 2;
  launched = launch_named_thread(
    track_simulation_worker_main,
    TRACK_SIMULATION_THREAD_STACK_BYTES,
    TRACK_SIMULATION_THREAD_PRIORITY, "Sim");
  track_simulation_globals.worker_task_item = launched;
  if(launched <= 0)
    {
      track_simulation_startup_signal = 0;
      track_simulation_startup_task_item = 0;
      track_simulation_globals.worker_running = 0;
      FreeSignal(startup_signal);
      goto fail;
    }
  received_signals = WaitSignal(startup_signal);
  track_simulation_startup_signal = 0;
  track_simulation_startup_task_item = 0;
  FreeSignal(startup_signal);
  if(received_signals < 0 ||
     (received_signals & startup_signal) == 0 ||
     track_simulation_globals.worker_running != 1)
    {
      shutdown_track_simulation();
      return 0;
    }
  return 1;

fail:
  dispose_track_simulation_resources();
  return 0;
}


void
dispose_track_simulation_resources(void)
{
  release_scheduled_race_object_nodes();
  dispose_track_effect_pools();
  destroy_rider_visual_pool();
  destroy_race_rider_runtime();
  destroy_track_object_runtime();
  destroy_road_track_runtime();
  destroy_race_object_scheduler();
}
