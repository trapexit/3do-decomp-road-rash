#ifndef ROADRASH_BLOCK_FILE_API_H
#define ROADRASH_BLOCK_FILE_API_H

#include "blockfile.h"

/* The 1.3 SDK exports this entry point with a historical spelling error. */
int32
GetBlockFIleBlockSize(BlockFilePtr block_file);
#define GetBlockFileBlockSize GetBlockFIleBlockSize

#endif
