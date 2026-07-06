#ifndef ROADRASH_APPLICATION_RUNTIME_H
#define ROADRASH_APPLICATION_RUNTIME_H

#include "async_stream_runtime.h"
#include "segment_topology_runtime.h"

int
initialize_application_runtime(void);
int
shutdown_application_runtime(void);
/* Fatal diagnostic: retain the error screen until the console is reset. */
void
show_application_error(const char *stage, int status, const char *detail);
void
abort_application_with_resource_error(int status);
int
wait_for_control_pad_press(void);
int
configure_music_option_for_available_memory(void);
void
update_music_option_for_memory_pressure(void);
int
initialize_async_stream_message_port(const RwTagArgument *tag_overrides);

void
load_rash_options_resources(void);
void
unload_rash_options_resources(void);
SegmentTopologyNode *
load_course_resources(void);
void
unload_course_resources(void);
void
load_road_particle_resources(void);
void
clear_road_particle_resources(void);

int
initialize_display_runtime(void);
int
dispose_display_runtime(void);
int
configure_display_clipping(int width,
                           int height);
void
initialize_rider_progression_profiles(void);
int
run_front_end_menu_loop(void);

int
initialize_track_simulation(SegmentTopologyNode *session);
void
shutdown_track_simulation(void);

int
load_score(void);
int
start_score(void);
int
stop_score(void);
int
unload_score(void);

#endif
