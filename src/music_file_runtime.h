#ifndef ROADRASH_MUSIC_FILE_RUNTIME_H
#define ROADRASH_MUSIC_FILE_RUNTIME_H

#include "iff_runtime.h"

enum MusicFileFormatValue
{
  MUSIC_INSTRUMENT_TEMPLATE_FORM_ID = 0x33494E53
};

enum RoadRashMusicLoadError
{
  ROADRASH_MUSIC_BAD_FORMAT_ERROR = -26,
  ROADRASH_MUSIC_NO_MEMORY_ERROR = -27
};

int
read_music_file_bytes(ByteFileReader *reader,
                      void           *destination,
                      int             byte_count);
int
load_instrument_template_from_byte_file(ByteFileReader *reader,
                                        int             start_position);

#endif
