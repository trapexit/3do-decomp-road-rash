#include "audio.h"
/* Reconstructed from the original function at 0x0003C5FC. */

#define ATTACHMENT_TAG_COUNT 5
#define ATTACHMENT_HOOK_TAG  0
#define ATTACHMENT_SAMPLE_TAG 1
#define ATTACHMENT_INSTRUMENT_TAG 2
#define ATTACHMENT_FLAGS_TAG 3
#define ATTACHMENT_END_TAG   4

int
create_audio_attachment(int instrument_item,
                        int sample_item,
                        int hook_name,
                        int attachment_flags)
{
  TagArg tags[ATTACHMENT_TAG_COUNT];

  tags[ATTACHMENT_HOOK_TAG].ta_Tag = AF_TAG_HOOKNAME;
  tags[ATTACHMENT_HOOK_TAG].ta_Arg = (void *)hook_name;
  tags[ATTACHMENT_SAMPLE_TAG].ta_Tag = AF_TAG_SAMPLE;
  tags[ATTACHMENT_SAMPLE_TAG].ta_Arg = (void *)sample_item;
  tags[ATTACHMENT_INSTRUMENT_TAG].ta_Tag = AF_TAG_INSTRUMENT;
  tags[ATTACHMENT_INSTRUMENT_TAG].ta_Arg = (void *)instrument_item;
  tags[ATTACHMENT_FLAGS_TAG].ta_Tag = AF_TAG_SET_FLAGS;
  tags[ATTACHMENT_FLAGS_TAG].ta_Arg = (void *)attachment_flags;
  tags[ATTACHMENT_END_TAG].ta_Tag = TAG_END;
  tags[ATTACHMENT_END_TAG].ta_Arg = 0;

  return CreateSizedItem(MKNODEID(AUDIONODE, AUDIO_ATTACHMENT_NODE), tags,
                         0);
}
