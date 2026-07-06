#include "audio_message_runtime.h"

#define MESSAGE_TAG_CAPACITY             3
#define CREATE_SMALL_MESSAGE_FLAG_MASK   0xFF
#define SMALL_MESSAGE_END_TAG_INDEX      2

Item
CreateConfiguredMsgItem(Item reply_port_item,
                        int  create_small_message)
{
  TagArg message_tags[MESSAGE_TAG_CAPACITY];
  int end_tag_index;

  end_tag_index = 1;
  message_tags[0].ta_Tag = CREATEMSG_TAG_REPLYPORT;
  message_tags[0].ta_Arg = (void *)reply_port_item;
  if((create_small_message & CREATE_SMALL_MESSAGE_FLAG_MASK) != 0)
    {
      message_tags[1].ta_Tag = CREATEMSG_TAG_MSG_IS_SMALL;
      message_tags[1].ta_Arg = 0;
      end_tag_index = SMALL_MESSAGE_END_TAG_INDEX;
    }
  message_tags[end_tag_index].ta_Tag = TAG_END;

  return CreateSizedItem(MKNODEID(KERNELNODE, MESSAGENODE),
                         message_tags, 0);
}
