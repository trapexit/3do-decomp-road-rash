#include "family_resource_runtime.h"
#include "front_end_cel_runtime.h"

typedef struct FamilyAnimationFrameVisit
{
  int frame;
  int frame_available;
  CansAnimationFrameVisitor visitor;
  void *visitor_context;
} FamilyAnimationFrameVisit;


static
void
visit_decoded_family_animation_frame(FamilyResourceValue value,
                                     void               *context)
{
  FamilyAnimationFrameVisit *visit;
  CansAnimationFrameOutput output;

  visit = (FamilyAnimationFrameVisit *)context;
  CANS_GetAnimFrame((CansAnimationData *)value, visit->frame, &output);
  if(output.ccb == 0)
    {
      return;
    }

  /*
   * The family group remains locked throughout this callback.  The visitor
   * may inspect or submit the decoded CEL immediately, but must not retain
   * any frame pointer after it returns.
   */
  visit->visitor(&output, visit->visitor_context);
  visit->frame_available = 1;
}


int
visit_family_resource_animation_frame(
    int                       object_ref,
    int                       resource_kind,
    int                       frame,
    CansAnimationFrameVisitor visitor,
    void                     *context)
{
  FamilyAnimationFrameVisit visit;

  if(visitor == 0)
    {
      return 0;
    }

  visit.frame = frame;
  visit.frame_available = 0;
  visit.visitor = visitor;
  visit.visitor_context = context;
  if(visit_family_resource_entry(
       object_ref, resource_kind,
       visit_decoded_family_animation_frame, &visit) == 0)
    {
      return 0;
    }
  return visit.frame_available;
}
