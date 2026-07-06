#include "debug.h"
#include "limits.h"
#include "stdio.h"
/* Reconstructed from the original function at 0x000415C8. */

#include "byte_file_runtime.h"
#include "dll_runtime.h"
#include "midi_player_runtime.h"
#include "score_context_api.h"
#include "audio.h"
#include "rw_semantic_data.h"

void
get_music_memory_vectors(RwMemoryAllocator *out_allocator,
                         RwMemoryReleaser  *out_releaser);
int
load_score_program_form(MidiPlayer     *player,
                        ByteFileReader *reader,
                        IffFormHeader  *form_header,
                        DLLList        *program_record_list);
int
load_score_sample_map_form(MidiPlayer     *player,
                           ByteFileReader *reader,
                           IffFormHeader  *form_header,
                           DLLList        *sample_item_list);
int
load_score_mixer_form(MidiPlayer     *player,
                      ByteFileReader *reader,
                      IffFormHeader  *form_header,
                      int            *out_mixer_template_item);
int
initialize_midi_player_program_map(MidiPlayer *player,
                                   DLLList    *program_record_list,
                                   DLLList    *sample_item_list,
                                   int         mixer_template_item);
int
cleanup_score_program_records(MidiPlayer *player,
                              DLLList    *program_record_list,
                              int         resources_transferred);
int
cleanup_score_sample_items(MidiPlayer *player,
                           DLLList    *sample_item_list,
                           int         resources_transferred);
int
DLL_GetErr(void);
DLLList *
DLL_New(RwMemoryAllocator allocator,
        RwMemoryReleaser  releaser);
int
DLL_Delete(DLLList *list);
int
IFF_FindForm(ByteFileReader *reader,
             unsigned int    form_type,
             int             search_offset,
             IffFormHeader  *out_form_header);
int
IFF_PeekChunkHeader(ByteFileReader *reader,
                    IffChunkHeader *out_chunk_header);
int
IFF_PeekFormHeader(ByteFileReader *reader,
                   IffFormHeader  *out_form_header);
int
IFF_SkipChunk(ByteFileReader       *reader,
              const IffChunkHeader *chunk_header);
int
IFF_SkipForm(ByteFileReader      *reader,
             const IffFormHeader *form_header);
int
IFF_EnterForm(ByteFileReader *reader);
int
BF_Seek(ByteFileReader    *reader,
        int                offset,
        ByteFileSeekOrigin origin);

int
load_midi_player_score_stream(MidiPlayer     *player,
                              ByteFileReader *reader,
                              int             file_offset,
                              int             file_length)
{
  RwMemoryAllocator allocator;
  RwMemoryReleaser releaser;
  IffChunkHeader chunk_header;
  IffFormHeader form_header;
  int resources_transferred;
  int form_scan_limit;
  int bytes_scanned;
  int chunk_bytes;
  int root_form_offset;
  DLLList *program_record_list;
  DLLList *sample_item_list;
  int mixer_template_item;
  int status;
  ScoreContext *score_context;
  int program_index;

  status = 0;
  program_record_list = 0;
  sample_item_list = 0;
  bytes_scanned = 0;
  mixer_template_item = -1;
  form_scan_limit = -1;
  resources_transferred = 0;
  releaser = 0;
  allocator = 0;

  if(player == 0)
    {
      score_loader_last_error = MIDI_ERROR_BAD_POINTER;
      return MIDI_ERROR_BAD_POINTER;
    }

  get_music_memory_vectors(&allocator, &releaser);

  if(reader != 0 && file_length == 0 && file_offset >= 0 &&
     file_offset <= (int)reader->file_status.fs_ByteCount)
    {
      file_length = (int)reader->file_status.fs_ByteCount - file_offset;
    }

  if(player->score_context == 0 || reader == 0 ||
     file_offset < 0 || file_length < IFF_FORM_HEADER_SIZE ||
     file_offset > (int)reader->file_status.fs_ByteCount ||
     file_length > (int)reader->file_status.fs_ByteCount - file_offset ||
     file_offset > INT_MAX - file_length)
    {
      status = MIDI_ERROR_BAD_POINTER;
      score_loader_last_error = status;
      return status;
    }
  if(player->score_loaded != 0)
    {
      unload_midi_player_score(player);
    }

  program_record_list = DLL_New(allocator, releaser);
  if(program_record_list == 0)
    {
      status = DLL_GetErr();
      if(status == 0)
        {
          status = MIDI_ERROR_NO_MEMORY;
        }
      goto failure;
    }

  sample_item_list = DLL_New(allocator, releaser);
  if(sample_item_list == 0)
    {
      status = DLL_GetErr();
      if(status == 0)
        {
          status = MIDI_ERROR_NO_MEMORY;
        }
      kprintf("DLL_New failed in OMP_Load_Via_SCON_Stream(), error %ld\n\n",
              status);
      fflush(stdout);
      goto failure;
    }

  status = BF_Seek(reader, file_offset, BYTE_FILE_SEEK_SET);
  if(status < 0)
    {
      goto failure;
    }

  status = IFF_FindForm(reader, SCORE_ROOT_FORM_ID,
                        mixer_template_item, &form_header);
  if(status < 0)
    {
      status = BF_Seek(reader, file_offset, BYTE_FILE_SEEK_SET);
      if(status < 0)
        {
          goto failure;
        }
      status = IFF_FindForm(reader, SCORE_CONTAINER_FORM_ID,
                            mixer_template_item, &form_header);
      if(status < 0)
        {
          goto failure;
        }
    }

  root_form_offset = reader->position;
  if(root_form_offset < file_offset ||
     root_form_offset > file_offset + file_length - IFF_FORM_HEADER_SIZE ||
     form_header.chunk_size < IFF_FORM_TYPE_SIZE ||
     form_header.chunk_size == INT_MAX ||
     form_header.chunk_size >
       file_offset + file_length - root_form_offset - IFF_CHUNK_HEADER_SIZE)
    {
      status = MIDI_ERROR_BAD_SCORE_IMAGE;
      goto failure;
    }

  status = IFF_EnterForm(reader);
  if(status < 0)
    {
      goto failure;
    }

  form_scan_limit =
    (form_header.chunk_size +
     (form_header.chunk_size & IFF_CHUNK_ALIGNMENT_MASK)) -
    IFF_FORM_TYPE_SIZE;
  if(form_scan_limit < 0 ||
     form_scan_limit > file_offset + file_length - reader->position)
    {
      status = MIDI_ERROR_BAD_SCORE_IMAGE;
      goto failure;
    }
  if(form_scan_limit > 0)
    {
      do
        {
          status = IFF_PeekChunkHeader(reader, &chunk_header);
          if(status != 0)
            {
              goto failure;
            }

          if(chunk_header.chunk_size < 0 ||
             chunk_header.chunk_size == INT_MAX)
            {
              status = MIDI_ERROR_BAD_SCORE_IMAGE;
              goto failure;
            }
          chunk_bytes = chunk_header.chunk_size +
                        (chunk_header.chunk_size & IFF_CHUNK_ALIGNMENT_MASK) +
                        IFF_CHUNK_HEADER_SIZE;
          if(chunk_bytes > form_scan_limit - bytes_scanned)
            {
              status = MIDI_ERROR_BAD_SCORE_IMAGE;
              goto failure;
            }
          if(chunk_header.chunk_id == IFF_FORM_CHUNK_ID)
            {
              status = IFF_PeekFormHeader(reader, &form_header);
              if(status != 0)
                {
                  goto failure;
                }

              if(form_header.form_type == SCORE_SAMPLE_MAP_FORM_ID)
                {
                  status = load_score_sample_map_form(player, reader, &form_header,
                                                      sample_item_list);
                  if(status != 0)
                    {
                      goto failure;
                    }
                }
              else if(form_header.form_type == SCORE_MIXER_FORM_ID)
                {
                  status = load_score_mixer_form(player, reader, &form_header,
                                                 &mixer_template_item);
                  if(status != 0)
                    {
                      goto failure;
                    }
                }
              else if(form_header.form_type ==
                      SCORE_PROGRAM_FORM_ID)
                {
                  status = load_score_program_form(player, reader, &form_header,
                                                   program_record_list);
                  if(status != 0)
                    {
                      goto failure;
                    }
                }
              else
                {
                  status = IFF_SkipForm(reader, &form_header);
                  if(status < 0)
                    {
                      goto failure;
                    }
                }
            }
          else
            {
              status = IFF_SkipChunk(reader, &chunk_header);
              if(status < 0)
                {
                  goto failure;
                }
            }
          bytes_scanned += chunk_bytes;
        } while(bytes_scanned < form_scan_limit);
    }

  if(bytes_scanned != form_scan_limit)
    {
      status = MIDI_ERROR_BAD_SCORE_IMAGE;
      goto failure;
    }

  status = initialize_midi_player_program_map(
    player, program_record_list, sample_item_list, mixer_template_item);
  if(status == 0)
    {
      resources_transferred = 1;
      goto finish;
    }

failure:
  if(status != 0)
    {
      /* Until the map is complete, the temporary lists still own every
         template and sample. Remove borrowed PI-map references first. */
      score_context = (ScoreContext *)player->score_context;
      if(score_context != 0)
        {
          for(program_index = 0;
              program_index < score_context->scon_PIMapSize;
              program_index++)
            {
              score_context->scon_PIMap[program_index].pimp_InsTemplate = 0;
            }
          if(score_context->scon_MixerIns >= 0)
            {
              if(TermScoreMixer(score_context) >= 0)
                {
                  mixer_template_item = -1;
                }
            }
        }
      score_loader_last_error = status;
      player->score_loaded = 0;
    }

finish:
  if(status == 0)
    {
      score_loader_last_error = 0;
      player->score_loaded = 1;
    }
  if(sample_item_list != 0)
    {
      cleanup_score_sample_items(player, sample_item_list, resources_transferred);
      DLL_Delete(sample_item_list);
    }
  if(program_record_list != 0)
    {
      cleanup_score_program_records(player, program_record_list, resources_transferred);
      DLL_Delete(program_record_list);
    }
  if(resources_transferred == 0 && mixer_template_item >= 0)
    {
      UnloadInsTemplate(mixer_template_item);
    }

  if(status != 0)
    {
      unload_midi_player_score(player);
    }
  return status;
}
