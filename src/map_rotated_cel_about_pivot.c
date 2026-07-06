#include "display_cel_runtime.h"

int
fixed_cosine(int angle);
int
fixed_sine(unsigned int angle);

void
map_rotated_cel_about_pivot(CCB *cel,
                            int  angle_256ths,
                            int  anchor_x,
                            int  anchor_y,
                            int  pivot_x,
                            int  pivot_y)
{
  int fixed_angle;
  int cosine;
  int sine;

  fixed_angle = (int)((unsigned int)angle_256ths <<
                      DISPLAY_COORDINATE_FRACTION_SHIFT);
  cosine = fixed_cosine(fixed_angle);
  sine = fixed_sine((unsigned int)fixed_angle);

  cel->ccb_XPos =
    (int)((unsigned int)anchor_x <<
          DISPLAY_COORDINATE_FRACTION_SHIFT) -
    cosine * pivot_x + sine * pivot_y;
  cel->ccb_YPos =
    (int)((unsigned int)anchor_y <<
          DISPLAY_COORDINATE_FRACTION_SHIFT) -
    sine * pivot_x - cosine * pivot_y;
  cel->ccb_HDX = (int)((unsigned int)cosine <<
                       DISPLAY_CEL_MAPPING_SCALE_SHIFT);
  cel->ccb_HDY = (int)((unsigned int)sine <<
                       DISPLAY_CEL_MAPPING_SCALE_SHIFT);
  cel->ccb_VDX = -sine;
  cel->ccb_VDY = cosine;
  cel->ccb_HDDX = 0;
  cel->ccb_HDDY = 0;
  cel->ccb_Flags |= CCB_LDSIZE;
}
