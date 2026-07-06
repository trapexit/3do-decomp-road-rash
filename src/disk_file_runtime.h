#ifndef DISK_FILE_RUNTIME_H
#define DISK_FILE_RUNTIME_H

int
disk_file_exists(char *path);
int
auto_maintain_nvram_filesystem(void);
int
load_disk_file(char  *path,
               char **buffer_out,
               int   *buffer_capacity_out);
int
write_disk_file(char *path,
                char *data,
                int   byte_count);
int
delete_disk_file(char *path);

#endif
