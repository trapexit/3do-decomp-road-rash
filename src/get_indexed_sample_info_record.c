#include "debug.h"
#include "stdio.h"

#include "dll_runtime.h"
#include "rw_semantic_data.h"

int
DLL_GetItemCount(int list);
int
DLL_GetErr(void);
int *
get_indexed_sample_info_record(int sample_info_list,
                               int requested_index)
{
  DLLIterator *iter;
  int *entry;
  int *result;
  int index;
  int count;

  result = (int *)-1;
  index = 0;
  if(sample_info_list == 0 || DLL_GetItemCount(sample_info_list) <= 0 ||
     requested_index < 0 ||
     requested_index >= DLL_GetItemCount(sample_info_list))
    {
      kprintf("Bad params in GetIndexedSampelInfoRec\n");
      fflush(stdout);
      return result;
    }

  iter = DLL_IterNew((DLLList *)sample_info_list, DLL_ITERATE_FORWARD);
  if(iter == 0)
    {
      count = DLL_GetErr();
      kprintf("InSampelList:DLL_IterNew(sampleInfoList): failed, error %ld\n",
              count);
      fflush(stdout);
    }
  else
    {
      entry = (int *)DLL_IterFirst(iter);
      while(DLL_IterValid(iter) != 0)
        {
          if(index == requested_index)
            {
              result = entry;
              break;
            }
          index++;
          entry = (int *)DLL_IterNext(iter);
        }
    }

  if(iter != 0)
    {
      DLL_IterDelete(iter);
    }
  return result;
}
