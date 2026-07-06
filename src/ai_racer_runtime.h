#ifndef ROADRASH_AI_RACER_RUNTIME_H
#define ROADRASH_AI_RACER_RUNTIME_H

#include "racer_runtime.h"
#include "track_segment_definition.h"

#ifndef offsetof
  #include "stddef.h"
#endif

typedef struct AiRacerDecisionState
{
  unsigned char reserved_000[0x160];
  int lane_selection_state;
  unsigned char row_marker;
  unsigned char current_stage;
  unsigned char lane_state_alignment[2];
  int lane_state_words[7];
  unsigned char reserved_184[0x10];
  int lane_reference_position;
} AiRacerDecisionState;

typedef struct AiTrackCursorNode
{
  int node_type;
  unsigned int node_flags;
  RoadTrackGraphNode *branch;
  unsigned char reserved_0c[0x04];
  TrackSegmentDefinition *segment_definition;
  int sample_count;
  RoadSegmentLaneRuntime *previous_lane;
  RoadSegmentLaneRuntime *next_lane;
} AiTrackCursorNode;

typedef struct AiLaneResourcePosition
{
  unsigned char reserved_00[0x08];
  int track_position;
} AiLaneResourcePosition;

typedef struct AiTrackObjectPosition
{
  int track_position;
  int vertical_position;
  int auxiliary_position;
  int lateral_position;
} AiTrackObjectPosition;

typedef struct AiRacerObject AiRacerObject;
struct AiRacerObject
{
  unsigned char reserved_000[0x10];
  int next_update_tick;
  int last_update_tick;
  int time_step;
  AiTrackObjectPosition position;
  RoadSegmentLaneRuntime *track_segment;
  unsigned char reserved_030[0x90];
  unsigned char status_flags;
  unsigned char status_alignment[3];
  unsigned char reserved_0c4[0x18];
  RaceObjectRenderCallback render_object;
  RaceObjectUpdateCallback prepare_update;
  unsigned char reserved_0e4[0x08];
  RacerVelocity velocity;
  unsigned char reserved_0f8[0x3C];
  int cached_track_position_a;
  int cached_track_position_b;
  RoadSegmentLaneRuntime *cached_track_segment_a;
  RoadSegmentLaneRuntime *cached_track_segment_b;
  unsigned char reserved_144[0x7C];
  AiRacerObject *owner;
  unsigned char reserved_1c4[0x10];
  AiRacerDecisionState *decision_state;
  unsigned char reserved_1d8[0x10];
  int next_rank_update_tick;
  int finish_distance;
  int race_rank;
  RaceObjectRenderCallback cached_state_render_object;
  RaceObjectUpdateCallback cached_state_prepare_update;
  unsigned char reserved_1fc[0xF4];
  int rider_mode;
  int recovery_kind;
  AiRacerObject *paired_state;
  unsigned char cursor_initialized;
  unsigned char cursor_alignment[3];
  AiTrackCursorNode *cursor_node;
  int cursor_position;
  int elapsed_time;
  int accumulated_track_delta;
  int target_speed;
  int movement_delay;
  RaceObjectRenderCallback cached_render_object;
  RaceObjectUpdateCallback cached_prepare_update;
};

#define AI_RACER_LAYOUT_ASSERT(name, expression) \
        typedef char name[(expression) ? 1 : -1]

AI_RACER_LAYOUT_ASSERT(AiRacerTimeStepOffsetIs18,
                       offsetof(AiRacerObject, time_step) == 0x18);
AI_RACER_LAYOUT_ASSERT(AiRacerNextUpdateTickOffsetIs10,
                       offsetof(AiRacerObject, next_update_tick) == 0x10);
AI_RACER_LAYOUT_ASSERT(AiRacerPositionOffsetIs1c,
                       offsetof(AiRacerObject, position) == 0x1C);
AI_RACER_LAYOUT_ASSERT(AiRacerTrackOffsetIs2c,
                       offsetof(AiRacerObject, track_segment) == 0x2C);
AI_RACER_LAYOUT_ASSERT(AiRacerStatusOffsetIsc0,
                       offsetof(AiRacerObject, status_flags) == 0xC0);
AI_RACER_LAYOUT_ASSERT(AiRacerRenderObjectOffsetIsdc,
                       offsetof(AiRacerObject, render_object) == 0xDC);
AI_RACER_LAYOUT_ASSERT(AiRacerPrepareUpdateOffsetIse0,
                       offsetof(AiRacerObject, prepare_update) == 0xE0);
AI_RACER_LAYOUT_ASSERT(AiRacerVelocityOffsetIsec,
                       offsetof(AiRacerObject, velocity) == 0xEC);
AI_RACER_LAYOUT_ASSERT(AiRacerOwnerOffsetIs1c0,
                       offsetof(AiRacerObject, owner) == 0x1C0);
AI_RACER_LAYOUT_ASSERT(AiRacerDecisionOffsetIs1d4,
                       offsetof(AiRacerObject, decision_state) == 0x1D4);
AI_RACER_LAYOUT_ASSERT(AiRacerDecisionLaneStateOffsetIs160,
                       offsetof(AiRacerDecisionState,
                                lane_selection_state) == 0x160);
AI_RACER_LAYOUT_ASSERT(AiRacerDecisionRowMarkerOffsetIs164,
                       offsetof(AiRacerDecisionState,
                                row_marker) == 0x164);
AI_RACER_LAYOUT_ASSERT(AiRacerDecisionLaneWordsOffsetIs168,
                       offsetof(AiRacerDecisionState,
                                lane_state_words) == 0x168);
AI_RACER_LAYOUT_ASSERT(AiRacerDecisionLaneReferenceOffsetIs194,
                       offsetof(AiRacerDecisionState,
                                lane_reference_position) == 0x194);
AI_RACER_LAYOUT_ASSERT(AiRacerPairedStateOffsetIs2f8,
                       offsetof(AiRacerObject, paired_state) == 0x2F8);
AI_RACER_LAYOUT_ASSERT(AiRacerCursorFlagOffsetIs2fc,
                       offsetof(AiRacerObject, cursor_initialized) == 0x2FC);
AI_RACER_LAYOUT_ASSERT(AiRacerCursorOffsetIs300,
                       offsetof(AiRacerObject, cursor_node) == 0x300);
AI_RACER_LAYOUT_ASSERT(AiRacerTargetSpeedOffsetIs310,
                       offsetof(AiRacerObject, target_speed) == 0x310);
AI_RACER_LAYOUT_ASSERT(AiRacerCachedRenderObjectOffsetIs318,
                       offsetof(AiRacerObject, cached_render_object) == 0x318);
AI_RACER_LAYOUT_ASSERT(AiRacerCachedPrepareUpdateOffsetIs31c,
                       offsetof(AiRacerObject, cached_prepare_update) == 0x31C);
AI_RACER_LAYOUT_ASSERT(AiRacerCachedStateRenderObjectOffsetIs1f4,
                       offsetof(AiRacerObject,
                                cached_state_render_object) == 0x1F4);
AI_RACER_LAYOUT_ASSERT(AiRacerCachedStatePrepareUpdateOffsetIs1f8,
                       offsetof(AiRacerObject,
                                cached_state_prepare_update) == 0x1F8);
AI_RACER_LAYOUT_ASSERT(AiRacerObjectSizeIs320,
                       sizeof(AiRacerObject) == 0x320);
AI_RACER_LAYOUT_ASSERT(AiTrackCursorBranchOffsetIs08,
                       offsetof(AiTrackCursorNode, branch) == 0x08);
AI_RACER_LAYOUT_ASSERT(AiTrackCursorFlagsOffsetIs04,
                       offsetof(AiTrackCursorNode, node_flags) == 0x04);
AI_RACER_LAYOUT_ASSERT(AiTrackCursorDefinitionOffsetIs10,
                       offsetof(AiTrackCursorNode,
                                segment_definition) == 0x10);
AI_RACER_LAYOUT_ASSERT(AiTrackCursorSampleCountOffsetIs14,
                       offsetof(AiTrackCursorNode, sample_count) == 0x14);
AI_RACER_LAYOUT_ASSERT(AiTrackCursorPreviousOffsetIs18,
                       offsetof(AiTrackCursorNode, previous_lane) == 0x18);
AI_RACER_LAYOUT_ASSERT(AiTrackCursorNextOffsetIs1c,
                       offsetof(AiTrackCursorNode, next_lane) == 0x1C);

#undef AI_RACER_LAYOUT_ASSERT

int
ai_racer_control(AiRacerObject *racer);
void
update_rider_pacing_state(char *rider);
int
update_racer_finish_distance(AiRacerObject *racer);
int
update_racer_race_rank(AiRacerObject *racer);
int
refresh_racer_race_rank_if_due(AiRacerObject *racer);
void
adjust_ai_racer_pacing_by_rank(void);

#endif
