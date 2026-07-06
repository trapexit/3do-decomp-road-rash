#include "rw_semantic_data.h"

void
resume_track_visibility_scan(void)
{
  track_visibility_scan.current_index = track_visibility_scan.resume_index;
}
