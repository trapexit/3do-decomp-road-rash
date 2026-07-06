#ifndef ROADRASH_TEXT_RENDER_RUNTIME_H
#define ROADRASH_TEXT_RENDER_RUNTIME_H

#include "graphics.h"

enum TextRenderConstants
{
  TEXT_GLYPH_CCB_COUNT = 4,
  TEXT_GLYPH_COUNT = 96,
  TEXT_COLOR_COMPONENT_MAX = 255,
  TEXT_DEFAULT_GLYPH_WIDTH = 255,
  TEXT_CHARACTER_BASE = 0x20,
  TEXT_GLYPH_INDEX_LIMIT = 0x7F,
  TEXT_ALIGNMENT_LEFT = 2,
  TEXT_ALIGNMENT_CENTER = 3,
  TEXT_ALIGNMENT_RIGHT = 4,
  TEXT_LAYOUT_VERTICAL = 5,
  TEXT_WIDTH_SIGN_SHIFT = 31
};

enum TextGlyphLayer
{
  TEXT_GLYPH_LAYER_FIRST = 0,
  TEXT_GLYPH_LAYER_SECOND = 1,
  TEXT_GLYPH_LAYER_THIRD = 2,
  TEXT_GLYPH_LAYER_FOURTH = 3
};

/* Font resources store relative PLUT, CCB, and glyph-source offsets. */
typedef struct TextFontResourceData
{
  unsigned char pascal_name[0x10];
  int line_height;
  void *glyph_plut;
  CCB *glyph_ccbs[TEXT_GLYPH_CCB_COUNT];
  unsigned char glyph_width_bytes[TEXT_GLYPH_COUNT][2];
  void *glyph_sources[TEXT_GLYPH_CCB_COUNT][TEXT_GLYPH_COUNT];
} TextFontResourceData;

typedef struct TextFontResourceImage
{
  unsigned int chunk_type;
  unsigned int chunk_size;
  TextFontResourceData font;
} TextFontResourceImage;

typedef struct TextFontEntry TextFontEntry;

struct TextFontEntry
{
  TextFontEntry *next;
  TextFontEntry *previous;
  TextFontResourceData *resource_data;
  void *owned_resource_data;
  unsigned int load_flags;
  int font_identifier;
  unsigned char red;
  unsigned char green;
  unsigned char blue;
  unsigned char color_padding;
  int shadow_x_offset;
  int shadow_y_offset;
  unsigned int draw_flags;
  unsigned char fallback_glyph_width;
  unsigned char width_padding[3];
  int glyph_pixc_override;
};

typedef struct TextRenderManager
{
  TextFontEntry *font_head;
  TextFontEntry *font_tail;
  TextFontEntry *active_font;
  unsigned int draw_flags;
  unsigned int render_flags;
  int last_font_identifier;
  CCB *glyph_ccbs[TEXT_GLYPH_CCB_COUNT];
} TextRenderManager;

#define TEXT_RENDER_LAYOUT_ASSERT(name, expression) \
        typedef char name[(expression) ? 1 : -1]

TEXT_RENDER_LAYOUT_ASSERT(TextFontResourcePlutOffsetIs20,
                          offsetof(TextFontResourceData, glyph_plut) == 0x14);
TEXT_RENDER_LAYOUT_ASSERT(TextFontResourceLineHeightOffsetIs16,
                          offsetof(TextFontResourceData, line_height) == 0x10);
TEXT_RENDER_LAYOUT_ASSERT(TextFontResourceCcbsOffsetIs24,
                          offsetof(TextFontResourceData, glyph_ccbs) == 0x18);
TEXT_RENDER_LAYOUT_ASSERT(TextFontResourceWidthsOffsetIs40,
                          offsetof(TextFontResourceData,
                                   glyph_width_bytes) == 0x28);
TEXT_RENDER_LAYOUT_ASSERT(TextFontResourceSourcesOffsetIs232,
                          offsetof(TextFontResourceData, glyph_sources) ==
                          0xE8);
TEXT_RENDER_LAYOUT_ASSERT(TextFontResourceImageDataOffsetIs8,
                          offsetof(TextFontResourceImage, font) == 0x08);
TEXT_RENDER_LAYOUT_ASSERT(TextFontEntryResourceOffsetIs8,
                          offsetof(TextFontEntry, resource_data) == 0x08);
TEXT_RENDER_LAYOUT_ASSERT(TextFontEntryIdentifierOffsetIs20,
                          offsetof(TextFontEntry, font_identifier) == 0x14);
TEXT_RENDER_LAYOUT_ASSERT(TextFontEntryShadowXOffsetIs28,
                          offsetof(TextFontEntry, shadow_x_offset) == 0x1C);
TEXT_RENDER_LAYOUT_ASSERT(TextFontEntryShadowYOffsetIs32,
                          offsetof(TextFontEntry, shadow_y_offset) == 0x20);
TEXT_RENDER_LAYOUT_ASSERT(TextFontEntrySizeIs48,
                          sizeof(TextFontEntry) == 0x30);
TEXT_RENDER_LAYOUT_ASSERT(TextManagerActiveFontOffsetIs8,
                          offsetof(TextRenderManager, active_font) == 0x08);
TEXT_RENDER_LAYOUT_ASSERT(TextManagerGlyphCcbsOffsetIs24,
                          offsetof(TextRenderManager, glyph_ccbs) == 0x18);
TEXT_RENDER_LAYOUT_ASSERT(TextManagerLastIdentifierOffsetIs20,
                          offsetof(TextRenderManager,
                                   last_font_identifier) == 0x14);
TEXT_RENDER_LAYOUT_ASSERT(TextManagerSizeIs40,
                          sizeof(TextRenderManager) == 0x28);
TEXT_RENDER_LAYOUT_ASSERT(TextCcbPlutOffsetIs12,
                          offsetof(CCB, ccb_PLUTPtr) == 0x0C);

#undef TEXT_RENDER_LAYOUT_ASSERT

void
apply_text_shadow_glyph_plut(void);
void
restore_active_font_glyph_plut(void);
void *
allocate_text_memory(int size,
                     int flags);
int
dispose_text_memory(void *memory,
                    int   unused_size,
                    int   unused_flags);
void *
load_text_resource_file(const char *path,
                        int         memory_flags,
                        int         trailing_bytes,
                        int        *file_size);
int
initialize_text_render_manager(void);
void
dispose_text_render_manager(void);
void
relocate_text_font_resource(TextFontEntry *font);
int
load_text_font(const char *path);
int
remove_text_font_entry(int font_identifier);
int
register_text_font_resource(void *resource_image);
void
set_text_shadow_offset(int horizontal,
                       int vertical);
void
select_first_text_font(void);
void
unlink_text_font_neighbors(TextFontEntry *font);
void
detach_text_font_entry(TextFontEntry *font);
int
unload_text_font(int font_identifier);
void
set_text_color(int red_component,
               int green_component,
               int blue_component);
int
select_text_font(int font_identifier);
int
set_text_fallback_glyph_width(int glyph_width);
void
set_text_glyph_pixc_override(int enabled);
int
measure_text_width(const unsigned char *text,
                   int                  character_spacing);
int
draw_text(int bitmap_item,
          int x,
          int y,
          int text_address,
          int character_spacing,
          int alignment);
int
draw_text_glyphs(int                  bitmap_item,
                 int                  x,
                 int                  y,
                 const unsigned char *text,
                 int                  character_spacing,
                 int                  layout);

#endif
