#include "cans_family_validation.h"
#include "cans_validated_frame.h"
#include "display_cel_runtime.h"
#include "family_resource_runtime.h"
#include "road_texture_cache_residency.h"
#include "roadside_object_rendering.h"
#include "rw_semantic_data.h"

#include "assert.h"
#include "string.h"

enum RoadsideObjectRenderConstant
{
  ROADSIDE_OBJECT_MODE_REPEATED = 0,
  ROADSIDE_OBJECT_MODE_STATIC = 1,
  ROADSIDE_OBJECT_MODE_MASK = 0xFF,
  ROADSIDE_OBJECT_SCALE_ONE = 8,
  ROADSIDE_OBJECT_SCALE_SHIFT = 3,
  ROADSIDE_OBJECT_DEPTH_INDEX_BIAS = 0x80,
  ROADSIDE_OBJECT_DEPTH_INDEX_SHIFT = 1,
  ROADSIDE_OBJECT_NEAR_WIDTH_LIMIT = 0x18000,
  ROADSIDE_OBJECT_FAR_WIDTH_LIMIT = 0x30000,
  ROADSIDE_OBJECT_MIDDLE_WIDTH_SHIFT = 1,
  ROADSIDE_OBJECT_FAR_WIDTH_SHIFT = 2,
  ROADSIDE_OBJECT_NEAR_BUCKET = 0,
  ROADSIDE_OBJECT_MIDDLE_BUCKET = 1,
  ROADSIDE_OBJECT_FAR_BUCKET = 2,
  ROADSIDE_OBJECT_REPEATED_FAMILY_INDEX = 0,
  ROADSIDE_OBJECT_STATIC_FAMILY_INDEX = 4,
  ROADSIDE_OBJECT_STATIC_CHILD_INDEX = 0,
  ROADSIDE_OBJECT_SELECTOR_MASK = 0xFF
};

enum RoadsideCollisionBindingConstant
{
  ROADSIDE_COLLISION_BINDING_LANE_COUNT = ROAD_SEGMENT_COUNT * 2,
  ROADSIDE_COLLISION_BINDING_HASH_MASK =
    ROADSIDE_COLLISION_BINDING_LANE_COUNT - 1,
  ROADSIDE_COLLISION_BINDING_POINTER_SHIFT = 2,
  ROADSIDE_COLLISION_BINDING_HASH_FOLD_SHIFT = 8,
  ROADSIDE_COLLISION_BINDING_HASH_WORD_SHIFT = 16,
  ROADSIDE_COLLISION_PUBLICATION_COUNT_SHIFT = 8,
  ROADSIDE_COLLISION_PUBLICATION_COUNT_MASK = 0x0000FF00,
  ROADSIDE_COLLISION_PUBLICATION_CACHED = 0x00010000,
  ROADSIDE_COLLISION_PUBLICATION_VALID = 0x00020000,
  ROADSIDE_COLLISION_PUBLICATION_MASK = 0x0003FF00
};

typedef struct RoadsideCollisionBindingState
{
  RoadSegmentLaneRuntime *lane;
  FamilyResourceBindingIdentity repeated_object_identity;
  FamilyResourceBindingIdentity
    static_object_identities[ROAD_STATIC_OBJECT_CACHE_COUNT];
} RoadsideCollisionBindingState;

static RoadsideCollisionBindingState
  roadside_collision_binding_states[
  ROADSIDE_COLLISION_BINDING_LANE_COUNT];
static const RoadSegmentLaneRuntime *last_collision_binding_lane;
static RoadsideCollisionBindingState *last_collision_binding_state;


static __inline
unsigned int
roadside_collision_binding_hash(const RoadSegmentLaneRuntime *lane)
{
  unsigned long address;

  address = (unsigned long)lane >>
            ROADSIDE_COLLISION_BINDING_POINTER_SHIFT;
  address ^= address >> ROADSIDE_COLLISION_BINDING_HASH_FOLD_SHIFT;
  address ^= address >> ROADSIDE_COLLISION_BINDING_HASH_WORD_SHIFT;
  return (unsigned int)address &
         ROADSIDE_COLLISION_BINDING_HASH_MASK;
}

void
reset_all_roadside_collision_binding_states(void)
{
  memset(roadside_collision_binding_states, 0,
         sizeof(roadside_collision_binding_states));
  last_collision_binding_lane = 0;
  last_collision_binding_state = 0;
}


static __inline
void
clear_family_resource_binding_identity(FamilyResourceBindingIdentity *identity)
{
  identity->selector = 0;
  identity->generation = 0;
}


static __inline
int
roadside_resource_binding_identity(unsigned int                   selector,
                                   FamilyResourceBindingIdentity *identity)
{
  unsigned int generation;
  unsigned int group_index;
  int family_id;

  clear_family_resource_binding_identity(identity);

  /* Family tables are refilled in place, so pointer identity alone cannot
     prove that cached frames still belong to the selected resource. */
  group_index = FAMILY_RESOURCE_GROUP_INDEX(selector);
  family_id =
    family_resource_globals.resident_family_ids[group_index];
  generation = family_resource_generations[group_index];
  if(family_id <= 0 || generation == 0)
    {
      return 0;
    }

  identity->selector = selector & ROADSIDE_OBJECT_SELECTOR_MASK;
  identity->generation = generation;
  return 1;
}


static
int
roadside_resource_binding_identity_is_current(const FamilyResourceBindingIdentity *identity,
                                              unsigned int                         selector)
{
  FamilyResourceBindingIdentity current;

  if(roadside_resource_binding_identity(selector, &current) == 0)
    {
      return 0;
    }
  return (identity->selector & ROADSIDE_OBJECT_SELECTOR_MASK) ==
         current.selector &&
         identity->generation == current.generation;
}


static __inline
RoadTextureBindingState *
roadside_texture_binding_state(RoadTextureCache *cache)
{
  int cache_index;

  cache_index = road_texture_cache_pool_index(cache);
  if(cache_index < 0)
    {
      return 0;
    }
  return &gRoadTextureBindingStates[cache_index];
}


static
RoadsideCollisionBindingState *
lookup_roadside_collision_binding_state(RoadSegmentLaneRuntime *lane,
                                        int                     allocate)
{
  RoadsideCollisionBindingState *binding_state;
  unsigned int hash_index;
  int state_index;

  if(lane == 0)
    {
      return 0;
    }
  if(lane == last_collision_binding_lane &&
     last_collision_binding_state != 0 &&
     last_collision_binding_state->lane == lane)
    {
      return last_collision_binding_state;
    }

  hash_index = roadside_collision_binding_hash(lane);
  for(state_index = 0;
      state_index < ROADSIDE_COLLISION_BINDING_LANE_COUNT;
      state_index++)
    {
      binding_state = &roadside_collision_binding_states[
        (hash_index + (unsigned int)state_index) &
        ROADSIDE_COLLISION_BINDING_HASH_MASK];
      if(binding_state->lane == lane)
        {
          last_collision_binding_lane = lane;
          last_collision_binding_state = binding_state;
          return last_collision_binding_state;
        }
      if(binding_state->lane == 0)
        {
          if(allocate == 0)
            {
              return 0;
            }
          memset(binding_state, 0, sizeof(*binding_state));
          binding_state->lane = lane;
          last_collision_binding_lane = lane;
          last_collision_binding_state = binding_state;
          return binding_state;
        }
    }
  return 0;
}


static __inline
RoadsideCollisionBindingState *
find_roadside_collision_binding_state(const RoadSegmentLaneRuntime *lane)
{
  return lookup_roadside_collision_binding_state(
    (RoadSegmentLaneRuntime *)lane, 0);
}


static __inline
RoadsideCollisionBindingState *
roadside_collision_binding_state(RoadSegmentLaneRuntime *lane)
{
  return lookup_roadside_collision_binding_state(lane, 1);
}


void
reset_roadside_collision_binding_state(RoadSegmentLaneRuntime *lane)
{
  RoadsideCollisionBindingState *binding_state;

  /* Population must invalidate a reused lane, but it must not eagerly
     allocate a sidecar for every startup lane.  Rendering allocates the
     dense table lazily when a lane first needs collision provenance. */
  binding_state = find_roadside_collision_binding_state(lane);
  if(binding_state != 0)
    {
      clear_family_resource_binding_identity(
        &binding_state->repeated_object_identity);
      memset(binding_state->static_object_identities, 0,
             sizeof(binding_state->static_object_identities));
    }
}


int
roadside_repeated_collision_provenance_is_current(const RoadSegmentLaneRuntime *lane)
{
  RoadsideCollisionBindingState *binding_state;

  binding_state = find_roadside_collision_binding_state(lane);
  if(binding_state == 0)
    {
      return 0;
    }
  return roadside_resource_binding_identity_is_current(
    &binding_state->repeated_object_identity,
    lane->repeated_objects.resource_selector);
}


int
roadside_static_collision_provenance_is_current(const RoadSegmentLaneRuntime *lane,
                                                int                           object_index)
{
  RoadsideCollisionBindingState *binding_state;

  if(lane == 0 || object_index < 0 ||
     object_index >= ROAD_STATIC_OBJECT_CACHE_COUNT)
    {
      return 0;
    }
  binding_state = find_roadside_collision_binding_state(lane);
  if(binding_state == 0)
    {
      return 0;
    }
  return roadside_resource_binding_identity_is_current(
    &binding_state->static_object_identities[object_index],
    lane->static_objects[object_index].resource_selector);
}


void
reset_road_texture_binding_state(RoadTextureBindingState *state_)
{
  if(state_ != NULL)
    memset(state_, 0, sizeof(*state_));
}


static __inline
int
decode_roadside_cel_extent(int encoded_extent)
{
  if(encoded_extent >= 0)
    {
      return (int)((unsigned int)encoded_extent &
                   CANS_CCB_DIMENSION_LITERAL_MASK);
    }

  return (int)(1U <<
               (encoded_extent & DISPLAY_CEL_SOURCE_SHIFT_MASK));
}


static
void
clear_roadside_animation_frames(CansAnimationFrameOutput *frames)
{
  int index;

  for(index = 0;
      index < ROAD_OBJECT_CEL_SCALE_BUCKET_COUNT;
      index++)
    {
      memset(&frames[index], 0, sizeof(frames[index]));
    }
}


static __inline
void
record_roadside_collision_publication(FamilyResourceBindingIdentity *identity,
                                     unsigned int                   flags_,
                                     unsigned int                   count_,
                                     int                            static_resource_)
{
  unsigned int metadata;

  if((static_resource_ != 0) && (count_ > ROAD_OBJECT_COLLISION_BOX_COUNT))
    count_ = ROAD_OBJECT_COLLISION_BOX_COUNT;
  metadata = (ROADSIDE_COLLISION_PUBLICATION_VALID |
              (count_ << ROADSIDE_COLLISION_PUBLICATION_COUNT_SHIFT));
  if((flags_ &
      ROAD_OBJECT_PLACEMENT_COLLISION_BOXES_CACHED) != 0)
    {
      metadata |= ROADSIDE_COLLISION_PUBLICATION_CACHED;
    }
  identity->selector =
    (identity->selector & ROADSIDE_OBJECT_SELECTOR_MASK) |
    metadata;
}


static __inline
void
capture_roadside_collision_publication(FamilyResourceBindingIdentity *identity,
                                      unsigned int                   flags_,
                                      unsigned int                   count_,
                                      int                            static_resource_)
{
  if((identity->selector &
      ROADSIDE_COLLISION_PUBLICATION_VALID) == 0)
    {
      record_roadside_collision_publication(identity,
                                           flags_,
                                           count_,
                                           static_resource_);
    }
}


static __inline
int
roadside_collision_identity_matches_resource(const FamilyResourceBindingIdentity *identity,
                                             const FamilyResourceBindingIdentity *resource_identity)
{
  return (identity->selector & ROADSIDE_OBJECT_SELECTOR_MASK) ==
         resource_identity->selector &&
         identity->generation == resource_identity->generation;
}


static __inline
void
publish_roadside_collision_identity(FamilyResourceBindingIdentity       *identity,
                                    const FamilyResourceBindingIdentity *resource_identity)
{
  unsigned int metadata;

  metadata = identity->selector &
             ROADSIDE_COLLISION_PUBLICATION_MASK;
  *identity = *resource_identity;
  identity->selector |= metadata;
}


static
void
invalidate_roadside_object_resource(CansAnimationData            **animation_slot,
                                    CansAnimationFrameOutput      *animation_frames,
                                    RoadTextureBindingState        *binding_state,
                                    int                             binding_slot)
{
  clear_road_texture_resource_binding(binding_state, binding_slot);
  *animation_slot = 0;
  clear_roadside_animation_frames(animation_frames);
  /* The oracle treats collision geometry as an independent lane
     publication.  A missing visual resource invalidates stale CEL pointers,
     but retains the existing/default collision bounds until a valid frame
     supplies replacement hotspots. */
}


static
void
cache_roadside_collision_bounds(RoadSegmentLaneRuntime         *lane_,
                               int                             placement_index_,
                               int                             static_resource_,
                               const CansAnimationFrameOutput *frame)
{
  const CansHotspotVector *source;
  RoadObjectBounds *bounds;
  StaticRoadsideObject *object;
  RepeatedRoadsideObjectGroup *group;
  RoadObjectBounds *object_bounds;
  unsigned char *collision_box_count;
  unsigned char *flags;
  int index;
  int count;
  int left;
  int right;
  int scale;

  assert(lane_ != NULL);
  assert(frame != NULL);
  assert((placement_index_ >= 0) &&
         (placement_index_ < ((static_resource_ != 0) ?
                              ROADSIDE_STATIC_OBJECT_COUNT :
                              ROAD_OBJECT_PLACEMENT_COUNT)));

  count = frame->hotspot_count;
  if(count > ROAD_OBJECT_COLLISION_BOX_COUNT)
    {
      count = ROAD_OBJECT_COLLISION_BOX_COUNT;
    }
  if(count <= 0)
    {
      return;
    }

  if(static_resource_ != 0)
    {
      object = &lane_->static_objects[placement_index_];
      collision_box_count = &object->collision_box_count;
      flags = &object->flags;
      object_bounds = object->bounds;
      scale = ROADSIDE_OBJECT_SCALE_ONE;
    }
  else
    {
      group = &lane_->repeated_objects;
      collision_box_count = &group->collision_box_count;
      flags = &group->flags;
      object_bounds = group->bounds;
      scale = group->scale;
    }

  // Only a positive-hotspot valid frame replaces the actual publication.
  // Failures, zero-hotspot frames and unused bound slots retain their bytes.
  *collision_box_count = 0;
  *flags = (unsigned char)(*flags &
                           ~ROAD_OBJECT_PLACEMENT_COLLISION_BOXES_CACHED);

  index = 0;
  do
    {
      source = &frame->auxiliary.hotspot_vectors[index];
      bounds = &object_bounds[index];
      bounds->left = source->start_x;
      bounds->top = source->start_y;
      bounds->right = source->end_x;
      bounds->bottom = source->end_y;

      if((*flags & ROAD_OBJECT_PLACEMENT_MIRRORED) != 0)
        {
          left = -bounds->right;
          right = -bounds->left;
          bounds->left = left;
          bounds->right = right;
        }

      if(scale != ROADSIDE_OBJECT_SCALE_ONE)
        {
          bounds->left =
            (scale * bounds->left) >> ROADSIDE_OBJECT_SCALE_SHIFT;
          bounds->right =
            (scale * bounds->right) >> ROADSIDE_OBJECT_SCALE_SHIFT;
          bounds->top =
            (scale * bounds->top) >> ROADSIDE_OBJECT_SCALE_SHIFT;
          bounds->bottom =
            (scale * bounds->bottom) >> ROADSIDE_OBJECT_SCALE_SHIFT;
        }

      index++;
    } while(index < count);

  *flags |= ROAD_OBJECT_PLACEMENT_COLLISION_BOXES_CACHED;
  // Publish the count last, after bounds and flags are complete.
  *collision_box_count = (unsigned char)count;
}


static __inline
CansAnimationFrameOutput *
prepare_roadside_object_frame(RoadRenderSide *road_side,
                             int             placement_index,
                             int             static_resource,
                             int             bucket)
{
  RoadTextureCache *cache;
  RoadTextureBindingState *binding_state;
  RoadsideCollisionBindingState *collision_binding_state;
  CansAnimationData **animation_slot;
  CansAnimationFrameOutput *animation_frames;
  CansAnimationFrameOutput *frame;
  FamilyResourceBindingIdentity *collision_identity;
  FamilyResourceBindingIdentity resource_identity;
  StaticRoadsideObject *object;
  RepeatedRoadsideObjectGroup *group;
  unsigned char *flags;
  unsigned char *collision_box_count;
  unsigned int resource_selector;
  int collision_identity_changed;
  int binding_identity_changed;
  int binding_slot;
  unsigned int generation;
  unsigned int group_index;

  assert(road_side != NULL);
  assert(road_side->lane != NULL);
  assert((placement_index >= 0) &&
         (placement_index < ((static_resource != 0) ?
                             ROADSIDE_STATIC_OBJECT_COUNT :
                             ROAD_OBJECT_PLACEMENT_COUNT)));

  cache = road_side->texture_cache;
  binding_state = road_side->texture_binding_state;
  if(binding_state == 0)
    {
      binding_state = roadside_texture_binding_state(cache);
    }
  if(binding_state == 0)
    {
      return 0;
    }

  if(static_resource != 0)
    {
      object = &road_side->lane->static_objects[placement_index];
      flags = &object->flags;
      collision_box_count = &object->collision_box_count;
      resource_selector = object->resource_selector;
      animation_slot = &cache->static_object_animations[placement_index];
      animation_frames = cache->static_object_frames[placement_index];
      binding_slot =
        ROAD_TEXTURE_BINDING_STATIC_OBJECT_FIRST + placement_index;
    }
  else
    {
      group = &road_side->lane->repeated_objects;
      flags = &group->flags;
      collision_box_count = &group->collision_box_count;
      resource_selector = group->resource_selector;
      animation_slot = &cache->repeated_object_animation;
      animation_frames = cache->repeated_object_frames;
      binding_slot = ROAD_TEXTURE_BINDING_REPEATED_OBJECT;
    }
  frame = &animation_frames[bucket];

  collision_binding_state = roadside_collision_binding_state(
    road_side->lane);
  if(collision_binding_state == 0)
    {
      clear_road_texture_resource_binding(
        binding_state, binding_slot);
      return 0;
    }
  if(static_resource != 0)
    {
      collision_identity =
        &collision_binding_state
        ->static_object_identities[placement_index];
    }
  else
    {
      collision_identity =
        &collision_binding_state->repeated_object_identity;
    }

  group_index = FAMILY_RESOURCE_GROUP_INDEX(resource_selector);
  generation = family_resource_generations[group_index];
  if(frame->ccb != 0 &&
     (*flags &
      ROAD_OBJECT_PLACEMENT_COLLISION_BOXES_CACHED) != 0 &&
     family_resource_globals.resident_family_ids[group_index] > 0 &&
     generation != 0 &&
     binding_state->resource_selectors[binding_slot] ==
       (unsigned char)(resource_selector &
                       ROADSIDE_OBJECT_SELECTOR_MASK) &&
     binding_state->resource_generations[binding_slot] == generation &&
     (collision_identity->selector & ROADSIDE_OBJECT_SELECTOR_MASK) ==
       binding_state->resource_selectors[binding_slot] &&
     collision_identity->generation == generation)
    {
      frame->ccb->ccb_SourcePtr = frame->pixel_data;
      return frame;
    }
  capture_roadside_collision_publication(collision_identity,
                                        *flags,
                                        *collision_box_count,
                                        static_resource);

  if(roadside_resource_binding_identity(
       resource_selector, &resource_identity) == 0)
    {
      invalidate_roadside_object_resource(
        animation_slot, animation_frames, binding_state, binding_slot);
      return 0;
    }

  binding_identity_changed =
    !road_texture_resource_binding_matches(
      binding_state, binding_slot, &resource_identity);
  collision_identity_changed =
    !roadside_collision_identity_matches_resource(
      collision_identity, &resource_identity);
  if(binding_identity_changed != 0)
    {
      clear_road_texture_resource_binding(binding_state, binding_slot);
      *animation_slot = 0;
      clear_roadside_animation_frames(animation_frames);
    }
  if(frame->ccb == 0)
    {
      if(*animation_slot == 0)
        {
          if(static_resource != 0)
            {
              *animation_slot = (CansAnimationData *)
                lookup_family_resource_child_for_render(
                  resource_selector,
                  ROADSIDE_OBJECT_STATIC_FAMILY_INDEX,
                  ROADSIDE_OBJECT_STATIC_CHILD_INDEX);
            }
          else
            {
              *animation_slot = (CansAnimationData *)
                lookup_family_resource_entry_for_render(
                  resource_selector,
                  ROADSIDE_OBJECT_REPEATED_FAMILY_INDEX);
            }
        }

      if(*animation_slot == 0 ||
         family_cans_animation_has_cached_valid_root(
           resource_selector, *animation_slot) == 0)
        {
          invalidate_roadside_object_resource(
            animation_slot, animation_frames, binding_state, binding_slot);
          return 0;
        }

      CANS_GetValidatedFamilyAnimFrame(*animation_slot, bucket, frame);
      if(frame->ccb == 0)
        {
          invalidate_roadside_object_resource(
            animation_slot, animation_frames, binding_state, binding_slot);
          return 0;
        }
    }

  if(collision_identity_changed != 0)
    {
      if(frame->hotspot_count > 0)
        {
          cache_roadside_collision_bounds(road_side->lane,
                                         placement_index,
                                         static_resource,
                                         frame);
        }
    }
  else if((*flags &
           ROAD_OBJECT_PLACEMENT_COLLISION_BOXES_CACHED) == 0 &&
          frame->hotspot_count > 0)
    {
      cache_roadside_collision_bounds(road_side->lane,
                                     placement_index,
                                     static_resource,
                                     frame);
    }
  record_roadside_collision_publication(collision_identity,
                                       *flags,
                                       *collision_box_count,
                                       static_resource);
  record_road_texture_resource_binding(
    binding_state, binding_slot, &resource_identity);
  publish_roadside_collision_identity(
    collision_identity, &resource_identity);
  frame->ccb->ccb_SourcePtr = frame->pixel_data;
  return frame;
}


// Constant-mode entry points let ARMCC specialize the shared render path.
static
__inline
int
_render_roadside_object_cel(RoadRenderSide *road_side_,
                          int             placement_index_,
                          int             use_static_resource_)
{
  CansAnimationFrameOutput *frame;
  RoadRenderSide *road_side;
  StaticRoadsideObject *object;
  RepeatedRoadsideObjectGroup *group;
  int placement_index;
  int object_scale;
  unsigned int flags;
  CCB *cel;
  int scale;
  int width;
  int bucket;
  int x;
  int y;
  int center_offset;
  int extent;
  int clip_height;
  int static_resource;
  DisplayAxisAlignedCelMapping mapping;

  assert(road_side_ != NULL);
  assert(road_side_->lane != NULL);
  road_side = road_side_;
  placement_index = placement_index_;
  static_resource =
    (use_static_resource_ & ROADSIDE_OBJECT_MODE_MASK) !=
    ROADSIDE_OBJECT_MODE_REPEATED;
  if(placement_index < 0 ||
     (static_resource != 0 &&
      placement_index >= ROAD_STATIC_OBJECT_CACHE_COUNT) ||
     (static_resource == 0 &&
      placement_index >= ROAD_OBJECT_PLACEMENT_COUNT))
    {
      return 0;
    }

  group = &road_side->lane->repeated_objects;
  object = NULL;
  if(static_resource != 0)
    {
      assert(placement_index < ROADSIDE_STATIC_OBJECT_COUNT);
      object = &road_side->lane->static_objects[placement_index];
      object_scale = ROADSIDE_OBJECT_SCALE_ONE;
    }
  else
    {
      object_scale = group->scale;
    }

  scale = road_renderer_state.reciprocal_table[
    (road_side->projection_depth + ROADSIDE_OBJECT_DEPTH_INDEX_BIAS) >>
    ROADSIDE_OBJECT_DEPTH_INDEX_SHIFT];
  width = (scale * object_scale);
  if(width < ROADSIDE_OBJECT_NEAR_WIDTH_LIMIT)
    {
      bucket = ROADSIDE_OBJECT_NEAR_BUCKET;
    }
  else if(width >= ROADSIDE_OBJECT_FAR_WIDTH_LIMIT)
    {
      width >>= ROADSIDE_OBJECT_FAR_WIDTH_SHIFT;
      bucket = ROADSIDE_OBJECT_FAR_BUCKET;
    }
  else
    {
      width >>= ROADSIDE_OBJECT_MIDDLE_WIDTH_SHIFT;
      bucket = ROADSIDE_OBJECT_MIDDLE_BUCKET;
    }

  frame = prepare_roadside_object_frame(
    road_side, placement_index, static_resource, bucket);
  if(frame == 0)
    {
      /* Collision publication is independent of visual-cache availability
         and remains valid across a failed render. */
      return 0;
    }

  cel = frame->ccb;
  if(static_resource != 0)
    {
      x = object->lateral_position;
      flags = object->flags;
    }
  else
    {
      x = group->lateral_positions[placement_index];
      flags = group->flags;
    }

  x += (road_side->next_sides[0]->world_x + road_side->world_x) >> 1;
  x -= road_renderer_state.motion.projection_origin_x;
  x = scale * x + ROAD_PROJECTION_SCREEN_X_CENTER;

  center_offset = width * frame->center_x;
  if((flags & ROAD_OBJECT_PLACEMENT_MIRRORED) != 0)
    {
      x += center_offset;
      if(x <= 0)
        {
          return x;
        }
      mapping.horizontal_delta = -width;
      extent = width * decode_roadside_cel_extent(cel->ccb_Width);
      if(x - extent >= ROAD_SCREEN_RIGHT)
        {
          return x - extent;
        }
    }
  else
    {
      x -= center_offset;
      if(x >= ROAD_SCREEN_RIGHT)
        {
          return x;
        }
      mapping.horizontal_delta = width;
      extent = x + width * decode_roadside_cel_extent(cel->ccb_Width);
      if(extent <= 0)
        {
          return extent;
        }
    }

  y = (static_resource != 0) ?
      object->surface_height : group->surface_heights[placement_index];
  y += (road_side->next_sides[0]->world_y + road_side->world_y) >> 1;
  y = road_renderer_state.motion.projection_origin_y - y;
  y = scale * y + ROAD_PROJECTION_SCREEN_Y_ORIGIN;
  y -= width * frame->center_y;

  clip_height = road_side->owner_node->sort_depth;
  if(clip_height > ROAD_SCREEN_ROAD_BOTTOM)
    {
      if(y >= ROAD_SCREEN_ROAD_BOTTOM + ROAD_FIXED_ONE)
        {
          return y;
        }
    }
  else if(y >= clip_height)
    {
      return y;
    }

  mapping.y = y;
  mapping.x = x;
  mapping.vertical_delta = width;
  return (int)append_axis_aligned_cel(cel, &mapping);
}


int
render_repeated_roadside_object_cel(RoadRenderSide *road_side_,
                                  int             placement_index_)
{
  assert(road_side_ != NULL);
  return _render_roadside_object_cel(road_side_,
                                   placement_index_,
                                   ROADSIDE_OBJECT_MODE_REPEATED);
}


int
render_static_roadside_object_cel(RoadRenderSide *road_side_,
                                int             placement_index_)
{
  assert(road_side_ != NULL);
  return _render_roadside_object_cel(road_side_,
                                   placement_index_,
                                   ROADSIDE_OBJECT_MODE_STATIC);
}
