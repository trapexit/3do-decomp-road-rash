/* Reconstructed from the original function at 0x00009274. */

int
RSRC_ReleaseResource(int,
                     int);
int
RSRC_DisposeResource(int,
                     int);

#define FRONT_END_CEL_RESOURCE_TYPE 0x43454C20U
#define FRONT_END_CEL_DISPOSE_MASK 0xFFU

static
int
call_rsrc_release_resource(int resource_type,
                           int resource_id)
{
  int (*fn)(int,
            int);

  fn = RSRC_ReleaseResource;
  return fn(resource_type, resource_id);
}


static
int
call_rsrc_dispose_resource(int resource_type,
                           int resource_id)
{
  int (*fn)(int,
            int);

  fn = RSRC_DisposeResource;
  return fn(resource_type, resource_id);
}


int
finalize_front_end_cel_resource(int resource_index,
                                int dispose_flags)
{
  resource_index++;
  if((dispose_flags & FRONT_END_CEL_DISPOSE_MASK) == 0)
    {
      return call_rsrc_release_resource(
        FRONT_END_CEL_RESOURCE_TYPE, resource_index);
    }
  return call_rsrc_dispose_resource(
    FRONT_END_CEL_RESOURCE_TYPE, resource_index);
}


#undef FRONT_END_CEL_RESOURCE_TYPE
#undef FRONT_END_CEL_DISPOSE_MASK
