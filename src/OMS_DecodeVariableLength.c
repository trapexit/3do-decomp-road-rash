#include "midi_player_runtime.h"

int
OMS_DecodeVariableLength(unsigned char *cursor,
                         int           *out_bytes_consumed)
{
  int byte_count;
  int value;
  int next_byte;

  value = *cursor++;
  byte_count = 1;
  if(value & MIDI_STATUS_PRESENT)
    {
      value &= MIDI_DATA_MASK;
      do
        {
          next_byte = *cursor++;
          byte_count++;
          value = ((value << MIDI_VARLEN_SHIFT) |
                   (next_byte & MIDI_DATA_MASK));
        } while(next_byte & MIDI_STATUS_PRESENT);
    }

  if(out_bytes_consumed != 0)
    {
      *out_bytes_consumed = byte_count;
    }
  return value;
}
