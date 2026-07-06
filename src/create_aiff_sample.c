#include "aiff_sample_metadata.h"

AiffSampleMetadata *
parse_aiff_sample_metadata(int                 form_type,
                           unsigned char      *common_chunk,
                           unsigned char      *marker_chunk,
                           unsigned char      *instrument_chunk,
                           AiffSampleMetadata *metadata);
int
create_sample_from_metadata(AiffSampleMetadata *metadata);

int
create_aiff_sample(int            form_type,
                   unsigned char *common_chunk,
                   unsigned char *marker_chunk,
                   unsigned char *instrument_chunk)
{
  AiffSampleMetadata metadata;

  parse_aiff_sample_metadata(form_type, common_chunk, marker_chunk,
                             instrument_chunk, &metadata);
  return create_sample_from_metadata(&metadata);
}
