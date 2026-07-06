#ifndef ROADRASH_HISTORICAL_CELUTILS_COMPATIBILITY_H
#define ROADRASH_HISTORICAL_CELUTILS_COMPATIBILITY_H

#include "graphics.h"

#define ROADRASH_ADD_TO_POINTER(pointer, offset) \
        ((void *)((char *)(pointer) + (long)(offset)))

#define CEL_PLUTPTR(ccb) \
        ((uint16 *)(((ccb)->ccb_Flags & CCB_PPABS) \
        ? (ccb)->ccb_PLUTPtr \
        : ROADRASH_ADD_TO_POINTER(&(ccb)->ccb_PLUTPtr, \
                                  (int32)(ccb)->ccb_PLUTPtr + 4)))

#define CEL_DATAPTR(ccb) \
        ((CelData *)(((ccb)->ccb_Flags & CCB_SPABS) \
        ? (ccb)->ccb_SourcePtr \
        : ROADRASH_ADD_TO_POINTER(&(ccb)->ccb_SourcePtr, \
                                  (int32)(ccb)->ccb_SourcePtr + 4)))

#endif
