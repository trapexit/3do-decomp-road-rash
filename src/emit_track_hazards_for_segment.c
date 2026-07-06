#include "bss_late_data.h"
#include "division_helpers.h"
#include "stdlib.h"
#include "track_segment_definition.h"
/* Reconstructed from the original function at 0x00020734. */

#define HAZARD_OFFSET_MASK 0xFFFU
#define HAZARD_OFFSET_SIGN_BIT 0x800U
#define HAZARD_OFFSET_SIGN_EXTENSION 0xFFFFF000U
#define HAZARD_KIND_SHIFT 8
#define HAZARD_KIND_MASK 0x3FU
#define HAZARD_DISABLED_KIND 0x3FU
#define HAZARD_EFFECT_FLAG 0x80000000U
#define HAZARD_TYPE_MASK 0x0FU
#define HAZARD_DIRECTION_FLAG 0x80U
#define HAZARD_RANDOM_RADIUS_SHIFT 14
#define HAZARD_RANDOM_RADIUS_MASK 0x3FCU
#define HAZARD_RANDOM_RANGE_SHIFT 2
#define HAZARD_RANDOM_CENTER_SHIFT 1
#define HAZARD_EFFECT_GROUP_SHIFT 24
#define HAZARD_DISABLE_MASK 0x3F00U

#define HAZARD_SLOT_LIMIT 2
#define HAZARD_SLOT_DIRECTION_MASK 1
#define HAZARD_SLOT_POSITIVE_OFFSET_MASK 2
#define HAZARD_COLLISION_LEFT_BOUND (-0x1000)
#define HAZARD_COLLISION_TOP_BOUND (-0x2000)
#define HAZARD_COLLISION_RIGHT_BOUND 0x1000
#define HAZARD_COLLISION_BOTTOM_BOUND 0

void *
spawn_track_hazard_rider(int                     travel_mode,
                         int                     family_resource_inventory,
                         int                     lateral_position,
                         int                     track_offset,
                         RoadSegmentLaneRuntime *track_lane);

static
int
sign_extend_hazard_offset(unsigned int packed_offset)
{
  if((packed_offset & HAZARD_OFFSET_SIGN_BIT) == 0)
    {
      return (int)(packed_offset & HAZARD_OFFSET_MASK);
    }
  return (int)(packed_offset | HAZARD_OFFSET_SIGN_EXTENSION);
}


void
emit_track_hazards_for_segment(TrackHazardTraversal   *traversal,
                               RoadSegmentLaneRuntime *runtime_lane)
{
  TrackHazardTable *table;
  TrackHazardEntry *entry;
  StaticRoadsideObject *hazard;
  unsigned int control;
  unsigned int detail;
  unsigned int random_radius;
  unsigned int slot_count;
  int target_frame;
  int entry_index;
  int entry_count;
  int offset;
  int random_value;
  int position;
  UnsignedDivisionResult division;

  runtime_lane->static_object_count = 0;

  table = traversal->table;
  if(table == 0)
    {
      return;
    }

  target_frame =
    traversal->track_position >> TRACK_POSITION_FRACTION_BITS;
  entry_index = traversal->entry_index;
  if(entry_index < 0)
    {
      return;
    }
  entry_count = get_track_hazard_entry_count(table);
  if(entry_count <= entry_index)
    {
      return;
    }

  entry = &table->entries[entry_index];
  while(entry_index < entry_count)
    {
      control = entry->control_word;
      if(((control >> TRACK_HAZARD_FRAME_SHIFT) &
          TRACK_HAZARD_FRAME_MASK) != (unsigned int)target_frame)
        {
          return;
        }

      if(((control >> HAZARD_KIND_SHIFT) &
          HAZARD_KIND_MASK) == HAZARD_DISABLED_KIND)
        {
          entry_index++;
          entry++;
          continue;
        }

      if((control & HAZARD_EFFECT_FLAG) != 0)
        {
          detail = entry->detail_word;
          offset = sign_extend_hazard_offset(detail);
          position =
            (runtime_lane->center_position + offset)
              << TRACK_POSITION_FRACTION_BITS;
          random_radius =
            (detail >> HAZARD_RANDOM_RADIUS_SHIFT) &
            HAZARD_RANDOM_RADIUS_MASK;

          random_value = rand();
          division = unsigned_divide_with_remainder(
            (random_radius << HAZARD_RANDOM_RANGE_SHIFT) + 1,
            (unsigned int)random_value);
          spawn_track_hazard_rider(
            (int)(control & HAZARD_TYPE_MASK),
            (int)((control >> HAZARD_KIND_SHIFT) &
                  TRACK_HAZARD_KIND_BYTE_MASK),
            position + (int)division.remainder -
            (int)(random_radius <<
                  HAZARD_RANDOM_CENTER_SHIFT),
            (int)(detail >> HAZARD_EFFECT_GROUP_SHIFT),
            runtime_lane);
          entry->control_word = control | HAZARD_DISABLE_MASK;
          entry_index++;
          entry++;
          continue;
        }

      slot_count = runtime_lane->static_object_count;
      runtime_lane->static_object_count =
        (unsigned char)(slot_count + 1);
      if(slot_count >= HAZARD_SLOT_LIMIT)
        {
          entry_index++;
          entry++;
          runtime_lane->static_object_count = HAZARD_SLOT_LIMIT;
          continue;
        }

      hazard = &runtime_lane->static_objects[slot_count];
      detail = entry->detail_word;
      offset = sign_extend_hazard_offset(detail);
      hazard->flags = 0;
      if((control & HAZARD_DIRECTION_FLAG) != 0)
        {
          hazard->flags = HAZARD_SLOT_DIRECTION_MASK;
        }
      if(offset > 0)
        {
          hazard->flags = (unsigned char)(
            hazard->flags | HAZARD_SLOT_POSITIVE_OFFSET_MASK);
        }

      hazard->lateral_position =
        runtime_lane->center_position + offset;
      hazard->resource_selector =
        (unsigned char)(control >> HAZARD_KIND_SHIFT);
      hazard->visibility_group =
        (unsigned char)(control & HAZARD_TYPE_MASK);
      hazard->collision_box_count = 1;
      hazard->bounds[0].left = HAZARD_COLLISION_LEFT_BOUND;
      hazard->bounds[0].top = HAZARD_COLLISION_TOP_BOUND;
      hazard->bounds[0].right = HAZARD_COLLISION_RIGHT_BOUND;
      hazard->bounds[0].bottom = HAZARD_COLLISION_BOTTOM_BOUND;

      entry_index++;
      entry++;
    }
}


#undef HAZARD_COLLISION_BOTTOM_BOUND
#undef HAZARD_COLLISION_RIGHT_BOUND
#undef HAZARD_COLLISION_TOP_BOUND
#undef HAZARD_COLLISION_LEFT_BOUND
#undef HAZARD_SLOT_POSITIVE_OFFSET_MASK
#undef HAZARD_SLOT_DIRECTION_MASK
#undef HAZARD_SLOT_LIMIT
#undef HAZARD_DISABLE_MASK
#undef HAZARD_EFFECT_GROUP_SHIFT
#undef HAZARD_RANDOM_CENTER_SHIFT
#undef HAZARD_RANDOM_RANGE_SHIFT
#undef HAZARD_RANDOM_RADIUS_MASK
#undef HAZARD_RANDOM_RADIUS_SHIFT
#undef HAZARD_DIRECTION_FLAG
#undef HAZARD_TYPE_MASK
#undef HAZARD_EFFECT_FLAG
#undef HAZARD_DISABLED_KIND
#undef HAZARD_KIND_MASK
#undef HAZARD_KIND_SHIFT
#undef HAZARD_OFFSET_SIGN_EXTENSION
#undef HAZARD_OFFSET_SIGN_BIT
#undef HAZARD_OFFSET_MASK
