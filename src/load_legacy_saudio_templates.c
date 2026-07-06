#include "saudio_subscriber_runtime.h"

long
LoadTemplates(SAudioTemplateRecord *templates,
              long                 *template_tags,
              long                  template_count)
{
  Item template_item;

  while(*template_tags != 0)
    {
      template_item = get_legacy_saudio_template_item(
        templates, *template_tags, template_count);
      if(template_item < 0)
        {
          return template_item;
        }
      template_tags++;
    }
  return 0;
}
