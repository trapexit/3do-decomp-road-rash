#ifndef ROADRASH_IFF_RUNTIME_H
#define ROADRASH_IFF_RUNTIME_H

#include "byte_file_runtime.h"

enum IffFormatValue
{
  IFF_CHUNK_HEADER_SIZE = 8,
  IFF_FORM_HEADER_SIZE = 0x0C,
  IFF_FORM_TYPE_SIZE = 4,
  IFF_CHUNK_ALIGNMENT_MASK = 1,
  IFF_FORM_CHUNK_ID = 0x464F524D
};

typedef struct IffChunkHeader
{
  unsigned int chunk_id;
  int chunk_size;
} IffChunkHeader;

typedef struct IffFormHeader
{
  unsigned int chunk_id;
  int chunk_size;
  unsigned int form_type;
} IffFormHeader;

int
IFF_PeekChunkHeader(ByteFileReader *reader,
                    IffChunkHeader *header);
int
IFF_PeekFormHeader(ByteFileReader *reader,
                   IffFormHeader  *header);
int
IFF_EnterForm(ByteFileReader *reader);
int
IFF_FindForm(ByteFileReader *reader,
             unsigned int    form_type,
             int             start_position,
             IffFormHeader  *header);
int
IFF_SkipChunk(ByteFileReader       *reader,
              const IffChunkHeader *header);
int
IFF_SkipForm(ByteFileReader      *reader,
             const IffFormHeader *form_header);
int
IFF_FindChunk(ByteFileReader *reader,
              unsigned int    chunk_id,
              int             start_position,
              IffChunkHeader *header);
int
IFF_ReadChunk(ByteFileReader *reader,
              unsigned int    chunk_id,
              int             start_position,
              int             destination,
              int            *chunk_allocation_bytes);
int
IFF_AlignReader(ByteFileReader *reader);
void *
find_iff_form_in_image(const void  *form_data,
                       unsigned int form_type,
                       int         *out_form_size);

#endif
