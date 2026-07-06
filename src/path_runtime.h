#ifndef ROADRASH_PATH_RUNTIME_H
#define ROADRASH_PATH_RUNTIME_H

enum PathRuntimeError
{
  PATH_RUNTIME_ERROR_TOO_LONG = -1
};

int
copy_path_with_capacity(char       *destination,
                        unsigned int capacity,
                        const char *source);
int
append_path_with_capacity(char       *destination,
                          unsigned int capacity,
                          const char *suffix);

#endif
