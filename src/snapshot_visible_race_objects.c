#include "bss_early_data.h"
#include "racer_runtime.h"
#include "rw_semantic_data.h"

enum VisibleRaceObjectConstants
{
  VISIBLE_RACE_OBJECT_SCAN_BIAS = 0x12,
  VISIBLE_RACE_OBJECT_TRACK_FRACTION_MASK = 0xFF,
  VISIBLE_RACE_OBJECT_SCAN_SPAN = 0x2000
};

int
update_hud_tracked_object(void);

int
snapshot_visible_race_objects(void)
{
  IntrusiveListHeader *list;
  RacerEntity *object;
  int scan_start;
  int scan_end;
  int visible_count;

  list = race_object_scheduler.object_list;
  object = (RacerEntity *)list->head_sentinel->next;
  race_object_scheduler.visible_track_node_count = 0;

  scan_start =
    track_simulation_globals.motion_source.packed_track_position;
  while(object->list_node.links.next != 0 &&
        object->position_z < scan_start + VISIBLE_RACE_OBJECT_SCAN_BIAS)
    {
      object = (RacerEntity *)object->list_node.links.next;
    }

  scan_end = (scan_start & ~VISIBLE_RACE_OBJECT_TRACK_FRACTION_MASK) +
             VISIBLE_RACE_OBJECT_SCAN_SPAN;
  while(object->list_node.links.next != 0)
    {
      if(object->render_object != 0)
        {
          object->previous_orientation = object->orientation;
          object->previous_animation = object->animation;
          object->position_snapshot.position_z = object->position_z;
          object->position_snapshot.position_y = object->position_y;
          object->position_snapshot.position_aux = object->position_aux;
          object->position_snapshot.position_x = object->position_x;
          object->position_snapshot.track_segment = object->track_segment;

          visible_count = race_object_scheduler.visible_track_node_count;
          gVisibleTrackNodes[visible_count] = object;
          race_object_scheduler.visible_track_node_count =
            visible_count + 1;
        }

      object = (RacerEntity *)object->list_node.links.next;
      if(object->list_node.links.next == 0)
        {
          break;
        }
      if(object->position_z >= scan_end)
        {
          break;
        }
      if(race_object_scheduler.visible_track_node_count >=
         VISIBLE_TRACK_NODE_CAPACITY)
        {
          break;
        }
    }

  return update_hud_tracked_object();
}
