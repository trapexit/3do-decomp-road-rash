#ifndef ROADRASH_HISTORICAL_MUSICERROR_COMPATIBILITY_H
#define ROADRASH_HISTORICAL_MUSICERROR_COMPATIBILITY_H

#include "operror.h"

#ifndef ER_LINKLIB
  #define ER_LINKLIB Make6Bit('L')
#endif

#ifndef ER_MUSICLIB
  #define ER_MUSICLIB MakeErrId('M', 'u')
#endif

#define ROADRASH_MAKE_MUSIC_ERROR(severity, error_class, error_number) \
        MakeErr(ER_LINKLIB, ER_MUSICLIB, severity, ER_E_SSTM, \
                error_class, error_number)

#define ML_ERR_DUPLICATE_NAME \
        ROADRASH_MAKE_MUSIC_ERROR(ER_SEVERE, ER_C_NSTND, 19)

#endif
