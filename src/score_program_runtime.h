#ifndef ROADRASH_SCORE_PROGRAM_RUNTIME_H
#define ROADRASH_SCORE_PROGRAM_RUNTIME_H

#include "dll_runtime.h"

struct ByteFileReader;
struct IffChunkHeader;
struct IffFormHeader;
struct MidiPlayer;

enum ScoreProgramFormatValue
{
  SCORE_PROGRAM_ATTRIBUTES_CHUNK_ID = 0x53504174,
  SCORE_PROGRAM_SAMPLE_REFS_CHUNK_ID = 0x53505352,
  SCORE_PROGRAM_INSTRUMENT_FORM_ID = 0x5350496E,
  SCORE_PROGRAM_PENDING_NUMBER = 0xFF,
  SCORE_PROGRAM_SAMPLE_INDEX_MASK = 0xFF
};

#define SCORE_RESOURCE_TRANSFER_FLAG_MASK 0xFF

enum ScoreProgramError
{
  SCORE_PROGRAM_BAD_ARGUMENT_ERROR = -21,
  SCORE_PROGRAM_RECORD_ALLOCATION_ERROR = 0x66 - 0x300
};

typedef struct ScoreProgramRecord
{
  unsigned char program_number;
  unsigned char priority;
  unsigned char max_voices;
  unsigned char reserved_03;
  int reserved_04;
  int instrument_template_item;
  DLLList *sample_index_list;
} ScoreProgramRecord;

typedef char ScoreProgramRecordSizeCheck[
    (sizeof(ScoreProgramRecord) == 0x10) ? 1 : -1];

int
read_score_chunk_bytes(struct ByteFileReader *reader,
                       void                  *destination,
                       int                    byte_count,
                       int                   *bytes_read);
int
enter_score_iff_chunk_payload(struct ByteFileReader *reader);
int
enter_score_iff_form_payload(struct ByteFileReader *reader);
char *
format_iff_id_text(unsigned int iff_id,
                   char        *text);
ScoreProgramRecord *
get_or_create_score_program_record(DLLList     *program_records,
                                   unsigned int program_number);
int
load_score_program_attributes_chunk(struct MidiPlayer           *player,
                                    struct ByteFileReader       *reader,
                                    const struct IffChunkHeader *chunk_header,
                                    ScoreProgramRecord          *program);
int
load_score_program_instrument_form(struct MidiPlayer          *player,
                                   struct ByteFileReader      *reader,
                                   const struct IffFormHeader *form_header,
                                   ScoreProgramRecord         *program);
int
load_score_program_sample_refs_chunk(struct MidiPlayer           *player,
                                     struct ByteFileReader       *reader,
                                     const struct IffChunkHeader *chunk_header,
                                     ScoreProgramRecord          *program);
int
cleanup_score_program_records(struct MidiPlayer *player,
                              DLLList           *program_records,
                              int                resources_transferred);
int
cleanup_score_sample_items(struct MidiPlayer *player,
                           DLLList           *sample_items,
                           int                resources_transferred);

#endif
