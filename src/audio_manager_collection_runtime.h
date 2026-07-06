#ifndef ROADRASH_AUDIO_MANAGER_COLLECTION_RUNTIME_H
#define ROADRASH_AUDIO_MANAGER_COLLECTION_RUNTIME_H

#include "audio_manager_voice_internal.h"
#include "dll_runtime.h"
#include "music_file_runtime.h"

enum AudioManagerCollectionFormatValue
{
  AUDIO_MANAGER_COLLECTION_FORM_ID = 0x414D436F,
  AUDIO_MANAGER_COLLECTION_ENTRY_FORM_ID = 0x49456E73,
  AUDIO_MANAGER_COLLECTION_ENTRY_INFO_ID = 0x49456E49
};

enum AudioManagerCollectionConstant
{
  AUDIO_MANAGER_COLLECTION_NAME_CAPACITY = 64,
  AUDIO_MANAGER_FIRST_DYNAMIC_OBJECT_KEY = 6,
  AUDIO_MANAGER_COLLECTION_OBJECT_ERROR = -19,
  AUDIO_MANAGER_ENTRY_VOICE_COUNT_OFFSET = 0,
  AUDIO_MANAGER_ENTRY_NAME_LENGTH_OFFSET = 4,
  AUDIO_MANAGER_ENTRY_NAME_OFFSET = 5
};

typedef struct AudioManagerCollectionFileSource
{
  ByteFileReader *reader;
  int offset;
} AudioManagerCollectionFileSource;

int
next_audio_manager_object_key(void);
int
parse_audio_manager_collection_entry(const void                *entry_image,
                                     AudioManagerLoadedObject **out_object);
int
parse_audio_manager_collection_image(const void *collection_image,
                                     DLLList    *loaded_objects);
int
load_audio_manager_collection_from_byte_file(const AudioManagerCollectionFileSource *source,
                                             DLLList                                *loaded_objects);

#endif
