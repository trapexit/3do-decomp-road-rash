#include "graphics.h"
#include "mem.h"

#include "display_system_runtime.h"
#include "memory_manager_runtime.h"
#include "platform.h"
#include "rw_semantic_data.h"

typedef struct UnsignedDivisionResult
{
  unsigned int quotient;
  unsigned int remainder;
} UnsignedDivisionResult;

__value_in_regs UnsignedDivisionResult
unsigned_divide_with_remainder(unsigned int divisor,
                               unsigned int numerator);

static Item display_screen_group;
static int display_graphics_open;
static int display_screen_group_added;

int
dispose_display_screens(void)
{
  int result;
  int index;

  if(display_screen_group > 0)
    {
      if(display_screen_group_added)
        {
          result = RemoveScreenGroup(display_screen_group);
          if(result < 0)
            {
              return result;
            }
          display_screen_group_added = 0;
        }
      result = DeleteScreenGroup(display_screen_group);
      if(result < 0)
        {
          return result;
        }
      display_screen_group = 0;
    }
  if(display_system_globals.bitmap_items != 0)
    {
      MEM_DisposePointer(display_system_globals.bitmap_items);
      display_system_globals.bitmap_items = 0;
    }
  for(index = 0; index < 4; index++)
    {
      display_system_globals.bitmaps[index] = 0;
      display_system_globals.screen_items[index] = 0;
    }
  display_system_globals.screen_count = 0;
  display_system_globals.bitmap_storage_pages = 0;
  display_system_globals.bitmap_storage_bytes = 0;
  display_system_globals.cached_bitmap_storage_pages = 0;
  display_system_globals.cached_bitmap_storage_bytes = 0;
  if(display_graphics_open)
    {
      result = CloseGraphicsFolio();
      if(result < 0)
        {
          return result;
        }
      display_graphics_open = 0;
    }
  return 0;
}

int
initialize_display_screens(int requested_screen_count, Item *screen_items)
{
  int screen_count;
  TagArg tags[3];
  GrafFolio *graphics;
  Screen *screen;
  Bitmap *bitmap;
  Item *bitmap_items;
  int index;
  int result;
  unsigned int page_rounding_size;
  UnsignedDivisionResult divmod;

  screen_count = requested_screen_count & 0xFF;
  if(screen_count > 4 || (screen_count != 0 && screen_items == 0))
    {
      return -1;
    }
  if(display_system_globals.screen_count != 0)
    {
      if(screen_count != display_system_globals.screen_count)
        {
          return -1;
        }
      for(index = 0; index < screen_count; index++)
        {
          screen_items[index] = display_system_globals.screen_items[index];
        }
      return 0;
    }
  if(display_screen_group > 0 ||
     display_system_globals.bitmap_items != 0)
    {
      result = dispose_display_screens();
      if(result < 0)
        {
          return result;
        }
    }
  if(!display_graphics_open)
    {
      result = OpenGraphicsFolio();
      if(result < 0)
        {
          return result;
        }
      display_graphics_open = 1;
    }
  if(screen_count == 0)
    {
      return 0;
    }

  bitmap_items = (Item *)MEM_NewPointer(
    screen_count * sizeof(*bitmap_items), MEMTYPE_CEL,
    MEM_POINTER_OPTIONS_DEFAULT);
  if(bitmap_items == 0)
    {
      result = -1;
      goto fail;
    }
  display_system_globals.bitmap_items = bitmap_items;
  graphics = GrafBase;
  tags[0].ta_Tag = CSG_TAG_SPORTBITS;
  tags[0].ta_Arg = (void *)GetBankBits(graphics->gf_ZeroPage);
  tags[1].ta_Tag = CSG_TAG_SCREENCOUNT;
  tags[1].ta_Arg = (void *)screen_count;
  tags[2].ta_Tag = CSG_TAG_DONE;
  tags[2].ta_Arg = 0;
  result = CreateScreenGroup(screen_items, tags);
  if(result < 0)
    {
      goto fail;
    }
  display_screen_group = result;
  result = AddScreenGroup(display_screen_group, 0);
  if(result < 0)
    {
      goto fail;
    }
  display_screen_group_added = 1;
  for(index = 0; index < screen_count; index++)
    {
      screen = (Screen *)LookupItem(screen_items[index]);
      if(screen == 0 || IsEmptyList(&screen->scr_BitmapList))
        {
          result = -1;
          goto fail;
        }
      bitmap = (Bitmap *)FirstNode(&screen->scr_BitmapList);
      bitmap_items[index] = bitmap->bm.n_Item;
      display_system_globals.bitmaps[index] = bitmap;
      display_system_globals.screen_items[index] = screen_items[index];
      result = EnableHAVG(screen_items[index]);
      if(result < 0)
        {
          goto fail;
        }
      result = EnableVAVG(screen_items[index]);
      if(result < 0)
        {
          goto fail;
        }
    }

  bitmap = display_system_globals.bitmaps[0];
  page_rounding_size = (unsigned int)(bitmap->bm_Width * bitmap->bm_Height);
  page_rounding_size = (page_rounding_size << 1) + graphics->gf_VRAMPageSize - 1;
  divmod = unsigned_divide_with_remainder(
    graphics->gf_VRAMPageSize, page_rounding_size);
  display_system_globals.bitmap_storage_pages = (int)divmod.quotient;
  display_system_globals.bitmap_storage_bytes =
    graphics->gf_VRAMPageSize * divmod.quotient;
  display_system_globals.cached_bitmap_storage_bytes =
    display_system_globals.bitmap_storage_bytes;
  display_system_globals.cached_bitmap_storage_pages =
    display_system_globals.bitmap_storage_pages;
  display_system_globals.active_screen = 0;
  display_system_globals.screen_count = (unsigned char)screen_count;
  return 0;

fail:
  dispose_display_screens();
  return result;
}
