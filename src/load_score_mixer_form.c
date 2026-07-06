#include "audio.h"
#include "music_file_runtime.h"
#include "score_program_runtime.h"

int
load_score_mixer_form(struct MidiPlayer *player,
                      ByteFileReader    *reader,
                      IffFormHeader     *mixer_form,
                      int               *out_mixer_template_item)
{
  int result;
  int item;

  if(reader == 0 || mixer_form == 0 ||
     out_mixer_template_item == 0)
    {
      return SCORE_PROGRAM_BAD_ARGUMENT_ERROR;
    }

  result = enter_score_iff_form_payload(reader);
  if(result >= 0)
    {
      item = load_instrument_template_from_byte_file(reader, -1);
      if(item >= 0)
        {
          if(*out_mixer_template_item >= 0)
            {
              UnloadInsTemplate(*out_mixer_template_item);
            }
          *out_mixer_template_item = item;
        }
      else
        {
          result = item;
        }
    }

  return result;
}
