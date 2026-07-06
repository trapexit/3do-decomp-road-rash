#ifndef BYTE_FILE_RUNTIME_H
#define BYTE_FILE_RUNTIME_H

#include "filesystem.h"
#include "io.h"
#include "platform.h"
#include "runtime_data_types.h"

#define BYTE_FILE_READER_SIZE       0x38
#define BYTE_FILE_ASYNC_STATE_SIZE  0x20
#define BYTE_FILE_SECTOR_SHIFT      11
#define BYTE_FILE_SECTOR_SIZE       FILESYSTEM_DEFAULT_BLOCKSIZE
#define BYTE_FILE_ERROR_TEXT_SIZE   0x50
#define BYTE_FILE_ERROR_SEEK_RANGE  (-4)
#define BYTE_FILE_EXTERNAL_BUFFER_FLAG 0x00000001U
#define BYTE_FILE_ERROR_LOCK_CREATE    (-2)
#define BYTE_FILE_SHARED_PREFIX_SIZE \
        (BYTE_FILE_READER_SIZE - sizeof(int))

typedef enum ByteFileSeekOrigin
{
  BYTE_FILE_SEEK_SET = 1,
  BYTE_FILE_SEEK_CURRENT = 2,
  BYTE_FILE_SEEK_END = 3
} ByteFileSeekOrigin;

typedef struct ByteFileReader
{
  Item file_item;
  FileStatus file_status;
  Item io_request_item;
  IOReq *io_request;
  int position;
} ByteFileReader;

typedef char ByteFileReaderSizeCheck[
    (sizeof(ByteFileReader) == BYTE_FILE_READER_SIZE) ? 1 : -1];

typedef struct ByteFileAsyncReadState
{
  ByteFileReader *reader;
  int start_position;
  int next_sector;
  int requested_bytes;
  int pending_sector_count;
  int completed_bytes;
  unsigned char *destination_cursor;
  unsigned char async_pending;
  unsigned char reserved[3];
} ByteFileAsyncReadState;

typedef char ByteFileAsyncReadStateSizeCheck[
    (sizeof(ByteFileAsyncReadState) == BYTE_FILE_ASYNC_STATE_SIZE) ? 1 : -1];

ByteFileReader *
BF_Open(const char *path,
        int         sharing_flags);
void
BF_Close(ByteFileReader *reader);
int
BF_Read(ByteFileReader *reader,
        void           *destination,
        int             byte_count);
int
BF_Seek(ByteFileReader    *reader,
        int                offset,
        ByteFileSeekOrigin origin);
int
BF_SeekHead(ByteFileReader    *reader,
            int                offset,
            ByteFileSeekOrigin origin);
ByteFileReader *
BF_Share(ByteFileReader *source_reader);
void
BF_ShareClose(ByteFileReader *reader);
ByteFileAsyncReadState *
BF_StartAsyncRead(ByteFileReader         *reader,
                  void                   *destination,
                  int                     byte_count,
                  int                     position,
                  ByteFileAsyncReadState *read_state);
int
BF_FinishAsyncRead(ByteFileAsyncReadState *read_state);

int
BF_InitReader(void             *sector_buffer,
              RwMemoryAllocator allocate_hook,
              RwMemoryReleaser  release_hook);
void
BF_SetHooks(RwMemoryAllocator allocate_hook,
            RwMemoryReleaser  release_hook);
void
BF_GetHooks(int *out_allocate,
            int *out_release);
void
release_block_file_sector_buffer(void);
int
BF_ReadBytes(ByteFileReader *reader,
             void           *destination,
             int             byte_count,
             int             position);
ByteFileReader *
SF_Open(const char     *path,
        ByteFileReader *reader,
        Item            reply_port_item);
Item
SF_CreateIOReqItem(Item file_item,
                   Item reply_port_item);
ByteFileReader *
SF_Share(ByteFileReader *source_reader,
         ByteFileReader *reader,
         Item            reply_port_item);
int
SF_Close(ByteFileReader *reader);
int
SF_Read(ByteFileReader *reader,
        void           *destination,
        int             sector_count,
        int             sector_number);
Err
SF_AsyncRead(ByteFileReader *reader,
             void           *destination,
             int             sector_count,
             int             sector_number);

#endif
