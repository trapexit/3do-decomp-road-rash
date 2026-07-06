#include "audio.h"

#include "dll_runtime.h"
#include "midi_player_runtime.h"
#include "score_program_runtime.h"

int
unload_sample_image(int sample_item, int release_sample_data);

int
cleanup_score_sample_items(MidiPlayer *player,
                           DLLList *sample_items,
                           int resources_transferred)
{
  DLLNode *node;
  DLLNode *allocation_node;
  MidiSampleAllocation *allocation;
  TagArg tags[2];
  int result;
  int release_result;
  int sample_item;
  int release_data;

  result = 0;
  if(sample_items == 0)
    {
      return result;
    }
  node = sample_items->head;
  while(node != 0)
    {
      sample_item = (int)node->value;
      release_data = 1;
      tags[0].ta_Tag = AF_TAG_ADDRESS;
      tags[0].ta_Arg = 0;
      tags[1].ta_Tag = TAG_END;
      tags[1].ta_Arg = 0;
      release_result = GetAudioItemInfo(sample_item, tags);
      if(release_result >= 0)
        {
          if(player->score_resource_list != 0)
            {
              allocation_node = ((DLLList *)player->score_resource_list)->head;
              while(allocation_node != 0)
                {
                  allocation = (MidiSampleAllocation *)allocation_node->value;
                  if(allocation->address == (int)tags[0].ta_Arg)
                    {
                      release_data = 0;
                      break;
                    }
                  allocation_node = allocation_node->next;
                }
            }
          if(release_data != 0 ||
             (resources_transferred & SCORE_RESOURCE_TRANSFER_FLAG_MASK) == 0)
            {
              release_result = unload_sample_image(sample_item, release_data);
            }
        }
      if(release_result < 0 && result >= 0)
        {
          result = release_result;
        }
      node = node->next;
    }
  return result;
}
