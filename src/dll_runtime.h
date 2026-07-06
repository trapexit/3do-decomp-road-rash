#ifndef DLL_RUNTIME_H
#define DLL_RUNTIME_H

#define DLL_ERROR_NULL_ARGUMENT (-1)
#define DLL_ERROR_NO_MEMORY     (-2)
#define DLL_ERROR_BAD_INDEX     (-3)
#define DLL_ERROR_EMPTY_LIST    (-4)
#define DLL_ITERATE_FORWARD     1

typedef int (*DLLCompareFunction)(int left_value,
                                  int right_value);

typedef struct DLLNode DLLNode;

struct DLLNode
{
  DLLNode *next;
  DLLNode *previous;
  void *value;
};

typedef struct DLLList
{
  int count;
  DLLNode *head;
  DLLNode *tail;
  DLLCompareFunction compare;
} DLLList;

typedef struct DLLIterator
{
  DLLList *list;
  DLLNode *current;
  unsigned char forward;
  unsigned char forward_alignment[3];
} DLLIterator;

int
DLL_GetIndexed(DLLList *list,
               int      index,
               void   **out_value);
DLLIterator *
DLL_IterNew(DLLList *list,
            int      forward);
int
DLL_IterReset(DLLIterator *iterator);
int
DLL_IterValid(DLLIterator *iterator);
int
DLL_IterFirst(DLLIterator *iterator);
int
DLL_IterNext(DLLIterator *iterator);
void
DLL_IterDelete(DLLIterator *iterator);

#endif
