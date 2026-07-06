
#include "graphics.h"

#include "application_runtime.h"
#include "display_system_runtime.h"
#include "memory_manager_runtime.h"
#include "rw_semantic_data.h"

int
get_display_bitmap(int screen_index);
int
get_display_bitmap_item(int screen_index);
int
configure_display_clipping(int width,
                           int height);
void
select_display_bitmap(int buffer_index);
int
allocate_solid_fill_cel(CCB **cel_output);
int *
set_solid_fill_cel_color(int *cel,
                         int  red,
                         int  green,
                         int  blue);
int
allocate_display_packet_buffers(int packet_count);
void
initialize_cel_dimension_preamble_table(void);
void
initialize_cel_shading_table(void);
int
queue_fullscreen_clear(void);
void
present_display_frame(int wait_for_vbl);

enum DisplayRuntimeInitializationConstant
{
  DISPLAY_RUNTIME_BUFFER_COUNT = 2,
  DISPLAY_RUNTIME_CE_CONTROL = 0x04000000,
  DISPLAY_RUNTIME_CLIP_WIDTH = 0x140,
  DISPLAY_RUNTIME_CLIP_HEIGHT = 0xF0,
  DISPLAY_RUNTIME_PACKET_COUNT = 0xFA,
  DISPLAY_RUNTIME_COLOR_MAX = 0x1F,
  DISPLAY_RUNTIME_COLOR_HALF = 0x0F,
  DISPLAY_RUNTIME_COLOR_BLUE = 0x14
};

int
dispose_display_runtime(void)
{
  int result;

  if(display_runtime_globals.vbl_io_request > 0)
    {
      result = DeleteVBLIOReq(display_runtime_globals.vbl_io_request);
      if(result < 0)
        {
          return result;
        }
      display_runtime_globals.vbl_io_request = 0;
    }
  if(display_runtime_globals.solid_fill_ccb != 0)
    {
      MEM_DisposePointer(display_runtime_globals.solid_fill_ccb);
      display_runtime_globals.solid_fill_ccb = 0;
    }
  if(display_runtime_globals.fallback_ccb != 0)
    {
      MEM_DisposePointer(display_runtime_globals.fallback_ccb);
      display_runtime_globals.fallback_ccb = 0;
    }
  if(display_runtime_globals.cel_packet_base != 0)
    {
      MEM_DisposePointer(display_runtime_globals.cel_packet_base);
      display_runtime_globals.cel_packet_base = 0;
    }
  if(display_runtime_globals.solid_pixel_base != 0)
    {
      MEM_DisposePointer(display_runtime_globals.solid_pixel_base);
      display_runtime_globals.solid_pixel_base = 0;
    }
  display_runtime_globals.cel_packet_cursor = 0;
  display_runtime_globals.cel_packet_limit = 0;
  display_runtime_globals.cel_packet_capacity = 0;
  display_runtime_globals.solid_pixel_cursor = 0;
  display_runtime_globals.solid_pixel_limit = 0;
  display_runtime_globals.solid_pixel_capacity = 0;
  display_runtime_globals.initialized = 0;
  return 0;
}

int
initialize_display_runtime(void)
{
  int index;
  int value;

  if(display_runtime_globals.initialized)
    {
      return 1;
    }
  if(dispose_display_runtime() < 0)
    {
      return 0;
    }

  index = 0;
  do
    {
      display_runtime_globals.bitmaps[index] =
        (Bitmap *)get_display_bitmap(index);
      if(display_runtime_globals.bitmaps[index] == 0)
        {
          return 0;
        }
      value = get_display_bitmap_item(index);
      if(value <= 0)
        {
          return 0;
        }
      display_runtime_globals.bitmap_items[index] = value;
      display_runtime_globals.saved_bitmap_items[index] = value;
      value = get_display_screen_item(index);
      if(value <= 0)
        {
          return 0;
        }
      display_runtime_globals.screen_items[index] = value;
      display_runtime_globals.saved_screen_items[index] = value;
      index++;
    } while(index < DISPLAY_RUNTIME_BUFFER_COUNT);

  display_runtime_globals.display_buffer_index = 0;
  display_runtime_globals.draw_buffer_index = 1;
  display_runtime_globals.vbl_io_request = GetVBLIOReq();
  if(display_runtime_globals.vbl_io_request < 0)
    {
      goto fail;
    }

  if(SetCEControl(display_runtime_globals.bitmap_items[0],
                  DISPLAY_RUNTIME_CE_CONTROL,
                  DISPLAY_RUNTIME_CE_CONTROL) < 0 ||
     SetCEControl(display_runtime_globals.bitmap_items[1],
                  DISPLAY_RUNTIME_CE_CONTROL,
                  DISPLAY_RUNTIME_CE_CONTROL) < 0 ||
     configure_display_clipping(DISPLAY_RUNTIME_CLIP_WIDTH,
                                DISPLAY_RUNTIME_CLIP_HEIGHT) < 0)
    {
      goto fail;
    }
  select_display_bitmap(0);

  if(allocate_solid_fill_cel(
       &display_runtime_globals.solid_fill_ccb) != 0)
    {
      goto fail;
    }
  else
    {
      set_solid_fill_cel_color(
        (int *)display_runtime_globals.solid_fill_ccb,
        0, DISPLAY_RUNTIME_COLOR_MAX, 0);
    }
  if(allocate_solid_fill_cel(
       &display_runtime_globals.fallback_ccb) != 0)
    {
      goto fail;
    }
  else
    {
      set_solid_fill_cel_color(
        (int *)display_runtime_globals.fallback_ccb,
        DISPLAY_RUNTIME_COLOR_HALF, DISPLAY_RUNTIME_COLOR_HALF,
        DISPLAY_RUNTIME_COLOR_BLUE);
    }

  if(display_runtime_globals.initialized == 0)
    {
      if(!allocate_display_packet_buffers(DISPLAY_RUNTIME_PACKET_COUNT))
        {
          goto fail;
        }
      initialize_cel_dimension_preamble_table();
      initialize_cel_shading_table();
    }
  display_runtime_globals.initialized = 1;

  index = 0;
  do
    {
      queue_fullscreen_clear();
      present_display_frame(1);
      index++;
    } while(index < DISPLAY_RUNTIME_BUFFER_COUNT);
  return 1;
fail:
  dispose_display_runtime();
  return 0;
}

