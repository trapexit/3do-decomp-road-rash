#include "music_file_runtime.h"
#include "score_program_runtime.h"

int
load_score_program_instrument_form(struct MidiPlayer          *player,
                                   ByteFileReader             *reader,
                                   const struct IffFormHeader *form_header,
                                   ScoreProgramRecord         *program)
{
  int result;
  int template_item;

  result = enter_score_iff_form_payload(reader);
  if(result >= 0)
    {
      template_item = load_instrument_template_from_byte_file(reader, -1);
      if(template_item >= 0)
        {
          program->instrument_template_item = template_item;
        }
      else
        {
          result = template_item;
        }
    }

  return result;
}
