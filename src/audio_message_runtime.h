#ifndef AUDIO_MESSAGE_RUNTIME_H
#define AUDIO_MESSAGE_RUNTIME_H

#include "platform.h"

typedef void (*AudioThreadEntry)(void *initial_argument,
                                 void *secondary_argument);

Item
NewMsgPort(uint32 *out_signal_mask);
uint32
GetMsgPortSignal(Item message_port_item);
Item
CreateConfiguredMsgItem(Item reply_port_item,
                        int  create_small_message);
void
RemoveMsgItem(Item message_item);

int32
WaitForMsg(Item      message_port_item,
           Item     *out_message_item,
           Message **out_message,
           void    **reserved_payload_output,
           Item      wanted_message_item);
boolean
PollForMsg(Item      message_port_item,
           Item     *out_message_item,
           Message **out_message,
           void    **out_payload,
           int32    *out_status);

int32
AM_WaitMsg(Item   message_port_item,
           Item  *out_message_item,
           int32 *out_message_value,
           int32 *out_message_value_size);
int32
AM_PollMsg(unsigned char *out_received,
           Item           message_port_item,
           Item          *out_message_item,
           int32         *out_message_value,
           int32         *out_message_value_size);
int32
AM_WaitReply(Item   message_port_item,
             Item   reply_message_item,
             int32 *out_reply_result,
             int   *out_reply_value,
             int32 *out_reply_value_size);

Item
AM_NewThread(AudioThreadEntry entry_point,
             int             *stack_size,
             int32            priority,
             const char      *thread_name,
             void           **out_stack_memory,
             void            *initial_argument,
             void            *secondary_argument);
int32
delete_audio_thread_item(Item thread_item);
char *
next_audio_thread_name(void);

#endif
