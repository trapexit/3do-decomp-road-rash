#include "score_context_api.h"

int
UnloadInsTemplate(int instrument_template_item);

int
unload_positive_score_instrument_templates(ScoreContext *score_context)
{
  int program_index;
  PIMapEntry *program;

  program_index = 0;
  if(score_context->scon_PIMapSize > 0)
    {
      do
        {
          program = &score_context->scon_PIMap[program_index];
          if(program->pimp_InsTemplate > 0)
            {
              UnloadInsTemplate(program->pimp_InsTemplate);
              program = &score_context->scon_PIMap[program_index];
              program->pimp_InsTemplate = 0;
            }
          program_index++;
        } while(program_index < score_context->scon_PIMapSize);
    }
  return 0;
}
