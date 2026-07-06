#include "async_stream_runtime.h"

/* Reconstructed from the original function at 0x00031350. */
void
upsert_stream_tag_argument(RwTagArgument       *entries,
                           int                  capacity,
                           const RwTagArgument *incoming)
{
  int index;
  int next;

  index = 0;
  if(entries[0].tag != 0)
    {
      for(;;)
        {
          if(entries[index].tag == incoming->tag)
            {
              entries[index].argument = incoming->argument;
              return;
            }
          index++;
          if(entries[index].tag == 0)
            {
              break;
            }
        }
    }

  next = index + 1;
  if(next >= capacity)
    {
      return;
    }

  entries[index] = *incoming;
  entries[next].tag = 0;
  entries[next].argument = 0;
}
