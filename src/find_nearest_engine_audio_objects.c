#include "race_audio_runtime.h"
#include "rw_semantic_data.h"

enum EngineAudioObjectClass
{
  ENGINE_AUDIO_CLASS_RIDER = 4,
  ENGINE_AUDIO_CLASS_VEHICLE = 0x10
};

enum EngineAudioSearchLimit
{
  ENGINE_AUDIO_MAXIMUM_TRACK_DISTANCE = 0x1E00
};

void
find_nearest_engine_audio_objects(RacerEntity **nearest_object,
                                  RacerEntity **second_nearest_object)
{
  int nearest_distance;
  int second_nearest_distance;
  int next_scan_complete;
  int previous_scan_complete;
  RacerEntity *player;
  IntrusiveListNode *next_node;
  IntrusiveListNode *previous_node;
  RacerEntity *candidate;
  int distance;
  int collision_class;

  nearest_distance = ENGINE_AUDIO_MAXIMUM_TRACK_DISTANCE;
  second_nearest_distance = ENGINE_AUDIO_MAXIMUM_TRACK_DISTANCE;
  next_scan_complete = 0;
  previous_scan_complete = 0;
  *nearest_object = 0;
  *second_nearest_object = 0;

  player = (RacerEntity *)race_rider_state.player_object;
  next_node = player->list_node.links.next;
  previous_node = player->list_node.links.previous;

  do
    {
      if(next_node->next == 0 || next_scan_complete != 0)
        {
          next_scan_complete = 1;
        }
      else
        {
          candidate = (RacerEntity *)next_node;
          distance = candidate->position_z -
                     track_simulation_globals.motion_source
                     .packed_track_position;
          if(distance < 0)
            {
              distance = -distance;
            }

          collision_class = candidate->collision_class;
          if(collision_class == ENGINE_AUDIO_CLASS_RIDER ||
             collision_class == ENGINE_AUDIO_CLASS_VEHICLE)
            {
              if(distance < nearest_distance)
                {
                  second_nearest_distance = nearest_distance;
                  *second_nearest_object = *nearest_object;
                  nearest_distance = distance;
                  *nearest_object = candidate;
                }
              else if(distance >= second_nearest_distance)
                {
                  next_scan_complete = 1;
                }
              else
                {
                  second_nearest_distance = distance;
                  *second_nearest_object = candidate;
                }
            }

          next_node = next_node->next;
        }

      if(previous_node->previous == 0 || previous_scan_complete != 0)
        {
          previous_scan_complete = 1;
        }
      else
        {
          candidate = (RacerEntity *)previous_node;
          distance = candidate->position_z -
                     track_simulation_globals.motion_source
                     .packed_track_position;
          if(distance < 0)
            {
              distance = -distance;
            }

          collision_class = candidate->collision_class;
          if(collision_class == ENGINE_AUDIO_CLASS_RIDER ||
             collision_class == ENGINE_AUDIO_CLASS_VEHICLE)
            {
              if(distance < nearest_distance)
                {
                  second_nearest_distance = nearest_distance;
                  *second_nearest_object = *nearest_object;
                  nearest_distance = distance;
                  *nearest_object = candidate;
                }
              else if(distance >= second_nearest_distance)
                {
                  previous_scan_complete = 1;
                }
              else
                {
                  second_nearest_distance = distance;
                  *second_nearest_object = candidate;
                }
            }

          previous_node = previous_node->previous;
        }
    } while(next_scan_complete == 0 || previous_scan_complete == 0);
}
