#include "memory_manager_runtime.h"
#include "rw_semantic_data.h"


enum MusicMemoryPressureThreshold
{
  MUSIC_OPTION_MINIMUM_DRAM_RUN_BYTES = 0x7A000,
  MUSIC_OPTION_DRAM_SAFETY_MARGIN_BYTES = 0x120,
  MUSIC_OPTION_MINIMUM_VRAM_RUN_BYTES = 0x92000,
  MUSIC_OPTION_VRAM_SAFETY_MARGIN_BYTES = 0x7C0
};
int
configure_music_option_for_available_memory(void)
{
  int available;

  /* Use the same contiguous bank requirements at startup and race entry. */
  available =
    MEM_GetLargestBlockSize(MEMORY_REQUEST_DRAM) >
      MUSIC_OPTION_MINIMUM_DRAM_RUN_BYTES +
      MUSIC_OPTION_DRAM_SAFETY_MARGIN_BYTES &&
    MEM_GetLargestBlockSize(MEMORY_REQUEST_VRAM) >
      MUSIC_OPTION_MINIMUM_VRAM_RUN_BYTES +
      MUSIC_OPTION_VRAM_SAFETY_MARGIN_BYTES;
  runtime_feature_flags.music_option_enabled.value =
    (unsigned char)available;
  return available;
}
