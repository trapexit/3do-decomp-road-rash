#ifndef ROADRASH_TRACK_OBJECT_QUERY_H
#define ROADRASH_TRACK_OBJECT_QUERY_H

#include "track_object_runtime.h"

TrackInteractionObject *
find_nearby_track_object_in_direction(TrackInteractionObject *origin,
                                      unsigned int            collision_mask,
                                      int                     search_direction,
                                      int                     maximum_distance);
TrackInteractionObject *
find_nearest_lateral_track_object(TrackInteractionObject *origin,
                                  unsigned int            collision_mask,
                                  int                     lateral_side);
TrackInteractionObject *
find_nearest_longitudinal_track_object(TrackInteractionObject *origin,
                                       unsigned int            collision_mask,
                                       int                     maximum_distance);
TrackInteractionObject *
find_track_object_in_direction(TrackInteractionObject *origin,
                               unsigned int            collision_mask,
                               int                     relative_mode,
                               int                     maximum_distance);
TrackInteractionObject *
find_track_object_in_lane(TrackInteractionObject *origin,
                          int                     lane,
                          int                     relative_mode,
                          unsigned int            collision_mask,
                          int                     maximum_distance);

#endif
