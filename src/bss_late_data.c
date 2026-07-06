#include "stddef.h"

#include "bss_late_data.h"

/* Compile-time ABI checks for every inferred original object shape. */
#define BSS_LATE_SIZE_CHECK(name, expression) \
        typedef char name[(expression) ? 1 : -1]

BSS_LATE_SIZE_CHECK(RoadProceduralCurvePointSize,
                    sizeof(RoadProceduralCurvePoint) == 0x08);
BSS_LATE_SIZE_CHECK(RoadProceduralCurveProfileSize,
                    sizeof(RoadProceduralCurveProfile) == 0x2000);
BSS_LATE_SIZE_CHECK(RoadRlanTraversalStateSize,
                    sizeof(RoadRlanTraversalState) == 0x1C);
BSS_LATE_SIZE_CHECK(RoadRlanEntryStartOffset,
                    offsetof(RoadRlanEntry, start_sample) == 0x00);
BSS_LATE_SIZE_CHECK(RoadRlanEntryEndOffset,
                    offsetof(RoadRlanEntry, end_sample) == 0x04);
BSS_LATE_SIZE_CHECK(RoadRlanEntryStartWidthsOffset,
                    offsetof(RoadRlanEntry, start_left_width) == 0x08);
BSS_LATE_SIZE_CHECK(RoadRlanEntryEndWidthsOffset,
                    offsetof(RoadRlanEntry, end_left_width) == 0x0A);
BSS_LATE_SIZE_CHECK(RoadRlanEntryLeftStepOffset,
                    offsetof(RoadRlanEntry, left_width_step) == 0x0C);
BSS_LATE_SIZE_CHECK(RoadRlanEntryRightStepOffset,
                    offsetof(RoadRlanEntry, right_width_step) == 0x10);
BSS_LATE_SIZE_CHECK(RoadRlanEntrySize,
                    sizeof(RoadRlanEntry) == 0x14);
BSS_LATE_SIZE_CHECK(RoadRlanResourceTagOffset,
                    offsetof(RoadRlanResource, tag) == 0x00);
BSS_LATE_SIZE_CHECK(RoadRlanResourceSizeOffset,
                    offsetof(RoadRlanResource, byte_size) == 0x04);
BSS_LATE_SIZE_CHECK(RoadRlanResourceCountOffset,
                    offsetof(RoadRlanResource, entry_count) == 0x08);
BSS_LATE_SIZE_CHECK(RoadRlanResourceEntriesOffset,
                    offsetof(RoadRlanResource, entries) == 0x10);
BSS_LATE_SIZE_CHECK(RoadRlanResourceMinimumSize,
                    sizeof(RoadRlanResource) == 0x24);
BSS_LATE_SIZE_CHECK(RoadRlanResourceOffset,
                    offsetof(RoadRlanTraversalState, resource) == 0x00);
BSS_LATE_SIZE_CHECK(RoadRlanEndPositionOffset,
                    offsetof(RoadRlanTraversalState, end_position) ==
                    0x04);
BSS_LATE_SIZE_CHECK(RoadRlanPositionOffset,
                    offsetof(RoadRlanTraversalState, track_position) ==
                    0x08);
BSS_LATE_SIZE_CHECK(RoadRlanEntryIndexOffset,
                    offsetof(RoadRlanTraversalState, entry_index) == 0x0C);
BSS_LATE_SIZE_CHECK(RoadRlanCurrentEntryOffset,
                    offsetof(RoadRlanTraversalState, current_entry) == 0x10);
BSS_LATE_SIZE_CHECK(RoadRlanLeftWidthOffset,
                    offsetof(RoadRlanTraversalState,
                             left_width_accumulator) == 0x14);
BSS_LATE_SIZE_CHECK(RoadRlanRightWidthOffset,
                    offsetof(RoadRlanTraversalState,
                             right_width_accumulator) == 0x18);
BSS_LATE_SIZE_CHECK(RoadPathSampleSize,
                    sizeof(RoadPathSample) == 0x02);
BSS_LATE_SIZE_CHECK(RoadPathResourceTagOffset,
                    offsetof(RoadPathResource, tag) == 0x00);
BSS_LATE_SIZE_CHECK(RoadPathResourceByteSizeOffset,
                    offsetof(RoadPathResource, byte_size) == 0x04);
BSS_LATE_SIZE_CHECK(RoadPathResourceSampleCountOffset,
                    offsetof(RoadPathResource, sample_count) == 0x08);
BSS_LATE_SIZE_CHECK(RoadPathResourceStartElevationOffset,
                    offsetof(RoadPathResource, start_elevation) == 0x0C);
BSS_LATE_SIZE_CHECK(RoadPathResourceEndElevationOffset,
                    offsetof(RoadPathResource, end_elevation) == 0x0E);
BSS_LATE_SIZE_CHECK(RoadPathResourceSamplesOffset,
                    offsetof(RoadPathResource, samples) == 0x10);
BSS_LATE_SIZE_CHECK(RoadPathTraversalStateSize,
                    sizeof(RoadPathTraversalState) == 0x0C);
BSS_LATE_SIZE_CHECK(RoadPathResourceOffset,
                    offsetof(RoadPathTraversalState, resource) == 0x00);
BSS_LATE_SIZE_CHECK(RoadPathElevationOffset,
                    offsetof(RoadPathTraversalState, elevation) == 0x04);
BSS_LATE_SIZE_CHECK(RoadPathTrackPositionOffset,
                    offsetof(RoadPathTraversalState, track_position) == 0x08);
BSS_LATE_SIZE_CHECK(RoadRsldTraversalStateSize,
                    sizeof(RoadRsldTraversalState) == 0x24);
BSS_LATE_SIZE_CHECK(RoadRsldEntryStartOffset,
                    offsetof(RoadRsldEntry, start_sample) == 0x00);
BSS_LATE_SIZE_CHECK(RoadRsldEntryEndOffset,
                    offsetof(RoadRsldEntry, end_sample) == 0x04);
BSS_LATE_SIZE_CHECK(RoadRsldEntryStartMarginsOffset,
                    offsetof(RoadRsldEntry, start_left_margin) == 0x08);
BSS_LATE_SIZE_CHECK(RoadRsldEntryEndMarginsOffset,
                    offsetof(RoadRsldEntry, end_left_margin) == 0x0A);
BSS_LATE_SIZE_CHECK(RoadRsldEntryStartDepthsOffset,
                    offsetof(RoadRsldEntry, start_left_depth) == 0x0C);
BSS_LATE_SIZE_CHECK(RoadRsldEntryEndDepthsOffset,
                    offsetof(RoadRsldEntry, end_left_depth) == 0x0E);
BSS_LATE_SIZE_CHECK(RoadRsldEntryResourceIdsOffset,
                    offsetof(RoadRsldEntry, edge_resource_ids) == 0x10);
BSS_LATE_SIZE_CHECK(RoadRsldEntryLeftMarginStepOffset,
                    offsetof(RoadRsldEntry, left_margin_step) == 0x14);
BSS_LATE_SIZE_CHECK(RoadRsldEntryRightMarginStepOffset,
                    offsetof(RoadRsldEntry, right_margin_step) == 0x18);
BSS_LATE_SIZE_CHECK(RoadRsldEntryLeftDepthStepOffset,
                    offsetof(RoadRsldEntry, left_depth_step) == 0x1C);
BSS_LATE_SIZE_CHECK(RoadRsldEntryRightDepthStepOffset,
                    offsetof(RoadRsldEntry, right_depth_step) == 0x20);
BSS_LATE_SIZE_CHECK(RoadRsldEntrySize,
                    sizeof(RoadRsldEntry) == 0x24);
BSS_LATE_SIZE_CHECK(RoadRsldResourceTagOffset,
                    offsetof(RoadRsldResource, tag) == 0x00);
BSS_LATE_SIZE_CHECK(RoadRsldResourceSizeOffset,
                    offsetof(RoadRsldResource, byte_size) == 0x04);
BSS_LATE_SIZE_CHECK(RoadRsldResourceCountOffset,
                    offsetof(RoadRsldResource, entry_count) == 0x08);
BSS_LATE_SIZE_CHECK(RoadRsldResourceEntriesOffset,
                    offsetof(RoadRsldResource, entries) == 0x10);
BSS_LATE_SIZE_CHECK(RoadRsldResourceMinimumSize,
                    sizeof(RoadRsldResource) == 0x34);
BSS_LATE_SIZE_CHECK(RoadRsldResourceOffset,
                    offsetof(RoadRsldTraversalState, resource) == 0x00);
BSS_LATE_SIZE_CHECK(RoadRsldEndPositionOffset,
                    offsetof(RoadRsldTraversalState, end_position) == 0x04);
BSS_LATE_SIZE_CHECK(RoadRsldPositionOffset,
                    offsetof(RoadRsldTraversalState, track_position) == 0x08);
BSS_LATE_SIZE_CHECK(RoadRsldEntryIndexOffset,
                    offsetof(RoadRsldTraversalState, entry_index) == 0x0C);
BSS_LATE_SIZE_CHECK(RoadRsldCurrentEntryOffset,
                    offsetof(RoadRsldTraversalState, current_entry) == 0x10);
BSS_LATE_SIZE_CHECK(RoadRsldLeftMarginOffset,
                    offsetof(RoadRsldTraversalState,
                             left_margin_accumulator) == 0x14);
BSS_LATE_SIZE_CHECK(RoadRsldRightMarginOffset,
                    offsetof(RoadRsldTraversalState,
                             right_margin_accumulator) == 0x18);
BSS_LATE_SIZE_CHECK(RoadRsldLeftDepthOffset,
                    offsetof(RoadRsldTraversalState,
                             left_depth_accumulator) == 0x1C);
BSS_LATE_SIZE_CHECK(RoadRsldRightDepthOffset,
                    offsetof(RoadRsldTraversalState,
                             right_depth_accumulator) == 0x20);
BSS_LATE_SIZE_CHECK(RoadSectionTraversalStateSize,
                    sizeof(RoadSectionTraversalState) == 0x14);
BSS_LATE_SIZE_CHECK(RoadSectionResourceOffset,
                    offsetof(RoadSectionTraversalState, resource) == 0x00);
BSS_LATE_SIZE_CHECK(RoadSectionPositionOffset,
                    offsetof(RoadSectionTraversalState, position) == 0x04);
BSS_LATE_SIZE_CHECK(RoadSectionEntryIndexOffset,
                    offsetof(RoadSectionTraversalState, entry_index) == 0x08);
BSS_LATE_SIZE_CHECK(RoadSectionActiveKindOffset,
                    offsetof(RoadSectionTraversalState, active_kind) == 0x0C);
BSS_LATE_SIZE_CHECK(RoadSectionActiveEntryOffset,
                    offsetof(RoadSectionTraversalState, active_entry) == 0x10);
BSS_LATE_SIZE_CHECK(RoadFamilyScheduleEntrySize,
                    sizeof(RoadFamilyScheduleEntry) == 0x0C);
BSS_LATE_SIZE_CHECK(RoadFamilyScheduleEntrySampleOffset,
                    offsetof(RoadFamilyScheduleEntry,
                             sample_index) == 0x00);
BSS_LATE_SIZE_CHECK(RoadFamilyScheduleEntryStateOffset,
                    offsetof(RoadFamilyScheduleEntry,
                             boundary_state) == 0x04);
BSS_LATE_SIZE_CHECK(RoadFamilyScheduleEntryGroupOffset,
                    offsetof(RoadFamilyScheduleEntry,
                             family_group_index) == 0x05);
BSS_LATE_SIZE_CHECK(RoadFamilyScheduleEntryIdOffset,
                    offsetof(RoadFamilyScheduleEntry,
                             packed_family_id) == 0x08);
BSS_LATE_SIZE_CHECK(RoadFamilyScheduleResourceTagOffset,
                    offsetof(RoadFamilyScheduleResource, tag) == 0x00);
BSS_LATE_SIZE_CHECK(RoadFamilyScheduleResourceSizeOffset,
                    offsetof(RoadFamilyScheduleResource,
                             byte_size) == 0x04);
BSS_LATE_SIZE_CHECK(RoadFamilyScheduleResourceCountOffset,
                    offsetof(RoadFamilyScheduleResource,
                             entry_count) == 0x08);
BSS_LATE_SIZE_CHECK(RoadFamilyScheduleResourceGroupCountOffset,
                    offsetof(RoadFamilyScheduleResource,
                             family_group_count) == 0x0C);
BSS_LATE_SIZE_CHECK(RoadFamilyScheduleResourceTableSizeOffset,
                    offsetof(RoadFamilyScheduleResource,
                             family_table_size_kib) == 0x0D);
BSS_LATE_SIZE_CHECK(RoadFamilyScheduleResourceBiasOffset,
                    offsetof(RoadFamilyScheduleResource,
                             forward_schedule_bias) == 0x0E);
BSS_LATE_SIZE_CHECK(RoadFamilyScheduleResourceEntriesOffset,
                    offsetof(RoadFamilyScheduleResource,
                             entries) == 0x18);
BSS_LATE_SIZE_CHECK(RoadFamilyScheduleResourceMinimumSize,
                    sizeof(RoadFamilyScheduleResource) == 0x24);
BSS_LATE_SIZE_CHECK(RoadFamilyScheduleTraversalStateSize,
                    sizeof(RoadFamilyScheduleTraversalState) == 0x14);
BSS_LATE_SIZE_CHECK(RoadFamilyScheduleTraversalResourceOffset,
                    offsetof(RoadFamilyScheduleTraversalState,
                             resource) == 0x00);
BSS_LATE_SIZE_CHECK(RoadFamilyScheduleTraversalPositionOffset,
                    offsetof(RoadFamilyScheduleTraversalState,
                             track_position) == 0x04);
BSS_LATE_SIZE_CHECK(RoadFamilyScheduleTraversalIndexOffset,
                    offsetof(RoadFamilyScheduleTraversalState,
                             entry_index) == 0x08);
BSS_LATE_SIZE_CHECK(RoadFamilyScheduleTraversalSampleOffset,
                    offsetof(RoadFamilyScheduleTraversalState,
                             sample_index) == 0x0C);
BSS_LATE_SIZE_CHECK(RoadFamilyScheduleTraversalDirectionOffset,
                    offsetof(RoadFamilyScheduleTraversalState,
                             traversal_direction) == 0x10);
BSS_LATE_SIZE_CHECK(RoadObjectResourceTagOffset,
                    offsetof(RoadObjectResource, tag) == 0x00);
BSS_LATE_SIZE_CHECK(RoadObjectResourceByteSizeOffset,
                    offsetof(RoadObjectResource, byte_size) == 0x04);
BSS_LATE_SIZE_CHECK(RoadObjectResourceEntryCountOffset,
                    offsetof(RoadObjectResource, entry_count) == 0x08);
BSS_LATE_SIZE_CHECK(RoadObjectResourceSampleCountOffset,
                    offsetof(RoadObjectResource, sample_count) == 0x0C);
BSS_LATE_SIZE_CHECK(RoadObjectResourceReservedOffset,
                    offsetof(RoadObjectResource, reserved_10) == 0x10);
BSS_LATE_SIZE_CHECK(RoadObjectResourceEntriesOffset,
                    offsetof(RoadObjectResource, entries) == 0x14);
BSS_LATE_SIZE_CHECK(RoadObjectTraversalStateSize,
                    sizeof(RoadObjectTraversalState) == 0x30);
BSS_LATE_SIZE_CHECK(RoadObjectTraversalResourceOffset,
                    offsetof(RoadObjectTraversalState, resource) == 0x00);
BSS_LATE_SIZE_CHECK(RoadObjectTraversalPositionOffset,
                    offsetof(RoadObjectTraversalState, track_position) ==
                    0x08);
BSS_LATE_SIZE_CHECK(RoadObjectTraversalEntryIndexOffset,
                    offsetof(RoadObjectTraversalState, entry_index) ==
                    0x0C);
BSS_LATE_SIZE_CHECK(RoadObjectTraversalAlternateSidesOffset,
                    offsetof(RoadObjectTraversalState,
                             alternate_sides) == 0x14);
BSS_LATE_SIZE_CHECK(RoadObjectTraversalMirrorSpriteOffset,
                    offsetof(RoadObjectTraversalState, mirror_sprite) ==
                    0x15);
BSS_LATE_SIZE_CHECK(RoadObjectTraversalWideSpacingOffset,
                    offsetof(RoadObjectTraversalState,
                             wide_spacing) == 0x16);
BSS_LATE_SIZE_CHECK(RoadObjectTraversalBaseSideOffset,
                    offsetof(RoadObjectTraversalState, base_left_side) ==
                    0x17);
BSS_LATE_SIZE_CHECK(RoadObjectTraversalRowCountOffset,
                    offsetof(RoadObjectTraversalState,
                             row_count_minus_one) == 0x18);
BSS_LATE_SIZE_CHECK(RoadObjectTraversalSelectorOffset,
                    offsetof(RoadObjectTraversalState,
                             resource_selector) == 0x19);
BSS_LATE_SIZE_CHECK(RoadObjectTraversalRowSpacingOffset,
                    offsetof(RoadObjectTraversalState,
                             row_spacing_minus_one) ==
                    0x1B);
BSS_LATE_SIZE_CHECK(RoadObjectTraversalRandomSpacingOffset,
                    offsetof(RoadObjectTraversalState,
                             randomize_lateral_spacing) == 0x1C);
BSS_LATE_SIZE_CHECK(RoadObjectTraversalColumnCountOffset,
                    offsetof(RoadObjectTraversalState,
                             column_count_minus_one) == 0x1D);
BSS_LATE_SIZE_CHECK(RoadObjectTraversalScaleOffset,
                    offsetof(RoadObjectTraversalState, scale_eighths) ==
                    0x1E);
BSS_LATE_SIZE_CHECK(RoadObjectTraversalSpacingOffset,
                    offsetof(RoadObjectTraversalState, lateral_spacing) ==
                    0x20);
BSS_LATE_SIZE_CHECK(RoadObjectTraversalStartOffset,
                    offsetof(RoadObjectTraversalState,
                             active_start_sample) == 0x24);
BSS_LATE_SIZE_CHECK(RoadObjectTraversalEndOffset,
                    offsetof(RoadObjectTraversalState,
                             active_end_sample) == 0x28);
BSS_LATE_SIZE_CHECK(RoadObjectTraversalCurrentOffset,
                    offsetof(RoadObjectTraversalState, current_sample) ==
                    0x2C);
BSS_LATE_SIZE_CHECK(RoadTerrainSurfaceSampleSize,
                    sizeof(RoadTerrainSurfaceSample) == 0x02);
BSS_LATE_SIZE_CHECK(RoadTerrainResourceHeaderSize,
                    sizeof(RoadTerrainResourceHeader) == 0x08);
BSS_LATE_SIZE_CHECK(RoadTerrainSurfaceSamplesResourceOffset,
                    offsetof(RoadTerrainSurfaceResource, samples) == 0x10);
BSS_LATE_SIZE_CHECK(RoadTerrainEdgeEntrySize,
                    sizeof(RoadTerrainEdgeEntry) == 0x10);
BSS_LATE_SIZE_CHECK(RoadTerrainEdgeEntryStartOffset,
                    offsetof(RoadTerrainEdgeEntry, start_sample) == 0x00);
BSS_LATE_SIZE_CHECK(RoadTerrainEdgeEntryInnerRadiusOffset,
                    offsetof(RoadTerrainEdgeEntry, inner_step_radius) ==
                    0x05);
BSS_LATE_SIZE_CHECK(RoadTerrainEdgeEntryInnerMidpointOffset,
                    offsetof(RoadTerrainEdgeEntry, inner_midpoint) == 0x06);
BSS_LATE_SIZE_CHECK(RoadTerrainEdgeEntryOuterRadiusOffset,
                    offsetof(RoadTerrainEdgeEntry, outer_step_radius) ==
                    0x07);
BSS_LATE_SIZE_CHECK(RoadTerrainEdgeEntryOuterMidpointOffset,
                    offsetof(RoadTerrainEdgeEntry, outer_midpoint_raw) ==
                    0x08);
BSS_LATE_SIZE_CHECK(RoadTerrainEdgeEntryFixedWidthOffset,
                    offsetof(RoadTerrainEdgeEntry, slope_width_raw) ==
                    0x09);
BSS_LATE_SIZE_CHECK(RoadTerrainEdgeEntryHeightRadiusOffset,
                    offsetof(RoadTerrainEdgeEntry, height_step_radius) ==
                    0x0A);
BSS_LATE_SIZE_CHECK(RoadTerrainEdgeEntryHeightMidpointOffset,
                    offsetof(RoadTerrainEdgeEntry, height_midpoint_raw) ==
                    0x0B);
BSS_LATE_SIZE_CHECK(RoadTerrainEdgeEntryBaseHeightOffset,
                    offsetof(RoadTerrainEdgeEntry, base_height) == 0x0C);
BSS_LATE_SIZE_CHECK(RoadTerrainEdgeResourceEntriesOffset,
                    offsetof(RoadTerrainEdgeResource, entries) == 0x18);
BSS_LATE_SIZE_CHECK(RoadHillTerrainControlSize,
                    sizeof(RoadHillTerrainControl) == 0x08);
BSS_LATE_SIZE_CHECK(RoadHillTerrainDefinitionControlsOffset,
                    offsetof(RoadHillTerrainDefinition, controls) == 0x0C);
BSS_LATE_SIZE_CHECK(RoadHillTerrainDefinitionSize,
                    sizeof(RoadHillTerrainDefinition) == 0x4C);
BSS_LATE_SIZE_CHECK(RoadHillTerrainResourceDefinitionsOffset,
                    offsetof(RoadHillTerrainResource, definitions) == 0x14);
BSS_LATE_SIZE_CHECK(RoadHillTerrainPointSize,
                    sizeof(RoadHillTerrainPoint) == 0x08);
BSS_LATE_SIZE_CHECK(RoadTerrainEdgeVariantSize,
                    sizeof(RoadTerrainEdgeVariantState) == 0x2C);
BSS_LATE_SIZE_CHECK(RoadHillTerrainVariantSize,
                    sizeof(RoadHillTerrainVariantState) == 0xF0);
BSS_LATE_SIZE_CHECK(RoadTerrainVariantSize,
                    sizeof(RoadTerrainVariantState) == 0xF0);
BSS_LATE_SIZE_CHECK(RoadTerrainTraversalStateSize,
                    sizeof(RoadTerrainTraversalState) == 0x104);
BSS_LATE_SIZE_CHECK(RoadTerrainResourceOffset,
                    offsetof(RoadTerrainTraversalState, resource) == 0x00);
BSS_LATE_SIZE_CHECK(RoadTerrainTagOffset,
                    offsetof(RoadTerrainTraversalState, resource_tag) ==
                    0x04);
BSS_LATE_SIZE_CHECK(RoadTerrainSurfaceSamplesOffset,
                    offsetof(RoadTerrainTraversalState,
                             surface_resource_samples) == 0x08);
BSS_LATE_SIZE_CHECK(RoadTerrainTrackPositionOffset,
                    offsetof(RoadTerrainTraversalState, track_position) ==
                    0x0C);
BSS_LATE_SIZE_CHECK(RoadTerrainSampleIndexOffset,
                    offsetof(RoadTerrainTraversalState, sample_index) ==
                    0x10);
BSS_LATE_SIZE_CHECK(RoadTerrainVariantStateOffset,
                    offsetof(RoadTerrainTraversalState, variant) ==
                    0x14);
BSS_LATE_SIZE_CHECK(RoadTerrainEdgeIndicesOffset,
                    offsetof(RoadTerrainTraversalState, variant) +
                    offsetof(RoadTerrainEdgeVariantState,
                             active_entry_indices) == 0x18);
BSS_LATE_SIZE_CHECK(RoadTerrainEdgeEntriesOffset,
                    offsetof(RoadTerrainTraversalState, variant) +
                    offsetof(RoadTerrainEdgeVariantState,
                             active_entries) == 0x20);
BSS_LATE_SIZE_CHECK(RoadTerrainEdgeInnerOffsetsOffset,
                    offsetof(RoadTerrainTraversalState, variant) +
                    offsetof(RoadTerrainEdgeVariantState,
                             inner_offsets) == 0x28);
BSS_LATE_SIZE_CHECK(RoadTerrainEdgeHeightAdjustmentsOffset,
                    offsetof(RoadTerrainTraversalState, variant) +
                    offsetof(RoadTerrainEdgeVariantState,
                             height_adjustments) == 0x30);
BSS_LATE_SIZE_CHECK(RoadTerrainEdgeOuterAdjustmentsOffset,
                    offsetof(RoadTerrainTraversalState, variant) +
                    offsetof(RoadTerrainEdgeVariantState,
                             outer_adjustments) == 0x38);
BSS_LATE_SIZE_CHECK(RoadHillTerrainDefinitionIndexOffset,
                    offsetof(RoadTerrainTraversalState, variant) +
                    offsetof(RoadHillTerrainVariantState,
                             definition_index) == 0x14);
BSS_LATE_SIZE_CHECK(RoadHillTerrainUnusedEntriesOffset,
                    offsetof(RoadTerrainTraversalState, variant) +
                    offsetof(RoadHillTerrainVariantState,
                             unused_shape_entries) == 0x20);
BSS_LATE_SIZE_CHECK(RoadHillTerrainCurrentDefinitionOffset,
                    offsetof(RoadTerrainTraversalState, variant) +
                    offsetof(RoadHillTerrainVariantState,
                             current_definition) == 0x40);
BSS_LATE_SIZE_CHECK(RoadHillTerrainIntervalStartsOffset,
                    offsetof(RoadTerrainTraversalState, variant) +
                    offsetof(RoadHillTerrainVariantState,
                             interval_starts) == 0x44);
BSS_LATE_SIZE_CHECK(RoadHillTerrainIntervalEndsOffset,
                    offsetof(RoadTerrainTraversalState, variant) +
                    offsetof(RoadHillTerrainVariantState,
                             interval_ends) == 0x64);
BSS_LATE_SIZE_CHECK(RoadHillTerrainCurrentPointsOffset,
                    offsetof(RoadTerrainTraversalState, variant) +
                    offsetof(RoadHillTerrainVariantState,
                             current_points) == 0x84);
BSS_LATE_SIZE_CHECK(RoadHillTerrainInterpolationStepsOffset,
                    offsetof(RoadTerrainTraversalState, variant) +
                    offsetof(RoadHillTerrainVariantState,
                             interpolation_steps) == 0xC4);
BSS_LATE_SIZE_CHECK(RoadClipLaneIndexOffset,
                    offsetof(RoadClipPlaybackState, lane_index) == 0x00);
BSS_LATE_SIZE_CHECK(RoadClipActiveClipOffset,
                    offsetof(RoadClipPlaybackState, active_clip) == 0x04);
BSS_LATE_SIZE_CHECK(RoadClipPositionOffset,
                    offsetof(RoadClipPlaybackState, clip_position) == 0x08);
BSS_LATE_SIZE_CHECK(RoadClipCenterPositionOffset,
                    offsetof(RoadClipPlaybackState, center_position) == 0x0C);
BSS_LATE_SIZE_CHECK(RoadClipLaneWidthStateOffset,
                    offsetof(RoadClipPlaybackState, lane_width_state) ==
                    0x10);
BSS_LATE_SIZE_CHECK(RoadClipPathStateOffset,
                    offsetof(RoadClipPlaybackState, path_state) == 0x2C);
BSS_LATE_SIZE_CHECK(RoadClipSlopeGeometryStateOffset,
                    offsetof(RoadClipPlaybackState,
                             slope_geometry_state) == 0x38);
BSS_LATE_SIZE_CHECK(RoadClipSectionStateOffset,
                    offsetof(RoadClipPlaybackState, section_state) == 0x5C);
BSS_LATE_SIZE_CHECK(RoadClipFamilyScheduleStateOffset,
                    offsetof(RoadClipPlaybackState,
                             family_schedule_state) == 0x70);
BSS_LATE_SIZE_CHECK(RoadClipObjectStateOffset,
                    offsetof(RoadClipPlaybackState, object_state) == 0x84);
BSS_LATE_SIZE_CHECK(RoadClipTerrainStateOffset,
                    offsetof(RoadClipPlaybackState, terrain_state) == 0xB4);
BSS_LATE_SIZE_CHECK(RoadClipHazardTraversalOffset,
                    offsetof(RoadClipPlaybackState, hazard_traversal) ==
                    0x1B8);
BSS_LATE_SIZE_CHECK(RoadClipUnresolvedTailOffset,
                    offsetof(RoadClipPlaybackState, reserved_tail) ==
                    0x1E0);
BSS_LATE_SIZE_CHECK(RoadClipPlaybackStateSize,
                    sizeof(RoadClipPlaybackState) ==
                    ROAD_CLIP_PLAYBACK_BYTES);
BSS_LATE_SIZE_CHECK(RoadTrackPositionOffset,
                    offsetof(RoadTrackTraversalState, track_position) ==
                    0x00);
BSS_LATE_SIZE_CHECK(RoadTrackActiveChannelOffset,
                    offsetof(RoadTrackTraversalState, active_channel) ==
                    0x04);
BSS_LATE_SIZE_CHECK(RoadTrackSelectorOverrideOffset,
                    offsetof(RoadTrackTraversalState,
                             selector_override_active) == 0x08);
BSS_LATE_SIZE_CHECK(RoadTrackClipBoundaryOffset,
                    offsetof(RoadTrackTraversalState,
                             clip_boundary_crossed) == 0x09);
BSS_LATE_SIZE_CHECK(RoadTrackDirectionOffset,
                    offsetof(RoadTrackTraversalState,
                             traversal_direction) == 0x0A);
BSS_LATE_SIZE_CHECK(RoadTrackTransitionStatusOffset,
                    offsetof(RoadTrackTraversalState, transition_status) ==
                    0x0B);
BSS_LATE_SIZE_CHECK(RoadTrackLinkedClipOffset,
                    offsetof(RoadTrackTraversalState, junction_progress) ==
                    0x0C);
BSS_LATE_SIZE_CHECK(RoadTrackActiveTransitionOffset,
                    offsetof(RoadTrackTraversalState, active_transition) ==
                    0x10);
BSS_LATE_SIZE_CHECK(RoadTrackRightStepOffset,
                    offsetof(RoadTrackTraversalState, right_step) == 0x14);
BSS_LATE_SIZE_CHECK(RoadTrackLeftStepOffset,
                    offsetof(RoadTrackTraversalState, left_step) == 0x18);
BSS_LATE_SIZE_CHECK(RoadTrackChannelCountOffset,
                    offsetof(RoadTrackTraversalState, channel_count) ==
                    0x1C);
BSS_LATE_SIZE_CHECK(RoadTrackTransitionEntryOffset,
                    offsetof(RoadTrackTraversalState,
                             main_fork_channel) == 0x20);
BSS_LATE_SIZE_CHECK(RoadTrackTransitionExitOffset,
                    offsetof(RoadTrackTraversalState,
                             alternate_fork_channel) == 0x24);
BSS_LATE_SIZE_CHECK(RoadTrackChannelsOffset,
                    offsetof(RoadTrackTraversalState, channels) == 0x28);
BSS_LATE_SIZE_CHECK(RoadTrackSecondChannelOffset,
                    offsetof(RoadTrackTraversalState, channels) +
                    sizeof(RoadClipPlaybackState) == 0x21C);
BSS_LATE_SIZE_CHECK(RoadTrackTraversalStateSize,
                    sizeof(RoadTrackTraversalState) ==
                    ROAD_TRACK_TRAVERSAL_BYTES);
BSS_LATE_SIZE_CHECK(RoadTrackTransitionTraversalOffset,
                    offsetof(RoadTrackTransitionRuntime, traversal) == 0x00);
BSS_LATE_SIZE_CHECK(RoadTrackTransitionClearanceOffset,
                    offsetof(RoadTrackTransitionRuntime,
                             branch_clearance_index) == 0x410);
BSS_LATE_SIZE_CHECK(RoadTrackTransitionRuntimeSize,
                    sizeof(RoadTrackTransitionRuntime) == 0x414);
BSS_LATE_SIZE_CHECK(RoadEdgeShapeSize,
                    sizeof(RoadEdgeShape) == 0x0C);
BSS_LATE_SIZE_CHECK(RoadObjectBoundsSize,
                    sizeof(RoadObjectBounds) == 0x10);
BSS_LATE_SIZE_CHECK(StaticRoadsideObjectSize,
                    sizeof(StaticRoadsideObject) == 0x2C);
BSS_LATE_SIZE_CHECK(StaticRoadsideObjectBoundsOffset,
                    offsetof(StaticRoadsideObject, bounds) ==
                    0x04);
BSS_LATE_SIZE_CHECK(StaticRoadsideObjectPositionOffset,
                    offsetof(StaticRoadsideObject, lateral_position) ==
                    0x24);
BSS_LATE_SIZE_CHECK(StaticRoadsideObjectHeightOffset,
                    offsetof(StaticRoadsideObject, surface_height) ==
                    0x28);
BSS_LATE_SIZE_CHECK(RepeatedRoadsideObjectGroupSize,
                    sizeof(RepeatedRoadsideObjectGroup) == 0x68);
BSS_LATE_SIZE_CHECK(RoadObjectPlacementBoxesOffset,
                    offsetof(RepeatedRoadsideObjectGroup,
                             bounds) == 0x08);
BSS_LATE_SIZE_CHECK(RoadObjectPlacementPositionsOffset,
                    offsetof(RepeatedRoadsideObjectGroup,
                             lateral_positions) == 0x28);
BSS_LATE_SIZE_CHECK(RoadObjectPlacementHeightsOffset,
                    offsetof(RepeatedRoadsideObjectGroup,
                             surface_heights) == 0x48);
BSS_LATE_SIZE_CHECK(RoadSegmentLaneRuntimeSize,
                    sizeof(RoadSegmentLaneRuntime) == 0x1B0);
BSS_LATE_SIZE_CHECK(RoadSegmentTopologyFlagsOffset,
                    offsetof(RoadSegmentLaneRuntime, topology_flags) == 0x36);
BSS_LATE_SIZE_CHECK(RoadSegmentCollisionKindOffset,
                    offsetof(RoadSegmentLaneRuntime, collision_kind) == 0x38);
BSS_LATE_SIZE_CHECK(RoadSegmentGeometryModeOffset,
                    offsetof(RoadSegmentLaneRuntime, geometry_mode) == 0x3C);
BSS_LATE_SIZE_CHECK(RoadSegmentStaticObjectCountOffset,
                    offsetof(RoadSegmentLaneRuntime,
                             static_object_count) == 0x3D);
BSS_LATE_SIZE_CHECK(RoadSegmentStaticObjectsOffset,
                    offsetof(RoadSegmentLaneRuntime, static_objects) ==
                    0x40);
BSS_LATE_SIZE_CHECK(RoadSegmentRepeatedObjectsOffset,
                    offsetof(RoadSegmentLaneRuntime, repeated_objects) ==
                    0x98);
BSS_LATE_SIZE_CHECK(RoadSegmentCurvatureProjectionStepOffset,
                    offsetof(RoadSegmentLaneRuntime,
                             curvature_projection_step) == 0x100);
BSS_LATE_SIZE_CHECK(RoadSegmentPathCurvatureStepOffset,
                    offsetof(RoadSegmentLaneRuntime,
                             path_curvature_step) == 0x104);
BSS_LATE_SIZE_CHECK(RoadSegmentPathElevationStepOffset,
                    offsetof(RoadSegmentLaneRuntime,
                             path_elevation_step) == 0x108);
BSS_LATE_SIZE_CHECK(RoadSegmentPathElevationOffset,
                    offsetof(RoadSegmentLaneRuntime, path_elevation) ==
                    0x10C);
BSS_LATE_SIZE_CHECK(RoadSegmentElevationProjectionStepOffset,
                    offsetof(RoadSegmentLaneRuntime,
                             elevation_projection_step) == 0x114);
BSS_LATE_SIZE_CHECK(RoadSegmentCenterPositionOffset,
                    offsetof(RoadSegmentLaneRuntime, center_position) == 0x118);
BSS_LATE_SIZE_CHECK(RoadSegmentRoadLeftOffset,
                    offsetof(RoadSegmentLaneRuntime, road_left) == 0x12C);
BSS_LATE_SIZE_CHECK(RoadSegmentShoulderLeftOffset,
                    offsetof(RoadSegmentLaneRuntime, shoulder_left) == 0x134);
BSS_LATE_SIZE_CHECK(RoadSegmentSurfaceProfileOffset,
                    offsetof(RoadSegmentLaneRuntime, surface_profile) == 0x13C);
BSS_LATE_SIZE_CHECK(RoadSegmentProfileShadingOffset,
                    offsetof(RoadSegmentLaneRuntime, profile_shading) == 0x16C);
BSS_LATE_SIZE_CHECK(RoadSegmentMarginsOffset,
                    offsetof(RoadSegmentLaneRuntime, edge_margins) == 0x184);
BSS_LATE_SIZE_CHECK(RoadSegmentEdgeDepthOffset,
                    offsetof(RoadSegmentLaneRuntime, edge_depth_offsets) ==
                    0x18C);
BSS_LATE_SIZE_CHECK(RoadSegmentSurfaceResourcesOffset,
                    offsetof(RoadSegmentLaneRuntime, surface_resource_ids) ==
                    0x194);
BSS_LATE_SIZE_CHECK(RoadSegmentEdgeResourcesOffset,
                    offsetof(RoadSegmentLaneRuntime, edge_resource_ids) ==
                    0x196);
BSS_LATE_SIZE_CHECK(RoadSegmentEdgeShapesOffset,
                    offsetof(RoadSegmentLaneRuntime, edge_shapes) == 0x198);
BSS_LATE_SIZE_CHECK(RoadSegmentRuntimeSize,
                    sizeof(RoadSegmentRuntime) == 0x390);
BSS_LATE_SIZE_CHECK(RoadSegmentLaneCountOffset,
                    offsetof(RoadSegmentRuntime, lane_count) == 0x1C);
BSS_LATE_SIZE_CHECK(RoadSegmentLeftTransitionProfileCountOffset,
                    offsetof(RoadSegmentRuntime,
                             left_transition_profile_count) == 0x1F);
BSS_LATE_SIZE_CHECK(RoadSegmentRightTransitionProfileCountOffset,
                    offsetof(RoadSegmentRuntime,
                             right_transition_profile_count) == 0x20);
BSS_LATE_SIZE_CHECK(RoadSegmentLeftStepOffset,
                    offsetof(RoadSegmentRuntime, left_step) == 0x24);
BSS_LATE_SIZE_CHECK(RoadSegmentTransitionTypeOffset,
                    offsetof(RoadSegmentRuntime, transition_type) == 0x2C);
BSS_LATE_SIZE_CHECK(RoadSegmentLanesOffset,
                    offsetof(RoadSegmentRuntime, lanes) == 0x30);
BSS_LATE_SIZE_CHECK(RoadParticleSlotSize,
                    sizeof(RoadParticleSlot) == 0x18);
BSS_LATE_SIZE_CHECK(RoadSegmentIndexViewSize,
                    sizeof(RoadSegmentIndexView) == 0xB60);
BSS_LATE_SIZE_CHECK(RoadParticleSentinelViewSize,
                    sizeof(RoadParticleSentinelView) == 0xB60);
BSS_LATE_SIZE_CHECK(RoadSegmentEffectWorkspaceSize,
                    sizeof(RoadSegmentEffectWorkspace) == 0xB60);
BSS_LATE_SIZE_CHECK(EventUtilityConfigurationSize,
                    sizeof(ConfigurationRequest) == 0x6C);
BSS_LATE_SIZE_CHECK(StandardStreamSize, sizeof(FILE) == 0xA0);

RoadProceduralCurveProfile
  gRoadProceduralCurveProfiles[ROAD_PROCEDURAL_PROFILE_COUNT];
RoadTrackTraversalState gRoadTrackForwardTraversal;
RoadTrackTraversalState gRoadTrackReverseTraversal;
RoadSegmentRuntime gRoadSegments[ROAD_SEGMENT_COUNT];
RoadSegmentEffectWorkspace gRoadSegmentEffectWorkspace;
void *gRoadSurfaceCelVariants[ROAD_SURFACE_STYLE_COUNT]
[ROAD_SURFACE_CEL_VARIANTS];
char gAudioWorkingDirectory[256];
void *gLoadedAudioObjects[AUDIO_OBJECT_SLOT_COUNT];
ConfigurationRequest gEventUtilityConfiguration;
FILE gStandardInputStream;
FILE gStandardOutputStream;
FILE gStandardErrorStream;

#undef BSS_LATE_SIZE_CHECK
