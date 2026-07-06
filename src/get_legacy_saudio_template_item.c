#include "audio.h"

#include "saudio_subscriber_runtime.h"

Item
get_legacy_saudio_template_item(SAudioTemplateRecord *templates,
                                long                  template_tag,
                                long                  template_count)
{
  long index;

  index = 0;
  if(template_count > 0)
    {
      do
        {
          if(templates->template_tag == template_tag)
            {
              if(templates->template_item <= 0)
                {
                  templates->template_item =
                    LoadInsTemplate((char *)templates->instrument_name,
                                    0);
                }
              return templates->template_item;
            }
          index++;
          templates++;
        } while(index < template_count);
    }

  return SAUDIO_ERROR_TEMPLATE_NOT_FOUND;
}
