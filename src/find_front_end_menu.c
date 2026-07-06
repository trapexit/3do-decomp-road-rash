/* Reconstructed from the original function at 0x00008D7C. */



#include "rw_semantic_data.h"

FrontEndMenu *
find_front_end_menu(int menu_id)
{
  FrontEndMenu *menu;

  menu = front_end_menus;
  while(menu->id != -1)
    {
      if(menu->id == menu_id)
        {
          return menu;
        }
      menu++;
    }
  return 0;
}
