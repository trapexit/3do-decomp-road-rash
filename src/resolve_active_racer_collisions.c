#include "limits.h"

#include "roadside_collision_runtime.h"

#include "rw_semantic_data.h"

enum ActiveRacerCollisionConstant
{
  RACER_STATUS_COLLISION_TARGET = 1,
  RACER_STATUS_COLLISION_SOURCE = 2,
  RACER_STATUS_COLLIDABLE = 4,
  RACER_STATUS_SOURCE_MASK =
    RACER_STATUS_COLLISION_SOURCE | RACER_STATUS_COLLIDABLE,
  RACER_STATUS_TARGET_MASK =
    RACER_STATUS_COLLISION_TARGET | RACER_STATUS_COLLIDABLE,
  RACER_POSITION_FRACTION_MASK = 0xFF,
  RACER_POSITION_FRACTION_CENTER = 0x80,
  ROAD_COLLISION_TOPOLOGY_MASK = 3,
  RACER_BOUNDS_UNSCANNED = -2,
  RACER_BOUNDS_UNAVAILABLE = -1
};

__inline
static
int
collision_absolute_distance(int distance)
{
  if(distance <= 0)
    {
      distance = -distance;
    }
  return distance;
}


static
int
_collision_max_half_length(const RacerEntity *first_,
                           int                tick_)
{
  const RacerEntity *node;
  int previous_z;
  int maximum;
  int half_length;

  maximum = first_->collision_half_length_z;
  if((unsigned int)maximum > (unsigned int)(INT_MAX / 2))
    {
      return RACER_BOUNDS_UNAVAILABLE;
    }

  previous_z = first_->position_z;
  node = (const RacerEntity *)first_->list_node.links.next;
  while(node->list_node.links.next != 0)
    {
      /* Preserve the original wrapped-distance behavior outside this span. */
      if(node->position_z < previous_z ||
         ((unsigned int)node->position_z -
          (unsigned int)first_->position_z) > (unsigned int)INT_MAX)
        {
          return RACER_BOUNDS_UNAVAILABLE;
        }

      if((node->status_flags & RACER_STATUS_TARGET_MASK) ==
         RACER_STATUS_TARGET_MASK ||
         (node->last_update_tick == tick_ &&
          (node->status_flags & RACER_STATUS_SOURCE_MASK) ==
          RACER_STATUS_SOURCE_MASK))
        {
          half_length = node->collision_half_length_z;
          if((unsigned int)half_length > (unsigned int)(INT_MAX / 2))
            {
              return RACER_BOUNDS_UNAVAILABLE;
            }
          if(half_length > maximum)
            {
              maximum = half_length;
            }
        }

      previous_z = node->position_z;
      node = (const RacerEntity *)node->list_node.links.next;
    }
  return maximum;
}


int
resolve_active_racer_collisions(IntrusiveListHeader *racers)
{
  RacerEntity *current;
  RacerEntity *other;
  int tick;
  int topology_mask;
  int distance;
  int maximum_half_length;
  int last_candidate_z;

  tick = frame_tick;
  collision_positions_adjusted[0] = 0;
  maximum_half_length = RACER_BOUNDS_UNSCANNED;

  current = (RacerEntity *)racers->head_sentinel->next;
  while(current->list_node.links.next != 0)
    {
      if(current->last_update_tick == tick &&
         (current->status_flags & RACER_STATUS_SOURCE_MASK) ==
         RACER_STATUS_SOURCE_MASK)
        {
          distance =
            (current->position_z & RACER_POSITION_FRACTION_MASK) -
            RACER_POSITION_FRACTION_CENTER;
          distance = collision_absolute_distance(distance);
          if(distance < current->collision_half_length_z)
            {
              if(check_roadside_object_collisions(current) != 0 &&
                 maximum_half_length >= 0)
                {
                  maximum_half_length = RACER_BOUNDS_UNAVAILABLE;
                }
            }

          if(maximum_half_length == RACER_BOUNDS_UNSCANNED)
            {
              maximum_half_length = _collision_max_half_length(current, tick);
            }
          last_candidate_z = INT_MAX;
          if(maximum_half_length >= 0)
            {
              distance =
                (current->collision_half_length_z + maximum_half_length);
              if(current->position_z <= (INT_MAX - distance))
                {
                  last_candidate_z = (current->position_z + distance);
                }
            }

          topology_mask = current->track_segment->topology_flags;
          other = (RacerEntity *)current->list_node.links.next;
          while(other->list_node.links.next != 0)
            {
              if(other->position_z > last_candidate_z)
                {
                  break;
                }

              if((other->status_flags & RACER_STATUS_TARGET_MASK) ==
                 RACER_STATUS_TARGET_MASK)
                {
                  distance = current->position_z - other->position_z;
                  distance = collision_absolute_distance(distance);
                  if(distance <= current->collision_half_length_z +
                     other->collision_half_length_z)
                    {
                      distance = current->position_x - other->position_x;
                      distance = collision_absolute_distance(distance);
                      if(distance <= current->collision_half_width_x +
                         other->collision_half_width_x &&
                         ((topology_mask &
                           other->track_segment->topology_flags) &
                          ROAD_COLLISION_TOPOLOGY_MASK) != 0)
                        {
                          distance =
                            (current->position_y +
                             current->collision_half_height_y) -
                            (other->position_y +
                             other->collision_half_height_y);
                          distance = collision_absolute_distance(distance);
                          if(distance <=
                             current->collision_half_height_y +
                             other->collision_half_height_y)
                            {
                              /* Responses and callbacks can invalidate both
                                 ordering and extents for later candidates. */
                              maximum_half_length = RACER_BOUNDS_UNAVAILABLE;
                              last_candidate_z = INT_MAX;
                              current->contact = other;
                              other->contact = current;
                              resolve_racer_entity_collision(current, other);

                              if(current->contact_callback != 0)
                                {
                                  current->contact_callback(current);
                                }
                              if(other->contact_callback != 0)
                                {
                                  other->contact_callback(other);
                                }
                            }
                        }
                    }
                }

              other = (RacerEntity *)other->list_node.links.next;
            }
        }

      current = (RacerEntity *)current->list_node.links.next;
    }

  return collision_positions_adjusted[0];
}
