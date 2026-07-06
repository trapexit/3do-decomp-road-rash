/* Reconstructed from the original function at 0x0004A908. */

#include "memory_manager_runtime.h"

void
DeleteMemPool(MemPoolPtr pool)
{
  MEM_DisposePointer(pool);
}
