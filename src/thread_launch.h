#ifndef ROADRASH_THREAD_LAUNCH_H
#define ROADRASH_THREAD_LAUNCH_H

typedef void (*ThreadEntryPoint)();

int
launch_named_thread(ThreadEntryPoint entry_point,
                    int              stack_size,
                    int              priority,
                    char            *name);

#endif
