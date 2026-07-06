#include "bss_late_data.h"
#include "rw_semantic_data.h"
#include "stdlib.h"

typedef struct DivModResult
{
  unsigned int quotient;
  unsigned int remainder;
} DivModResult;

__value_in_regs DivModResult
signed_divide_with_remainder(unsigned int divisor,
                             unsigned int numerator);

#define ROAD_PARTICLE_HORIZONTAL_PHASE_MINIMUM ((int)0xC0000001U)

enum RoadParticleAllocationConstant
{
  ROAD_PARTICLE_SLOT_CAPACITY = 0x64,
  ROAD_PARTICLE_CEL_VARIANT_COUNT = 0x0F,
  ROAD_PARTICLE_HORIZONTAL_PHASE_RANGE = 0x7FFFFFFE,
  ROAD_PARTICLE_VERTICAL_PHASE_RANGE = 0x3FFFFFFF,
  ROAD_PARTICLE_HALF_HEIGHT_RANGE = 0x300,
  ROAD_PARTICLE_HALF_HEIGHT_MINIMUM = 0x300,
  ROAD_PARTICLE_HALF_WIDTH_RANGE = 0x180,
  ROAD_PARTICLE_HALF_WIDTH_MINIMUM = 0x180
};

RoadParticleSlot *
allocate_road_particle_slot(void)
{
  int random_value;
  DivModResult divmod;
  RoadParticleSlot *slot;

  slot = 0;
  if(particle_resource_globals.active_count <
     ROAD_PARTICLE_SLOT_CAPACITY)
    {
      slot = gRoadSegmentEffectWorkspace.particle_effects.particle_slots;
      while((slot->active_flags & 1) != 0)
        {
          slot++;
        }

      slot->active_flags = 1;

      random_value = rand();
      divmod = signed_divide_with_remainder(
        ROAD_PARTICLE_CEL_VARIANT_COUNT, random_value);
      slot->cel = particle_resource_globals.cel_pointers[divmod.remainder];

      random_value = rand();
      divmod = signed_divide_with_remainder(
        ROAD_PARTICLE_HORIZONTAL_PHASE_RANGE, random_value);
      slot->horizontal_phase =
        divmod.remainder + ROAD_PARTICLE_HORIZONTAL_PHASE_MINIMUM;

      random_value = rand();
      divmod = signed_divide_with_remainder(
        ROAD_PARTICLE_VERTICAL_PHASE_RANGE, random_value);
      slot->vertical_phase = divmod.remainder;

      random_value = rand();
      divmod = signed_divide_with_remainder(
        ROAD_PARTICLE_HALF_HEIGHT_RANGE, random_value);
      slot->half_height =
        divmod.remainder + ROAD_PARTICLE_HALF_HEIGHT_MINIMUM;

      random_value = rand();
      divmod = signed_divide_with_remainder(
        ROAD_PARTICLE_HALF_WIDTH_RANGE, random_value);
      slot->half_width =
        divmod.remainder + ROAD_PARTICLE_HALF_WIDTH_MINIMUM;

      particle_resource_globals.active_count += 1;
    }

  return slot;
}


#undef ROAD_PARTICLE_HORIZONTAL_PHASE_MINIMUM
