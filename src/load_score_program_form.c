#include "iff_runtime.h"
#include "score_program_runtime.h"

int
load_score_program_form(struct MidiPlayer *player,
                        ByteFileReader    *reader,
                        IffFormHeader     *program_form,
                        DLLList           *program_records)
{
  IffFormHeader inner;
  IffChunkHeader outer;
  ScoreProgramRecord *program;
  int limit;
  int offset;
  int result;

  offset = 0;
  program = get_or_create_score_program_record(
    program_records, SCORE_PROGRAM_PENDING_NUMBER);
  if(program == 0)
    {
      return SCORE_PROGRAM_RECORD_ALLOCATION_ERROR;
    }

  result = IFF_EnterForm(reader);
  if(result < 0)
    {
      return result;
    }

  limit = program_form->chunk_size - IFF_FORM_TYPE_SIZE;
  if(limit <= 0)
    {
      return result;
    }

  do
    {
      result = IFF_PeekChunkHeader(reader, &outer);
      if(result != 0)
        {
          return result;
        }

      offset += outer.chunk_size +
                (outer.chunk_size & IFF_CHUNK_ALIGNMENT_MASK) +
                IFF_CHUNK_HEADER_SIZE;
      if(outer.chunk_id == IFF_FORM_CHUNK_ID)
        {
          result = IFF_PeekFormHeader(reader, &inner);
          if(result != 0)
            {
              return result;
            }

          if(inner.form_type == SCORE_PROGRAM_INSTRUMENT_FORM_ID)
            {
              result = load_score_program_instrument_form(
                player, reader, &inner, program);
              if(result != 0)
                {
                  return result;
                }
            }
          else
            {
              result = IFF_SkipForm(reader, &inner);
              if(result < 0)
                {
                  return result;
                }
            }
        }
      else if(outer.chunk_id == SCORE_PROGRAM_ATTRIBUTES_CHUNK_ID)
        {
          result = load_score_program_attributes_chunk(
            player, reader, &outer, program);
          if(result != 0)
            {
              return result;
            }
        }
      else if(outer.chunk_id == SCORE_PROGRAM_SAMPLE_REFS_CHUNK_ID)
        {
          result = load_score_program_sample_refs_chunk(
            player, reader, &outer, program);
          if(result != 0)
            {
              return result;
            }
        }
      else
        {
          result = IFF_SkipChunk(reader, &outer);
          if(result < 0)
            {
              return result;
            }
        }

      result = IFF_AlignReader(reader);
      if(result != 0)
        {
          return result;
        }
    } while(offset < limit);

  return result;
}
