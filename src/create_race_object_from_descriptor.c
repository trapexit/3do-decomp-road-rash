
#include "race_object_creation.h"

IntrusiveListNode *
acquire_pooled_intrusive_node(IntrusiveListHeader *object_pool);

RacerEntity *
create_race_object_from_descriptor(IntrusiveListHeader                *object_pool,
                                   const RaceObjectCreationDescriptor *descriptor,
                                   int                                 lateral_position,
                                   int                                 track_offset,
                                   RoadSegmentLaneRuntime             *track_segment)
{
  RoadSegmentRuntime *segment;
  RacerEntity *object;
  int track_position;

  object = (RacerEntity *)acquire_pooled_intrusive_node(object_pool);
  if(object == 0)
    {
      return 0;
    }

  object->status_flags = descriptor->status_flags;
  object->collision_class = descriptor->collision_class;
  object->render_object =
    (RaceObjectRenderCallback)descriptor->render_object;
  object->prepare_update =
    (RaceObjectUpdateCallback)descriptor->prepare_update;
  object->perform_update =
    (RaceObjectUpdateCallback)descriptor->perform_update;
  object->reset_object =
    (RaceObjectResetCallback)descriptor->reset_object;

  object->update_interval_ticks = descriptor->update_interval_ticks;
  object->last_update_tick =
    frame_tick - descriptor->update_interval_ticks;
  object->next_update_tick = frame_tick;

  object->time_step = 0;
  object->reserved_0a8 = 0;
  object->animation_state_target = 0;
  object->reserved_0b4 = 0;
  object->reserved_0b0 = 0;
  object->render_position_z = 0;

  segment = (RoadSegmentRuntime *)track_segment->resource_handle;
  track_position = segment->track_position + track_offset;
  object->position_z = track_position;
  object->position_y = 0;
  object->position_aux = sample_road_cross_section_height(
    track_segment, lateral_position, (unsigned int)track_position);
  object->position_x = lateral_position;
  object->track_segment = track_segment;

  object->previous_position_z = object->position_z;
  object->previous_position_y = object->position_y;
  object->previous_position_aux = object->position_aux;
  object->previous_position_x = object->position_x;
  object->previous_track_segment = object->track_segment;
  object->orientation.base_heading = 0;
  object->orientation.movement_heading = 0;
  object->orientation.steering_heading = 0;

  activate_scheduled_race_object(object);
  return object;
}


void
insert_intrusive_list_node_after(IntrusiveListNode *position,
                                 IntrusiveListNode *node)
{
  IntrusiveListNode *next;

  next = position->next;
  next->previous = node;
  node->next = next;
  position->next = node;
  node->previous = position;
}
