#ifndef ROADRASH_RUNTIME_DATA_TYPES_H
#define ROADRASH_RUNTIME_DATA_TYPES_H

typedef struct BigEndianByteFlag
{
  unsigned char value;
  unsigned char alignment[3];
} BigEndianByteFlag;

typedef int (*RwMemoryAllocator)(int size,
                                 int memory_type);
typedef void (*RwMemoryReleaser)(int memory,
                                 int size);
typedef struct AudioMixerRuntime AudioMixerRuntime;

typedef struct MusicLibraryGlobals
{
  int last_error;
  unsigned char custom_memory_enabled;
  unsigned char memory_alignment[3];
  RwMemoryAllocator allocate;
  RwMemoryReleaser release;
  BigEndianByteFlag audio_folio_open;
  int loaded_audio_object_count;
  int live_sound_count;
  AudioMixerRuntime *global_mixer;
  unsigned char mixer_enabled;
  unsigned char mixer_alignment[3];
  unsigned char secondary_mixer_enabled;
  unsigned char secondary_alignment[3];
  unsigned char suppress_directory_changes;
  unsigned char directory_alignment[3];
  void *allocator_context_buffer;
  unsigned int start_instrument_tags[5];
  int start_instrument_result;
  int cached_decode_instrument;
  int decoder_reference_count;
} MusicLibraryGlobals;

extern MusicLibraryGlobals music_library_globals;
extern int score_loader_last_error;

#endif
