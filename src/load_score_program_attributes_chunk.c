#include "iff_runtime.h"
#include "score_program_runtime.h"

typedef struct ScoreProgramAttributes
{
  unsigned char program_number;
  unsigned char priority;
  unsigned char max_voices;
  unsigned char reserved_03;
  int reserved_04;
} ScoreProgramAttributes;

int
load_score_program_attributes_chunk(struct MidiPlayer    *player,
                                    ByteFileReader       *reader,
                                    const IffChunkHeader *chunk_header,
                                    ScoreProgramRecord   *program)
{
  ScoreProgramAttributes attributes;
  int result;

  result = enter_score_iff_chunk_payload(reader);
  if(result < 0)
    {
      return result;
    }

  result = read_score_chunk_bytes(reader, &attributes,
                                  chunk_header->chunk_size, 0);
  if(result < 0)
    {
      return result;
    }

  result = IFF_AlignReader(reader);
  if(result != 0)
    {
      return result;
    }

  program->program_number = attributes.program_number;
  program->priority = attributes.priority;
  program->max_voices = attributes.max_voices;
  program->reserved_03 = attributes.reserved_03;
  program->reserved_04 = attributes.reserved_04;
  return result;
}
