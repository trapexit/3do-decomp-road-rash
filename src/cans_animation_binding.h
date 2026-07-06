#ifndef ROADRASH_CANS_ANIMATION_BINDING_H
#define ROADRASH_CANS_ANIMATION_BINDING_H

#ifndef offsetof
  #include "stddef.h"
#endif

#include "front_end_cel_runtime.h"
#include "road_projected_cel.h"
#include "rw_semantic_data.h"

enum { CANS_ANIMATION_CHANNEL_RECORD_BYTES = 28 };

typedef struct CansAnimationGroup
{
  int entry_number;
  int variant_count;
  int duration;
  int reserved;
} CansAnimationGroup;

typedef struct CansAnimationNameRecord
{
  int resource_key;
  int metadata[3];
} CansAnimationNameRecord;

typedef struct CansAnimationChannelRecord
{
  unsigned char bytes[CANS_ANIMATION_CHANNEL_RECORD_BYTES];
} CansAnimationChannelRecord;

typedef struct CansAnimationRenderObject
{
  unsigned char reserved_000[0x44];
  int render_phase;
  int vertical_position_fraction;
  int vertical_position;
  int depth_fraction;
  unsigned char reserved_054[0x14];
  int tilt_angle;
  int bank_angle;
  unsigned char reserved_070[0x1C];
  AnimationResourcePair resources;
  unsigned char reserved_094[0x04];
  CelAnimationBinding *binding;
  unsigned char reserved_09c[0x0C];
  RoadProjectedCelDescriptor projected_cel;
  unsigned char reserved_0c0[0x04];
  int render_class;
  unsigned char reserved_0c8[0x08];
  int view_angle;
} CansAnimationRenderObject;

typedef struct CansRenderOrientation
{
  int horizontal;
  int vertical;
  int bank;
} CansRenderOrientation;

#define CANS_RENDER_LAYOUT_ASSERT(name, expression) \
        typedef char name[(expression) ? 1 : -1]

CANS_RENDER_LAYOUT_ASSERT(CansRenderResourcesOffsetIs8c,
                          offsetof(CansAnimationRenderObject,
                                   resources) == 0x8C);
CANS_RENDER_LAYOUT_ASSERT(CansRenderBindingOffsetIs98,
                          offsetof(CansAnimationRenderObject,
                                   binding) == 0x98);
CANS_RENDER_LAYOUT_ASSERT(CansRenderProjectedCelOffsetIsa8,
                          offsetof(CansAnimationRenderObject,
                                   projected_cel) == 0xA8);
CANS_RENDER_LAYOUT_ASSERT(CansRenderClassOffsetIsc4,
                          offsetof(CansAnimationRenderObject,
                                   render_class) == 0xC4);
CANS_RENDER_LAYOUT_ASSERT(CansRenderViewAngleOffsetIsd0,
                          offsetof(CansAnimationRenderObject,
                                   view_angle) == 0xD0);

#undef CANS_RENDER_LAYOUT_ASSERT

const CansAnimationGroup *
find_cans_animation_group(const AnimationResourcePair *resources,
                          int                          entry_index);
int
get_cans_animation_entry_flags(const AnimationResourcePair *resources,
                               unsigned int                 entry_index,
                               int                          channel);
const
int *
get_cans_animation_flag_table(const AnimationResourcePair *resources,
                              unsigned int                 entry_index);
const
CansAnimationNameRecord *
get_cans_animation_name_table(const AnimationResourcePair *resources,
                              int                          entry_index);
const
CansAnimationChannelRecord *
get_cans_animation_channel_record(const AnimationResourcePair *resources,
                                  int                          entry_index,
                                  int                          channel);
int
find_cans_animation_name(const AnimationResourcePair *resources,
                         int                          resource_key,
                         int                          start_index);

int
resolve_cans_animation_binding(const AnimationResourcePair *resources,
                               const AnimationBindingSpec  *spec,
                               CelAnimationBinding         *binding);
int
resolve_cans_animation_bindings(const AnimationResourcePair *resources,
                                const AnimationBindingSpec  *specs,
                                CelAnimationBinding         *bindings,
                                int                          count);
void
decode_cans_animation_frame(const AnimationResourcePair *resources,
                            int                          entry_index,
                            int                          channel_index,
                            CansAnimationFrameOutput    *frame_output);
void
calculate_cans_render_orientation(CansAnimationRenderObject        *object,
                                  CansRenderOrientation            *orientation,
                                  const RoadProjectedRenderContext *render_context);
void
prepare_cans_animation_frame_render(CansAnimationRenderObject  *object,
                                    RoadProjectedRenderContext *render_context);
int
car_animation_render(CansAnimationRenderObject  *object,
                     RoadProjectedRenderContext *render_context);

#endif
