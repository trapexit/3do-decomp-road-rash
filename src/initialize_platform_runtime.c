#include "event.h"
#include "graphics.h"
#include "operamath.h"
#include "io.h"

#include "application_runtime.h"
#include "display_system_runtime.h"
#include "memory_manager_runtime.h"
#include "platform.h"
#include "rw_semantic_data.h"

static int platform_math_open;
static int platform_events_open;
static int platform_initialized;

int dispose_display_screens(void);

int
shutdown_platform_runtime(void)
{
  int result;

  platform_initialized = 0;
  if(platform_events_open)
    {
      result = KillEventUtility();
      if(result < 0)
        {
          return result;
        }
      platform_events_open = 0;
    }
  if(display_system_globals.vbl_io_request > 0)
    {
      result = DeleteVBLIOReq(display_system_globals.vbl_io_request);
      if(result < 0)
        {
          return result;
        }
      display_system_globals.vbl_io_request = 0;
    }
  if(display_system_globals.vram_io_request > 0)
    {
      result = DeleteVRAMIOReq(display_system_globals.vram_io_request);
      if(result < 0)
        {
          return result;
        }
      display_system_globals.vram_io_request = 0;
    }
  if(platform_math_open)
    {
      result = CloseMathFolio();
      if(result < 0)
        {
          return result;
        }
      platform_math_open = 0;
    }
  return dispose_display_screens();
}

int
initialize_platform_runtime(const MemoryInitTag *memory_tags)
{
  int result;

  if(platform_initialized)
    {
      return 0;
    }
  result = shutdown_platform_runtime();
  if(result < 0)
    {
      show_application_error("shutdown_platform_runtime", result, 0);
      return result;
    }
  result = MEM_Init(memory_tags);
  if(result != 0)
    {
      show_application_error("MEM_Init", result, 0);
      return result;
    }
  result = initialize_display_screens(2, display_system_globals.screen_items);
  if(result < 0)
    {
      show_application_error("initialize_display_screens", result, 0);
      return result;
    }
  result = initialize_vram_io_request();
  if(result < 0)
    {
      show_application_error("initialize_vram_io_request", result, 0);
      return result;
    }
  result = OpenMathFolio();
  if(result < 0)
    {
      show_application_error("OpenMathFolio", result, 0);
      return result;
    }
  platform_math_open = 1;
  result = InitEventUtility(2, 0, LC_ISFOCUSED);
  if(result < 0)
    {
      show_application_error("InitEventUtility", result, 0);
      return result;
    }
  platform_events_open = 1;
  result = GetVBLIOReq();
  display_system_globals.vbl_io_request = result;
  if(result < 0)
    {
      show_application_error("GetVBLIOReq", result, 0);
      return result;
    }
  platform_initialized = 1;
  return 0;
}
