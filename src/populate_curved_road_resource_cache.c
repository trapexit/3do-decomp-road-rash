#include "bss_early_data.h"
#include "bss_late_data.h"
#include "family_resource_runtime.h"

#define CURVED_ROAD_RESOURCE_ROW_COUNT    2
#define CURVED_ROAD_RESOURCE_COLUMN_COUNT 3

void
populate_curved_road_resource_cache(RoadRenderSide *road_side)
{
  int row;
  int column;
  void **previous;
  void **current;
  int value;
  int selector;

  selector = road_side->lane->surface_selector;
  previous = road_side->texture_cache->procedural_sources[0];

  row = 0;
  while(row < CURVED_ROAD_RESOURCE_ROW_COUNT)
    {
      current = road_side->texture_cache->procedural_sources[row];
      column = 0;
      while(column < CURVED_ROAD_RESOURCE_COLUMN_COUNT)
        {
          value = lookup_family_resource_grandchild_for_render(
            selector, FAMILY_RESOURCE_CURVED_ROAD_CATEGORY, row, column);
          current[column] = (void *)value;
          if(row == CURVED_ROAD_RESOURCE_ROW_COUNT - 1)
            {
              if(value == 0 ||
                 FAMILY_RESOURCE_CHUNK_TAG(value) !=
                 FAMILY_RESOURCE_CLGP_TAG)
                {
                  current[column] = previous[column];
                }
            }
          column++;
        }
      row++;
    }
}
