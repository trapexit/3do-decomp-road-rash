#ifndef ROADRASH_SDK_AUDIO_COMPAT_H
#define ROADRASH_SDK_AUDIO_COMPAT_H

#include "audio.h"

/*
 * The deferred DataStreamer video stack still links the 1.3 audio library,
 * where MakeSample is public. V24 headers retain the symbol in the library
 * but omit its obsolete prototype in favor of CreateSample. Non-video code
 * must use the native V24 APIs directly.
 */
extern
Item
MakeSample(uint32  num_bytes,
           TagArg *tags);

Err
open_audio_folio_compat(void);

#define OpenAudioFolio() open_audio_folio_compat()

#endif
