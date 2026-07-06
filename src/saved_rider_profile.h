#ifndef SAVED_RIDER_PROFILE_H
#define SAVED_RIDER_PROFILE_H

#define SAVED_PROFILE_PATH "/NVRAM/EA_RRX"
#define SAVED_PROFILE_SIGNATURE 0x01234567

#define SAVED_PROFILE_PATH_BUFFER_LENGTH 32
#define SAVED_PROFILE_RECORD_SIZE 0xB4
#define SAVED_PROFILE_PATH_SLOT_OFFSET 12
#define SAVED_PROFILE_PROGRESSION_ROW_COUNT 10
#define SAVED_PROFILE_PROGRESSION_VALUE_COUNT 3
#define SAVED_PROFILE_RIDER_TYPE_COUNT 10
#define SAVED_PROFILE_PROGRESSION_LEVEL_COUNT 5
#define SAVED_PROFILE_VARIANT_COUNT 5
#define SAVED_PROFILE_BIKE_STYLE_COUNT 15
#define SAVED_PROFILE_COMPLETED_COURSE_MASK 0x1F

typedef struct SavedRiderProgressionRow
{
  unsigned char status;
  unsigned char alignment[3];
  int values[SAVED_PROFILE_PROGRESSION_VALUE_COUNT];
} SavedRiderProgressionRow;

typedef struct SavedRiderProfile
{
  int signature;
  unsigned char music_enabled;
  unsigned char engine_audio_enabled;
  unsigned char option_alignment[2];
  signed char rider_type;
  signed char level;
  signed char current_variant;
  unsigned char completed_courses;
  signed char rider_style;
  unsigned char profile_alignment[3];
  int balance;
  SavedRiderProgressionRow
    progression[SAVED_PROFILE_PROGRESSION_ROW_COUNT];
} SavedRiderProfile;

typedef char SavedRiderProfileSizeCheck[
    sizeof(SavedRiderProfile) == SAVED_PROFILE_RECORD_SIZE ? 1 : -1];

int
saved_rider_profile_is_valid(const SavedRiderProfile *profile,
                             int                      byte_count);

#endif
