#include "midi_player_runtime.h"
#include "string.h"
char *
OMP_NextIFFChunk(char *chunk)
{
  IffChunkHeader header;
  char *next_chunk;

  memcpy(&header, chunk, IFF_CHUNK_HEADER_SIZE);
  next_chunk = chunk + header.chunk_size + IFF_CHUNK_HEADER_SIZE;
  if(((int)next_chunk & IFF_CHUNK_ALIGNMENT_MASK) != 0)
    {
      next_chunk++;
    }

  return next_chunk;
}
