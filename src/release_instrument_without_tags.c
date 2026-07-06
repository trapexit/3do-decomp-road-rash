#include "audio.h"

Err
release_instrument_without_tags(Item instrument)
{
  return ReleaseInstrument(instrument, 0);
}
