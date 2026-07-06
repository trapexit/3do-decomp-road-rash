#ifndef ROADRASH_TRACK_EFFECT_RUNTIME_H
#define ROADRASH_TRACK_EFFECT_RUNTIME_H

#ifndef offsetof
  #include "stddef.h"
#endif

#include "race_object_creation.h"
#include "road_projected_cel.h"
#include "road_segment_runtime.h"
#include "rw_semantic_data.h"

enum TrackEffectRuntimeCount
{
  TRACK_EFFECT_CHILD_COUNT = 4,
  TRACK_EFFECT_PHASE_COUNT = 3,
  TRACK_EFFECT_CHILD_POOL_CAPACITY = 40,
  TRACK_EFFECT_PARENT_POOL_CAPACITY = 10,
  TRACK_EFFECT_CHILD_OBJECT_SIZE = 0x100
};

enum TrackEffectSide
{
  TRACK_EFFECT_LEFT_SIDE = 0,
  TRACK_EFFECT_RIGHT_SIDE = 1
};

typedef struct TrackEffectSpawnRecord
{
  int start_cell;
  int end_cell;
  unsigned char encoded_mode;
  signed char spread;
  signed char simulation_scale;
  signed char preserve_child_updates;
} TrackEffectSpawnRecord;

typedef RoadProjectedCelDescriptor TrackEffectRenderDescriptor;

struct TrackEffectObject;

typedef struct TrackEffectParentState
{
  struct TrackEffectObject *children[TRACK_EFFECT_CHILD_COUNT];
  int phase_durations[TRACK_EFFECT_PHASE_COUNT];
  int spawn_track_position;
  int spawn_end_position;
  unsigned char spawn_mode;
  signed char spawn_spread;
  signed char simulation_scale;
  signed char preserve_child_updates;
} TrackEffectParentState;

typedef struct TrackEffectChildState
{
  struct TrackEffectObject *parent;
  int phase_index;
  int phase_timer;
  int effect_type;
  signed char spread_offset;
  unsigned char spread_alignment[3];
} TrackEffectChildState;

typedef union TrackEffectState
{
  TrackEffectParentState parent;
  TrackEffectChildState child;
} TrackEffectState;

typedef struct TrackEffectObject
{
  unsigned char reserved_000[0x18];
  int time_step;
  int track_position;
  int vertical_position;
  int auxiliary_position;
  int lateral_position;
  RoadSegmentLaneRuntime *track_segment;
  unsigned char reserved_030[0x14];
  int render_y;
  int render_x_fraction_8_8;
  int render_x;
  int render_frame_8_8;
  unsigned char reserved_054[0x54];
  TrackEffectRenderDescriptor render_descriptor;
  unsigned char object_flags;
  unsigned char object_flags_alignment[3];
  int object_type;
  unsigned char reserved_0c8[0x14];
  TrackEffectCallback render;
  TrackEffectCallback update;
  TrackEffectCallback secondary_update;
  TrackEffectCallback destroy;
  TrackEffectState state;
} TrackEffectObject;

TrackEffectObject *
create_track_effect_child(int                     effect_type,
                          int                     side,
                          int                     interpolation_fraction,
                          RoadSegmentLaneRuntime *segment,
                          int                     spread_offset,
                          TrackEffectObject      *parent);
void
spawn_track_effect_parent(const TrackEffectSpawnRecord *record,
                          RoadSegmentLaneRuntime       *origin_segment);
int
dispose_track_effect_pools(void);
void
apply_track_effect_spawn_record(TrackEffectSpawnRecord *record,
                                RoadSegmentLaneRuntime *origin_segment);
void
track_effect_parent_update(TrackEffectObject *effect);
void
track_effect_parent_destroy(TrackEffectObject *effect);
void
track_effect_child_destroy(void);
void
track_effect_child_draw(TrackEffectObject          *effect,
                        RoadProjectedRenderContext *render_context);

#define TRACK_EFFECT_LAYOUT_ASSERT(name, expression) \
        typedef char name[(expression) ? 1 : -1]

TRACK_EFFECT_LAYOUT_ASSERT(TrackEffectTimeStepOffsetIs18,
                           offsetof(TrackEffectObject, time_step) == 0x18);
TRACK_EFFECT_LAYOUT_ASSERT(TrackEffectTrackPositionOffsetIs1c,
                           offsetof(TrackEffectObject, track_position) ==
                           0x1C);
TRACK_EFFECT_LAYOUT_ASSERT(TrackEffectSegmentOffsetIs2c,
                           offsetof(TrackEffectObject, track_segment) ==
                           0x2C);
TRACK_EFFECT_LAYOUT_ASSERT(TrackEffectRenderYOffsetIs44,
                           offsetof(TrackEffectObject, render_y) == 0x44);
TRACK_EFFECT_LAYOUT_ASSERT(TrackEffectRenderDescriptorOffsetIsA8,
                           offsetof(TrackEffectObject, render_descriptor) ==
                           0xA8);
TRACK_EFFECT_LAYOUT_ASSERT(TrackEffectFlagsOffsetIsC0,
                           offsetof(TrackEffectObject, object_flags) == 0xC0);
TRACK_EFFECT_LAYOUT_ASSERT(TrackEffectRenderCallbackOffsetIsDc,
                           offsetof(TrackEffectObject, render) == 0xDC);
TRACK_EFFECT_LAYOUT_ASSERT(TrackEffectStateOffsetIsEc,
                           offsetof(TrackEffectObject, state) == 0xEC);
TRACK_EFFECT_LAYOUT_ASSERT(TrackEffectParentDurationsOffsetIsFc,
                           offsetof(TrackEffectObject,
                                    state.parent.phase_durations) == 0xFC);
TRACK_EFFECT_LAYOUT_ASSERT(TrackEffectParentModeOffsetIs110,
                           offsetof(TrackEffectObject,
                                    state.parent.spawn_mode) == 0x110);
TRACK_EFFECT_LAYOUT_ASSERT(TrackEffectSpawnRecordSizeIs0c,
                           sizeof(TrackEffectSpawnRecord) == 0x0C);
TRACK_EFFECT_LAYOUT_ASSERT(TrackEffectChildPhaseOffsetIsF0,
                           offsetof(TrackEffectObject,
                                    state.child.phase_index) == 0xF0);
TRACK_EFFECT_LAYOUT_ASSERT(TrackEffectChildSpreadOffsetIsFc,
                           offsetof(TrackEffectObject,
                                    state.child.spread_offset) == 0xFC);
TRACK_EFFECT_LAYOUT_ASSERT(TrackEffectObjectSizeIs114,
                           sizeof(TrackEffectObject) == 0x114);

#undef TRACK_EFFECT_LAYOUT_ASSERT

#endif
