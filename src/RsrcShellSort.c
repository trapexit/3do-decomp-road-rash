/* RsrcShellSort - reconstructed from the 0x00038DD8 routine. */

#include "resource_manager_runtime.h"

typedef struct DivModResult
{
  unsigned int quotient;
  unsigned int remainder;
} DivModResult;

enum ResourceShellSortConstant
{
  RESOURCE_SHELL_GAP_MULTIPLIER = 2,
  RESOURCE_SHELL_GAP_DIVISOR = 3
};

__value_in_regs DivModResult
signed_divide_with_remainder(unsigned int divisor,
                             unsigned int numerator);

void
RsrcShellSort(ResourceRecord **resources,
              int              resource_count)
{
  int gap;
  int index;
  int scan;
  ResourceRecord *entry;
  ResourceRecord *other;
  DivModResult divmod;

  gap = 1;
  while(gap <= resource_count)
    {
      gap = gap + gap * RESOURCE_SHELL_GAP_MULTIPLIER + 1;
    }

  do
    {
      divmod = signed_divide_with_remainder(
        RESOURCE_SHELL_GAP_DIVISOR, (unsigned int)gap);
      gap = (int)divmod.quotient;
      index = gap;
      while(index < resource_count)
        {
          entry = resources[index];
          scan = index;
          while(scan >= gap)
            {
              other = resources[scan - gap];
              if(other->file_position <= entry->file_position)
                {
                  break;
                }
              resources[scan] = other;
              scan -= gap;
            }
          resources[scan] = entry;
          index++;
        }
    } while(gap != 1);
}
