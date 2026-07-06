#include "intrusive_list.h"
#include "racer_runtime.h"
#include "track_object_runtime.h"

#include "assert.h"

enum HudTrackedObjectConstant
{
  HUD_TRACKED_OBJECT_KIND = 8,
  HUD_TRACKED_OBJECT_FIRST_DIRECTION = 0,
  HUD_TRACKED_OBJECT_SECOND_DIRECTION = 1,
  HUD_TRACKED_OBJECT_FIRST_RANGE = 0x4000,
  HUD_TRACKED_OBJECT_SECOND_RANGE = 0x2000,
  HUD_TRACKED_OBJECT_FIRST_DEFAULT_DISTANCE = 0x2010,
  HUD_TRACKED_OBJECT_SECOND_DEFAULT_DISTANCE = 0x4010,
  HUD_TRACKED_OBJECT_DISTANCE_WEIGHT = 2,
  HUD_TRACKED_OBJECT_MAXIMUM_DISTANCE = 0x4000
};

typedef struct RacerPositionLeadPair
{
  int position_z;
  int position_y;
} RacerPositionLeadPair;

typedef struct RacerPositionTailTriple
{
  int position_aux;
  int position_x;
  struct RoadSegmentLaneRuntime *track_segment;
} RacerPositionTailTriple;

RacerEntity *
find_nearby_track_object_in_direction(RacerEntity *origin,
                                      int          object_kind,
                                      int          direction,
                                      int          search_range);

static
int
absolute_nonpositive(int value)
{
  return value <= 0 ? -value : value;
}


// The prefix is sorted. Reproduce its adjacent-swap boundary updates while
// finding the insertion point, then move the misplaced node only once.
static
__inline
void
_reinsert_schedule_node(IntrusiveListNode *node_,
                        IntrusiveListNode *previous_,
                        IntrusiveListNode *head_)
{
  IntrusiveListNode *before;
  IntrusiveListNode *after;
  IntrusiveListNode *next;
  int position_z;

  assert(node_->previous == previous_);
  position_z = ((RacerEntity *)node_)->position_z;
  before = previous_;
  if(node_ == race_object_scheduler.first_visible_object)
    race_object_scheduler.first_visible_object = previous_;

  do
    {
      if(before == race_object_scheduler.last_visible_object)
        race_object_scheduler.last_visible_object = before->previous;
      before = before->previous;
    }
  while((before != head_) &&
        (((RacerEntity *)before)->position_z > position_z));

  after = before->next;
  next = node_->next;
  previous_->next = next;
  next->previous = previous_;
  before->next = node_;
  node_->previous = before;
  node_->next = after;
  after->previous = node_;
}


void
sort_race_object_schedule_by_track_position(IntrusiveListHeader *list)
{
  IntrusiveListNode *head;
  IntrusiveListNode *previous;
  IntrusiveListNode *node;
  IntrusiveListNode *next;
  int previous_position_z;
  int position_z;

  head = list->head_sentinel;
  previous = head->next;
  node = previous->next;
  assert(node != NULL);
  if(node->next == NULL)
    return;

  previous_position_z = ((RacerEntity *)previous)->position_z;
  do
    {
      next = node->next;
      position_z = ((RacerEntity *)node)->position_z;
      if(previous_position_z > position_z)
        {
          _reinsert_schedule_node(node, previous, head);
        }
      else
        {
          previous = node;
          previous_position_z = position_z;
        }

      node = next;
    }
  while(node->next != NULL);
}


void
update_hud_tracked_object(void)
{
  RacerEntity *player;
  RacerEntity *origin;
  RacerEntity *first;
  RacerEntity *second;
  RacerEntity *selected;
  int first_distance;
  int second_distance;
  int selected_distance;

  player = (RacerEntity *)race_rider_state.player_object;
  origin = player->paired_state;
  first = find_nearby_track_object_in_direction(
    origin, HUD_TRACKED_OBJECT_KIND,
    HUD_TRACKED_OBJECT_FIRST_DIRECTION,
    HUD_TRACKED_OBJECT_FIRST_RANGE);
  second = find_nearby_track_object_in_direction(
    origin, HUD_TRACKED_OBJECT_KIND,
    HUD_TRACKED_OBJECT_SECOND_DIRECTION,
    HUD_TRACKED_OBJECT_SECOND_RANGE);

  first_distance = HUD_TRACKED_OBJECT_FIRST_DEFAULT_DISTANCE;
  if(first != 0)
    {
      first_distance = absolute_nonpositive(
        origin->position_z - first->position_z);
    }

  selected = second;
  second_distance = HUD_TRACKED_OBJECT_SECOND_DEFAULT_DISTANCE;
  if(second != 0)
    {
      second_distance = absolute_nonpositive(
        origin->position_z - second->position_z);
    }

  selected_distance = second_distance;
  if(second_distance >
     first_distance * HUD_TRACKED_OBJECT_DISTANCE_WEIGHT)
    {
      selected = first;
      selected_distance = first_distance;
    }

  if(selected_distance <= HUD_TRACKED_OBJECT_MAXIMUM_DISTANCE &&
     selected != 0)
    {
      hud_globals.tracked_object = selected;
    }
  else
    {
      hud_globals.tracked_object = 0;
    }
}


void
snapshot_racer_entity_render_state(RacerEntity *object)
{
  object->previous_orientation = object->orientation;
  *(RacerPositionLeadPair *)&object->position_snapshot.position_z =
    *(RacerPositionLeadPair *)&object->position_z;
  *(RacerPositionTailTriple *)&object->position_snapshot.position_aux =
    *(RacerPositionTailTriple *)&object->position_aux;
}


void
snapshot_track_object_render_state(TrackObject *object)
{
  *(RacerPositionLeadPair *)&object->position_snapshot.position_z =
    *(RacerPositionLeadPair *)&object->position.track_position;
  *(RacerPositionTailTriple *)&object->position_snapshot.position_aux =
    *(RacerPositionTailTriple *)&object->position.auxiliary_position;
  object->previous_orientation = object->orientation;
}
