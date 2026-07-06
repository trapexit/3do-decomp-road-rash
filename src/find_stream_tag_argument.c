#include "async_stream_runtime.h"

/* Reconstructed from the original function at 0x00030CCC. */
const
RwTagArgument *
find_stream_tag_argument(unsigned int         tag,
                         const RwTagArgument *overrides,
                         const RwTagArgument *defaults)
{
  const RwTagArgument *entry;

  entry = overrides;
  if(entry != 0 && entry->tag != 0)
    {
      do
        {
          if(entry->tag == tag)
            {
              return entry;
            }
          entry++;
        } while(entry->tag != 0);
    }

  entry = defaults;
  if(entry->tag != 0)
    {
      do
        {
          if(entry->tag == tag)
            {
              return entry;
            }
          entry++;
        } while(entry->tag != 0);
    }

  return 0;
}
