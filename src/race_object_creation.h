#ifndef ROADRASH_RACE_OBJECT_CREATION_H
#define ROADRASH_RACE_OBJECT_CREATION_H

#include "intrusive_list.h"
#include "racer_runtime.h"

RacerEntity *
create_race_object_from_descriptor(IntrusiveListHeader                *object_pool,
                                   const RaceObjectCreationDescriptor *descriptor,
                                   int                                 lateral_position,
                                   int                                 track_offset,
                                   RoadSegmentLaneRuntime             *track_segment);
IntrusiveListNode *
activate_scheduled_race_object(RacerEntity *object);
IntrusiveListNode *
insert_intrusive_list_node_by_int_key(IntrusiveListHeader *list,
                                      IntrusiveListNode   *node,
                                      int                  key_offset,
                                      int                  sort_ascending);

#endif
