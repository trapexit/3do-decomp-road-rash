#include "audio.h"
/* Reconstructed from the original function at 0x0003E218. */



#include "rw_semantic_data.h"

enum SampleImageUnloadConstant
{
  SAMPLE_IMAGE_INFO_ADDRESS_INDEX = 0,
  SAMPLE_IMAGE_INFO_SIZE_INDEX = 1,
  SAMPLE_IMAGE_INFO_END_INDEX = 2,
  SAMPLE_IMAGE_INFO_TAG_COUNT = 3,
  SAMPLE_IMAGE_RELEASE_FLAG_MASK = 0xFF
};

int
unload_sample_image(int sample_item,
                    int release_sample_data)
{
  TagArg tags[SAMPLE_IMAGE_INFO_TAG_COUNT];
  int result;
  void (*callback)(int,
                   int);

  release_sample_data &= SAMPLE_IMAGE_RELEASE_FLAG_MASK;
  if(sample_item < 0)
    {
      return sample_item;
    }

  if(release_sample_data != 0)
    {
      tags[SAMPLE_IMAGE_INFO_ADDRESS_INDEX].ta_Tag = AF_TAG_ADDRESS;
      tags[SAMPLE_IMAGE_INFO_ADDRESS_INDEX].ta_Arg = 0;
      tags[SAMPLE_IMAGE_INFO_SIZE_INDEX].ta_Tag = AF_TAG_NUMBYTES;
      tags[SAMPLE_IMAGE_INFO_SIZE_INDEX].ta_Arg = 0;
      tags[SAMPLE_IMAGE_INFO_END_INDEX].ta_Tag = TAG_END;

      result = GetAudioItemInfo(sample_item, tags);
      if(result < 0)
        {
          return result;
        }

      result = UnloadSample(sample_item);
      if(result < 0)
        {
          return result;
        }
      if(tags[SAMPLE_IMAGE_INFO_ADDRESS_INDEX].ta_Arg != 0)
        {
          callback = (void (*)(int, int)) music_library_globals.release;
          callback(
            (int)tags[SAMPLE_IMAGE_INFO_ADDRESS_INDEX].ta_Arg,
            (int)tags[SAMPLE_IMAGE_INFO_SIZE_INDEX].ta_Arg);
        }
      return result;
    }

  return UnloadSample(sample_item);
}
