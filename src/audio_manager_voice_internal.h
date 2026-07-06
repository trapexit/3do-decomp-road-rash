#ifndef ROADRASH_AUDIO_MANAGER_VOICE_INTERNAL_H
#define ROADRASH_AUDIO_MANAGER_VOICE_INTERNAL_H

#include "audio_manager_voice.h"
#include "audio_manager_voice_group.h"

enum AudioManagerVoiceInternalConstant
{
  AUDIO_MANAGER_BYTE_MASK = 0xFF,
  AUDIO_MANAGER_SORT_KEY_OPTION_MASK = 0x100,
  AUDIO_MANAGER_NO_AUTOMATIC_VOICE_RESULT = 1,
  AUDIO_MANAGER_STOP_INSTRUMENT_AT_SAMPLE_END = 1,
  AUDIO_MANAGER_OBJECT_VOICE_CAPACITY = 16,
  AUDIO_MANAGER_VOICE_KEY_SHIFT = 16,
  AUDIO_MANAGER_VOICE_INDEX_MASK = 0xFFFF,
  AUDIO_MANAGER_BAD_OBJECT_ERROR = -21,
  AUDIO_MANAGER_NO_MEMORY_ERROR = -27,
  AUDIO_MANAGER_VOICE_SAMPLE_ALLOCATION_ERROR = -27,
  AUDIO_MANAGER_VOICE_INSTRUMENT_PRIORITY = 0x64,
  AUDIO_MANAGER_VOICE_DEFAULT_PAN = 0x40,
  AUDIO_MANAGER_VOICE_STANDARD_PLAYBACK_RATE = 0x8000,
  AUDIO_MANAGER_VOICE_MAXIMUM_PLAYBACK_RATE = 0xFFFF,
  AUDIO_MANAGER_VOICE_MAXIMUM_AMPLITUDE = 0x7FFF,
  AUDIO_MANAGER_START_TAG_TERMINATOR_INDEX = 2,
  AUDIO_MANAGER_VOICE_SAMPLE_SIZE = 0x14,
  AUDIO_MANAGER_VOICE_FIRST_SAMPLE_OFFSET = 0x30,
  AUDIO_MANAGER_VOICE_SAMPLE_CHAIN_FLAGS_OFFSET = 0x44,
  AUDIO_MANAGER_VOICE_SIZE = 0x48,
  AUDIO_MANAGER_VOICE_FREQUENCY_CACHE_VALID = 1,
  AUDIO_MANAGER_VOICE_AMPLITUDE_CACHE_VALID = 2,
  AUDIO_MANAGER_VOICE_SAMPLE_STOP_AT_END_FLAG = 0x02000000,
  AUDIO_MANAGER_VOICE_SAMPLE_WRAP_FLAG = 0x04000000,
  AUDIO_MANAGER_VOICE_SAMPLE_LINK_FLAG = 0x08000000
};

enum AudioManagerTemplateSourceKind
{
  AUDIO_MANAGER_TEMPLATE_SOURCE_FILE = 1,
  AUDIO_MANAGER_TEMPLATE_SOURCE_ITEM = 2,
  AUDIO_MANAGER_TEMPLATE_SOURCE_IMAGE = 3
};

enum AudioManagerLoadedObjectError
{
  AUDIO_MANAGER_INVALID_VOICE_COUNT_ERROR = -34,
  AUDIO_MANAGER_VOICE_UNAVAILABLE_ERROR = -29
};

typedef struct AudioManagerVoiceSample
{
  struct AudioManagerVoiceSample *next;
  int sample_item;
  int attachment_item;
  int cue_item;
  unsigned int attachment_flags;
} AudioManagerVoiceSample;

struct AudioManagerVoice
{
  int instrument_item;
  int instrument_template_item;
  AudioManagerVoiceGroup *group;
  unsigned char instrument_lock_state;
  unsigned char is_owned;
  // Logical amplitude/frequency also track failed writes; only valid bits certify DSP state.
  unsigned char knob_cache_valid;
  unsigned char reserved_alignment[1];
  int group_sort_key;
  int amplitude;
  int frequency;
  int pan;
  int frequency_knob_item;
  int amplitude_knob_item;
  int primary_output_connection_key;
  int secondary_output_connection_key;
  AudioManagerVoiceSample first_sample;
  unsigned int sample_chain_flags;
};

typedef char AudioManagerVoiceSampleSizeCheck[
    (sizeof(AudioManagerVoiceSample) ==
     AUDIO_MANAGER_VOICE_SAMPLE_SIZE) ? 1 : -1];
typedef char AudioManagerVoiceSizeCheck[
    (sizeof(AudioManagerVoice) == AUDIO_MANAGER_VOICE_SIZE) ? 1 : -1];

typedef AudioManagerVoice AudioManagerVoicePrefix;

typedef struct AudioManagerLoadedObject
{
  int instrument_template_item;
  unsigned char lookup_flags;
  unsigned char decoder_retained;
  unsigned char alignment_padding[2];
  char *instrument_name;
  int object_key;
  int voice_count;
  int voice_search_start;
  AudioManagerVoice *voices[AUDIO_MANAGER_OBJECT_VOICE_CAPACITY];
} AudioManagerLoadedObject;

typedef struct AudioManagerInstrumentTemplateImage
{
  const void *image_address;
  int image_length;
  const char *instrument_name;
} AudioManagerInstrumentTemplateImage;

typedef char AudioManagerLoadedObjectSizeCheck[
    (sizeof(AudioManagerLoadedObject) == 0x58) ? 1 : -1];

int
is_audio_manager_adpcm_instrument(const char *instrument_name);
int
retain_audio_manager_adpcm_decoder(AudioManagerLoadedObject *audio_object);
int
release_audio_manager_adpcm_decoder(AudioManagerLoadedObject *audio_object);
AudioManagerVoice *
create_audio_manager_voice_record(int template_source_kind,
                                  int instrument_template_item);
char *
strip_audio_manager_instrument_path(char *instrument_path);
AudioManagerLoadedObject *
create_audio_manager_loaded_object(int         template_source_kind,
                                   const void *template_source,
                                   const char *instrument_name,
                                   int         object_key,
                                   int         voice_count);
int
destroy_audio_manager_loaded_object(AudioManagerLoadedObject *audio_object);
int
stop_and_clear_audio_manager_loaded_object_voices(AudioManagerLoadedObject *audio_object);
int
stop_and_clear_all_audio_manager_loaded_object_voices(void);
AudioManagerLoadedObject *
load_audio_manager_object_from_file(const char *instrument_path,
                                    int         voice_count,
                                    int         object_key);
AudioManagerLoadedObject *
create_audio_manager_object_from_template_image(
  const AudioManagerInstrumentTemplateImage *template_image,
  int
  voice_count,
  int
  object_key);
void
destroy_all_audio_manager_loaded_objects(void);
void
decode_audio_manager_voice_id(int  voice_id,
                              int *object_key,
                              int *voice_index);
AudioManagerLoadedObject *
find_audio_manager_object_by_key(int object_key);
AudioManagerLoadedObject *
get_audio_manager_object_by_index(int object_index);
AudioManagerVoice *
resolve_audio_manager_voice_details(int                        voice_id,
                                    AudioManagerLoadedObject **audio_object,
                                    int                       *object_key,
                                    int                       *voice_index);
AudioManagerLoadedObject *
find_audio_manager_object_for_sample(int sample_item);
int
find_compatible_audio_manager_object_key_for_sample(int sample_item);
char *
select_audio_manager_sample_player_name(int   sample_item,
                                        int   variable_pitch,
                                        char *instrument_name);
int
audio_manager_strings_equal_ignore_case(const char *left,
                                        const char *right);
AudioManagerVoice *
claim_audio_manager_object_voice(AudioManagerLoadedObject *audio_object,
                                 int                      *voice_id);
int
claim_audio_manager_voice_by_object_key(int object_key);
int
stop_audio_manager_voice_record(volatile AudioManagerVoice *voice);
int
replace_audio_manager_voice_samples(AudioManagerVoice *voice,
                                    int               *sample_items,
                                    int                sample_count,
                                    int                sample_mode);
int
enable_audio_manager_voice_sample_linking(AudioManagerVoice *voice,
                                          int                sample_chain_option);
int
append_audio_manager_voice_sample(AudioManagerVoice *voice,
                                  int                sample_item,
                                  int                stop_instrument_at_end);
int
append_audio_manager_voice_sample_by_id(int voice_id,
                                        int sample_item,
                                        int sample_mode);
void
clear_audio_manager_voice_samples(AudioManagerVoice *voice);
int
initialize_audio_manager_voice_instrument(AudioManagerVoice *voice);
void
destroy_audio_manager_voice_instrument(AudioManagerVoice *voice);
int
free_audio_manager_voice_record(AudioManagerVoice *voice);
int
grab_audio_manager_voice_knobs(AudioManagerVoice *voice);
void
release_audio_manager_voice_knobs(AudioManagerVoice *voice);
int
connect_audio_manager_voice_to_global_mixer(AudioManagerVoice *voice);
int
disconnect_audio_manager_voice_from_global_mixer(AudioManagerVoice *voice);
int
set_audio_manager_voice_record_pan(AudioManagerVoice *voice,
                                   int                pan);
int
set_audio_manager_voice_record_stereo_spread(AudioManagerVoice *voice,
                                             int                spread);
int
attach_all_audio_manager_voice_samples(AudioManagerVoice *voice);
int
attach_audio_manager_voice_sample(AudioManagerVoice       *voice,
                                  AudioManagerVoiceSample *sample);
void
detach_audio_manager_voice_sample_attachments(AudioManagerVoice *voice);
int
link_audio_manager_voice_sample_attachments(AudioManagerVoice *voice);
AudioManagerVoiceSample *
get_audio_voice_sample_by_index(AudioManagerVoice *voice,
                                int                sample_index);
int
set_audio_manager_voice_record_group_sort_key(AudioManagerVoicePrefix *voice,
                                              int                      sort_key);
int
get_audio_manager_voice_record_group_sort_key(AudioManagerVoicePrefix *voice,
                                              int                     *sort_key);
int
start_audio_manager_voice_record(AudioManagerVoice *voice,
                                 int                playback_rate,
                                 int                amplitude);
int
release_audio_manager_voice_record(AudioManagerVoice *voice);
int
advance_audio_manager_object_voice_search(AudioManagerLoadedObject *audio_object);
int
get_audio_manager_object_voice_search_start(AudioManagerLoadedObject *audio_object);
int
is_audio_manager_object_voice_available_for_sort_key(AudioManagerLoadedObject *audio_object,
                                                     int                       voice_index,
                                                     int                       requested_sort_key);
AudioManagerVoice *
select_audio_manager_object_voice(AudioManagerLoadedObject *audio_object,
                                  int                       requested_sort_key,
                                  int                      *voice_index_out);

#endif
