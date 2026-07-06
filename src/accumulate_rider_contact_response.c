#include "rider_contact_runtime.h"

enum RiderContactResponseConstant
{
  RIDER_CONTACT_RESPONSE_LIMIT = 0x100000
};

void
accumulate_rider_contact_response(RacerEntity *recipient,
                                  RacerEntity *source,
                                  int          response_slot)
{
  RiderInteractionRuntime *recipient_interaction;
  RiderInteractionRuntime *source_interaction;
  RiderInteractionMetric *response;
  RiderInteractionMetric *metric;
  signed char metric_index;
  signed char component_index;
  int value;

  recipient_interaction = recipient->paired_state->interaction_data;
  source_interaction = source->paired_state->interaction_data;
  response_slot = (signed char)response_slot;

  metric_index = source_interaction->metric_index;
  if(metric_index >= RIDER_INTERACTION_METRIC_COUNT)
    {
      return;
    }

  response = &recipient_interaction->response_deltas[response_slot];
  metric = &recipient_interaction->current_metrics[metric_index];
  component_index = 0;
  do
    {
      value = metric->components[component_index] +
              response->components[component_index];
      metric->components[component_index] = value;
      if(value > RIDER_CONTACT_RESPONSE_LIMIT)
        {
          metric->components[component_index] =
            RIDER_CONTACT_RESPONSE_LIMIT;
        }
      else if(value < -RIDER_CONTACT_RESPONSE_LIMIT)
        {
          metric->components[component_index] =
            -RIDER_CONTACT_RESPONSE_LIMIT;
        }
      component_index++;
    } while(component_index < RIDER_INTERACTION_METRIC_COMPONENT_COUNT);
}


void
rider_update_mode_0(void)
{
}
