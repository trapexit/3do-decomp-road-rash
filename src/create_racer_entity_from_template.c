#include "bss_late_data.h"
#include "race_object_creation.h"
#include "racer_runtime.h"

enum RacerCreationValue
{
  RACER_INITIAL_SURFACE_ACCELERATION_MINIMUM = -37
};

void *
create_racer_entity_from_template(void                        *object_pool,
                                  const RacerCreationTemplate *creation_template,
                                  int                          lateral_position,
                                  int                          track_offset,
                                  RoadSegmentLaneRuntime      *track_segment)
{
  RoadSegmentRuntime *segment;
  RacerEntity *racer;
  int track_position;

  racer = (RacerEntity *)create_race_object_from_descriptor(
    (IntrusiveListHeader *)object_pool,
    (const RaceObjectCreationDescriptor *)creation_template,
    lateral_position, track_offset,
    track_segment);
  if(racer == 0)
    {
      return 0;
    }

  racer->base_slip_budget = creation_template->base_slip_budget;
  racer->active_slip_budget = creation_template->active_slip_budget;
  racer->reserved_150 = creation_template->reserved_150;
  racer->base_drag = creation_template->base_drag;
  racer->surface_contact_scale_8_8 =
    creation_template->surface_contact_scale_8_8;
  racer->minimum_acceleration =
    creation_template->minimum_acceleration;
  racer->contact_threshold = creation_template->contact_threshold;
  racer->impact_strength = creation_template->impact_strength;
  racer->impact_scale = creation_template->impact_scale;
  racer->bounce_scale = creation_template->bounce_scale;
  racer->collision_half_width_x =
    creation_template->collision_half_width_x;
  racer->collision_half_length_z =
    creation_template->collision_half_length_z;
  racer->collision_half_height_y =
    creation_template->collision_half_height_y;
  racer->collision_impulse_scale_8_8 =
    creation_template->collision_impulse_scale_8_8;
  racer->collision_mass = creation_template->collision_mass;
  racer->contact_callback = creation_template->contact;
  racer->template_enabled = creation_template->template_enabled;
  racer->template_visible = creation_template->template_visible;

  racer->collision_scratch = 0;
  racer->contact_impulse.forward = 0;
  racer->contact_impulse.lateral = 0;
  racer->contact_impulse.vertical = 0;
  racer->velocity.forward = 0;
  racer->velocity.lateral = 0;
  racer->velocity.vertical = 0;
  racer->reserved_0f8 = 0;
  racer->reserved_0fc = 0;
  racer->drive_acceleration = 0;
  racer->steering_force = 0;
  racer->surface_acceleration_min =
    RACER_INITIAL_SURFACE_ACCELERATION_MINIMUM;
  racer->surface_acceleration_max = 0;
  racer->track_contact = 0;
  racer->heading = 0;
  racer->right_surface_bound = 0;
  racer->surface_drag = 0;

  segment = (RoadSegmentRuntime *)track_segment->resource_handle;
  track_position = segment->track_position + track_offset;
  racer->right_collision_bound = track_position;
  racer->left_collision_bound = track_position;
  racer->primary_track = track_segment;
  racer->collision_peer = 0;
  racer->secondary_track = track_segment;
  racer->impact_cooldown = 0;
  racer->collision_profile_id = 0;

  return racer;
}
