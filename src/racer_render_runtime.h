#ifndef ROADRASH_RACER_RENDER_RUNTIME_H
#define ROADRASH_RACER_RENDER_RUNTIME_H

#ifndef offsetof
  #include "stddef.h"
#endif

#include "cans_animation_binding.h"
#include "road_projected_cel.h"
#include "rw_semantic_data.h"

typedef struct RacerRenderObject RacerRenderObject;

typedef struct RacerRenderContext
{
  unsigned char reserved_000[0xFC];
  struct RoadTextureCache *texture_cache;
} RacerRenderContext;

struct RacerRenderObject
{
  unsigned char reserved_000[0x44];
  int render_interpolation;
  int render_y_fraction;
  int render_y;
  int render_depth_fraction;
  unsigned char *resource_entry_table;
  unsigned char reserved_058[0x10];
  int render_tilt;
  unsigned char reserved_06c[0x20];
  AnimationResourcePair render_resources;
  unsigned char reserved_094[0x04];
  CelAnimationBinding *render_binding;
  unsigned char auxiliary_resource_id;
  signed char auxiliary_frame_count;
  unsigned char auxiliary_alignment[2];
  unsigned char reserved_0a0[0x08];
  RoadProjectedCelDescriptor projected_cel;
  unsigned char reserved_0c0[0x100];
  RacerRenderObject *owner;
  unsigned char reserved_1c4[0x134];
  RacerRenderObject *paired_state;
};

typedef struct RacerAuxiliaryProjectedFrame
{
  CansAnimationFrameOutput animation_frame;
  RoadProjectedCelDescriptor projected_cel;
} RacerAuxiliaryProjectedFrame;

#define RACER_RENDER_LAYOUT_ASSERT(name, expression) \
        typedef char name[(expression) ? 1 : -1]

RACER_RENDER_LAYOUT_ASSERT(RoadProjectedCelDescriptorSizeIs18,
                           sizeof(RoadProjectedCelDescriptor) == 0x18);
RACER_RENDER_LAYOUT_ASSERT(RacerRenderInterpolationOffsetIs44,
                           offsetof(RacerRenderObject,
                                    render_interpolation) == 0x44);
RACER_RENDER_LAYOUT_ASSERT(RacerRenderResourceTableOffsetIs54,
                           offsetof(RacerRenderObject,
                                    resource_entry_table) == 0x54);
RACER_RENDER_LAYOUT_ASSERT(RacerRenderTiltOffsetIs68,
                           offsetof(RacerRenderObject, render_tilt) == 0x68);
RACER_RENDER_LAYOUT_ASSERT(RacerRenderResourcesOffsetIs8c,
                           offsetof(RacerRenderObject,
                                    render_resources) == 0x8C);
RACER_RENDER_LAYOUT_ASSERT(RacerRenderBindingOffsetIs98,
                           offsetof(RacerRenderObject,
                                    render_binding) == 0x98);
RACER_RENDER_LAYOUT_ASSERT(RacerRenderAuxiliaryOffsetIs9c,
                           offsetof(RacerRenderObject,
                                    auxiliary_resource_id) == 0x9C);
RACER_RENDER_LAYOUT_ASSERT(RacerProjectedCelOffsetIsa8,
                           offsetof(RacerRenderObject,
                                    projected_cel) == 0xA8);
RACER_RENDER_LAYOUT_ASSERT(RacerRenderOwnerOffsetIs1c0,
                           offsetof(RacerRenderObject, owner) == 0x1C0);
RACER_RENDER_LAYOUT_ASSERT(RacerRenderPairedStateOffsetIs2f8,
                           offsetof(RacerRenderObject,
                                    paired_state) == 0x2F8);
RACER_RENDER_LAYOUT_ASSERT(RacerAuxiliaryProjectedFrameSizeIs54,
                           sizeof(RacerAuxiliaryProjectedFrame) == 0x54);

#undef RACER_RENDER_LAYOUT_ASSERT

#endif
