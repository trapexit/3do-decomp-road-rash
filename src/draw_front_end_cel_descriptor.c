#include "display_cel_runtime.h"
#include "front_end_cel_runtime.h"
#include "rw_semantic_data.h"

int
load_front_end_cel_resource(int *descriptor_words,
                            int *resource_info_words);
int
finalize_front_end_cel_resource(int resource_id,
                                int release_mode);

int
draw_front_end_cel_descriptor(const FrontEndCelDescriptor *descriptor)
{
  FrontEndCelResourceInfo resource_info;
  DisplayRectangle rectangle;
  CCB *node;
  int x;
  int y;

  if(load_front_end_cel_resource(
       (int *)descriptor, (int *)&resource_info) == 0)
    {
      return 0;
    }

  node = resource_info.animation.ccb;
  if(node != 0)
    {
      if((node->ccb_Flags & CCB_LAST) == 0)
        {
          x = node->ccb_XPos >> DISPLAY_COORDINATE_FRACTION_SHIFT;
          y = node->ccb_YPos >> DISPLAY_COORDINATE_FRACTION_SHIFT;
        }
      else
        {
          x = resource_info.x;
          y = resource_info.y;
        }

      do
        {
          calculate_cel_display_rectangle(
            node, &rectangle, x, y, DISPLAY_CEL_SCALE_ONE);
          map_encoded_cel_to_rectangle(node, &rectangle);
          draw_shaded_cel(node);
          node = node->ccb_NextPtr;
        } while(node != 0);
    }

  finalize_front_end_cel_resource(descriptor->resource_id, 0);
  return submit_cel_batch();
}
