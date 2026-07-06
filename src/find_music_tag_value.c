#include "midi_player_runtime.h"

int
find_music_tag_value(MusicTagValue *tag_list,
                     int            wanted_tag,
                     int           *out_value,
                     int           *out_index)
{
  int found;
  int tag_index;
  MusicTagValue *tag_entry;

  found = 0;
  tag_index = 0;
  if(tag_list != 0 && tag_list[0].tag != 0)
    {
      do
        {
          tag_entry = &tag_list[tag_index];
          if(tag_entry->tag == wanted_tag)
            {
              found = 1;
              if(out_value != 0)
                {
                  *out_value = tag_entry->value;
                }
              if(out_index != 0)
                {
                  *out_index = tag_index;
                }
            }
          tag_index++;
        } while(tag_list[tag_index].tag != 0);
    }
  return found;
}
