#include "event.h"
#include "platform.h"

#include "debug.h"
#include "graphics.h"
#include "stdio.h"
#include "string.h"

#include "application_runtime.h"
#include "rw_semantic_data.h"

enum ApplicationErrorDisplay
{
  APPLICATION_ERROR_MARGIN = 16,
  APPLICATION_ERROR_LINE_CHARS = 36,
  APPLICATION_ERROR_LINE_HEIGHT = 12
};

static int
draw_application_error_lines(Item bitmap, GrafCon *pen,
                             const char *text, int max_lines)
{
  char line[APPLICATION_ERROR_LINE_CHARS + 1];
  int length;
  int result;

  while(text != 0 && *text != 0 && max_lines > 0)
    {
      length = 0;
      while(*text != 0 && *text != '\n' &&
            length < APPLICATION_ERROR_LINE_CHARS)
        {
          line[length++] = *text++;
        }
      if(*text == '\n')
        {
          text++;
        }
      line[length] = 0;
      pen->gc_PenX = APPLICATION_ERROR_MARGIN;
      result = DrawText8(pen, bitmap, (const unsigned char *)line);
      if(result < 0)
        {
          return result;
        }
      pen->gc_PenY += APPLICATION_ERROR_LINE_HEIGHT;
      max_lines--;
    }
  return 0;
}

static Item
get_application_error_screen(void)
{
  Item screen_item;
  Item group;
  Screen *screen;
  TagArg tags[2];
  int index;
  int result;

  /* Reuse the system screen even if the game's display/font setup failed. */
  for(index = 0; index < 4; index++)
    {
      screen_item = display_system_globals.screen_items[index];
      screen = (Screen *)CheckItem(screen_item, NST_GRAPHICS, SCREENNODE);
      if(screen != 0 && !IsEmptyList(&screen->scr_BitmapList))
        {
          return screen_item;
        }
    }

  /* Early startup may not have created any game screens yet. */
  tags[0].ta_Tag = CSG_TAG_SCREENCOUNT;
  tags[0].ta_Arg = (void *)1;
  tags[1].ta_Tag = CSG_TAG_DONE;
  tags[1].ta_Arg = 0;
  group = CreateScreenGroup(&screen_item, tags);
  if(group < 0)
    {
      return group;
    }
  result = AddScreenGroup(group, 0);
  if(result < 0)
    {
      return result;
    }
  return screen_item;
}

void
show_application_error(const char *stage, int status, const char *detail)
{
  Item screen_item;
  Screen *screen;
  Bitmap *bitmap;
  GrafCon pen;
  char code[40];
  int result;

  if(stage == 0)
    {
      stage = "Unknown operation";
    }
  kprintf("APPLICATION ERROR: %s status=%ld (0x%08lx)\n",
          stage, (long)status, (unsigned long)status);
  if(detail != 0)
    {
      kprintf("Detail: %s\n", detail);
    }

  result = OpenGraphicsFolio();
  if(result < 0)
    {
      goto halt;
    }
  screen_item = get_application_error_screen();
  if(screen_item < 0)
    {
      result = screen_item;
      goto halt;
    }
  screen = (Screen *)CheckItem(screen_item, NST_GRAPHICS, SCREENNODE);
  if(screen == 0 || IsEmptyList(&screen->scr_BitmapList))
    {
      result = -1;
      goto halt;
    }
  bitmap = (Bitmap *)FirstNode(&screen->scr_BitmapList);

  /* Avoid the game's allocator, fonts, CEL queue and SPORT I/O request. */
  memset(bitmap->bm_Buffer, 0, bitmap->bm_Width * bitmap->bm_Height * 2);
  result = ResetCurrentFont();
  if(result < 0)
    {
      goto halt;
    }
  result = ResetScreenColors(screen_item);
  if(result < 0)
    {
      goto halt;
    }
  result = SetClipOrigin(bitmap->bm.n_Item, 0, 0);
  if(result >= 0)
    {
      result = SetClipWidth(bitmap->bm.n_Item, bitmap->bm_Width);
    }
  if(result >= 0)
    {
      result = SetClipHeight(bitmap->bm.n_Item, bitmap->bm_Height);
    }
  if(result < 0)
    {
      goto halt;
    }
  memset(&pen, 0, sizeof(pen));
  SetFGPen(&pen, 0x7FFF);
  SetBGPen(&pen, 0);
  pen.gc_PenY = 24;
  result = draw_application_error_lines(
    bitmap->bm.n_Item, &pen, "ROAD RASH - ERROR", 1);
  if(result < 0)
    {
      goto halt;
    }
  pen.gc_PenY = 52;
  result = draw_application_error_lines(bitmap->bm.n_Item, &pen, stage, 2);
  if(result < 0)
    {
      goto halt;
    }
  sprintf(code, "Status: %d\nHex: 0x%08X", status, (unsigned int)status);
  pen.gc_PenY = 88;
  result = draw_application_error_lines(bitmap->bm.n_Item, &pen, code, 2);
  if(result < 0)
    {
      goto halt;
    }
  pen.gc_PenY = 124;
  result = draw_application_error_lines(bitmap->bm.n_Item, &pen, detail, 5);
  if(result < 0)
    {
      goto halt;
    }
  pen.gc_PenY = 208;
  result = draw_application_error_lines(
    bitmap->bm.n_Item, &pen, "Photograph this screen, then reset.", 1);
  if(result >= 0)
    {
      result = DisplayScreen(screen_item, 0);
    }

halt:
  if(result < 0)
    {
      kprintf("Error screen unavailable: %ld\n", (long)result);
    }
  /* Do not tear down the display, return to the menu, or reboot past the error. */
  for(;;)
    {
    }
}

void
abort_application_with_resource_error(int status)
{
  show_application_error("Resource load", status, 0);
}


int
wait_for_control_pad_press(void)
{
  ControlPadEventData event;

  do
    {
      GetControlPad(1, 0, &event);
    } while(event.cped_ButtonBits != 0);

  do
    {
      GetControlPad(1, 0, &event);
    } while(event.cped_ButtonBits == 0);

  return event.cped_ButtonBits;
}
