#include "io.h"
#include "item.h"
#include "kernelnodes.h"

#define SECTOR_FILE_IO_TAG_COUNT       3
#define SECTOR_FILE_DEVICE_TAG_INDEX   0
#define SECTOR_FILE_REPLY_TAG_INDEX    1
#define SECTOR_FILE_END_TAG_INDEX      2

Item
SF_CreateIOReqItem(Item file_item,
                   Item reply_port_item)
{
  TagArg tags[SECTOR_FILE_IO_TAG_COUNT];

  tags[SECTOR_FILE_DEVICE_TAG_INDEX].ta_Tag = CREATEIOREQ_TAG_DEVICE;
  tags[SECTOR_FILE_DEVICE_TAG_INDEX].ta_Arg = (void *)file_item;
  tags[SECTOR_FILE_REPLY_TAG_INDEX].ta_Tag =
    CREATEIOREQ_TAG_REPLYPORT;
  tags[SECTOR_FILE_REPLY_TAG_INDEX].ta_Arg = (void *)reply_port_item;
  tags[SECTOR_FILE_END_TAG_INDEX].ta_Tag = TAG_END;
  tags[SECTOR_FILE_END_TAG_INDEX].ta_Arg = 0;
  return CreateSizedItem(MKNODEID(KERNELNODE, IOREQNODE), tags, 0);
}
