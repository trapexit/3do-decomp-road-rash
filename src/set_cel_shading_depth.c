#include "rw_semantic_data.h"

enum CelShadingDepthConstant
{
  CEL_SHADING_MODE_BASE = 8,
  CEL_SHADING_ROW_BASE = 0x13,
  CEL_SHADING_NEGATIVE_ROUNDING = 3,
  CEL_SHADING_ROW_SHIFT = 2
};

int
set_cel_shading_depth(int depth)
{
  int row_offset;

  display_runtime_globals.shading_mode =
    CEL_SHADING_MODE_BASE - depth;

  row_offset = CEL_SHADING_ROW_BASE - depth;
  if(row_offset < 0)
    {
      row_offset += CEL_SHADING_NEGATIVE_ROUNDING;
    }
  row_offset >>= CEL_SHADING_ROW_SHIFT;
  display_runtime_globals.shading_row_offset = row_offset;
  return row_offset;
}
