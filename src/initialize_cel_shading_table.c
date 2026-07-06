#include "bss_early_data.h"

void
write_cel_shading_pixc_entry(int           shade_level,
                             int           shade_column,
                             unsigned int *pixc_entry);

void
initialize_cel_shading_table(void)
{
  int row;
  int column;
  unsigned int *pixc_entry;

  row = 0;
  while(row < CEL_PIXC_SHADE_ROWS)
    {
      column = 0;
      while(column < CEL_PIXC_SHADE_COLUMNS)
        {
          pixc_entry = &gCelPixcShadingTable[row][column];
          *pixc_entry = 0;
          write_cel_shading_pixc_entry(row, column, pixc_entry);
          column++;
        }
      row++;
    }
}
