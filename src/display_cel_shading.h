#ifndef ROADRASH_DISPLAY_CEL_SHADING_H
#define ROADRASH_DISPLAY_CEL_SHADING_H

#include "bss_early_data.h"
#include "display_cel_runtime.h"

enum
{
  CEL_SHADE_ROW_MAX = 15,
  CEL_SHADE_COLUMN_MAX = 31,
  CEL_SHADE_INDEX_MASK = 0x7F,
  CEL_SHADE_NEUTRAL_COLUMN = 18,
  CEL_SHADE_WEIGHT_SCALE = 8,
  CEL_SHADE_WEIGHT_SHIFT = 3,
  CEL_SHADE_BASE_BIAS = 0x48000,
  CEL_SHADE_COMPONENT_SHIFT = 16,
  CEL_SHADE_COLUMN_SCALE = 3,
  CEL_SHADE_COLUMN_ROUNDING = 0x10,
  CEL_SHADE_COLUMN_SHIFT = 5,
  CEL_SHADE_HALF_SHIFT = 1,
  CEL_SIGN_BIT_SHIFT = 31
};

#define CEL_SHADE_HIGH_MASK 0xFFFF0000U
#define CEL_SHADE_INDEXED_FLAG 0x00008000U
#define CEL_SHADE_OVERRIDE_PIXC 0x1DC01DC0U

static
__inline
int
clamp_cel_shade_row(int value)
{
  if(value < 0)
    {
      return 0;
    }
  if(value > CEL_SHADE_ROW_MAX)
    {
      return CEL_SHADE_ROW_MAX;
    }
  return value;
}


static
__inline
int
clamp_cel_shade_column(int value)
{
  if(value < 0)
    {
      return 0;
    }
  if(value > CEL_SHADE_COLUMN_MAX)
    {
      return CEL_SHADE_COLUMN_MAX;
    }
  return value;
}


static
__inline
int
divide_cel_shade_by_weight(int value)
{
  if(value < 0)
    {
      value += CEL_SHADE_WEIGHT_SCALE - 1;
    }
  return value >> CEL_SHADE_WEIGHT_SHIFT;
}


static
__inline
int
divide_cel_shade_by_two(int value)
{
  value += (int)((unsigned int)value >> CEL_SIGN_BIT_SHIFT);
  return value >> CEL_SHADE_HALF_SHIFT;
}


static
__inline
void
copy_display_cel_source_fields(CCB         *packet,
                               const CCB   *source,
                               unsigned int extra_flags)
{
  /* extra_flags carries caller-specific mapping bits (LDSIZE, LDPRS) so the
     packet header is written once instead of being read back, OR-ed, and
     written again by the caller. */
  packet->ccb_Flags =
    (source->ccb_Flags & ~(CCB_LAST | CCB_ACSC | CCB_TWD)) |
    CCB_NPABS | CCB_SPABS | CCB_PPABS | CCB_ALSC | CCB_ACE |
    extra_flags;
  /* ccb_PIXC is deliberately not copied: every caller writes the packet's
     pixel control once from shade_display_cel_pixc(), which returns the
     source value unchanged on the paths that do not shade. */
  packet->ccb_SourcePtr = source->ccb_SourcePtr;
  packet->ccb_PLUTPtr = source->ccb_PLUTPtr;
  packet->ccb_PRE0 = source->ccb_PRE0;
  packet->ccb_PRE1 = source->ccb_PRE1;
}


static
__inline
void
copy_display_cel_mapping_fields(CCB         *packet,
                                const CCB   *source,
                                unsigned int extra_flags)
{
  copy_display_cel_source_fields(packet, source, extra_flags);
  packet->ccb_HDX = source->ccb_HDX;
  packet->ccb_HDY = source->ccb_HDY;
  packet->ccb_VDX = source->ccb_VDX;
  packet->ccb_VDY = source->ccb_VDY;
  packet->ccb_HDDX = source->ccb_HDDX;
  packet->ccb_HDDY = source->ccb_HDDY;
}


static
__inline
void
copy_display_mapped_cel_fields(CCB         *packet,
                               const CCB   *source,
                               unsigned int extra_flags)
{
  copy_display_cel_mapping_fields(packet, source, extra_flags);
  packet->ccb_XPos = source->ccb_XPos;
  packet->ccb_YPos = source->ccb_YPos;
  packet->ccb_Width = source->ccb_Width;
  packet->ccb_Height = source->ccb_Height;
}


static
__inline
int
decode_cel_shade_base(unsigned int pixel_control)
{
  unsigned int shifted;

  shifted = CEL_SHADE_BASE_BIAS +
            (pixel_control << CEL_SHADE_COMPONENT_SHIFT);
  return (int)shifted >> CEL_SHADE_COMPONENT_SHIFT;
}


/* Returns the pixel control the packet should carry.  The packet's copied
   PIXC is not read back or rewritten here: the caller stores the returned
   word once, so a shaded append writes the packet header once instead of
   writing the copied value and then overwriting it. */
static
__inline
unsigned int
shade_display_cel_pixc(CCB         *cel,
                       unsigned int pixel_control)
{
  unsigned int high;
  int mode;
  int row;
  int column;
  int base;
  int remaining_weight;

  if((cel->ccb_PRE0 & PRE0_BPP_MASK) == PRE0_BPP_8)
    {
      if(display_runtime_globals.special_ccb_override != 0)
        {
          return CEL_SHADE_OVERRIDE_PIXC;
        }
      return pixel_control;
    }

  high = pixel_control & CEL_SHADE_HIGH_MASK;
  mode = display_runtime_globals.shading_mode;

  if(mode > 0)
    {
      if(high != 0)
        {
          return high |
                 gCelPixcShadingTable[mode][CEL_SHADE_NEUTRAL_COLUMN];
        }

      if((pixel_control & CEL_SHADE_INDEXED_FLAG) != 0)
        {
          row = (int)(pixel_control & CEL_SHADE_INDEX_MASK) +
                display_runtime_globals.shading_row_offset;
          row = clamp_cel_shade_row(row);
          return high |
                 gCelPixcShadingTable[row][CEL_SHADE_NEUTRAL_COLUMN];
        }

      base = decode_cel_shade_base(pixel_control);
      remaining_weight = CEL_SHADE_WEIGHT_SCALE - mode;
      column = ((base * CEL_SHADE_COLUMN_SCALE +
                 CEL_SHADE_COLUMN_ROUNDING) >>
                CEL_SHADE_COLUMN_SHIFT) * remaining_weight +
               mode * CEL_SHADE_NEUTRAL_COLUMN;
      column = divide_cel_shade_by_weight(column);

      row = divide_cel_shade_by_two(CEL_SHADE_COLUMN_MAX - base);
      row = divide_cel_shade_by_weight(row * remaining_weight +
                                       mode * mode);
      row = clamp_cel_shade_row(row);
      column = clamp_cel_shade_column(column);
      return high | gCelPixcShadingTable[row][column];
    }

  if(high != 0)
    {
      return pixel_control;
    }

  if((pixel_control & CEL_SHADE_INDEXED_FLAG) != 0)
    {
      row = (int)(pixel_control & CEL_SHADE_INDEX_MASK) +
            display_runtime_globals.shading_row_offset;
      row = clamp_cel_shade_row(row);
      return high |
             gCelPixcShadingTable[row][CEL_SHADE_NEUTRAL_COLUMN];
    }

  base = decode_cel_shade_base(pixel_control);
  column = (base * CEL_SHADE_COLUMN_SCALE +
            CEL_SHADE_COLUMN_ROUNDING) >> CEL_SHADE_COLUMN_SHIFT;
  row = divide_cel_shade_by_two(CEL_SHADE_COLUMN_MAX - base);
  row = clamp_cel_shade_row(row);
  column = clamp_cel_shade_column(column);
  return high | gCelPixcShadingTable[row][column];
}


#endif
