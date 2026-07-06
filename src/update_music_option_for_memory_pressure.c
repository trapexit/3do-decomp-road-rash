
#include "application_runtime.h"
#include "rw_semantic_data.h"


void
update_music_option_for_memory_pressure(void)
{
  if(runtime_feature_flags.music_option_enabled.value != 0)
    {
      configure_music_option_for_available_memory();
    }
}
