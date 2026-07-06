#include "io.h"

#include "event.h"
#include "graphics.h"
#include "item.h"

#include "display_cel_runtime.h"
#include "front_end_cel_runtime.h"
#include "front_end_transition_runtime.h"
#include "rw_semantic_data.h"
#include "stream_input_runtime.h"

enum FrontEndCompletionConstant
{
  FRONT_END_COMPLETION_START_Y = DISPLAY_HEIGHT
};

#define FRONT_END_COMPLETION_SKIP_BUTTONS \
        (ControlA | ControlB | ControlC | ControlStart | ControlX)

int
load_front_end_cel_resource(int *descriptor_words,
                            int *resource_info_words);
int
finalize_front_end_cel_resource(int resource_id,
                                int release_mode);

int
play_front_end_completion_animation(void)
{
  FrontEndCelResourceInfo root_info;
  FrontEndCelResourceInfo primary_info;
  DisplayRectangle root_rectangle;
  DisplayRectangle primary_rectangle;
  CCB *root;
  CCB *primary;
  CCB *node;
  int timer;
  int x;
  int first_frame;
  int frame;
  unsigned int input;
  int root_acquired;
  int primary_acquired;
  int result;

  root = 0;
  primary = 0;
  x = 0;
  first_frame = 0;
  root_acquired = 0;
  primary_acquired = 0;

  if(load_front_end_cel_resource(
       (int *)front_end_cel_group_300, (int *)&root_info) != 0)
    {
      root_acquired = 1;
      root = root_info.animation.ccb;
      calculate_cel_display_rectangle(
        root, &root_rectangle,
        0, 0, DISPLAY_CEL_SCALE_ONE);
      map_encoded_cel_to_rectangle(root, &root_rectangle);
    }

  timer = GetVBLIOReq();
  if(load_front_end_cel_resource(
       (int *)front_end_cel_group_301, (int *)&primary_info) != 0)
    {
      primary_acquired = 1;
      primary = primary_info.animation.ccb;
      if(primary != 0)
        {
          x = primary->ccb_XPos >> DISPLAY_COORDINATE_FRACTION_SHIFT;
          first_frame = -primary->ccb_Height;
        }

      for(frame = FRONT_END_COMPLETION_START_Y;
          frame >= first_frame; frame--)
        {
          sample_stream_input_pads();
          input = get_stream_input_buttons(
            FRONT_END_TRANSITION_INPUT_PAD);
          if((input & FRONT_END_COMPLETION_SKIP_BUTTONS) != 0)
            {
              result = DeleteItem(timer);
              goto release_resources;
            }

          node = primary;
          while(node != 0)
            {
              calculate_cel_display_rectangle(
                node, &primary_rectangle,
                x, frame, DISPLAY_CEL_SCALE_ONE);
              map_encoded_cel_to_rectangle(node, &primary_rectangle);
              append_shaded_cel(root);
              append_shaded_cel(node);
              submit_cel_batch();
              node = node->ccb_NextPtr;
            }

          if(WaitVBLDefer(timer, 1) >= 0)
            {
              WaitIO(timer);
            }
          /* The wait above is this frame's field boundary; present without
             waiting for a second one. */
          present_display_frame(0);
        }
    }

  result = DeleteItem(timer);

release_resources:
  if(primary_acquired != 0)
    {
      finalize_front_end_cel_resource(
        front_end_cel_group_301->resource_id, 0);
    }
  if(root_acquired != 0)
    {
      finalize_front_end_cel_resource(
        front_end_cel_group_300->resource_id, 0);
    }
  return result;
}


#undef FRONT_END_COMPLETION_SKIP_BUTTONS
