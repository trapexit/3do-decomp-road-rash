#ifndef ROADRASH_ROADSIDE_OBJECT_RENDERING_H
#define ROADRASH_ROADSIDE_OBJECT_RENDERING_H

#include "bss_early_data.h"
#include "bss_late_data.h"
#include "road_geometry.h"

// Clear the caller's established binding state. NULL is a no-op.
void
reset_road_texture_binding_state(RoadTextureBindingState *state_);
void
reset_all_roadside_collision_binding_states(void);
void
reset_roadside_collision_binding_state(RoadSegmentLaneRuntime *lane);
int
roadside_repeated_collision_provenance_is_current(const RoadSegmentLaneRuntime *lane);
int
roadside_static_collision_provenance_is_current(const RoadSegmentLaneRuntime *lane,
                                                int                           object_index);
int
render_repeated_roadside_object_cel(RoadRenderSide *road_side_,
                                  int             placement_index_);
int
render_static_roadside_object_cel(RoadRenderSide *road_side_,
                                int             placement_index_);
int
render_repeated_roadside_objects_in_range(RoadRenderSide *road_side,
                                          int             range_start,
                                          int             range_end);
int
render_static_roadside_objects_in_range(RoadRenderSide *road_side,
                                        unsigned int    visibility_mask,
                                        int             range_start,
                                        int             range_end);
int
render_roadside_objects_in_range(RoadRenderSide *road_side,
                                 int             range_start,
                                 int             range_end);
int
render_roadside_objects_by_lane_bands(RoadRenderSide *road_side);
int
render_road_node_objects(RoadRenderNode *node);
CCB *
render_roadside_edge_cel(RoadRenderSide *road_side,
                         RoadSide        side);
CCB *
render_roadside_margin_transition(RoadRenderSide *road_side,
                                  RoadSide        side);

#endif
