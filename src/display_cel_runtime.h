#ifndef ROADRASH_DISPLAY_CEL_RUNTIME_H
#define ROADRASH_DISPLAY_CEL_RUNTIME_H

#include "graphics.h"
#include "hardware.h"

#include "road_geometry.h"
#include "rw_semantic_data.h"

enum
{
  DISPLAY_BUFFER_COUNT = 2,
  DISPLAY_WIDTH = 320,
  DISPLAY_HEIGHT = 240,
  DISPLAY_COORDINATE_FRACTION_SHIFT = 16,
  DISPLAY_CEL_MAPPING_SCALE_SHIFT = 4,
  DISPLAY_CEL_SOURCE_SHIFT_MASK = 0x1F,
  DISPLAY_RECTANGLE_HDX_SHIFT = 19,
  DISPLAY_RECTANGLE_VDY_SHIFT = 15,
  DISPLAY_CEL_EXTENT_SIGN_SHIFT = 23,
  DISPLAY_CEL_EXTENT_EXPONENT_MASK = 0x1F,
  DISPLAY_CEL_SCALE_SHIFT = 16,
  DISPLAY_CEL_SCALE_ONE = 0x10000,
  DISPLAY_CEL_SCALE_HALF = 0x8000,
  DISPLAY_CEL_HDX_ONE = 0x100000,
  DISPLAY_CEL_HDX_FACTOR_SHIFT = 12,
  DISPLAY_CEL_VDY_FACTOR_SHIFT = 16,
  DISPLAY_CEL_ENCODED_FACTOR_SHIFT = 15,
  DISPLAY_SOLID_PIXEL_WORD_COUNT = 4,
  DISPLAY_SOLID_PIXEL_REPEAT_INDEX = 2
};

typedef struct DisplayRectangle
{
  int left;
  int top;
  int right;
  int bottom;
} DisplayRectangle;

typedef struct DisplayPoint
{
  int x;
  int y;
} DisplayPoint;

typedef struct DisplayQuad
{
  DisplayPoint top_left;
  DisplayPoint top_right;
  DisplayPoint bottom_right;
  DisplayPoint bottom_left;
} DisplayQuad;

typedef struct DisplayAxisAlignedCelMapping
{
  int y;
  int x;
  int vertical_delta;
  int horizontal_delta;
} DisplayAxisAlignedCelMapping;

int
advance_display_buffer_index(int current_index);
int
open_display_item_handles(void);
void
close_display_item_handles(void);
void
select_display_system_screen(int screen_index);
int
submit_cel_batch(void);
void
present_display_frame(int wait_for_vbl);

int *
map_cel_to_rectangle(CCB                    *cel,
                     const DisplayRectangle *rectangle);
void
calculate_cel_display_rectangle(const CCB        *cel,
                                DisplayRectangle *rectangle,
                                int               x,
                                int               y,
                                int               scale);
void
map_encoded_cel_to_rectangle(CCB                    *cel,
                             const DisplayRectangle *rectangle);
CCB *
append_unmapped_cel(const CCB *source);
CCB *
append_cel_mapping_packet(const CCB *source);
void
append_shaded_cel(const CCB *source);
void
draw_shaded_cel(const CCB *source);
void
translate_display_rectangle(DisplayRectangle *rectangle,
                            int               horizontal_offset,
                            int               vertical_offset);
void
set_display_rectangle(DisplayRectangle *rectangle,
                      int               left,
                      int               top,
                      int               right,
                      int               bottom);
int32 *
map_cel_at_native_scale(CCB *cel,
                        int  x,
                        int  y);
void
map_rotated_cel_about_pivot(CCB *cel,
                            int  angle_256ths,
                            int  anchor_x,
                            int  anchor_y,
                            int  pivot_x,
                            int  pivot_y);
CCB *
append_solid_rectangle_cel(const DisplayRectangle *rectangle,
                           unsigned int            color);
int *
map_solid_quad_to_cel(CCB               *cel,
                      const DisplayQuad *quad);
CCB *
append_solid_quad_cel(const DisplayQuad *quad,
                      unsigned int       color);
CCB *
append_mapped_quad_cel(const CCB         *source,
                       const DisplayQuad *quad);
CCB *
append_horizontal_quad_cel(const CCB         *source,
                           const DisplayQuad *quad);
CCB *
append_axis_aligned_cel(const CCB                          *source,
                        const DisplayAxisAlignedCelMapping *mapping);
uint32
read_cel_preamble_words(const CCB *cel,
                        uint32    *pre0_word,
                        uint32    *pre1_word);
CelData *
select_cel_source_row(CCB *cel,
                      int  source_row);
void
map_cel_to_horizontal_quad(CCB            *cel,
                           const RoadQuad *quad);
void
map_cel_to_perspective_quad(CCB            *cel,
                            const RoadQuad *quad);
CCB *
queue_fullscreen_clear(void);

#endif
