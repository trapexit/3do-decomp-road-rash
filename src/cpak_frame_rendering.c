#include "stdio.h"
#include "string.h"

#include "codec.h"

#include "application_runtime.h"
#include "cpak_subscriber_runtime.h"
#include "data_stream_runtime.h"
#include "rw_semantic_data.h"

enum CPakFrameRenderingConstants
{
  CPAK_CHANNEL_HEADER_READY = 0x10000,
  CPAK_SCREEN_WIDTH_PIXELS = 320,
  CPAK_SCREEN_HEIGHT_PIXELS = 240,
  CPAK_HORIZONTAL_PIXEL_STRIDE = 4,
  CPAK_FRAME_BUFFER_ROW_BYTES = 640,
  CPAK_BITMAP_BYTES_PER_PIXEL = 2,
  CPAK_SIGN_BIT_SHIFT = 31,
  CPAK_DIVIDE_BY_TWO_SHIFT = 1
};

typedef struct RoadRashCPakRecord
{
  CPakRec standard;
  int32 remaining_frames;
} RoadRashCPakRecord;

void
calculate_cpak_destination(long   *base_address,
                           long   *row_bytes,
                           long    horizontal_position,
                           long    vertical_position,
                           long    width,
                           long    height,
                           Bitmap *bitmap);

int32
is_time_for_next_cpak_frame(CPakRecPtr record)
{
  CinePakFramePtr current_frame;
  uint32 current_stream_time;
  boolean should_decode;

  if(DSGetClock(record->streamCBPtr, &current_stream_time) < 0)
    {
      return false;
    }

  should_decode = true;
  if((uint32)record->lastCurTime <= current_stream_time)
    {
      if(cpak_subscriber_globals.clock_loop_detected == 0)
        {
          record->lastCurTime = (int32)current_stream_time;
          current_frame = record->curFramePtr;
          if(current_frame == 0)
            {
              return true;
            }
          if((uint32)(current_frame->time + current_frame->duration) >=
             current_stream_time)
            {
              should_decode = false;
            }
          return should_decode;
        }
    }

  record->lastCurTime = (int32)current_stream_time;
  if(cpak_subscriber_globals.clock_loop_detected == 0 &&
     stream_end_of_file_seen != 0)
    {
      cpak_subscriber_globals.clock_loop_detected = true;
    }

  return true;
}


int32
is_cpak_frame_ready_for_display(CPakRecPtr record)
{
  CinePakFramePtr current_frame;
  uint32 current_stream_time;

  if(DSGetClock(record->streamCBPtr, &current_stream_time) < 0)
    {
      return false;
    }

  if((uint32)record->lastCurTime > current_stream_time ||
     cpak_subscriber_globals.clock_loop_detected != 0)
    {
      record->lastCurTime = (int32)current_stream_time;
      if(cpak_subscriber_globals.clock_loop_detected == 0 &&
         stream_end_of_file_seen != 0)
        {
          cpak_subscriber_globals.clock_loop_detected = true;
        }
      return true;
    }

  record->lastCurTime = (int32)current_stream_time;
  current_frame = record->curFramePtr;
  if(current_frame == 0)
    {
      return true;
    }

  return current_stream_time >= (uint32)current_frame->time;
}


int32
draw_cpak_frame_to_buffer(CPakContextPtr context,
                          CPakRecPtr     record,
                          Bitmap        *bitmap,
                          int32          horizontal_position,
                          int32          vertical_position)
{
  RoadRashCPakRecord *runtime_record;
  CPakChannelPtr channel;
  CinePakHeaderPtr header;
  CinePakFramePtr frame;
  SubsChunkDataPtr chunk;
  SubscriberMsgPtr message;
  long destination_address;
  long destination_row_bytes;
  uint32 current_stream_time;
  boolean first_chunk;

  runtime_record = (RoadRashCPakRecord *)record;
  channel = &context->channel[record->channel];
  first_chunk = true;

  while(is_time_for_next_cpak_frame(record) || first_chunk)
    {
      first_chunk = false;
      if(!poll_cpak_chunk(context, record, &chunk, &message))
        {
          break;
        }

      if(chunk->subChunkType == FHDR_CHUNK_TYPE)
        {
          header = (CinePakHeaderPtr)chunk;
          cpak_subscriber_globals.clock_loop_detected = false;
          stream_end_of_file_seen = false;

          if(runtime_record->remaining_frames == 0)
            {
              memcpy(&record->cpHeader, header,
                     sizeof(record->cpHeader));
              channel->status |= CPAK_CHANNEL_HEADER_READY;
              channel->imageData.width = record->cpHeader.width;
              channel->imageData.height = record->cpHeader.height;
              record->curFramePtr = 0;
              runtime_record->remaining_frames = record->cpHeader.count;
            }
          else
            {
              runtime_record->remaining_frames += header->count;
            }

          queue_cpak_chunk_for_reply(context, message);
        }
      else if(chunk->subChunkType == FRME_CHUNK_TYPE)
        {
          runtime_record->remaining_frames--;
          frame = (CinePakFramePtr)chunk;
          record->curFramePtr = frame;

          if(record->curSubMsg != 0)
            {
              queue_cpak_chunk_for_reply(context, record->curSubMsg);
            }

          if((channel->status & CPAK_CHANNEL_HEADER_READY) == 0)
            {
              show_application_error("Movie: decode before Cinepak header",
                                     -1, 0);
            }
          else
            {
              calculate_cpak_destination(
                &destination_address, &destination_row_bytes,
                horizontal_position, vertical_position,
                channel->imageData.width, channel->imageData.height,
                bitmap);
              PreDecompress(context->filmCodec, frame->frameData);
              Decompress(context->filmCodec, frame->frameData,
                         (char *)destination_address,
                         destination_row_bytes);
            }

          record->curSubMsg = message;
        }
      else
        {
          printf("GetCPakCel --- Unknown Chunk Type: '%.4s'",
                 &chunk->subChunkType);
        }
    }

  if(DSGetClock(record->streamCBPtr, &current_stream_time) >= 0)
    {
      frame = record->curFramePtr;
      if(frame != 0 &&
         (uint32)(frame->time + frame->duration) <
         current_stream_time &&
         record->curSubMsg != 0)
        {
          queue_cpak_chunk_for_reply(context, record->curSubMsg);
          record->curSubMsg = 0;
          record->curFramePtr = 0;
        }
    }

  return runtime_record->remaining_frames;
}


void
calculate_cpak_destination(long   *base_address,
                           long   *row_bytes,
                           long    horizontal_position,
                           long    vertical_position,
                           long    width,
                           long    height,
                           Bitmap *bitmap)
{
  long centered_position;

  if(horizontal_position == -1)
    {
      centered_position = CPAK_SCREEN_WIDTH_PIXELS - width;
      centered_position +=
        (long)((uint32)centered_position >> CPAK_SIGN_BIT_SHIFT);
      centered_position >>= CPAK_DIVIDE_BY_TWO_SHIFT;
      horizontal_position =
        centered_position * CPAK_HORIZONTAL_PIXEL_STRIDE;
    }
  else
    {
      horizontal_position *= CPAK_HORIZONTAL_PIXEL_STRIDE;
    }

  if(vertical_position == -1)
    {
      centered_position = CPAK_SCREEN_HEIGHT_PIXELS - height;
      centered_position +=
        (long)((uint32)centered_position >> CPAK_SIGN_BIT_SHIFT);
      centered_position >>= CPAK_DIVIDE_BY_TWO_SHIFT;
      vertical_position = centered_position & ~1;
    }
  else
    {
      vertical_position &= ~1;
    }

  *base_address = (long)bitmap->bm_Buffer + horizontal_position +
                  vertical_position * CPAK_FRAME_BUFFER_ROW_BYTES;
  *row_bytes = bitmap->bm_Width * CPAK_BITMAP_BYTES_PER_PIXEL;
}
