#include "stddef.h"

#include "bss_early_data.h"

/* Two saved riders, ten rider types per save, and six challenge records. */
OpponentProgressionRecord
  gRiderProgressionProfiles[RIDER_PROFILE_COUNT][RIDER_TYPE_COUNT];
OpponentProgressionRecord
  gChallengeProgressionProfiles[CHALLENGE_PROFILE_COUNT];

/* Front-end CEL resource parent/child relationships. */
FrontEndCelHierarchyEntry gFrontEndCelHierarchy[FRONT_END_CEL_COUNT];

/* Audio setup path and the ten-entry positional-event ring. */
char gAudioDirectoryPath[AUDIO_DIRECTORY_PATH_SIZE];
SpatialAudioEvent gSpatialEventQueue[SPATIAL_EVENT_CAPACITY];

/* Timed racer work and the visible track-node selection list. */
PendingRacerEvent gPendingRacerEvents[PENDING_RACER_EVENT_CAPACITY];
struct RacerEntity *gVisibleTrackNodes[VISIBLE_TRACK_NODE_CAPACITY];

/* The renderer owns 34 road nodes and two texture caches per node. */
RoadRenderNode gRoadRenderNodes[ROAD_RENDER_NODE_COUNT];
RoadTextureCache gRoadTextureCachePool[ROAD_TEXTURE_CACHE_COUNT];
RoadTextureCache gRoadTextureCacheTemplate;
RoadTextureBindingState
  gRoadTextureBindingStates[ROAD_TEXTURE_CACHE_COUNT];
int gRoadRenderDepths[ROAD_RENDER_DEPTH_COUNT];

/* CANS animation bindings populated from the named rider resources. */
CelAnimationBinding gPlayerOneFrontFlipBindings[7];
CelAnimationBinding gPlayerOneRollBindings[8];
CelAnimationBinding gPlayerOneRunBindings[6];
CelAnimationBinding gPlayerTwoFrontFlipBindings[7];

/* Generated CEL PRE0/PRE1 encodings and the 16 by 32 PIXC shade matrix. */
unsigned int gCelDimensionPreambleTable[CEL_DIMENSION_ENTRY_COUNT];
unsigned int
  gCelPixcShadingTable[CEL_PIXC_SHADE_ROWS][CEL_PIXC_SHADE_COLUMNS];

typedef char OpponentProgressionRecordSizeIs472[
    sizeof(OpponentProgressionRecord) == 472 ? 1 : -1];
typedef char FrontEndCelHierarchyEntrySizeIs12[
    sizeof(FrontEndCelHierarchyEntry) == 12 ? 1 : -1];
typedef char SpatialAudioEventSizeIs28[
    sizeof(SpatialAudioEvent) == 28 ? 1 : -1];
typedef char SpatialAudioEventTypeOffsetIs0[
    offsetof(SpatialAudioEvent, event_type) == 0x00 ? 1 : -1];
typedef char SpatialAudioEventPositionXOffsetIs4[
    offsetof(SpatialAudioEvent, position_x) == 0x04 ? 1 : -1];
typedef char SpatialAudioEventPositionYOffsetIs8[
    offsetof(SpatialAudioEvent, position_y) == 0x08 ? 1 : -1];
typedef char SpatialAudioEventPositionZOffsetIs12[
    offsetof(SpatialAudioEvent, position_z) == 0x0C ? 1 : -1];
typedef char SpatialAudioEventSampleImageOffsetIs16[
    offsetof(SpatialAudioEvent, sample_image) == 0x10 ? 1 : -1];
typedef char SpatialAudioEventReservedZeroOffsetIs20[
    offsetof(SpatialAudioEvent, reserved_zero) == 0x14 ? 1 : -1];
typedef char SpatialAudioEventForwardVelocityOffsetIs24[
    offsetof(SpatialAudioEvent, forward_velocity) == 0x18 ? 1 : -1];
typedef char PendingRacerEventSizeIs8[
    sizeof(PendingRacerEvent) == 8 ? 1 : -1];
typedef char RoadRenderSideSizeIs260[
    sizeof(RoadRenderSide) == 260 ? 1 : -1];
typedef char RoadRenderSideProjectedPathElevationOffsetIs176[
  offsetof(RoadRenderSide, projected_path_elevation) == 0xB0 ? 1 : -1];
typedef char RoadRenderSideTextureBindingStateOffsetIs228[
  offsetof(RoadRenderSide, texture_binding_state) == 0xE4 ? 1 : -1];
typedef char RoadRenderNodeSizeIs548[
    sizeof(RoadRenderNode) == 548 ? 1 : -1];
typedef char RoadTextureCacheSizeIs648[
    sizeof(RoadTextureCache) == 648 ? 1 : -1];
typedef char RoadProjectionCursorSizeIs40[
    sizeof(RoadProjectionCursor) == 40 ? 1 : -1];
typedef char CelAnimationBindingSizeIs20[
    sizeof(CelAnimationBinding) == 20 ? 1 : -1];
typedef char CelAnimationBindingEntryIndexOffsetIs0[
    offsetof(CelAnimationBinding, entry_index) == 0x00 ? 1 : -1];
typedef char CelAnimationBindingChannelOffsetIs4[
    offsetof(CelAnimationBinding, channel) == 0x04 ? 1 : -1];
typedef char CelAnimationBindingGridFlagOffsetIs9[
    offsetof(CelAnimationBinding, uses_variant_grid) == 0x09 ? 1 : -1];
typedef char CelAnimationBindingDurationOffsetIs12[
    offsetof(CelAnimationBinding, duration_ticks) == 0x0C ? 1 : -1];
typedef char CelAnimationBindingTransitionOffsetIs13[
    offsetof(CelAnimationBinding, transition_command) == 0x0D ? 1 : -1];
typedef char CelAnimationBindingFrameFlagsOffsetIs16[
    offsetof(CelAnimationBinding, frame_flags) == 0x10 ? 1 : -1];

typedef char RoadTextureCacheSourcesOffsetIs4[
    offsetof(RoadTextureCache, surface_sources) == 0x04 ? 1 : -1];
typedef char RoadTextureCacheReservedSourceOffsetIs84[
    offsetof(RoadTextureCache, reserved_source) == 0x54 ? 1 : -1];
typedef char RoadTextureCacheRepeatedAnimationOffsetIs88[
    offsetof(RoadTextureCache, repeated_object_animation) == 0x58 ? 1 : -1];
typedef char RoadTextureCacheRepeatedFramesOffsetIs92[
    offsetof(RoadTextureCache, repeated_object_frames) == 0x5C ? 1 : -1];
typedef char RoadTextureCacheStaticAnimationsOffsetIs272[
    offsetof(RoadTextureCache, static_object_animations) == 0x110 ? 1 : -1];
typedef char RoadTextureCacheStaticFramesOffsetIs280[
    offsetof(RoadTextureCache, static_object_frames) == 0x118 ? 1 : -1];
typedef char RoadTextureCacheRacerAuxiliaryAnimationsOffsetIs640[
    offsetof(RoadTextureCache, racer_auxiliary_animations) == 0x280 ? 1 : -1];
typedef char RoadRenderSideLaneOffsetIs12[
    offsetof(RoadRenderSide, lane) == 0x0C ? 1 : -1];
typedef char RoadRenderSideDepthOffsetIs24[
    offsetof(RoadRenderSide, projection_depth) == 0x18 ? 1 : -1];
typedef char RoadRenderSideScaleOffsetIs32[
    offsetof(RoadRenderSide, projection_scale) == 0x20 ? 1 : -1];
typedef char RoadRenderSideOriginOffsetIs72[
    offsetof(RoadRenderSide, projection_origin_x) == 0x48 ? 1 : -1];
typedef char RoadRenderSideHorizonOffsetIs76[
    offsetof(RoadRenderSide, horizon_y) == 0x4C ? 1 : -1];
typedef char RoadRenderSideLaneEdgesOffsetIs80[
    offsetof(RoadRenderSide, lane_edges) == 0x50 ? 1 : -1];
typedef char RoadRenderSideModeGeometryOffsetIs112[
    offsetof(RoadRenderSide, mode_geometry) == 0x70 ? 1 : -1];
typedef char RoadRenderSideAttachmentOffsetIs160[
    offsetof(RoadRenderSide, attachment_points) == 0xA0 ? 1 : -1];
typedef char RoadRenderSideCurveProfileOffsetIs180[
    offsetof(RoadRenderSide, curve_profile) == 0xB4 ? 1 : -1];
typedef char RoadRenderSideCurveControlOffsetIs244[
    offsetof(RoadRenderSide, curve_control) == 0xF4 ? 1 : -1];
typedef char RoadRenderSideCacheOffsetIs252[
    offsetof(RoadRenderSide, texture_cache) == 0xFC ? 1 : -1];
typedef char RoadRenderSideActiveOffsetIs256[
    offsetof(RoadRenderSide, active) == 0x100 ? 1 : -1];
typedef char RoadRenderNodeLeftSideOffsetIs20[
    offsetof(RoadRenderNode, left_side) == 0x14 ? 1 : -1];
typedef char RoadRenderNodeSidesOffsetIs28[
    offsetof(RoadRenderNode, sides) == 0x1C ? 1 : -1];
typedef char RoadProjectionCursorDepthOffsetIs16[
    offsetof(RoadProjectionCursor, projection_depth) == 0x10 ? 1 : -1];
typedef char RoadProjectionCursorAttachmentOffsetIs32[
    offsetof(RoadProjectionCursor, left_attachment) == 0x20 ? 1 : -1];
