#ifndef MIDI_PLAYER_RUNTIME_H
#define MIDI_PLAYER_RUNTIME_H

#define MIDI_ERROR_BAD_POINTER              (~0x14)
#define MIDI_ERROR_NO_MEMORY                (~0x1A)
#define MIDI_ERROR_BAD_SCORE_IMAGE          (~0x1B)
#define MIDI_ERROR_ILLEGAL_MESSAGE_STATUS   (-0x1A)

#define MIDI_STREAM_ALLOCATION_SIZE         0x20
#define MIDI_TRACK_STATE_SIZE               0x54
#define MIDI_ACTIVE_NOTES_PER_TRACK         8
#define MIDI_MESSAGE_BUFFER_SIZE             4
#define MIDI_ONE_DATA_BYTE                   1
#define MIDI_TWO_DATA_BYTES                  2
#define MIDI_VARLEN_SHIFT                    7
#define MIDI_MESSAGE_STATUS_SHIFT            24
#define MIDI_MESSAGE_DATA_SHIFT              16
#define MIDI_DECODE_RESULT_WORDS             2

#define IFF_CHUNK_HEADER_SIZE               8
#define IFF_FORM_HEADER_SIZE                0x0C
#define IFF_FORM_TYPE_SIZE                  4
#define IFF_CHUNK_ALIGNMENT_MASK             1
#define IFF_CHUNK_HEADER_WORDS               2
#define IFF_FORM_HEADER_WORDS                3
#define IFF_FORM_SIZE_INDEX                  1
#define IFF_FORM_TYPE_INDEX                  2
#define EAMS_GLOBAL_INFO_WORDS               2
#define EAMS_TRACK_COUNT_INDEX               1
#define IFF_FORM_CHUNK_ID                   0x464F524D
#define EAMS_GLOBAL_INFO_CHUNK_ID            0x47496E66
#define EAMS_TRACK_FORM_ID                   0x45414D54
#define EAMS_TRACK_DATA_CHUNK_ID             0x54446174
#define EAMS_TRACK_COUNT_MASK                0xFFFF

#define SCORE_ROOT_FORM_ID                   0x53434F52
#define SCORE_CONTAINER_FORM_ID              0x53434F4E
#define SCORE_SAMPLE_MAP_FORM_ID             0x4D534D50
#define SCORE_MIXER_FORM_ID                  0x534D7872
#define SCORE_PROGRAM_FORM_ID                0x53507267

#define MIDI_STATUS_PRESENT                  0x80
#define MIDI_DATA_MASK                       0x7F
#define MIDI_COMMAND_MASK                    0xF0
#define MIDI_CHANNEL_MASK                    0x0F
#define MIDI_NOTE_OFF                        0x80
#define MIDI_NOTE_ON                         0x90
#define MIDI_POLYPHONIC_AFTERTOUCH            0xA0
#define MIDI_CONTROL_CHANGE                  0xB0
#define MIDI_PROGRAM_CHANGE                  0xC0
#define MIDI_CHANNEL_PRESSURE                 0xD0
#define MIDI_PITCH_BEND                      0xE0
#define MIDI_META_EVENT                      0xFF
#define MIDI_ALL_SOUND_OFF_CONTROLLER        0x78
#define MIDI_NO_CONTROLLER                   0xFF
#define MIDI_NO_EVENT_TIME                   0xFFFFFFFF
#define MIDI_REPEAT_FOREVER                  0x7FFFFFFF
#define MIDI_STREAM_STATUS_FINISHED          1
#define MIDI_PLAYER_UNEXPECTED_COMMAND_ERROR (-0x28)
#define MIDI_PLAYER_UNEXPECTED_RESUME_ERROR  (-0x27)

#define MP_TAG_SCORE_IMAGE                   1
#define MP_TAG_SCORE_FILE                    2
#define MP_TAG_THREAD_PRIORITY               3
#define MP_TAG_MIXER_AMPLITUDE               4
#define MIDI_PLAYER_DEFAULT_THREAD_PRIORITY  0xBD
#define MIDI_PLAYER_DEFAULT_MIXER_AMPLITUDE  (-1)
#define MIDI_PLAYER_FILE_HANDLE_INDEX        0
#define MIDI_PLAYER_FILE_OFFSET_INDEX        1
#define MIDI_PLAYER_FILE_LENGTH_INDEX        2
#define MIDI_PLAYER_PRIORITY_PADDING_LAST    2

#define MIDI_PLAYER_STATE_STOPPED            2
#define MIDI_PLAYER_STATE_PLAYING            7
#define MIDI_PLAYER_STATE_PAUSED             8
#define MIDI_PLAYER_STATE_TERMINATED         1
#define MIDI_PLAYER_COMMAND_START            1
#define MIDI_PLAYER_COMMAND_PAUSE            2
#define MIDI_PLAYER_COMMAND_RESUME           3
#define MIDI_PLAYER_COMMAND_RESET            4
#define MIDI_PLAYER_COMMAND_SHUTDOWN         5

#define MIDI_PLAYER_TIMER_ITEM_NODE          0x405
#define MIDI_PLAYER_AUDIO_CLOCK_RATE         0x00F00000
#define MIDI_PLAYER_ALLOCATION_SIZE          0x44
#define MIDI_PLAYER_RUNTIME_ALLOCATION_SIZE  0x2C
#define MIDI_PLAYER_THREAD_STACK_SIZE        0x5000
#define MIDI_PLAYER_WORK_PATH_CAPACITY       0x100
#define MIDI_SCORE_PROGRAM_COUNT             0x10
#define MIDI_SCORE_PROGRAM_COUNT_MASK        0xFF
#define MIDI_ERROR_SCORE_FILE_OPEN           (-0x21)
#define MIDI_ERROR_MISSING_SCORE_SOURCE      (-0x15)

struct ByteFileReader;
struct ScoreContext;

typedef struct IffChunkHeader
{
  unsigned int chunk_id;
  int chunk_size;
} IffChunkHeader;

typedef struct IffFormHeader
{
  unsigned int chunk_id;
  int chunk_size;
  unsigned int form_type;
} IffFormHeader;

typedef struct MidiActiveNoteState
{
  unsigned char note_off_status;
  unsigned char note_number;
  unsigned char reserved[2];
  unsigned int release_time;
} MidiActiveNoteState;

typedef struct MusicTagValue
{
  int tag;
  int value;
} MusicTagValue;

typedef struct MidiTrackState
{
  unsigned char *data;
  unsigned char *cursor;
  unsigned int next_event_time;
  unsigned char active;
  unsigned char running_status;
  unsigned char reserved[2];
  MidiActiveNoteState active_notes[MIDI_ACTIVE_NOTES_PER_TRACK];
  int active_note_count;
} MidiTrackState;

typedef struct MidiStream
{
  unsigned char *image;
  void *score_context;
  unsigned int start_time;
  int image_size;
  int reserved_10;
  int track_count;
  MidiTrackState *tracks;
  int remaining_repeats;
} MidiStream;

typedef struct MidiPlayerRuntime
{
  int creator_port;
  int reply_port;
  int control_port;
  int control_reply_item;
  int ready_signal;
  unsigned char stop_requested;
  unsigned char stop_padding[3];
  volatile int state;
  int thread_item;
  unsigned char thread_priority;
  unsigned char priority_padding[3];
  void *stack_memory;
  int stack_size;
} MidiPlayerRuntime;

typedef struct MidiPlayer
{
  MidiPlayerRuntime *runtime;
  void *score_context;
  MidiStream *stream;
  MidiStream *pending_stream;
  int callback;
  int callback_context;
  int reserved_18;
  int reserved_1c;
  int audio_clock_item;
  int saved_audio_rate;
  int reserved_28;
  unsigned char score_loaded;
  unsigned char score_loaded_padding[3];
  int repeat_count;
  void *score_resource_list;
  int mixer_amplitude;
  int mixer_voice_count;
  int reserved_40;
} MidiPlayer;

typedef char MidiPlayerSizeCheck[
    (sizeof(MidiPlayer) == MIDI_PLAYER_ALLOCATION_SIZE) ? 1 : -1];

typedef struct MidiSampleAllocation
{
  int address;
  int byte_count;
} MidiSampleAllocation;

typedef MidiStream *(*MidiPlayerCallback)(int         status,
                                          MidiStream *stream,
                                          int         callback_context);

typedef struct MidiPlayerConfig
{
  int *score_image;
  struct ByteFileReader *file_handle;
  int file_offset;
  int file_length;
  unsigned char thread_priority;
  unsigned char priority_padding[3];
  MidiPlayerRuntime *runtime_state;
  int mixer_amplitude;
} MidiPlayerConfig;

MidiStream *
MP_CreateMIDIStream(unsigned char *image,
                    int            image_size);
MidiStream *
OMP_CreateMIDIStream(unsigned char *image,
                     int            image_size);
int
OMP_DeleteMIDIStream(MidiStream *stream);
int
OMP_ParseMIDIStreamImage(MidiStream *stream,
                         int         image_size);
char *
OMP_NextIFFChunk(char *chunk);
int
MP_DeleteMIDIStream(MidiStream *stream);
MidiPlayer *
MP_CreateMIDIPlayer(MusicTagValue *tags);
MidiPlayer *
OMP_CreateMIDIPlayer(MidiPlayerConfig *config);
int
OMP_DeleteMIDIPlayer(MidiPlayerRuntime *runtime);
int
MP_DeleteMIDIPlayer(MidiPlayer *player);
int
MP_SetMIDIStream(MidiPlayer *player,
                 MidiStream *stream,
                 int         repeat_count);
int
MP_StartMIDIPlayer(MidiPlayer *player,
                   int         repeat_count);
int
MP_StopMIDIPlayer(MidiPlayer *player);
int
MP_PauseMIDIPlayer(MidiPlayer *player);
int
MP_ResumeMIDIPlayer(MidiPlayer *player);
int
MP_GetMIDIPlayerState(MidiPlayer *player);

char *
copy_audio_working_directory(char *path);
MidiPlayer *
create_midi_player(int program_count);
void
delete_midi_player(MidiPlayer *player);
int
load_midi_player_score_file(MidiPlayer *player,
                            const char *path);
int
load_midi_player_score_stream(MidiPlayer            *player,
                              struct ByteFileReader *reader,
                              int                    file_offset,
                              int                    file_length);
int
unload_midi_player_score(MidiPlayer *player);
int
unload_positive_score_instrument_templates(struct ScoreContext *score_context);
int
claim_midi_player_audio_clock(MidiPlayer *player);
int
release_midi_player_audio_clock(MidiPlayer *player);
int
free_midi_player_channel_instruments(MidiPlayer *player);
int
wait_for_midi_player_resume(MidiPlayer *player,
                            int        *time_offset);
int
process_midi_player_control_message(MidiPlayer *player,
                                    int        *time_offset);
int
run_midi_player_stream(MidiPlayer *player);
void
midi_player_thread_main(MidiPlayerConfig *config,
                        MidiPlayer      **out_player);

int
OMS_ResetMIDIStream(MidiStream  *stream,
                    unsigned int start_time);
int
OMS_BumpMIDIStream(MidiStream   *stream,
                   unsigned int  current_time,
                   unsigned int *out_next_event_time,
                   int          *out_time_offset);
int
OMS_FlushActiveNotes(MidiStream *stream);

#endif
