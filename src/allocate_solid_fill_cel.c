#include "bss_early_data.h"
#include "graphics.h"
#include "memory_manager_runtime.h"

enum SolidFillCelConstant
{
  SOLID_FILL_CEL_BYTES = 0x50,
  SOLID_FILL_PIXEL_OFFSET = 0x44,
  SOLID_FILL_DIMENSION_INDEX = 2
};

#define SOLID_FILL_INITIAL_FLAGS ((uint32)0x7F660100U)
#define SOLID_FILL_DEFAULT_PIXC ((uint32)0x1F001F00U)
#define SOLID_FILL_DEFAULT_PRE0 ((uint32)0x56U)
#define SOLID_FILL_FLAGS_CLEAR ((uint32)0x10U)
#define SOLID_FILL_FLAGS_SET ((uint32)0x20U)

int
allocate_solid_fill_cel(CCB **cel_output)
{
  CCB *cel;
  int dimension_preamble;

  cel = (CCB *)MEM_NewPointer(
    SOLID_FILL_CEL_BYTES, MEMORY_REQUEST_ANY,
    MEM_POINTER_OPTIONS_DEFAULT);
  if(cel == 0)
    {
      return 1;
    }

  cel->ccb_Flags = SOLID_FILL_INITIAL_FLAGS;
  cel->ccb_NextPtr = 0;
  cel->ccb_SourcePtr =
    (CelData *)((char *)cel + SOLID_FILL_PIXEL_OFFSET);
  cel->ccb_PLUTPtr = 0;
  cel->ccb_XPos = 0;
  cel->ccb_YPos = 0;
  cel->ccb_HDX = 0;
  cel->ccb_HDY = 0;
  cel->ccb_VDX = 0;
  cel->ccb_VDY = 0;
  cel->ccb_HDDX = 0;
  cel->ccb_HDDY = 0;
  cel->ccb_PIXC = SOLID_FILL_DEFAULT_PIXC;
  cel->ccb_PRE0 = SOLID_FILL_DEFAULT_PRE0;
  cel->ccb_PRE1 = 1;
  dimension_preamble =
    (int)gCelDimensionPreambleTable[SOLID_FILL_DIMENSION_INDEX];
  cel->ccb_Width = dimension_preamble;
  cel->ccb_Height = dimension_preamble;
  cel->ccb_Flags =
    (cel->ccb_Flags & ~SOLID_FILL_FLAGS_CLEAR) |
    SOLID_FILL_FLAGS_SET;
  *cel_output = cel;
  return 0;
}


#undef SOLID_FILL_FLAGS_SET
#undef SOLID_FILL_FLAGS_CLEAR
#undef SOLID_FILL_DEFAULT_PRE0
#undef SOLID_FILL_DEFAULT_PIXC
#undef SOLID_FILL_INITIAL_FLAGS
