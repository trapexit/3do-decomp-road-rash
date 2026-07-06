#ifndef STREAM_AUDIO_PLAYBACK_H
#define STREAM_AUDIO_PLAYBACK_H

#include "saudio_subscriber_runtime.h"

typedef int (*AsyncStreamAudioInitializeCallback)(const SAudioHeaderChunk *header);
typedef void (*AsyncStreamAudioSubmitCallback)(const unsigned char *sample_data,
                                               unsigned int         sample_byte_count);

void
merge_tag_values(TagArg       *destination_tags,
                 const TagArg *source_tags);
int
retain_stream_audio_playback(void);
int
initialize_stream_audio_playback(const SAudioHeaderChunk *header);
void
release_stream_audio_playback(void);
void
submit_stream_audio_chunk(const unsigned char *sample_data,
                          unsigned int         sample_byte_count);

#endif
