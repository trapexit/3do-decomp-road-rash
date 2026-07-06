/* Original address: 0x0003F0D4. */

#define TAG_ARGUMENT_WORD_COUNT 2

int
read_tag_argument_at_index(int *tag_list,
                           int  index,
                           int *out_tag,
                           int *out_value)
{
  int *entry;
  int value;

  if(tag_list == 0)
    {
      return 0;
    }

  entry = tag_list + index * TAG_ARGUMENT_WORD_COUNT;
  value = entry[0];
  if(value == 0)
    {
      return 0;
    }
  if(out_tag != 0)
    {
      *out_tag = value;
    }
  if(out_value != 0)
    {
      *out_value = entry[1];
    }
  return 1;
}
