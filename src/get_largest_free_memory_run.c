#include "platform.h"

#define MEM_FREE_BITMAP_BITS_PER_WORD 32

int
get_largest_free_memory_run(uint32 memory_type)
{
  List *memory_lists;
  MemList *memory_list;
  MemHdr *memory_header;
  uint32 free_page_word;
  int largest_run_pages;
  int current_run_pages;
  int word_index;
  int bit_index;

  largest_run_pages = 0;
  memory_lists = KernelBase->kb_MemFreeLists;
  memory_list = (MemList *)FirstNode(memory_lists);
  memory_header = memory_list->meml_MemHdr;

  while((memory_header->memh_Types & memory_type) == 0)
    {
      if((Node *)memory_list == LastNode(memory_lists))
        {
          return 0;
        }
      memory_list = (MemList *)NextNode((Node *)memory_list);
      memory_header = memory_list->meml_MemHdr;
    }

  current_run_pages = 0;
  word_index = 0;
  while(word_index < memory_header->memh_FreePageBitsSize)
    {
      free_page_word = memory_header->memh_FreePageBits[word_index];
      bit_index = 0;
      while(bit_index < MEM_FREE_BITMAP_BITS_PER_WORD)
        {
          if((free_page_word & (1U << bit_index)) != 0)
            {
              current_run_pages++;
            }
          else
            {
              if(current_run_pages > largest_run_pages)
                {
                  largest_run_pages = current_run_pages;
                }
              current_run_pages = 0;
            }
          bit_index++;
        }
      word_index++;
    }

  return memory_header->memh_PageSize * largest_run_pages;
}


#undef MEM_FREE_BITMAP_BITS_PER_WORD
