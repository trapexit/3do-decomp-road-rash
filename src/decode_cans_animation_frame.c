#include "cans_animation_binding.h"

#include "string.h"

enum
{
  CANS_CHANNEL_ANIMATION_INDEX_OFFSET = 2,
  CANS_ANIMATION_INDEX_BASE = 1
};

#define CANS_READ_SIGNED_BIG_ENDIAN_16(bytes) \
        ((short)(((int)(bytes)[0] << 8) | (bytes)[1]))

void
decode_cans_animation_frame(const AnimationResourcePair *resources_,
                            int                          entry_index_,
                            int                          channel_index_,
                            CansAnimationFrameOutput    *frame_output_)
{
  const CansAnimationChannelRecord *channel_record;
  const unsigned char *entry;
  int value;
  int index;

  if(frame_output_ == NULL)
    return;

  if((resources_ == NULL) || (resources_->cans_resource == NULL) ||
     (resources_->animation_resource == NULL) ||
     (entry_index_ < 0) || (channel_index_ < 0))
    {
      memset(frame_output_, 0, sizeof(*frame_output_));
      return;
    }

  channel_record = get_cans_animation_channel_record(resources_,
                                                     entry_index_,
                                                     channel_index_);
  if(channel_record == NULL)
    {
      memset(frame_output_, 0, sizeof(*frame_output_));
      return;
    }

  entry = (channel_record->bytes + CANS_CHANNEL_ANIMATION_INDEX_OFFSET);
  value = get_cans_animation_entry_flags(resources_,
                                         (unsigned int)entry_index_,
                                         channel_index_);
  index = (CANS_READ_SIGNED_BIG_ENDIAN_16(entry) - CANS_ANIMATION_INDEX_BASE);

  // Normal decodes clear every output field, including rejection and OFSS
  // paths. Metadata requests copy only the prefix, so retain the zero tail.
  if(index == CANS_METADATA_FRAME)
    memset(frame_output_, 0, sizeof(*frame_output_));

  CANS_GetAnimFrame((CansAnimationData *)resources_->animation_resource,
                   index,
                   frame_output_);
  frame_output_->animation_flags = value;
}


#undef CANS_READ_SIGNED_BIG_ENDIAN_16
