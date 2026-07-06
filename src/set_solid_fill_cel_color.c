enum SolidFillColorConstant
{
  SOLID_FILL_COMPONENT_MASK = 0x1F,
  SOLID_FILL_RED_SHIFT = 10,
  SOLID_FILL_GREEN_SHIFT = 5,
  SOLID_FILL_DUPLICATE_SHIFT = 16,
  SOLID_FILL_CEL_SOURCE_WORD = 2,
  SOLID_FILL_PIXEL_REPEAT_INDEX = 2
};

int *
set_solid_fill_cel_color(int *cel,
                         int  red,
                         int  green,
                         int  blue)
{
  int *dst;
  int value;

  dst = (int *)cel[SOLID_FILL_CEL_SOURCE_WORD];
  value = ((red & SOLID_FILL_COMPONENT_MASK) << SOLID_FILL_RED_SHIFT) |
          ((green & SOLID_FILL_COMPONENT_MASK) << SOLID_FILL_GREEN_SHIFT) |
          (blue & SOLID_FILL_COMPONENT_MASK);
  value += value << SOLID_FILL_DUPLICATE_SHIFT;
  dst[0] = value;
  dst[SOLID_FILL_PIXEL_REPEAT_INDEX] = value;
  return dst + SOLID_FILL_PIXEL_REPEAT_INDEX;
}
