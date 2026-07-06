#include "io.h"

#include "event.h"
#include "graphics.h"
#include "item.h"

#include "display_cel_runtime.h"
#include "front_end_cel_runtime.h"
#include "front_end_transition_runtime.h"
#include "rw_semantic_data.h"
#include "stream_input_runtime.h"

#define FRONT_END_TRANSITION_SKIP_BUTTONS \
        (ControlA | ControlB | ControlC | ControlStart | ControlX)

int
load_front_end_cel_resource(int *descriptor,
                            int *resource_info);
int
finalize_front_end_cel_resource(int resource_id,
                                int release_mode);

int
front_end_play_transition(void)
{
  FrontEndCelResourceInfo root_info;
  FrontEndCelResourceInfo primary_info;
  FrontEndCelResourceInfo secondary_info;
  DisplayRectangle root_rectangle;
  DisplayRectangle primary_rectangle;
  DisplayRectangle secondary_rectangle;
  CCB *root;
  CCB *primary;
  CCB *secondary;
  CCB *primary_node;
  CCB *secondary_node;
  FrontEndCelDescriptor *primary_descriptor;
  FrontEndCelDescriptor *secondary_descriptor;
  Item timer;
  int primary_x;
  int secondary_x;
  int previous_y;
  int current_y;
  int delta_y;
  int frame;
  int frame_limit;
  uint32 input;
  int root_acquired;
  int primary_acquired;
  int secondary_acquired;
  int result;

  root = 0;
  primary = 0;
  secondary = 0;
  root_acquired = 0;
  primary_acquired = 0;
  secondary_acquired = 0;
  primary_descriptor = front_end_cel_group_003;
  secondary_descriptor = primary_descriptor + 1;
  primary_x = 0;
  secondary_x = 0;
  previous_y = 0;

  if(load_front_end_cel_resource(
       (int *)front_end_cel_group_004, (int *)&root_info) != 0)
    {
      root_acquired = 1;
      root = root_info.animation.ccb;
      calculate_cel_display_rectangle(
        root, &root_rectangle, 0, 0, DISPLAY_CEL_SCALE_ONE);
      map_encoded_cel_to_rectangle(root, &root_rectangle);
    }

  timer = GetVBLIOReq();
  if(load_front_end_cel_resource(
       (int *)primary_descriptor, (int *)&primary_info) != 0)
    {
      primary_acquired = 1;
      primary = primary_info.animation.ccb;
      if(primary == 0)
        {
          goto transition_complete;
        }
      primary_x = primary->ccb_XPos >>
                  DISPLAY_COORDINATE_FRACTION_SHIFT;

      for(frame = DISPLAY_HEIGHT; frame >= 0; frame--)
        {
          sample_stream_input_pads();
          input = get_stream_input_buttons(FRONT_END_TRANSITION_INPUT_PAD);
          if((input & FRONT_END_TRANSITION_SKIP_BUTTONS) != 0)
            {
              result = DeleteItem(timer);
              goto release_resources;
            }

          primary_node = primary;
          while(primary_node != 0)
            {
              calculate_cel_display_rectangle(
                primary_node, &primary_rectangle,
                primary_x, frame, DISPLAY_CEL_SCALE_ONE);
              map_encoded_cel_to_rectangle(
                primary_node, &primary_rectangle);
              append_shaded_cel(root);
              append_shaded_cel(primary_node);
              submit_cel_batch();
              primary_node = primary_node->ccb_NextPtr;
            }

          if(WaitVBLDefer(timer, 1) >= 0)
            {
              WaitIO(timer);
            }
          /* The wait above is this frame's field boundary; present without
             waiting for a second one. */
          present_display_frame(0);
        }

      if(load_front_end_cel_resource(
           (int *)secondary_descriptor,
           (int *)&secondary_info) != 0)
        {
          secondary_acquired = 1;
          secondary = secondary_info.animation.ccb;
          if(secondary == 0)
            {
              goto transition_complete;
            }
          previous_y = primary->ccb_Height;

          while(primary_descriptor->resource_id != -1 &&
                secondary_descriptor->resource_id != -1)
            {
              if(secondary != 0)
                {
                  secondary_x = secondary->ccb_XPos >>
                                DISPLAY_COORDINATE_FRACTION_SHIFT;
                }
              current_y = primary->ccb_Height;

              for(frame = current_y; frame >= 0; frame--)
                {
                  sample_stream_input_pads();
                  input = get_stream_input_buttons(
                    FRONT_END_TRANSITION_INPUT_PAD);
                  if((input & FRONT_END_TRANSITION_SKIP_BUTTONS) != 0)
                    {
                      result = DeleteItem(timer);
                      goto release_resources;
                    }

                  delta_y = frame - previous_y;
                  primary_node = primary;
                  secondary_node = secondary;
                  while(secondary_node != 0)
                    {
                      calculate_cel_display_rectangle(
                        primary_node, &primary_rectangle,
                        primary_x, delta_y, DISPLAY_CEL_SCALE_ONE);
                      calculate_cel_display_rectangle(
                        secondary_node, &secondary_rectangle,
                        secondary_x, frame, DISPLAY_CEL_SCALE_ONE);
                      map_encoded_cel_to_rectangle(
                        primary_node, &primary_rectangle);
                      map_encoded_cel_to_rectangle(
                        secondary_node, &secondary_rectangle);
                      draw_shaded_cel(root);
                      draw_shaded_cel(primary_node);
                      draw_shaded_cel(secondary_node);
                      primary_node = primary_node->ccb_NextPtr;
                      secondary_node = secondary_node->ccb_NextPtr;
                    }

                  if(WaitVBLDefer(timer, 1) >= 0)
                    {
                      WaitIO(timer);
                    }
                  /* The wait above is this frame's field boundary; present
                     without waiting for a second one. */
                  present_display_frame(0);
                }

              if(secondary_acquired != 0)
                {
                  finalize_front_end_cel_resource(
                    secondary_descriptor->resource_id, 0);
                  secondary_acquired = 0;
                }
              if(primary_acquired != 0)
                {
                  finalize_front_end_cel_resource(
                    primary_descriptor->resource_id, 0);
                  primary_acquired = 0;
                }

              secondary_descriptor++;
              secondary = 0;
              if(load_front_end_cel_resource(
                   (int *)secondary_descriptor,
                   (int *)&secondary_info) != 0)
                {
                  secondary_acquired = 1;
                  secondary = secondary_info.animation.ccb;
                }

              primary_descriptor++;
              primary = 0;
              if(load_front_end_cel_resource(
                   (int *)primary_descriptor,
                   (int *)&primary_info) != 0)
                {
                  primary_acquired = 1;
                  primary = primary_info.animation.ccb;
                }
              if(primary == 0 || secondary == 0)
                {
                  break;
                }
              primary_x = primary->ccb_XPos >>
                          DISPLAY_COORDINATE_FRACTION_SHIFT;
              previous_y = primary->ccb_Height;
            }

          if(primary == 0)
            {
              goto transition_complete;
            }
          frame = 0;
          frame_limit = primary->ccb_Height +
                        FRONT_END_TRANSITION_EXIT_FRAMES;
          if(frame_limit >= 0)
            {
              do
                {
                  primary_node = primary;
                  while(primary_node != 0)
                    {
                      calculate_cel_display_rectangle(
                        primary_node, &primary_rectangle,
                        primary_x, frame, DISPLAY_CEL_SCALE_ONE);
                      map_encoded_cel_to_rectangle(
                        primary_node, &primary_rectangle);
                      draw_shaded_cel(root);
                      draw_shaded_cel(primary_node);
                      primary_node = primary_node->ccb_NextPtr;
                    }

                  if(WaitVBLDefer(timer, 1) >= 0)
                    {
                      WaitIO(timer);
                    }
                  /* The wait above is this frame's field boundary; present
                     without waiting for a second one. */
                  present_display_frame(0);
                  frame--;
                } while(frame >= -frame_limit);
            }
        }
    }

transition_complete:
  DeleteItem(timer);
  race_input_latch_state.event_latched = 1;
  front_end_preview_state.transition_completed = 1;
  result = 1;

release_resources:
  if(secondary_acquired != 0)
    {
      finalize_front_end_cel_resource(
        secondary_descriptor->resource_id, 0);
    }
  if(primary_acquired != 0)
    {
      finalize_front_end_cel_resource(
        primary_descriptor->resource_id, 0);
    }
  if(root_acquired != 0)
    {
      finalize_front_end_cel_resource(
        front_end_cel_group_004->resource_id, 0);
    }
  return result;
}


#undef FRONT_END_TRANSITION_SKIP_BUTTONS
