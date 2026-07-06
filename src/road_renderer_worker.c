#include "task.h"

#include "bss_early_data.h"
#include "display_cel_runtime.h"
#include "division_helpers.h"
#include "family_resource_runtime.h"
#include "memory_manager_runtime.h"
#include "racer_auxiliary_cache.h"
#include "road_renderer_runtime.h"
#include "roadside_object_rendering.h"
#include "rw_semantic_data.h"
#include "thread_launch.h"

enum RoadRendererWorkerConstant
{
  ROAD_RENDERER_THREAD_STACK_BYTES = 0x0C00,
  ROAD_RENDERER_THREAD_PRIORITY = 0x32,
  ROAD_RENDERER_SOURCE_LAYER_INDEX = 1,
  ROAD_RENDERER_BACK_LAYER_INDEX = 3,
  ROAD_SURFACE_VARIANT_ROW_COUNT = 3,
  ROAD_SURFACE_VARIANT_SOURCE_COLUMN = 0,
  ROAD_SURFACE_VARIANT_MIRROR_COLUMN = 1,
  ROAD_RENDERER_PROJECTION_DEPTH_COUNT = 0x20,
  ROAD_RECIPROCAL_PROJECTION_BASE_INDEX = 0x100,
  ROAD_RECIPROCAL_PROJECTION_INDEX_STRIDE = 0x80,
  ROAD_RENDERER_FIXED_POINT_SHIFT = 16,
  ROAD_SCROLL_DECAY_SHIFT = 2,
  ROAD_SCROLL_DECAY_BIAS = 3,
  ROAD_RENDERER_BIT_LENGTH_WORD_SHIFT = 16,
  ROAD_RENDERER_BIT_LENGTH_BYTE_SHIFT = 8,
  ROAD_RENDERER_BIT_LENGTH_WORD_ADDEND = 0x10,
  ROAD_RENDERER_BIT_LENGTH_BYTE_ADDEND = 8,
  ROAD_OVERLAY_DURATION_TICKS = 0x3C,
  ROAD_OVERLAY_END_TICK = 0xB3,
  ROAD_OVERLAY_LAST_RESOURCE_INDEX = 2,
  ROAD_OVERLAY_PROGRESS_X = 0x80,
  ROAD_OVERLAY_PROGRESS_Y = 0x50,
  ROAD_OVERLAY_CHORD_X = 0x5B,
  ROAD_OVERLAY_CHORD_Y = 0x35
};

enum RoadRendererWorkerStartupStatus
{
  ROAD_RENDERER_WORKER_NOT_READY = 0,
  ROAD_RENDERER_WORKER_STARTING,
  ROAD_RENDERER_WORKER_READY,
  ROAD_RENDERER_WORKER_FAILED
};

#define TRACK_SIMULATION_RENDER_CONTROL_MASK 0x00600000U

void
select_display_bitmap(int buffer_index);
int
request_track_simulation_completion(unsigned int control_bits,
                                    unsigned int render_control);
void
render_road_frame(void);

static char road_renderer_thread_name[] = "Renderer";
volatile int road_renderer_worker_shutdown_requested;
static volatile int road_renderer_worker_startup_status;
static volatile int road_renderer_worker_startup_signal;
static volatile int road_renderer_worker_startup_task_item;

static
void
notify_road_renderer_worker_parent(void)
{
  int startup_signal;
  int startup_task_item;

  startup_signal = road_renderer_worker_startup_signal;
  startup_task_item = road_renderer_worker_startup_task_item;
  if(startup_task_item > 0 && startup_signal != 0)
    {
      SendSignal(startup_task_item, startup_signal);
    }
}


int
initialize_road_renderer_worker(void)
{
  int row;
  int cache_index;
  int received_signals;
  int result;
  int startup_signal;

  select_display_bitmap(0);
  road_renderer_state.frame_count = 0;
  road_renderer_state.wake_signal = 0;
  road_renderer_state.worker_running = 0;
  road_renderer_state.worker_task_item = 0;
  road_renderer_worker_shutdown_requested = 0;
  road_renderer_worker_startup_status = ROAD_RENDERER_WORKER_NOT_READY;
  if(road_renderer_state.reciprocal_table == 0)
    {
      initialize_road_reciprocal_table();
    }
  if(road_renderer_state.reciprocal_table == 0)
    {
      road_renderer_worker_startup_status = ROAD_RENDERER_WORKER_FAILED;
      return 0;
    }

  road_renderer_state.road_layers[ROAD_RENDERER_BACK_LAYER_INDEX] =
    road_renderer_state.road_layers[ROAD_RENDERER_SOURCE_LAYER_INDEX];
  for(row = 0; row < ROAD_SURFACE_VARIANT_ROW_COUNT; row++)
    {
      road_surface_cel_variants[row][ROAD_SURFACE_VARIANT_MIRROR_COLUMN] =
        road_surface_cel_variants[row][ROAD_SURFACE_VARIANT_SOURCE_COLUMN];
    }
  for(cache_index = 0;
      cache_index < ROAD_TEXTURE_CACHE_COUNT;
      cache_index++)
    {
      gRoadTextureCachePool[cache_index].resource_key = 0;
    }

  startup_signal = AllocSignal(0);
  if(startup_signal <= 0)
    {
      road_renderer_worker_startup_status = ROAD_RENDERER_WORKER_FAILED;
      stop_road_renderer_worker();
      return 0;
    }
  road_renderer_worker_startup_task_item = CURRENTTASK->t.n_Item;
  road_renderer_worker_startup_signal = startup_signal;
  road_renderer_worker_startup_status = ROAD_RENDERER_WORKER_STARTING;
  result = launch_named_thread(
    road_renderer_worker_main, ROAD_RENDERER_THREAD_STACK_BYTES,
    ROAD_RENDERER_THREAD_PRIORITY, road_renderer_thread_name);
  road_renderer_state.worker_task_item = result;
  if(result <= 0)
    {
      road_renderer_worker_startup_signal = 0;
      road_renderer_worker_startup_task_item = 0;
      FreeSignal(startup_signal);
      road_renderer_worker_startup_status = ROAD_RENDERER_WORKER_FAILED;
      stop_road_renderer_worker();
      return 0;
    }

  received_signals = WaitSignal(startup_signal);
  road_renderer_worker_startup_signal = 0;
  road_renderer_worker_startup_task_item = 0;
  FreeSignal(startup_signal);
  if(received_signals < 0 ||
     (received_signals & startup_signal) == 0 ||
     road_renderer_worker_startup_status != ROAD_RENDERER_WORKER_READY)
    {
      stop_road_renderer_worker();
      return 0;
    }
  return 1;
}


int
wait_for_road_renderer_signal(void)
{
  int signal;

  do
    {
      signal = WaitSignal(road_renderer_state.wake_signal);
      if(signal < 0 || (signal & SIGF_ABORT) != 0)
        {
          road_renderer_state.worker_running = 0;
          return signal;
        }
    } while((signal & road_renderer_state.wake_signal) == 0);
  return signal;
}


int
release_road_renderer_worker_resources(void)
{
  int result;
  int wake_signal;

  road_renderer_state.road_layers[ROAD_RENDERER_BACK_LAYER_INDEX] = 0;
  wake_signal = road_renderer_state.wake_signal;
  road_renderer_state.wake_signal = 0;
  result = FreeSignal(wake_signal);
  return result;
}


int
lookup_road_renderer_bit_length(int value)
{
  int addend;

  addend = 0;
  if(value < 0)
    {
      value = -value;
    }
  if(value >> ROAD_RENDERER_BIT_LENGTH_WORD_SHIFT != 0)
    {
      value >>= ROAD_RENDERER_BIT_LENGTH_WORD_SHIFT;
      addend = ROAD_RENDERER_BIT_LENGTH_WORD_ADDEND;
    }
  if(value >> ROAD_RENDERER_BIT_LENGTH_BYTE_SHIFT != 0)
    {
      value >>= ROAD_RENDERER_BIT_LENGTH_BYTE_SHIFT;
      addend += ROAD_RENDERER_BIT_LENGTH_BYTE_ADDEND;
    }

  return road_renderer_state.bit_length[value] + addend;
}


void
initialize_road_renderer_projection_state(void)
{
  RoadRenderNode *node;
  SignedDivisionResult division;
  int index;
  int row;
  int column;

  for(index = 0;
      index < ROAD_RENDERER_PROJECTION_DEPTH_COUNT;
      index++)
    {
      division = signed_divide_with_remainder(
        (unsigned int)road_renderer_state.reciprocal_table[
          ROAD_RECIPROCAL_PROJECTION_BASE_INDEX +
          index * ROAD_RECIPROCAL_PROJECTION_INDEX_STRIDE],
        (unsigned int)road_depth_input_distances[index] <<
          ROAD_RENDERER_FIXED_POINT_SHIFT);
      gRoadRenderDepths[index] = division.quotient;
    }
  gRoadRenderDepths[ROAD_RENDERER_PROJECTION_DEPTH_COUNT] =
    gRoadRenderDepths[ROAD_RENDERER_PROJECTION_DEPTH_COUNT - 1];
  gRoadRenderDepths[ROAD_RENDERER_PROJECTION_DEPTH_COUNT + 1] =
    gRoadRenderDepths[ROAD_RENDERER_PROJECTION_DEPTH_COUNT - 1];

  node = gRoadRenderNodes;
  for(index = 0; index < ROAD_RENDER_NODE_COUNT; index++)
    {
      node->left_side = &node->sides[0];
      node->right_side = &node->sides[1];
      node->sides[0].owner_node = node;
      node->sides[1].owner_node = node;
      node++;
    }

  for(row = 0; row < ROAD_SURFACE_SOURCE_ROWS; row++)
    {
      for(column = 0; column < ROAD_SOURCE_SIDE_COUNT; column++)
        {
          gRoadTextureCacheTemplate.surface_sources[row][column] = 0;
        }
    }
  for(row = 0; row < ROAD_EDGE_SOURCE_ROWS; row++)
    {
      for(column = 0; column < ROAD_SOURCE_SIDE_COUNT; column++)
        {
          gRoadTextureCacheTemplate.edge_sources[row][column] = 0;
        }
    }
  for(row = 0; row < ROAD_JOIN_SOURCE_ROWS; row++)
    {
      for(column = 0; column < ROAD_SOURCE_SIDE_COUNT; column++)
        {
          gRoadTextureCacheTemplate.join_sources[row][column] = 0;
        }
    }
  for(row = 0; row < ROAD_PROCEDURAL_SOURCE_ROWS; row++)
    {
      for(column = 0;
          column < ROAD_PROCEDURAL_SOURCE_COLUMNS;
          column++)
        {
          gRoadTextureCacheTemplate.procedural_sources[row][column] = 0;
        }
    }
  gRoadTextureCacheTemplate.reserved_source = 0;
  gRoadTextureCacheTemplate.repeated_object_animation = 0;
  for(index = 0;
      index < ROAD_OBJECT_CEL_SCALE_BUCKET_COUNT;
      index++)
    {
      gRoadTextureCacheTemplate.repeated_object_frames[index].ccb = 0;
    }
  for(row = 0; row < ROAD_STATIC_OBJECT_CACHE_COUNT; row++)
    {
      gRoadTextureCacheTemplate.static_object_animations[row] = 0;
      for(column = 0;
          column < ROAD_OBJECT_CEL_SCALE_BUCKET_COUNT;
          column++)
        {
          gRoadTextureCacheTemplate.static_object_frames[row][column].ccb =
            0;
        }
    }
  for(index = 0; index < ROAD_STATIC_OBJECT_CACHE_COUNT; index++)
    {
      gRoadTextureCacheTemplate.racer_auxiliary_animations[index] = 0;
    }
}


void
road_renderer_worker_main(void)
{
  int signal;

  signal = AllocSignal(0);
  if(signal <= 0)
    {
      road_renderer_state.wake_signal = 0;
      road_renderer_state.worker_running = 0;
      road_renderer_worker_startup_status = ROAD_RENDERER_WORKER_FAILED;
      notify_road_renderer_worker_parent();
      return;
    }

  road_renderer_state.wake_signal = signal;
  if(road_renderer_worker_shutdown_requested != 0)
    {
      release_road_renderer_worker_resources();
      road_renderer_worker_startup_status = ROAD_RENDERER_WORKER_NOT_READY;
      notify_road_renderer_worker_parent();
      return;
    }

  if(open_display_item_handles() == 0)
    {
      release_road_renderer_worker_resources();
      road_renderer_worker_startup_status = ROAD_RENDERER_WORKER_FAILED;
      notify_road_renderer_worker_parent();
      return;
    }
  initialize_road_renderer_projection_state();
  if(road_renderer_worker_shutdown_requested != 0)
    {
      release_road_renderer_worker_resources();
      close_display_item_handles();
      road_renderer_worker_startup_status = ROAD_RENDERER_WORKER_NOT_READY;
      notify_road_renderer_worker_parent();
      return;
    }
  road_renderer_state.worker_running = 1;
  road_renderer_worker_startup_status = ROAD_RENDERER_WORKER_READY;
  notify_road_renderer_worker_parent();

  while(road_renderer_state.worker_running != 0)
    {
      wait_for_road_renderer_signal();
      process_road_renderer_frame();
    }

  release_road_renderer_worker_resources();
  close_display_item_handles();
  road_renderer_worker_startup_status = ROAD_RENDERER_WORKER_NOT_READY;
}


void
process_road_renderer_frame(void)
{
  SignedDivisionResult division;
  unsigned int control_bits;
  int phase;
  int reduced_phase;

  if(road_renderer_state.worker_running == 0)
    {
      return;
    }

  control_bits = track_simulation_globals.control_bits;
  if(request_track_simulation_completion(
       control_bits,
       control_bits & TRACK_SIMULATION_RENDER_CONTROL_MASK) <= 0)
    {
      return;
    }
  wait_for_road_renderer_signal();
  if(road_renderer_state.worker_running == 0)
    {
      return;
    }

  phase = road_renderer_state.motion.road_scroll_phase;
  if(phase < 0)
    {
      phase += ROAD_SCROLL_DECAY_BIAS;
    }
  phase >>= ROAD_SCROLL_DECAY_SHIFT;
  reduced_phase = phase;
  if(reduced_phase < 0)
    {
      reduced_phase += ROAD_SCROLL_DECAY_BIAS;
    }
  road_renderer_state.motion.road_scroll_phase =
    phase + (reduced_phase >> ROAD_SCROLL_DECAY_SHIFT);

  if(acquire_family_resource_render_barrier() == 0)
    {
      return;
    }
  render_road_frame();

  if(input_thread_state.chord_processing_active == 0)
    {
      if(input_thread_state.overlay_progress != 0)
        {
          division = signed_divide_with_remainder(
            ROAD_OVERLAY_DURATION_TICKS,
            ROAD_OVERLAY_END_TICK -
            input_thread_state.overlay_progress);
          map_cel_at_native_scale(
            append_unmapped_cel((const CCB *)
                                road_renderer_state.overlay_resource_items[
                                  ROAD_OVERLAY_LAST_RESOURCE_INDEX -
                                  division.quotient]),
            ROAD_OVERLAY_PROGRESS_X, ROAD_OVERLAY_PROGRESS_Y);
        }
    }
  else
    {
      map_cel_at_native_scale(
        append_unmapped_cel((const CCB *)
                            input_thread_state.overlay_resource_item),
        ROAD_OVERLAY_CHORD_X, ROAD_OVERLAY_CHORD_Y);
    }

  submit_cel_batch();
  release_family_resource_render_barrier();
  present_display_frame(1);
  road_renderer_state.frame_count++;
}


#undef TRACK_SIMULATION_RENDER_CONTROL_MASK
