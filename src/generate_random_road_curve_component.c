#include "stdlib.h"

#include "division_helpers.h"
#include "road_procedural_curve.h"

enum RoadProceduralCurveGenerationValue
{
  ROAD_CURVE_SIGN_BIT_SHIFT = 31,
  ROAD_CURVE_FIXED_FRACTION_BITS = 16,
  ROAD_CURVE_POINT_COMPONENT_COUNT =
    sizeof(RoadProceduralCurvePoint) / sizeof(int)
};

static
int
halve_towards_zero(int value)
{
  return (value +
          (int)((unsigned int)value >> ROAD_CURVE_SIGN_BIT_SHIFT)) >> 1;
}


void
generate_random_road_curve_component(int  point_count,
                                     int  target_minimum,
                                     int  target_maximum,
                                     int  maximum_target_delta,
                                     int  minimum_run_length,
                                     int  maximum_run_length,
                                     int *component_output)
{
  SignedDivisionResult signed_result;
  UnsignedDivisionResult unsigned_result;
  int current_fixed;
  int point_index;
  int run_length;
  int target_span;
  int run_length_span;
  int target_delta;
  int step_fixed;

  current_fixed = target_minimum +
                  halve_towards_zero(target_maximum - target_minimum);
  point_index = 0;
  if(point_count <= 0)
    {
      return;
    }

  target_span = target_maximum - target_minimum + 1;
  run_length_span = maximum_run_length - minimum_run_length + 1;
  run_length = 0;
  step_fixed = 0;

  do
    {
      if(run_length == 0)
        {
          unsigned_result = unsigned_divide_with_remainder(
            (unsigned int)run_length_span, (unsigned int)rand());
          run_length = (int)unsigned_result.remainder +
                       minimum_run_length;

          signed_result = signed_divide_with_remainder(
            (unsigned int)target_span, (unsigned int)rand());
          target_delta = (int)signed_result.remainder + target_minimum -
                         (current_fixed >> ROAD_CURVE_FIXED_FRACTION_BITS);
          if(target_delta > maximum_target_delta)
            {
              target_delta = maximum_target_delta;
            }
          else if(target_delta < -maximum_target_delta)
            {
              target_delta = -maximum_target_delta;
            }

          signed_result = signed_divide_with_remainder(
            (unsigned int)run_length,
            (unsigned int)target_delta <<
              ROAD_CURVE_FIXED_FRACTION_BITS);
          step_fixed = signed_result.quotient;
        }

      component_output[
        point_index * ROAD_CURVE_POINT_COMPONENT_COUNT] =
        current_fixed >> ROAD_CURVE_FIXED_FRACTION_BITS;
      current_fixed += step_fixed;
      run_length--;
      point_index++;
    } while(point_index < point_count);
}
