#ifndef ROADRASH_WAIT_PORT_PROVIDER_BRIDGE_H
#define ROADRASH_WAIT_PORT_PROVIDER_BRIDGE_H

#include "types.h"

extern
Item
_WaitPort(Item port,
          Item message);
Item
WaitPort(Item port,
         Item message);

#endif
