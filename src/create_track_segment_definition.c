#include "track_segment_definition.h"

#include "memory_manager_runtime.h"

TrackSegmentDefinition *
create_track_segment_definition(int                         sample_count,
                                RoadPathResource           *path_resource,
                                const RoadRlanResource     *lane_width_resource,
                                RoadFamilyScheduleResource *family_schedule_resource,
                                RoadObjectResource         *object_resource,
                                void                       *terrain_chunk,
                                const RoadRsldResource     *slope_geometry_resource,
                                RoadSectionResource        *section_resource,
                                TrackHazardTable           *hazard_table)
{
  TrackSegmentDefinition *definition;

  definition = MEM_NewPointer(sizeof(TrackSegmentDefinition),
                              MEMORY_REQUEST_ANY,
                              MEM_POINTER_OPTIONS_DEFAULT);
  if(definition == 0)
    {
      return 0;
    }

  definition->sample_count = sample_count;
  definition->path_resource = path_resource;
  definition->lane_width_resource = lane_width_resource;
  definition->family_schedule_resource = family_schedule_resource;
  definition->terrain_chunk = terrain_chunk;
  definition->parent_junction = 0;
  definition->section_resource = section_resource;
  definition->hazard_table = hazard_table;
  definition->slope_geometry_resource = slope_geometry_resource;
  definition->object_resource = object_resource;
  definition->finish_distance = 0;
  definition->finish_sample_offset = 0;

  return definition;
}
