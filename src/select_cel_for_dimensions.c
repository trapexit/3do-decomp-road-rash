#include "cel_dimension_resource.h"

#include "bss_early_data.h"
#include "hardware.h"
#include "rw_semantic_data.h"

CCB *
select_cel_for_dimensions(void *resource_data,
                          int   width,
                          int   height)
{
  unsigned char *resource_bytes;
  CelDimensionResource *resource;
  unsigned int kind;

  if(resource_data == 0)
    {
      return 0;
    }

  resource_bytes = (unsigned char *)resource_data;
  if(*(unsigned int *)resource_bytes == CEL_DIMENSION_RESOURCE_MAGIC)
    {
      resource_bytes += CEL_DIMENSION_RESOURCE_TAG_SIZE;
    }
  else if(*(unsigned int *)(resource_bytes -
                            CEL_DIMENSION_RESOURCE_TAG_SIZE) !=
          CEL_DIMENSION_RESOURCE_MAGIC)
    {
      return 0;
    }

  resource = (CelDimensionResource *)resource_bytes;
  kind = resource->flags & CEL_DIMENSION_RESOURCE_KIND_MASK;
  if(kind == CEL_DIMENSION_RESOURCE_GRID)
    {
      return select_cel_from_dimension_grid(resource, width, height);
    }
  if(kind == CEL_DIMENSION_RESOURCE_THRESHOLDS)
    {
      return select_cel_from_dimension_thresholds(resource, width, height);
    }
  return 0;
}


CCB *
select_cel_from_dimension_thresholds(CelDimensionResource *resource,
                                     int                   width,
                                     int                   height)
{
  unsigned char *resource_bytes;
  unsigned char *dimension_table;
  unsigned char *dimension_pair;
  int *offset_table;
  unsigned char *payload;
  CCB *ccb;
  unsigned int flags;
  int ccb_flags;
  int requested_dimension;
  int selection_index;

  if(resource == 0)
    {
      return 0;
    }

  resource_bytes = (unsigned char *)resource;
  flags = resource->flags;
  dimension_table = resource_bytes +
                    (resource->dimension_table_word_offset <<
                     CEL_DIMENSION_RESOURCE_WORD_SHIFT);
  offset_table = (int *)(resource_bytes +
                         (resource->offset_table_word_offset <<
                          CEL_DIMENSION_RESOURCE_WORD_SHIFT));

  /* A singleton has no request-dependent choice. In particular, do not
     normalize a width/height that cannot change its dimension pair. */
  selection_index = 0;
  dimension_pair = dimension_table;
  {
    int low;
    int high;

    high = resource->horizontal_count;
    if(high != 0)
      {
        requested_dimension = width;
        if((flags & CEL_DIMENSION_RESOURCE_USE_HEIGHT) != 0)
          {
            requested_dimension = height;
            dimension_table++;
          }

        if((flags & CEL_DIMENSION_RESOURCE_LOGARITHMIC) != 0)
          {
            if(requested_dimension < CEL_DIMENSION_RESOURCE_LOOKUP_LIMIT)
              {
                requested_dimension =
                  road_renderer_state.bit_length[requested_dimension];
              }
            else
              {
                requested_dimension = CEL_DIMENSION_RESOURCE_MAXIMUM_LOG;
              }
          }

        /* Four entries need exactly the same two comparisons as bisection,
           without midpoint updates or a loop. Equality chooses the later
           entry, including duplicate thresholds and the index-zero fallback. */
        if(high == 3)
          {
            if(dimension_table[(2 * CEL_DIMENSION_RESOURCE_PAIR_SIZE)] <=
               requested_dimension)
              {
                selection_index =
                  (2 + (dimension_table[(3 * CEL_DIMENSION_RESOURCE_PAIR_SIZE)] <=
                        requested_dimension));
              }
            else
              {
                selection_index =
                  (dimension_table[CEL_DIMENSION_RESOURCE_PAIR_SIZE] <=
                   requested_dimension);
              }
          }
        else
          {
            low = 0;
            while(low < high)
              {
                int middle;

                middle = (((low + high) + 1) >> 1);
                if(dimension_table[(middle * CEL_DIMENSION_RESOURCE_PAIR_SIZE)] <=
                   requested_dimension)
                  {
                    low = middle;
                  }
                else
                  {
                    high = (middle - 1);
                  }
              }

            selection_index = low;
          }

        dimension_pair = (dimension_table +
                          (selection_index * CEL_DIMENSION_RESOURCE_PAIR_SIZE));
        if((flags & CEL_DIMENSION_RESOURCE_USE_HEIGHT) != 0)
          {
            dimension_pair--;
          }
      }
  }

  ccb = &resource->ccb;
  /* Read the header flags once: the |= below used to reload the same word
     after the payload stores, costing an extra load per selection. */
  ccb_flags = ccb->ccb_Flags;
  if((flags & CEL_DIMENSION_RESOURCE_LOGARITHMIC) != 0)
    {
      ccb->ccb_Width = (int)(dimension_pair[0] |
                             CEL_DIMENSION_EXPONENT_ENCODING);
      ccb->ccb_Height = (int)(dimension_pair[1] |
                              CEL_DIMENSION_EXPONENT_ENCODING);
    }
  else
    {
      ccb->ccb_Width =
        (int)gCelDimensionPreambleTable[dimension_pair[0]];
      ccb->ccb_Height =
        (int)gCelDimensionPreambleTable[dimension_pair[1]];
    }

  ccb->ccb_PLUTPtr = (void *)(ccb + 1);
  payload = resource_bytes + offset_table[selection_index];
  ccb->ccb_PRE0 = *(unsigned int *)payload;
  payload += sizeof(unsigned int);
  if((ccb_flags & CCB_PACKED) == 0)
    {
      ccb->ccb_PRE1 = *(unsigned int *)payload;
      payload += sizeof(unsigned int);
    }
  ccb->ccb_SourcePtr = (CelData *)payload;
  ccb->ccb_Flags = ccb_flags | CCB_CCBPRE;
  return ccb;
}


CCB *
select_cel_from_dimension_grid(CelDimensionResource *resource,
                               int                   width,
                               int                   height)
{
  unsigned char *resource_bytes;
  int *row_offset_table;
  int *column_offset_table;
  unsigned char *payload;
  CCB *ccb;
  unsigned int flags;
  int horizontal_index;
  int vertical_index;
  int horizontal_limit;
  int vertical_limit;
  int horizontal_base;
  int vertical_base;

  if(resource == 0)
    {
      return 0;
    }

  /* The grid limits and exponents are byte fields of the resource header.
     Nothing here writes that header (the CCB fields written below start at
     offset 8), so read them once each instead of once per use. */
  horizontal_limit = resource->horizontal_count;
  vertical_limit = resource->vertical_count;
  horizontal_base = resource->horizontal_base_exponent;
  vertical_base = resource->vertical_base_exponent;

  if(width >= CEL_DIMENSION_RESOURCE_LOOKUP_LIMIT)
    {
      horizontal_index = horizontal_limit;
    }
  else
    {
      horizontal_index = road_renderer_state.bit_length[width] -
                         horizontal_base;
      if(horizontal_index < 0)
        {
          horizontal_index = 0;
        }
      else if(horizontal_index > horizontal_limit)
        {
          horizontal_index = horizontal_limit;
        }
    }

  if(height >= CEL_DIMENSION_RESOURCE_LOOKUP_LIMIT)
    {
      vertical_index = vertical_limit;
    }
  else
    {
      vertical_index = road_renderer_state.bit_length[height] -
                       vertical_base;
      if(vertical_index < 0)
        {
          vertical_index = 0;
        }
      else if(vertical_index > vertical_limit)
        {
          vertical_index = vertical_limit;
        }
    }

  resource_bytes = (unsigned char *)resource;
  row_offset_table = (int *)(resource_bytes +
                             (resource->offset_table_word_offset <<
                              CEL_DIMENSION_RESOURCE_WORD_SHIFT));
  column_offset_table = (int *)(resource_bytes +
                                row_offset_table[vertical_index]);
  payload = resource_bytes + column_offset_table[horizontal_index];

  ccb = &resource->ccb;
  flags = (unsigned int)ccb->ccb_Flags;
  ccb->ccb_PRE0 = *(unsigned int *)payload;
  payload += sizeof(unsigned int);
  if((flags & CCB_PACKED) == 0)
    {
      ccb->ccb_PRE1 = *(unsigned int *)payload;
      payload += sizeof(unsigned int);
    }
  ccb->ccb_SourcePtr = (CelData *)payload;
  ccb->ccb_Flags = (int)(flags | CCB_CCBPRE);
  ccb->ccb_PLUTPtr = (void *)(ccb + 1);
  ccb->ccb_Width = (int)(
    (horizontal_base + horizontal_index) |
    CEL_DIMENSION_EXPONENT_ENCODING);
  ccb->ccb_Height = (int)(
    (vertical_base + vertical_index) |
    CEL_DIMENSION_EXPONENT_ENCODING);
  return ccb;
}
