#include "iff_runtime.h"
#include "score_program_runtime.h"

enum IffIdTextIndex
{
  IFF_ID_TEXT_FIRST = 0,
  IFF_ID_TEXT_SECOND = 1,
  IFF_ID_TEXT_THIRD = 2,
  IFF_ID_TEXT_FOURTH = 3,
  IFF_ID_TEXT_END = 4
};

enum IffIdTextShift
{
  IFF_ID_FIRST_SHIFT = 24,
  IFF_ID_SECOND_SHIFT = 16,
  IFF_ID_THIRD_SHIFT = 8
};

int
enter_score_iff_form_payload(ByteFileReader *reader)
{
  int result;

  result = BF_Seek(reader, IFF_FORM_HEADER_SIZE,
                   BYTE_FILE_SEEK_CURRENT);
  if(result >= 0)
    {
      result = 0;
    }
  return result;
}


char *
format_iff_id_text(unsigned int iff_id,
                   char        *text)
{
  if(text != 0)
    {
      text[IFF_ID_TEXT_FIRST] =
        (unsigned char)(iff_id >> IFF_ID_FIRST_SHIFT);
      text[IFF_ID_TEXT_SECOND] =
        (unsigned char)(iff_id >> IFF_ID_SECOND_SHIFT);
      text[IFF_ID_TEXT_THIRD] =
        (unsigned char)(iff_id >> IFF_ID_THIRD_SHIFT);
      text[IFF_ID_TEXT_FOURTH] = (unsigned char)iff_id;
      text[IFF_ID_TEXT_END] = 0;
    }
  return text;
}
