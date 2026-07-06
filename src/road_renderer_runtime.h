#ifndef ROADRASH_ROAD_RENDERER_RUNTIME_H
#define ROADRASH_ROAD_RENDERER_RUNTIME_H

extern volatile int road_renderer_worker_shutdown_requested;

int
initialize_road_renderer_worker(void);
int
stop_road_renderer_worker(void);
int
signal_road_renderer_worker(void);
int
wait_for_road_renderer_signal(void);
int
release_road_renderer_worker_resources(void);
int
lookup_road_renderer_bit_length(int value);
void
initialize_road_renderer_projection_state(void);
void
road_renderer_worker_main(void);
void
process_road_renderer_frame(void);

int
load_road_reciprocal_table(void);
int
initialize_road_reciprocal_table(void);
int
generate_road_reciprocal_table(int phase_step);

#endif
