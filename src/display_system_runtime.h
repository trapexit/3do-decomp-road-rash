#ifndef ROADRASH_DISPLAY_SYSTEM_RUNTIME_H
#define ROADRASH_DISPLAY_SYSTEM_RUNTIME_H

#include "item.h"

struct MemoryInitTag;

int
initialize_display_screens(int   requested_screen_count,
                           Item *screen_items);
int
dispose_display_screens(void);
void
fill_active_display_bitmap(unsigned int color);
int
initialize_vram_io_request(void);
int
initialize_platform_runtime(const struct MemoryInitTag *memory_tags);
int
shutdown_platform_runtime(void);
int
get_display_screen_item(int screen_index);
int
fade_display_screens_to_black(int frame_count);
int
copy_display_bitmap(int destination_index,
                    int source_index);

#endif
