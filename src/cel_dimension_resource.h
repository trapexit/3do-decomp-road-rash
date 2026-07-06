#ifndef ROADRASH_CEL_DIMENSION_RESOURCE_H
#define ROADRASH_CEL_DIMENSION_RESOURCE_H

#include "graphics_ccb.h"

enum CelDimensionResourceValue
{
  CEL_DIMENSION_RESOURCE_MAGIC = 0x434C4750,
  CEL_DIMENSION_RESOURCE_TAG_SIZE = 8,
  CEL_DIMENSION_RESOURCE_KIND_MASK = 3,
  CEL_DIMENSION_RESOURCE_GRID = 0,
  CEL_DIMENSION_RESOURCE_THRESHOLDS = 1,
  CEL_DIMENSION_RESOURCE_USE_HEIGHT = 0x20,
  CEL_DIMENSION_RESOURCE_LOGARITHMIC = 0x40,
  CEL_DIMENSION_RESOURCE_LOOKUP_LIMIT = 0x100,
  CEL_DIMENSION_RESOURCE_MAXIMUM_LOG = 8,
  CEL_DIMENSION_RESOURCE_WORD_SHIFT = 2,
  CEL_DIMENSION_RESOURCE_PAIR_SIZE = 2,
  CEL_DIMENSION_RESOURCE_DESCRIPTOR_SIZE = 0x4C
};

#define CEL_DIMENSION_EXPONENT_ENCODING 0x80000000U

typedef struct CelDimensionResource
{
  unsigned char flags;
  unsigned char offset_table_word_offset;
  unsigned char dimension_table_word_offset;
  unsigned char reserved;
  unsigned char horizontal_count;
  unsigned char vertical_count;
  unsigned char horizontal_base_exponent;
  unsigned char vertical_base_exponent;
  CCB ccb;
} CelDimensionResource;

typedef char CelDimensionResourceSizeCheck[
    (sizeof(CelDimensionResource) ==
     CEL_DIMENSION_RESOURCE_DESCRIPTOR_SIZE) ? 1 : -1];

CCB *
select_cel_for_dimensions(void *resource,
                          int   width,
                          int   height);
CCB *
select_cel_from_dimension_thresholds(CelDimensionResource *resource,
                                     int                   width,
                                     int                   height);
CCB *
select_cel_from_dimension_grid(CelDimensionResource *resource,
                               int                   width,
                               int                   height);

#endif
