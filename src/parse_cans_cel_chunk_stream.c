#include "front_end_cel_runtime.h"
#include "rw_semantic_data.h"

/* Reconstructed from the original function at 0x00006958. */

CCB *
parse_cans_cel_chunk_stream(CansChunkHeader *first_chunk,
                            void           **out_plut)
{
  CansChunkHeader *chunk;
  CCB *ccb;
  CelData *pixel_data;
  void *plut;
  int complete;

  ccb = 0;
  pixel_data = 0;
  plut = 0;
  complete = 0;

  if(out_plut != 0)
    {
      *out_plut = 0;
    }

  chunk = first_chunk;
  do
    {
      if(chunk->tag == CANS_CHUNK_TAG_PDAT)
        {
          pixel_data = (CelData *)
                       ((CansPixelDataChunk *)chunk)->pixels;
          complete = 1;
        }
      else if(chunk->tag == CANS_CHUNK_TAG_PLUT)
        {
          plut = ((CansPlutChunk *)chunk)->entries;
          if(out_plut != 0)
            {
              *out_plut = plut;
            }
        }
      else if(chunk->tag == CANS_CHUNK_TAG_RPDT)
        {
          pixel_data = (CelData *)
                       ((CansLegacyReducedPixelDataChunk *)chunk)->pixels;
          complete = 1;
        }
      else if(chunk->tag == CANS_CHUNK_TAG_AACL)
        {
          CansAaclChunk *aacl;

          aacl = (CansAaclChunk *)chunk;
          ccb = (CCB *)(aacl->payload + aacl->ccb_offset);
          plut = aacl->payload + aacl->plut_offset;
          if(out_plut != 0)
            {
              *out_plut = plut;
            }
          complete = 1;
        }
      else if(chunk->tag == CANS_CHUNK_TAG_CCB)
        {
          ccb = &((CansCcbChunk *)chunk)->ccb;
        }

      chunk = (CansChunkHeader *)
              ((char *)chunk + chunk->byte_count);
    } while(complete == 0);

  if(ccb != 0)
    {
      if((ccb->ccb_Flags & CCB_PPABS) != 0 && plut != 0)
        {
          ccb->ccb_PLUTPtr = plut;
        }
      if((ccb->ccb_Flags & CCB_SPABS) != 0)
        {
          ccb->ccb_SourcePtr = pixel_data;
        }
      resolve_ccb_dimension_preambles(ccb);
    }
  else
    {
      ccb = display_runtime_globals.fallback_ccb;
    }

  return ccb;
}
