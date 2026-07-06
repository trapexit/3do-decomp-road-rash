#include "async_stream_runtime.h"

/* Reconstructed from the original function at 0x00030C50. */
void
apply_stream_tag_overrides(RwTagArgument       *defaults,
                           const RwTagArgument *overrides)
{
  RwTagArgument *default_entry;
  const RwTagArgument *override_entry;

  if(defaults == 0 || overrides == 0 || overrides->tag == 0)
    {
      return;
    }

  override_entry = overrides;
  do
    {
      default_entry = defaults;
      if(default_entry->tag != 0)
        {
          do
            {
              if(default_entry->tag == override_entry->tag)
                {
                  default_entry->argument = override_entry->argument;
                  break;
                }
              default_entry++;
            } while(default_entry->tag != 0);
        }

      override_entry++;
    } while(override_entry->tag != 0);
}
