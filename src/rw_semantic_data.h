#ifndef ROADRASH_RW_SEMANTIC_DATA_H
#define ROADRASH_RW_SEMANTIC_DATA_H

#include "graphics.h"
#include "item.h"
#include "operamath.h"

#include "bss_early_data.h"
#include "bss_late_data.h"
#include "runtime_data_types.h"
#include "text_render_runtime.h"

typedef struct AudioFolio AudioFolio;
struct IntrusiveListHeader;
struct IntrusiveListNode;
struct IntrusiveListStorage;
struct MemoryPressureMonitor;
struct RacerEntity;

extern MathFolio *_MathBase;
extern Item GrafFolioNum;
extern GrafFolio *GrafBase;
extern Item AudioFolioItem;
extern AudioFolio *AudioBase;

/*
 * Semantic declarations for the original initialized-data image.
 * Original addresses live in launchme/ledger_fragments, never in source.
 */

#define OPERAMATH_ATAN_RATIO_COUNT 258
#define OPERAMATH_QUARTER_SINE_COUNT 4098
#define LEGACY_RANDOM_STATE_COUNT 54
#define SAUDIO_TEMPLATE_COUNT 10
#define SCORE_MIXER_INPUT_COUNT 11
#define SCORE_MIXER_GAIN_COUNT 12
#define RACE_RIDER_COUNT 16
#define BIKE_SPEC_CACHE_COUNT 15
#define RIDER_PROGRESSION_LEVEL_COUNT 5
#define RIDER_BIKE_STYLE_COUNT BIKE_SPEC_CACHE_COUNT
#define RIDER_NAME_BYTES 15
#define RIDER_TYPE_COUNT_RW 16
#define OPPONENT_NAME_LEVELS 5
#define OPPONENTS_PER_LEVEL 6
#define RIDER_UPDATE_HANDLER_COUNT 11
#define OPPONENT_CHOICE_LEVEL_COUNT 5
#define OPPONENT_CHOICE_RIDER_COUNT 8
#define PROFILE_SLOT_COUNT 8
#define PLAYER_PROFILE_COUNT 2
#define TRACK_EFFECT_RESOURCE_COUNT 3
#define TRACK_EFFECT_TYPE_COUNT 4
#define GAME_SOUND_RESOURCE_COUNT 19
#define RIDER_PHYSICS_SURFACE_COUNT 4
#define RIDER_PHYSICS_LANE_COUNT 8
#define RENDERER_BIT_LENGTH_COUNT 256
#define ROAD_DEPTH_INPUT_COUNT 34
#define CEL_PIXEL_FORMAT_COUNT 7
#define ROAD_DRAW_ORDER_COUNT 3
#define ROAD_DRAW_COMMAND_COUNT 8
#define ANIMATION_CHANNEL_SPEC_COUNT 163
#define RIDER_ONE_ANIMATION_BINDING_COUNT 142
#define RIDER_ONE_ANIMATION_STATE_COUNT 50
#define RIDER_TWO_ANIMATION_BINDING_COUNT 28
#define RIDER_TWO_ANIMATION_STATE_COUNT 51
#define CAR_ANIMATION_COUNT 15
#define CAR_ANIMATION_VARIANT_COUNT 3
#define FAMILY_REQUEST_COUNT 8
#define FAMILY_GROUP_COUNT 4
#define DISPLAY_EASING_COUNT 257
#define TRIG_LOOKUP_LOW_COUNT 128
#define TRIG_LOOKUP_HIGH_COUNT 129
#define COURSE_VARIANT_COUNT 6
#define COURSE_CATALOG_COUNT 20
#define RASH_OPTIONS_CATALOG_COUNT 89
#define RUNTIME_FEATURE_FLAG_COUNT 8
#define RIDER_FRAME_DESCRIPTOR_GROUP_COUNT 7
#define HUD_COORDINATE_COUNT 23
#define PARTICLE_RESOURCE_COUNT 15
#define BACKGROUND_AUDIO_NAME_COUNT 14

typedef struct RaceBoostConfig
{
  int recharge_step;
  int active_speed_scale_8_8;
  int charge_capacity;
  int remaining_charge_units;
  int activation_threshold;
  int activation_cooldown_ticks;
  int charge_8_8;
} RaceBoostConfig;

typedef struct RaceRiderState
{
  int initialization_state;
  int rider_count;
  void *object_pool;
  void *state_pool;
  void *player_object;
  void *challenge_object;
  void *rider_objects[RACE_RIDER_COUNT];
  void *rider_states[RACE_RIDER_COUNT];
  void *bike_spec_cache[BIKE_SPEC_CACHE_COUNT];
  void *bike_specs_resource;
  int status;
  unsigned char active_challenge_index;
  unsigned char challenge_index_alignment[3];
  int pacing_update_deadline;
  int rank_penalties[3];
  int rank_bonuses[3];
  int pacing_percent_bounds[2];
} RaceRiderState;

typedef int (*RacerLifecycleCallback)();
typedef void (*RacerContactCallback)(struct RacerEntity *object);

typedef struct RacerCreationTemplate
{
  unsigned int class_flags;
  int child_capacity;
  int resource_capacity;
  RacerLifecycleCallback create;
  RacerLifecycleCallback update;
  RacerLifecycleCallback control;
  RacerLifecycleCallback destroy;
  int base_slip_budget;
  int active_slip_budget;
  int reserved_150;
  int base_drag;
  int surface_contact_scale_8_8;
  int minimum_acceleration;
  int contact_threshold;
  int impact_strength;
  int impact_scale;
  int bounce_scale;
  int collision_half_width_x;
  int collision_half_length_z;
  int collision_half_height_y;
  int collision_impulse_scale_8_8;
  int collision_mass;
  RacerContactCallback contact;
  int template_enabled;
  int template_visible;
} RacerCreationTemplate;

typedef struct RacerMotionDefaults
{
  int collision_half_width_x;
  int collision_half_length_z;
  int collision_half_height_y;
  int collision_impulse_scale_8_8;
  int collision_mass;
  RacerContactCallback collision_response;
} RacerMotionDefaults;

typedef struct RiderRuntimeSlot
{
  void *resource_item;
  unsigned int rider_flags_and_style;
  int bike_rating;
  int rider_rating;
  int race_score;
  unsigned int packed_resource_selector;
  int level_tuning[6];
  int rank_weight;
  int maximum_health;
  int recovery_health_ceiling;
  int current_health;
} RiderRuntimeSlot;

typedef struct ResourceRefPair
{
  int object_offset;
  unsigned int packed_resource_ref;
} ResourceRefPair;

typedef int (*RiderUpdateCallback)(void *rider);

typedef struct InputChordState
{
  unsigned int button_mask;
  signed char control_pad;
  unsigned char control_pad_alignment[3];
  int transition_state;
} InputChordState;

typedef struct InputThreadState
{
  unsigned char polling_enabled;
  unsigned char polling_alignment[3];
  unsigned char worker_running;
  unsigned char worker_alignment[3];
  unsigned char deferred_event_pending;
  unsigned char deferred_event_alignment[3];
  int deferred_event_ticks;
  int deferred_event_started_at;
  int vbl_io_request;
  InputChordState primary_chord;
  InputChordState alternate_chord;
  int chord_processing_active;
  int overlay_resource_item;
  int overlay_progress;
  int task_item;
  int wake_signal;
} InputThreadState;

typedef struct OpponentChoiceRecord
{
  int resource_choices[6][2];
  signed char preferred_rider_types[3][2];
  unsigned char alignment[2];
} OpponentChoiceRecord;

typedef struct OpponentChoiceLevel
{
  OpponentChoiceRecord riders[OPPONENT_CHOICE_RIDER_COUNT];
} OpponentChoiceLevel;

typedef struct FrontEndCelDescriptor
{
  int resource_id;
  int parent_index;
  signed char child_selector;
  unsigned char selector_alignment[3];
} FrontEndCelDescriptor;

typedef enum FrontEndCommandType
{
  FRONT_END_COMMAND_END = -1,
  FRONT_END_COMMAND_WAIT = 0,
  FRONT_END_COMMAND_SHOW_CELS = 1,
  FRONT_END_COMMAND_PRESENT = 2,
  FRONT_END_COMMAND_RESERVED_THREE = 3,
  FRONT_END_COMMAND_PLAY_STREAM = 4,
  FRONT_END_COMMAND_RESERVED_FIVE = 5,
  FRONT_END_COMMAND_RESERVED_SIX = 6,
  FRONT_END_COMMAND_CALLBACK = 7
} FrontEndCommandType;

typedef struct FrontEndCommandHeader
{
  int type;
} FrontEndCommandHeader;

typedef void (*FrontEndCommandCallback)(int first,
                                        int second);
typedef int (*FrontEndConditionCallback)(int first,
                                         int second);

typedef struct FrontEndWaitCommand
{
  int type;
  int ticks;
} FrontEndWaitCommand;

typedef struct FrontEndShowCelsCommand
{
  int type;
  FrontEndCelDescriptor *descriptors;
} FrontEndShowCelsCommand;

typedef struct FrontEndPresentCommand
{
  int type;
} FrontEndPresentCommand;

typedef struct FrontEndPlayStreamCommand
{
  int type;
  const unsigned char *stream_path;
} FrontEndPlayStreamCommand;

typedef struct FrontEndCallbackCommand
{
  int type;
  FrontEndCommandCallback callback;
  int first_argument;
  int second_argument;
} FrontEndCallbackCommand;

typedef struct FrontEndAction
{
  int id;
  FrontEndCommandHeader **enter_commands;
  FrontEndCommandHeader **selection_exit_commands;
  int up_action_id;
  int down_action_id;
  int left_action_id;
  int right_action_id;
  FrontEndConditionCallback condition;
  int condition_first_argument;
  int condition_second_argument;
  int success_menu_id;
  int failure_menu_id;
} FrontEndAction;

typedef struct FrontEndMenu
{
  int id;
  FrontEndCommandHeader **enter_commands;
  FrontEndCommandHeader **exit_commands;
  int current_action_id;
  unsigned char force_selection;
  unsigned char selection_alignment[3];
} FrontEndMenu;

typedef struct FrontEndAssetState
{
  void *background_audio_resource;
  void *font_resource;
  unsigned int action_navigation_mask;
  int menu_audio_item;
} FrontEndAssetState;

typedef struct RaceModeState
{
  unsigned char game_mode;
  unsigned char player_count;
  signed char active_player_index;
  unsigned char music_enabled;
  unsigned char engine_audio_enabled;
  signed char finish_position;
  unsigned char option_alignment[2];
  int outcome_mode;
  signed char previous_background_audio_index;
  signed char background_audio_index;
  unsigned char preview_alignment[2];
} RaceModeState;

typedef struct RiderProfile
{
  signed char rider_type;
  signed char rider_style;
  signed char current_variant;
  signed char level;
  unsigned char completed_courses;
  unsigned char engine_audio_enabled;
  unsigned char profile_alignment[2];
  int balance;
  int opponent_rider_types[4];
  int opponent_resource_ids[4];
  int opponent_styles[4];
} RiderProfile;

typedef struct RaceResultEntry
{
  int rider_name_index;
  int elapsed_ticks;
} RaceResultEntry;

typedef struct FrontEndPreviewState
{
  int profile_player_index;
  int profile_operation_state;
  unsigned char initial_refresh_pending;
  unsigned char refresh_alignment[3];
  unsigned char operation_busy;
  unsigned char operation_alignment[3];
  unsigned char transition_completed;
  unsigned char completion_alignment[3];
  int selected_resource_id;
  int next_menu_override_id;
} FrontEndPreviewState;

typedef void (*TrackEffectCallback)();

typedef struct TrackEffectPools
{
  void *child_pool;
  void *parent_pool;
  CCB *cel_source;
} TrackEffectPools;

typedef struct TrackEffectCreationDescriptor
{
  unsigned char object_flags;
  unsigned char flags_alignment[3];
  int object_type;
  int update_delay_ticks;
  TrackEffectCallback render;
  TrackEffectCallback update;
  TrackEffectCallback secondary_update;
  TrackEffectCallback destroy;
} TrackEffectCreationDescriptor;

typedef struct RaceInputLatchState
{
  unsigned char event_latched;
  unsigned char input_alignment[3];
  int reserved_event_word;
  unsigned int previous_input;
  int previous_input_tick;
  int reserved_tick_word;
} RaceInputLatchState;

typedef struct AudioVoiceSlot
{
  int voice_item;
  int sample_item;
} AudioVoiceSlot;

struct AudioManagerVoiceGroup;

typedef struct AudioEffectChannel
{
  struct AudioManagerVoiceGroup *voice_group;
  AudioVoiceSlot channel;
  int auxiliary_voice_item;
} AudioEffectChannel;

typedef struct PursuitVoiceGroup
{
  struct AudioManagerVoiceGroup *voice_group;
  int horn_voice_item;
  int siren_voice_item;
  int siren_blast_voice_item;
} PursuitVoiceGroup;

struct MidiPlayer;
struct MidiStream;

typedef struct ScorePlaybackState
{
  struct MidiPlayer *player;
  int player_status;
  void *event_image;
  int event_image_size;
  struct MidiStream *stream;
} ScorePlaybackState;

typedef struct GameAudioState
{
  AudioVoiceSlot engine_voices[3];
  AudioEffectChannel effect_channels[2];
  PursuitVoiceGroup pursuit_voices;
  int sound_sample_items[GAME_SOUND_RESOURCE_COUNT];
  ScorePlaybackState score;
  unsigned char siren_active;
  unsigned char siren_alignment[3];
  unsigned char siren_blast_active;
  unsigned char blast_alignment[3];
  int siren_phase_ticks;
  unsigned char initialized;
  unsigned char initialization_alignment[3];
  unsigned char score_enabled;
  unsigned char score_alignment[3];
  char score_configuration_suffix[8];
  char score_event_suffix[8];
  void *sound_resource_file;
  unsigned char boost_sound_active;
  unsigned char boost_alignment[3];
  unsigned char alternate_engine_sound_active;
  unsigned char alternate_engine_alignment[3];
  int queued_event_count;
  int queued_event_write_index;
  int queued_event_read_index;
  int transient_sample_item;
  int voice_pitch[5];
  int pitch_step[4];
  int voice_volume[4];
  int voice_pan[4];
  int sliding_voice_gain;
  int siren_blast_pitch;
  int last_player_mode;
  int repeated_event_countdown;
} GameAudioState;

typedef struct RaceObjectSchedulerState
{
  int camera_follow_distance;
  int camera_height_offset;
  int pending_racer_count;
  unsigned char player_finished;
  unsigned char pending_alignment[3];
  int reserved_scheduler_word;
  int visible_track_node_count;
  struct IntrusiveListHeader *object_list;
  unsigned char removal_pending;
  unsigned char removal_alignment[3];
  int object_update_tick;
  struct IntrusiveListNode *first_visible_object;
  struct IntrusiveListNode *last_visible_object;
  int current_frame_counter;
  int previous_frame_counter;
} RaceObjectSchedulerState;

typedef struct TrackVisibilityScanState
{
  int current_index;
  int resume_index;
} TrackVisibilityScanState;

typedef struct RiderPhysicsTuning
{
  int steering_heading_reference;
  unsigned char reserved_surface_flag;
  unsigned char traction_alignment[3];
  int player_slip_ticks;
  signed char surface_drag[RIDER_PHYSICS_SURFACE_COUNT];
  int base_grip[RIDER_PHYSICS_SURFACE_COUNT];
  int steering_gain[RIDER_PHYSICS_SURFACE_COUNT];
  int steering_correction[RIDER_PHYSICS_SURFACE_COUNT];
  int skid_speed_loss[RIDER_PHYSICS_SURFACE_COUNT];
  int lane_surface_grip[RIDER_PHYSICS_LANE_COUNT]
  [RIDER_PHYSICS_SURFACE_COUNT];
  int acceleration_drag;
} RiderPhysicsTuning;

typedef struct RendererMotionState
{
  int source_position_x;
  int source_position_y;
  int source_position_z;
  int projection_origin_x;
  int projection_origin_y;
  int projection_origin_z;
  int packed_track_position;
  int road_scroll_phase;
  int camera_velocity;
  int camera_acceleration;
  BigEndianByteFlag periodic_event_flags;
} RendererMotionState;

typedef struct RoadRendererState
{
  CCB *road_layers[4];
  void *overlay_resource_items[3];
  unsigned char road_side_selector;
  unsigned char selector_alignment[3];
  unsigned int control_flags;
  int texture_resource_threshold;
  unsigned char bit_length[RENDERER_BIT_LENGTH_COUNT];
  unsigned char startup_render_mode;
  unsigned char depth_alignment[3];
  int input_latch;
  int road_horizon;
  int *reciprocal_table;
  unsigned char worker_running;
  unsigned char worker_alignment[3];
  int worker_task_item;
  RendererMotionState motion;
  int frame_count;
  int wake_signal;
} RoadRendererState;

typedef struct AnimationBindingSpec
{
  int resource_key;
  unsigned char channel;
  unsigned char transition_command;
  unsigned short alignment;
} AnimationBindingSpec;

typedef AnimationBindingSpec AnimationChannelSpec;

typedef struct AudioMixerTemplateDefaults
{
  int maximum_amplitude;
  const char *instrument_names[4];
} AudioMixerTemplateDefaults;

typedef struct CarAnimationLoadState
{
  unsigned char animation_count;
  unsigned char count_alignment[3];
  int resource_file_item;
} CarAnimationLoadState;

typedef struct AnimationResourcePair
{
  void *cans_resource;
  void *animation_resource;
} AnimationResourcePair;

typedef AnimationResourcePair CarAnimationResourcePair;

typedef struct SegmentRepairGlobals
{
  void *resource_destination;
  void *current_segment;
  int repair_status;
  void *repair_config;
  int scan_workspace[7];
  int destination_count;
  int completion_state;
  int repair_pass_count;
  int last_result;
} SegmentRepairGlobals;

typedef struct FamilyResourceRequest
{
  int family_id;
  int group_index;
  int load_mode;
} FamilyResourceRequest;

typedef struct FamilyResourceGlobals
{
  char directory[12];
  char file_stem[12];
  int resource_file_item;
  int worker_task_item;
  int wake_signal;
  unsigned char worker_running;
  unsigned char worker_alignment[3];
  FamilyResourceRequest request_queue[FAMILY_REQUEST_COUNT];
  int request_write_index;
  int request_read_index;
  int resident_family_ids[FAMILY_GROUP_COUNT];
  int group_locks[FAMILY_GROUP_COUNT];
  void *group_tables[FAMILY_GROUP_COUNT];
  int group_count;
  int table_bytes;
  int worker_stack_bytes;
  int worker_request[FAMILY_GROUP_COUNT];
  int last_seek_result;
} FamilyResourceGlobals;

typedef struct TrackSimulationGlobals
{
  BigEndianByteFlag simulation_active;
  unsigned char enabled;
  unsigned char enabled_alignment[3];
  RendererMotionState motion_source;
  int applied_track_position;
  unsigned int control_bits;
  int update_signal;
  int completion_signal;
  unsigned char worker_running;
  unsigned char worker_alignment[3];
  int worker_task_item;
  int two_tick_event_countdown;
  int three_tick_event_countdown;
  int speed_scale;
  void *active_object;
  int active_workspace[7];
  unsigned char packed_divisor;
  unsigned char divisor_alignment[3];
  int last_result;
} TrackSimulationGlobals;

typedef struct TrackObjectModeTemplate
{
  unsigned int object_flags;
  int collision_radius;
  int horizontal_scale;
  int vertical_scale;
  int lifetime_ticks;
  RacerContactCallback contact;
} TrackObjectModeTemplate;

typedef struct RandomSeedScratch
{
  int seed_high_product;
  int global_high_product;
} RandomSeedScratch;

typedef struct DisplayRuntimeGlobals
{
  int saved_screen_items[2];
  int saved_bitmap_items[2];
  int active_bitmap_index;
  int screen_items[2];
  struct Bitmap *bitmaps[2];
  int bitmap_items[2];
  int vbl_io_request;
  int draw_buffer_index;
  int display_buffer_index;
  unsigned char enabled;
  unsigned char enabled_alignment[3];
  struct CCB *cel_packet_base;
  struct CCB *cel_packet_limit;
  struct CCB *cel_packet_cursor;
  int cel_packet_capacity;
  unsigned char synchronize_to_vbl;
  unsigned char synchronize_to_vbl_alignment[3];
  int last_present_vbl;
  unsigned int default_ccb_flags;
  unsigned int *solid_pixel_base;
  unsigned int *solid_pixel_cursor;
  unsigned int *solid_pixel_limit;
  int solid_pixel_capacity;
  struct CCB *solid_fill_ccb;
  struct CCB *fallback_ccb;
  unsigned char initialized;
  unsigned char initialized_alignment[3];
  unsigned char easing_lookup[DISPLAY_EASING_COUNT];
  unsigned char easing_alignment[3];
  unsigned char trig_lookup_low[TRIG_LOOKUP_LOW_COUNT];
  int trig_lookup_high[TRIG_LOOKUP_HIGH_COUNT];
  unsigned char special_ccb_override;
  unsigned char special_ccb_alignment[3];
  int shading_mode;
  int shading_row_offset;
  RandomSeedScratch random_seed_scratch;
} DisplayRuntimeGlobals;

typedef struct RaceObjectCreationDescriptor
{
  unsigned char status_flags;
  unsigned char status_alignment[3];
  int collision_class;
  int update_interval_ticks;
  RacerLifecycleCallback render_object;
  RacerLifecycleCallback prepare_update;
  RacerLifecycleCallback perform_update;
  RacerLifecycleCallback reset_object;
} RaceObjectCreationDescriptor;

typedef struct TrackRuntimeGlobals
{
  int family_schedule_window_span;
  struct RoadTrackGraphNode *track_root;
  int selector;
  unsigned char selector_override;
  unsigned char selector_alignment[3];
  unsigned char family_schedule_events_enabled;
  unsigned char schedule_enable_alignment[3];
  unsigned char async_family_resource_loads_enabled;
  unsigned char async_load_alignment[3];
  void *road_segment_pool;
  RaceObjectCreationDescriptor road_object_descriptor;
  void *allocation_pool;
  int bucket_handles[2];
  int current_track_position;
  int left_bound;
  int right_bound;
  int finish_threshold;
  unsigned char initialized;
  unsigned char initialized_alignment[3];
  struct RoadTrackGraphNode *traversal_root;
} TrackRuntimeGlobals;

typedef struct ResourceCatalogHandles
{
  int small_digit_font;
  int large_digit_font;
  void *rash_options_resource;
  void *course_resource;
} ResourceCatalogHandles;

typedef struct ResourceCatalogEntry
{
  void *destination;
  unsigned int chunk_type;
  int resource_index;
  void *node;
  void *resource;
} ResourceCatalogEntry;

typedef struct RuntimeFeatureFlags
{
  BigEndianByteFlag music_option_enabled;
  BigEndianByteFlag game_audio_enabled;
  BigEndianByteFlag track_objects_enabled;
  BigEndianByteFlag unused_slot_3;
  BigEndianByteFlag alternate_hud_layout;
  BigEndianByteFlag streaming_audio_enabled;
  BigEndianByteFlag unused_slot_6;
  BigEndianByteFlag background_audio_enabled;
} RuntimeFeatureFlags;

typedef struct ResourceInfoState
{
  void *resource;
  int resource_item;
  int status;
} ResourceInfoState;

typedef AnimationBindingSpec AnimationFrameDescriptor;

typedef struct MoviePlaybackControl
{
  unsigned char active;
  unsigned char active_alignment[3];
  int frame_delay;
} MoviePlaybackControl;

typedef struct DisplaySystemGlobals
{
  Item vbl_io_request;
  Item vram_io_request;
  Item screen_items[5];
  Bitmap *bitmaps[4];
  Item *bitmap_items;
  int bitmap_storage_bytes;
  int bitmap_storage_pages;
  int cached_bitmap_storage_bytes;
  int cached_bitmap_storage_pages;
  signed char active_screen;
  unsigned char active_alignment[3];
  unsigned char screen_count;
  unsigned char count_alignment[3];
} DisplaySystemGlobals;

typedef struct ParticleResourceGlobals
{
  void *palette_node;
  void *resource_nodes[PARTICLE_RESOURCE_COUNT];
  void *cel_pointers[PARTICLE_RESOURCE_COUNT];
  int active_count;
  void *node_slots[2];
  int render_state[3];
} ParticleResourceGlobals;

typedef struct HudCoordinate
{
  int x;
  int y;
} HudCoordinate;

typedef struct HudGlobals
{
  HudCoordinate coordinates[HUD_COORDINATE_COUNT];
  char formatted_text[20];
  CCB *primary_hud_cel;
  int gauge_animation_item;
  CCB *boost_bar_ccb;
  CCB *speedometer_needle_ccb;
  CCB *health_gauge_needle_ccb;
  void *rider_resource_items[2];
  struct RacerEntity *tracked_object;
  int smoothed_speed;
} HudGlobals;

typedef struct RoadSurfaceRenderGlobals
{
  unsigned char disabled;
  unsigned char disabled_alignment[3];
  int configuration[12];
  void *primary_ccb;
  void *fallback_ccb;
  int column_count;
  int cached_depth;
  int cached_sine;
  int cached_cosine;
} RoadSurfaceRenderGlobals;

typedef struct AudioPlaybackState
{
  BigEndianByteFlag active;
  int channel_count;
  int sample_item;
  int playback_voice_id;
  int buffer_bytes;
  void *buffer_a;
  void *buffer_b;
  int chunk_bytes;
  int active_buffer_index;
  int amplitude;
} AudioPlaybackState;

typedef struct AudioSourceDescriptor
{
  int source_metadata[10];
  int sample_width_bits;
  int sample_rate;
  int channel_count;
  unsigned int compression_type;
  int compression_ratio;
  int flags;
} AudioSourceDescriptor;

typedef struct BackgroundAudioGlobals
{
  int stream_state;
  unsigned char enabled;
  unsigned char enabled_alignment[3];
  int stream_argument;
  const char *names[BACKGROUND_AUDIO_NAME_COUNT];
  int name_count;
  unsigned char selection_state;
  unsigned char selection_state_alignment[3];
  int gain_step;
  int gain_cap;
  int current_gain;
  int last_adjustment_time;
} BackgroundAudioGlobals;

typedef struct MemoryAllocatorGlobals
{
  int lock_item;
  void *linked_list_head;
  void *bank_pointer;
  void *block_list_head;
  void *block_list_tail;
  void *heap_list_head;
  struct MemoryPressureMonitor **memory_pressure_monitor_head_ref;
  int reserved_state[5];
  int primary_memory_type;
  int secondary_memory_type;
} MemoryAllocatorGlobals;

typedef struct ResourceManagerGlobals
{
  void *resource_list;
  int lock_item;
  int current_item;
} ResourceManagerGlobals;

typedef struct RwTagArgument
{
  unsigned int tag;
  unsigned int argument;
} RwTagArgument;

typedef struct RwMemoryVectors
{
  RwMemoryAllocator allocate;
  RwMemoryReleaser release;
} RwMemoryVectors;

typedef struct SAudioTemplateRecord
{
  unsigned int template_tag;
  int template_item;
  const char *instrument_name;
} SAudioTemplateRecord;

typedef struct ThreadCreationDefaults
{
  RwTagArgument tags[9];
  char *next_thread_name;
} ThreadCreationDefaults;

typedef struct BlockFileApiGlobals
{
  int last_error;
  RwMemoryAllocator allocate;
  RwMemoryReleaser release;
} BlockFileApiGlobals;

typedef struct LinkedListMemoryApi
{
  int last_error;
  RwMemoryAllocator allocate;
  RwMemoryReleaser release;
} LinkedListMemoryApi;

typedef struct BlockFileSectorCache
{
  void *sector_buffer;
  int valid_bytes;
  int sector_number;
  int file_item;
  unsigned int flags;
  int lock_item;
} BlockFileSectorCache;

typedef struct CPakSubscriberGlobals
{
  void *context_pool;
  void *record_pool;
  int clock_loop_detected;
} CPakSubscriberGlobals;

typedef struct EventBrokerClientState
{
  int category;
  int trigger_mask;
  void *configuration;
  void *event_frame;
  int message_port;
  int message_item;
  void *message;
  int broker_item;
} EventBrokerClientState;

typedef struct LegacyRandomGeneratorState
{
  unsigned int values[LEGACY_RANDOM_STATE_COUNT];
  int lag_index;
  int cursor_index;
} LegacyRandomGeneratorState;

typedef struct CamStdioRuntime
{
  int device_item;
  int io_request_item;
  void *io_request;
  void *standard_input;
  void *standard_output;
  void *standard_error;
} CamStdioRuntime;

extern unsigned int operamath_atan_ratio_table[OPERAMATH_ATAN_RATIO_COUNT];
extern unsigned int operamath_quarter_sine_table[OPERAMATH_QUARTER_SINE_COUNT];

extern void *file_folio_base;
extern EventBrokerClientState event_broker_client;

extern unsigned int ansi_random_seed;
extern LegacyRandomGeneratorState legacy_random_generator;
extern unsigned int portable_random_seed;

extern CamStdioRuntime cam_stdio_runtime;
extern RwTagArgument cam_io_request_tags[2];

extern void *stream_acquisition_context_pool;
extern void *stream_context_pool;
extern int stream_end_of_file_seen;
extern CPakSubscriberGlobals cpak_subscriber_globals;
extern void *saudio_subscriber_context_pool;
extern void *ctrl_subscriber_context_pool;
extern SAudioTemplateRecord
  saudio_initial_templates[SAUDIO_TEMPLATE_COUNT];
extern int saudio_template_count;

extern char uppercase_hex_digits[20];
extern const char *score_mixer_destination_names[SCORE_MIXER_INPUT_COUNT];
extern RwTagArgument score_note_start_tags[3];
extern const char *score_mixer_left_gain_names[SCORE_MIXER_GAIN_COUNT];
extern const char *score_mixer_right_gain_names[SCORE_MIXER_GAIN_COUNT];
extern RwMemoryVectors custom_memory_vectors;

extern ThreadCreationDefaults audio_manager_thread_defaults;
extern BlockFileApiGlobals block_file_api_globals;
extern BlockFileSectorCache block_file_sector_cache;
extern RwMemoryVectors byte_file_memory_vectors;
extern int block_file_last_error;
extern LinkedListMemoryApi linked_list_memory_api;
extern int memory_system_last_error;
extern int resource_system_last_error;

extern void *kernel_vector_base;
extern RaceBoostConfig race_boost_config;
extern RaceRiderState race_rider_state;
extern RacerCreationTemplate challenge_object_template;
extern RacerCreationTemplate challenge_state_template;
extern RacerCreationTemplate opponent_object_template;
extern RacerCreationTemplate opponent_state_template;
extern RacerCreationTemplate player_object_template;
extern RacerCreationTemplate player_state_template;
extern RacerMotionDefaults upright_motion_defaults;
extern RacerMotionDefaults impact_motion_defaults;
extern RiderRuntimeSlot rider_runtime_slots[RACE_RIDER_COUNT];
extern signed char rider_runtime_selectors[RACE_RIDER_COUNT];
extern ResourceRefPair
  rider_resource_references[2][RACE_RIDER_COUNT];
extern void *rider_resource_items[RACE_RIDER_COUNT];
extern void *challenge_resource_items[8];
extern char rider_names[RIDER_TYPE_COUNT_RW][RIDER_NAME_BYTES];
extern char opponent_names[OPPONENT_NAME_LEVELS]
[OPPONENTS_PER_LEVEL][RIDER_NAME_BYTES];
extern unsigned char opponent_names_alignment[2];
extern unsigned char
  rider_style_defaults[RIDER_TYPE_COUNT_RW]
                      [RIDER_PROGRESSION_LEVEL_COUNT];
extern int opponent_speed_percent[RIDER_PROGRESSION_LEVEL_COUNT];
extern int rider_animation_fallback_states[50];
extern int opponent_resource_group_a[8];
extern int opponent_resource_group_b[8];
extern int opponent_resource_group_c[8];
extern OpponentProgressionRecord
  rider_progression_defaults[RIDER_TYPE_COUNT];
extern RiderUpdateCallback
  rider_update_handlers[RIDER_UPDATE_HANDLER_COUNT];
extern int rider_pacing_factor_adjustments[20];
extern int rider_contact_scale_by_mode[4];
extern int rider_contact_delay_thresholds[5];
extern InputThreadState input_thread_state;
extern OpponentChoiceLevel
  opponent_choice_levels[OPPONENT_CHOICE_LEVEL_COUNT];
extern FrontEndPresentCommand front_end_present_command_000;
extern FrontEndWaitCommand front_end_wait_command_000;
extern FrontEndCelDescriptor front_end_cel_group_000[2];
extern FrontEndCelDescriptor front_end_cel_group_001[2];
extern FrontEndCelDescriptor front_end_cel_group_002[2];
extern FrontEndShowCelsCommand front_end_show_cels_command_000;
extern FrontEndShowCelsCommand front_end_show_cels_command_001;
extern FrontEndShowCelsCommand front_end_show_cels_command_002;
extern FrontEndWaitCommand front_end_wait_command_001;
extern FrontEndCelDescriptor front_end_cel_group_003[14];
extern FrontEndCallbackCommand front_end_callback_command_000;
extern FrontEndCelDescriptor front_end_cel_group_004[2];
extern FrontEndShowCelsCommand front_end_show_cels_command_003;
extern FrontEndCommandHeader *front_end_command_sequence_000[8];
extern FrontEndCelDescriptor front_end_cel_group_005[2];
extern FrontEndCelDescriptor front_end_cel_group_006[2];
extern FrontEndCallbackCommand front_end_callback_command_001;
extern FrontEndCallbackCommand front_end_callback_command_002;
extern FrontEndCallbackCommand front_end_callback_command_003;
extern FrontEndCelDescriptor front_end_cel_group_007[5];
extern FrontEndShowCelsCommand front_end_show_cels_command_004;
extern FrontEndCelDescriptor front_end_cel_group_008[6];
extern FrontEndShowCelsCommand front_end_show_cels_command_005;
extern FrontEndCommandHeader *front_end_command_sequence_001[5];
extern FrontEndCelDescriptor front_end_cel_group_009[5];
extern FrontEndShowCelsCommand front_end_show_cels_command_006;
extern FrontEndCelDescriptor front_end_cel_group_010[6];
extern FrontEndShowCelsCommand front_end_show_cels_command_007;
extern FrontEndCommandHeader *front_end_command_sequence_002[5];
extern FrontEndCelDescriptor front_end_cel_group_011[5];
extern FrontEndShowCelsCommand front_end_show_cels_command_008;
extern FrontEndCelDescriptor front_end_cel_group_012[6];
extern FrontEndShowCelsCommand front_end_show_cels_command_009;
extern FrontEndCommandHeader *front_end_command_sequence_003[5];
extern FrontEndCallbackCommand front_end_callback_command_004;
extern FrontEndCallbackCommand front_end_callback_command_005;
extern FrontEndCallbackCommand front_end_callback_command_006;
extern FrontEndCallbackCommand front_end_callback_command_007;
extern FrontEndCallbackCommand front_end_callback_command_008;
extern FrontEndCelDescriptor front_end_cel_group_013[2];
extern FrontEndCallbackCommand front_end_callback_command_009;
extern FrontEndCommandHeader *front_end_command_sequence_004[2];
extern FrontEndCelDescriptor front_end_cel_group_014[6];
extern FrontEndShowCelsCommand front_end_show_cels_command_010;
extern FrontEndCelDescriptor front_end_cel_group_015[8];
extern FrontEndShowCelsCommand front_end_show_cels_command_011;
extern FrontEndCommandHeader *front_end_command_sequence_005[7];
extern FrontEndCelDescriptor front_end_cel_group_016[6];
extern FrontEndShowCelsCommand front_end_show_cels_command_012;
extern FrontEndCelDescriptor front_end_cel_group_017[8];
extern FrontEndShowCelsCommand front_end_show_cels_command_013;
extern FrontEndCommandHeader *front_end_command_sequence_006[7];
extern FrontEndCelDescriptor front_end_cel_group_018[6];
extern FrontEndShowCelsCommand front_end_show_cels_command_014;
extern FrontEndCelDescriptor front_end_cel_group_019[8];
extern FrontEndShowCelsCommand front_end_show_cels_command_015;
extern FrontEndCommandHeader *front_end_command_sequence_007[7];
extern FrontEndCelDescriptor front_end_cel_group_020[6];
extern FrontEndShowCelsCommand front_end_show_cels_command_016;
extern FrontEndCelDescriptor front_end_cel_group_021[8];
extern FrontEndShowCelsCommand front_end_show_cels_command_017;
extern FrontEndCommandHeader *front_end_command_sequence_008[7];
extern FrontEndCelDescriptor front_end_cel_group_022[6];
extern FrontEndShowCelsCommand front_end_show_cels_command_018;
extern FrontEndCelDescriptor front_end_cel_group_023[8];
extern FrontEndShowCelsCommand front_end_show_cels_command_019;
extern FrontEndCommandHeader *front_end_command_sequence_009[7];
extern FrontEndCelDescriptor front_end_cel_group_024[6];
extern FrontEndShowCelsCommand front_end_show_cels_command_020;
extern FrontEndCelDescriptor front_end_cel_group_025[8];
extern FrontEndShowCelsCommand front_end_show_cels_command_021;
extern FrontEndCommandHeader *front_end_command_sequence_010[6];
extern FrontEndCelDescriptor front_end_cel_group_026[6];
extern FrontEndShowCelsCommand front_end_show_cels_command_022;
extern FrontEndCelDescriptor front_end_cel_group_027[8];
extern FrontEndShowCelsCommand front_end_show_cels_command_023;
extern FrontEndCommandHeader *front_end_command_sequence_011[7];
extern FrontEndCelDescriptor front_end_cel_group_028[6];
extern FrontEndShowCelsCommand front_end_show_cels_command_024;
extern FrontEndCelDescriptor front_end_cel_group_029[8];
extern FrontEndShowCelsCommand front_end_show_cels_command_025;
extern FrontEndCommandHeader *front_end_command_sequence_012[7];
extern FrontEndCelDescriptor front_end_cel_group_030[6];
extern FrontEndShowCelsCommand front_end_show_cels_command_026;
extern FrontEndCelDescriptor front_end_cel_group_031[8];
extern FrontEndShowCelsCommand front_end_show_cels_command_027;
extern FrontEndCommandHeader *front_end_command_sequence_013[7];
extern FrontEndCelDescriptor front_end_cel_group_032[6];
extern FrontEndShowCelsCommand front_end_show_cels_command_028;
extern FrontEndCelDescriptor front_end_cel_group_033[8];
extern FrontEndShowCelsCommand front_end_show_cels_command_029;
extern FrontEndCommandHeader *front_end_command_sequence_014[7];
extern FrontEndCelDescriptor front_end_cel_group_034[6];
extern FrontEndShowCelsCommand front_end_show_cels_command_030;
extern FrontEndCelDescriptor front_end_cel_group_035[8];
extern FrontEndShowCelsCommand front_end_show_cels_command_031;
extern FrontEndCommandHeader *front_end_command_sequence_015[7];
extern FrontEndCelDescriptor front_end_cel_group_036[6];
extern FrontEndShowCelsCommand front_end_show_cels_command_032;
extern FrontEndCelDescriptor front_end_cel_group_037[8];
extern FrontEndShowCelsCommand front_end_show_cels_command_033;
extern FrontEndCommandHeader *front_end_command_sequence_016[6];
extern FrontEndCallbackCommand front_end_callback_command_010;
extern FrontEndCallbackCommand front_end_callback_command_011;
extern FrontEndCallbackCommand front_end_callback_command_012;
extern FrontEndCallbackCommand front_end_callback_command_013;
extern FrontEndCallbackCommand front_end_callback_command_014;
extern FrontEndCallbackCommand front_end_callback_command_015;
extern FrontEndCallbackCommand front_end_callback_command_016;
extern FrontEndCallbackCommand front_end_callback_command_017;
extern FrontEndCallbackCommand front_end_callback_command_018;
extern FrontEndCelDescriptor front_end_cel_group_038[6];
extern FrontEndShowCelsCommand front_end_show_cels_command_034;
extern FrontEndCommandHeader *front_end_command_sequence_017[5];
extern FrontEndCelDescriptor front_end_cel_group_039[6];
extern FrontEndShowCelsCommand front_end_show_cels_command_035;
extern FrontEndCommandHeader *front_end_command_sequence_018[5];
extern FrontEndCelDescriptor front_end_cel_group_040[6];
extern FrontEndShowCelsCommand front_end_show_cels_command_036;
extern FrontEndCommandHeader *front_end_command_sequence_019[5];
extern FrontEndCelDescriptor front_end_cel_group_041[6];
extern FrontEndShowCelsCommand front_end_show_cels_command_037;
extern FrontEndCommandHeader *front_end_command_sequence_020[5];
extern FrontEndCelDescriptor front_end_cel_group_042[6];
extern FrontEndShowCelsCommand front_end_show_cels_command_038;
extern FrontEndCommandHeader *front_end_command_sequence_021[5];
extern FrontEndCelDescriptor front_end_cel_group_043[6];
extern FrontEndShowCelsCommand front_end_show_cels_command_039;
extern FrontEndCommandHeader *front_end_command_sequence_022[5];
extern FrontEndCelDescriptor front_end_cel_group_044[6];
extern FrontEndShowCelsCommand front_end_show_cels_command_040;
extern FrontEndCommandHeader *front_end_command_sequence_023[5];
extern FrontEndCelDescriptor front_end_cel_group_045[6];
extern FrontEndShowCelsCommand front_end_show_cels_command_041;
extern FrontEndCommandHeader *front_end_command_sequence_024[5];
extern FrontEndCelDescriptor front_end_cel_group_046[4];
extern FrontEndCelDescriptor front_end_cel_group_047[2];
extern FrontEndCallbackCommand front_end_callback_command_019;
extern FrontEndCallbackCommand front_end_callback_command_020;
extern FrontEndCelDescriptor front_end_cel_group_048[3];
extern FrontEndShowCelsCommand front_end_show_cels_command_042;
extern FrontEndCelDescriptor front_end_cel_group_049[5];
extern FrontEndShowCelsCommand front_end_show_cels_command_043;
extern FrontEndCommandHeader *front_end_command_sequence_025[7];
extern FrontEndCelDescriptor front_end_cel_group_050[5];
extern FrontEndShowCelsCommand front_end_show_cels_command_044;
extern FrontEndCommandHeader *front_end_command_sequence_026[4];
extern FrontEndCelDescriptor front_end_cel_group_051[5];
extern FrontEndShowCelsCommand front_end_show_cels_command_045;
extern FrontEndCommandHeader *front_end_command_sequence_027[4];
extern FrontEndCelDescriptor front_end_cel_group_052[3];
extern FrontEndShowCelsCommand front_end_show_cels_command_046;
extern FrontEndCelDescriptor front_end_cel_group_053[5];
extern FrontEndShowCelsCommand front_end_show_cels_command_047;
extern FrontEndCommandHeader *front_end_command_sequence_028[7];
extern FrontEndCelDescriptor front_end_cel_group_054[3];
extern FrontEndShowCelsCommand front_end_show_cels_command_048;
extern FrontEndCelDescriptor front_end_cel_group_055[5];
extern FrontEndShowCelsCommand front_end_show_cels_command_049;
extern FrontEndCommandHeader *front_end_command_sequence_029[7];
extern FrontEndCelDescriptor front_end_cel_group_056[3];
extern FrontEndShowCelsCommand front_end_show_cels_command_050;
extern FrontEndCelDescriptor front_end_cel_group_057[5];
extern FrontEndShowCelsCommand front_end_show_cels_command_051;
extern FrontEndCommandHeader *front_end_command_sequence_030[7];
extern FrontEndCelDescriptor front_end_cel_group_058[3];
extern FrontEndShowCelsCommand front_end_show_cels_command_052;
extern FrontEndCelDescriptor front_end_cel_group_059[5];
extern FrontEndShowCelsCommand front_end_show_cels_command_053;
extern FrontEndCommandHeader *front_end_command_sequence_031[7];
extern FrontEndCelDescriptor front_end_cel_group_060[3];
extern FrontEndShowCelsCommand front_end_show_cels_command_054;
extern FrontEndCelDescriptor front_end_cel_group_061[5];
extern FrontEndShowCelsCommand front_end_show_cels_command_055;
extern FrontEndCommandHeader *front_end_command_sequence_032[7];
extern FrontEndCelDescriptor front_end_cel_group_062[3];
extern FrontEndShowCelsCommand front_end_show_cels_command_056;
extern FrontEndCelDescriptor front_end_cel_group_063[5];
extern FrontEndShowCelsCommand front_end_show_cels_command_057;
extern FrontEndCommandHeader *front_end_command_sequence_033[7];
extern FrontEndCelDescriptor front_end_cel_group_064[3];
extern FrontEndCelDescriptor front_end_cel_group_065[3];
extern FrontEndCelDescriptor front_end_cel_group_066[3];
extern FrontEndCelDescriptor front_end_cel_group_067[3];
extern FrontEndCelDescriptor front_end_cel_group_068[3];
extern FrontEndCelDescriptor front_end_cel_group_069[3];
extern FrontEndCelDescriptor front_end_cel_group_070[3];
extern FrontEndCelDescriptor front_end_cel_group_071[3];
extern FrontEndCelDescriptor front_end_cel_group_072[3];
extern FrontEndCelDescriptor front_end_cel_group_073[3];
extern FrontEndCelDescriptor front_end_cel_group_074[3];
extern FrontEndCelDescriptor front_end_cel_group_075[3];
extern FrontEndCelDescriptor front_end_cel_group_076[3];
extern FrontEndCelDescriptor front_end_cel_group_077[3];
extern FrontEndCelDescriptor front_end_cel_group_078[3];
extern FrontEndShowCelsCommand front_end_show_cels_command_058;
extern FrontEndCelDescriptor front_end_cel_group_079[5];
extern FrontEndShowCelsCommand front_end_show_cels_command_059;
extern FrontEndCallbackCommand front_end_callback_command_021;
extern FrontEndCommandHeader *front_end_command_sequence_034[8];
extern FrontEndCelDescriptor front_end_cel_group_080[3];
extern FrontEndShowCelsCommand front_end_show_cels_command_060;
extern FrontEndCelDescriptor front_end_cel_group_081[5];
extern FrontEndShowCelsCommand front_end_show_cels_command_061;
extern FrontEndCommandHeader *front_end_command_sequence_035[7];
extern FrontEndCelDescriptor front_end_cel_group_082[7];
extern FrontEndShowCelsCommand front_end_show_cels_command_062;
extern FrontEndCommandHeader *front_end_command_sequence_036[4];
extern FrontEndCelDescriptor front_end_cel_group_083[7];
extern FrontEndShowCelsCommand front_end_show_cels_command_063;
extern FrontEndCommandHeader *front_end_command_sequence_037[4];
extern FrontEndCelDescriptor front_end_cel_group_084[7];
extern FrontEndShowCelsCommand front_end_show_cels_command_064;
extern FrontEndCommandHeader *front_end_command_sequence_038[4];
extern FrontEndCelDescriptor front_end_cel_group_085[7];
extern FrontEndShowCelsCommand front_end_show_cels_command_065;
extern FrontEndCommandHeader *front_end_command_sequence_039[4];
extern FrontEndCelDescriptor front_end_cel_group_086[7];
extern FrontEndShowCelsCommand front_end_show_cels_command_066;
extern FrontEndCommandHeader *front_end_command_sequence_040[4];
extern FrontEndCelDescriptor front_end_cel_group_087[7];
extern FrontEndShowCelsCommand front_end_show_cels_command_067;
extern FrontEndCommandHeader *front_end_command_sequence_041[4];
extern FrontEndCelDescriptor front_end_cel_group_088[7];
extern FrontEndShowCelsCommand front_end_show_cels_command_068;
extern FrontEndCommandHeader *front_end_command_sequence_042[4];
extern FrontEndCelDescriptor front_end_cel_group_089[7];
extern FrontEndShowCelsCommand front_end_show_cels_command_069;
extern FrontEndCommandHeader *front_end_command_sequence_043[4];
extern FrontEndCelDescriptor front_end_cel_group_090[7];
extern FrontEndShowCelsCommand front_end_show_cels_command_070;
extern FrontEndCommandHeader *front_end_command_sequence_044[4];
extern FrontEndCelDescriptor front_end_cel_group_091[7];
extern FrontEndShowCelsCommand front_end_show_cels_command_071;
extern FrontEndCommandHeader *front_end_command_sequence_045[4];
extern FrontEndCelDescriptor front_end_cel_group_092[7];
extern FrontEndShowCelsCommand front_end_show_cels_command_072;
extern FrontEndCommandHeader *front_end_command_sequence_046[4];
extern FrontEndCelDescriptor front_end_cel_group_093[7];
extern FrontEndShowCelsCommand front_end_show_cels_command_073;
extern FrontEndCommandHeader *front_end_command_sequence_047[4];
extern FrontEndCelDescriptor front_end_cel_group_094[7];
extern FrontEndShowCelsCommand front_end_show_cels_command_074;
extern FrontEndCommandHeader *front_end_command_sequence_048[4];
extern FrontEndCelDescriptor front_end_cel_group_095[7];
extern FrontEndShowCelsCommand front_end_show_cels_command_075;
extern FrontEndCommandHeader *front_end_command_sequence_049[4];
extern FrontEndCelDescriptor front_end_cel_group_096[7];
extern FrontEndShowCelsCommand front_end_show_cels_command_076;
extern FrontEndCommandHeader *front_end_command_sequence_050[4];
extern FrontEndCelDescriptor front_end_cel_group_097[7];
extern FrontEndShowCelsCommand front_end_show_cels_command_077;
extern FrontEndCommandHeader *front_end_command_sequence_051[4];
extern FrontEndCelDescriptor front_end_cel_group_098[7];
extern FrontEndShowCelsCommand front_end_show_cels_command_078;
extern FrontEndCommandHeader *front_end_command_sequence_052[4];
extern FrontEndCelDescriptor front_end_cel_group_099[7];
extern FrontEndShowCelsCommand front_end_show_cels_command_079;
extern FrontEndCommandHeader *front_end_command_sequence_053[4];
extern FrontEndCelDescriptor front_end_cel_group_100[7];
extern FrontEndShowCelsCommand front_end_show_cels_command_080;
extern FrontEndCommandHeader *front_end_command_sequence_054[4];
extern FrontEndCelDescriptor front_end_cel_group_101[7];
extern FrontEndShowCelsCommand front_end_show_cels_command_081;
extern FrontEndCommandHeader *front_end_command_sequence_055[4];
extern FrontEndCelDescriptor front_end_cel_group_102[5];
extern FrontEndShowCelsCommand front_end_show_cels_command_082;
extern FrontEndCommandHeader *front_end_command_sequence_056[4];
extern FrontEndCelDescriptor front_end_cel_group_103[5];
extern FrontEndShowCelsCommand front_end_show_cels_command_083;
extern FrontEndCommandHeader *front_end_command_sequence_057[4];
extern FrontEndCelDescriptor front_end_cel_group_104[5];
extern FrontEndShowCelsCommand front_end_show_cels_command_084;
extern FrontEndCommandHeader *front_end_command_sequence_058[4];
extern FrontEndCelDescriptor front_end_cel_group_105[5];
extern FrontEndShowCelsCommand front_end_show_cels_command_085;
extern FrontEndCommandHeader *front_end_command_sequence_059[4];
extern FrontEndCelDescriptor front_end_cel_group_106[5];
extern FrontEndShowCelsCommand front_end_show_cels_command_086;
extern FrontEndCommandHeader *front_end_command_sequence_060[4];
extern FrontEndCelDescriptor front_end_cel_group_107[5];
extern FrontEndShowCelsCommand front_end_show_cels_command_087;
extern FrontEndCallbackCommand front_end_callback_command_022;
extern FrontEndCommandHeader *front_end_command_sequence_061[5];
extern FrontEndCelDescriptor front_end_cel_group_108[5];
extern FrontEndShowCelsCommand front_end_show_cels_command_088;
extern FrontEndCallbackCommand front_end_callback_command_023;
extern FrontEndCommandHeader *front_end_command_sequence_062[5];
extern FrontEndCelDescriptor front_end_cel_group_109[5];
extern FrontEndShowCelsCommand front_end_show_cels_command_089;
extern FrontEndCallbackCommand front_end_callback_command_024;
extern FrontEndCommandHeader *front_end_command_sequence_063[5];
extern FrontEndCelDescriptor front_end_cel_group_110[5];
extern FrontEndShowCelsCommand front_end_show_cels_command_090;
extern FrontEndCallbackCommand front_end_callback_command_025;
extern FrontEndCommandHeader *front_end_command_sequence_064[5];
extern FrontEndCelDescriptor front_end_cel_group_111[5];
extern FrontEndShowCelsCommand front_end_show_cels_command_091;
extern FrontEndCallbackCommand front_end_callback_command_026;
extern FrontEndCommandHeader *front_end_command_sequence_065[5];
extern FrontEndCelDescriptor front_end_cel_group_112[5];
extern FrontEndShowCelsCommand front_end_show_cels_command_092;
extern FrontEndCommandHeader *front_end_command_sequence_066[4];
extern FrontEndCelDescriptor front_end_cel_group_113[5];
extern FrontEndShowCelsCommand front_end_show_cels_command_093;
extern FrontEndCommandHeader *front_end_command_sequence_067[4];
extern FrontEndCelDescriptor front_end_cel_group_114[7];
extern FrontEndShowCelsCommand front_end_show_cels_command_094;
extern FrontEndCommandHeader *front_end_command_sequence_068[4];
extern FrontEndCelDescriptor front_end_cel_group_115[5];
extern FrontEndShowCelsCommand front_end_show_cels_command_095;
extern FrontEndCommandHeader *front_end_command_sequence_069[4];
extern FrontEndCelDescriptor front_end_cel_group_116[5];
extern FrontEndShowCelsCommand front_end_show_cels_command_096;
extern FrontEndCommandHeader *front_end_command_sequence_070[4];
extern FrontEndCallbackCommand front_end_callback_command_027;
extern FrontEndCallbackCommand front_end_callback_command_028;
extern FrontEndCelDescriptor front_end_cel_group_117[2];
extern FrontEndShowCelsCommand front_end_show_cels_command_097;
extern FrontEndCommandHeader *front_end_command_sequence_071[5];
extern FrontEndCelDescriptor front_end_cel_group_118[2];
extern FrontEndShowCelsCommand front_end_show_cels_command_098;
extern FrontEndCommandHeader *front_end_command_sequence_072[4];
extern FrontEndCelDescriptor front_end_cel_group_119[2];
extern FrontEndShowCelsCommand front_end_show_cels_command_099;
extern FrontEndCommandHeader *front_end_command_sequence_073[4];
extern FrontEndCelDescriptor front_end_cel_group_120[2];
extern FrontEndShowCelsCommand front_end_show_cels_command_100;
extern FrontEndCommandHeader *front_end_command_sequence_074[4];
extern FrontEndCelDescriptor front_end_cel_group_121[2];
extern FrontEndShowCelsCommand front_end_show_cels_command_101;
extern FrontEndCommandHeader *front_end_command_sequence_075[4];
extern FrontEndCelDescriptor front_end_cel_group_122[2];
extern FrontEndShowCelsCommand front_end_show_cels_command_102;
extern FrontEndCommandHeader *front_end_command_sequence_076[4];
extern FrontEndCelDescriptor front_end_cel_group_123[2];
extern FrontEndShowCelsCommand front_end_show_cels_command_103;
extern FrontEndCommandHeader *front_end_command_sequence_077[4];
extern FrontEndCelDescriptor front_end_cel_group_124[2];
extern FrontEndShowCelsCommand front_end_show_cels_command_104;
extern FrontEndCommandHeader *front_end_command_sequence_078[4];
extern FrontEndCelDescriptor front_end_cel_group_125[2];
extern FrontEndShowCelsCommand front_end_show_cels_command_105;
extern FrontEndCommandHeader *front_end_command_sequence_079[4];
extern FrontEndCelDescriptor front_end_cel_group_126[1];
extern FrontEndShowCelsCommand front_end_show_cels_command_106;
extern FrontEndCallbackCommand front_end_callback_command_029;
extern FrontEndCommandHeader *front_end_command_sequence_080[4];
extern FrontEndCelDescriptor front_end_cel_group_127[5];
extern FrontEndShowCelsCommand front_end_show_cels_command_107;
extern FrontEndCelDescriptor front_end_cel_group_128[6];
extern FrontEndShowCelsCommand front_end_show_cels_command_108;
extern FrontEndCommandHeader *front_end_command_sequence_081[5];
extern FrontEndCelDescriptor front_end_cel_group_129[5];
extern FrontEndShowCelsCommand front_end_show_cels_command_109;
extern FrontEndCelDescriptor front_end_cel_group_130[6];
extern FrontEndShowCelsCommand front_end_show_cels_command_110;
extern FrontEndCommandHeader *front_end_command_sequence_082[5];
extern FrontEndCelDescriptor front_end_cel_group_131[5];
extern FrontEndShowCelsCommand front_end_show_cels_command_111;
extern FrontEndCelDescriptor front_end_cel_group_132[6];
extern FrontEndShowCelsCommand front_end_show_cels_command_112;
extern FrontEndCommandHeader *front_end_command_sequence_083[5];
extern FrontEndCelDescriptor front_end_cel_group_133[5];
extern FrontEndShowCelsCommand front_end_show_cels_command_113;
extern FrontEndCelDescriptor front_end_cel_group_134[6];
extern FrontEndShowCelsCommand front_end_show_cels_command_114;
extern FrontEndCommandHeader *front_end_command_sequence_084[5];
extern FrontEndCallbackCommand front_end_callback_command_030;
extern FrontEndCommandHeader *front_end_command_sequence_085[3];
extern FrontEndCallbackCommand front_end_callback_command_031;
extern FrontEndCommandHeader *front_end_command_sequence_086[3];
extern FrontEndCallbackCommand front_end_callback_command_032;
extern FrontEndCommandHeader *front_end_command_sequence_087[3];
extern FrontEndCallbackCommand front_end_callback_command_033;
extern FrontEndCommandHeader *front_end_command_sequence_088[3];
extern FrontEndCallbackCommand front_end_callback_command_034;
extern FrontEndCallbackCommand front_end_callback_command_035;
extern FrontEndCallbackCommand front_end_callback_command_036;
extern FrontEndCallbackCommand front_end_callback_command_037;
extern FrontEndCallbackCommand front_end_callback_command_038;
extern FrontEndCallbackCommand front_end_callback_command_039;
extern FrontEndCallbackCommand front_end_callback_command_040;
extern FrontEndCallbackCommand front_end_callback_command_041;
extern FrontEndCallbackCommand front_end_callback_command_042;
extern FrontEndCallbackCommand front_end_callback_command_043;
extern FrontEndCallbackCommand front_end_callback_command_044;
extern FrontEndCelDescriptor front_end_cel_group_135[6];
extern FrontEndShowCelsCommand front_end_show_cels_command_115;
extern FrontEndCelDescriptor front_end_cel_group_136[10];
extern FrontEndShowCelsCommand front_end_show_cels_command_116;
extern FrontEndCommandHeader *front_end_command_sequence_089[7];
extern FrontEndCelDescriptor front_end_cel_group_137[6];
extern FrontEndShowCelsCommand front_end_show_cels_command_117;
extern FrontEndCelDescriptor front_end_cel_group_138[10];
extern FrontEndShowCelsCommand front_end_show_cels_command_118;
extern FrontEndCommandHeader *front_end_command_sequence_090[7];
extern FrontEndCelDescriptor front_end_cel_group_139[6];
extern FrontEndShowCelsCommand front_end_show_cels_command_119;
extern FrontEndCelDescriptor front_end_cel_group_140[10];
extern FrontEndShowCelsCommand front_end_show_cels_command_120;
extern FrontEndCommandHeader *front_end_command_sequence_091[7];
extern FrontEndCelDescriptor front_end_cel_group_141[6];
extern FrontEndShowCelsCommand front_end_show_cels_command_121;
extern FrontEndCelDescriptor front_end_cel_group_142[10];
extern FrontEndShowCelsCommand front_end_show_cels_command_122;
extern FrontEndCommandHeader *front_end_command_sequence_092[7];
extern FrontEndCelDescriptor front_end_cel_group_143[6];
extern FrontEndShowCelsCommand front_end_show_cels_command_123;
extern FrontEndCelDescriptor front_end_cel_group_144[10];
extern FrontEndShowCelsCommand front_end_show_cels_command_124;
extern FrontEndCommandHeader *front_end_command_sequence_093[7];
extern FrontEndCelDescriptor front_end_cel_group_145[6];
extern FrontEndShowCelsCommand front_end_show_cels_command_125;
extern FrontEndCelDescriptor front_end_cel_group_146[7];
extern FrontEndShowCelsCommand front_end_show_cels_command_126;
extern FrontEndCommandHeader *front_end_command_sequence_094[6];
extern FrontEndCelDescriptor front_end_cel_group_147[3];
extern FrontEndPlayStreamCommand front_end_play_stream_command_000;
extern FrontEndShowCelsCommand front_end_show_cels_command_127;
extern FrontEndCommandHeader *front_end_command_sequence_095[4];
extern FrontEndCelDescriptor front_end_cel_group_148[8];
extern FrontEndShowCelsCommand front_end_show_cels_command_128;
extern FrontEndCommandHeader *front_end_command_sequence_096[5];
extern FrontEndCelDescriptor front_end_cel_group_149[8];
extern FrontEndShowCelsCommand front_end_show_cels_command_129;
extern FrontEndCommandHeader *front_end_command_sequence_097[5];
extern FrontEndCelDescriptor front_end_cel_group_150[9];
extern FrontEndShowCelsCommand front_end_show_cels_command_130;
extern FrontEndCommandHeader *front_end_command_sequence_098[3];
extern FrontEndCelDescriptor front_end_cel_group_151[9];
extern FrontEndShowCelsCommand front_end_show_cels_command_131;
extern FrontEndCommandHeader *front_end_command_sequence_099[3];
extern FrontEndCelDescriptor front_end_cel_group_152[9];
extern FrontEndShowCelsCommand front_end_show_cels_command_132;
extern FrontEndCommandHeader *front_end_command_sequence_100[3];
extern FrontEndCelDescriptor front_end_cel_group_153[3];
extern FrontEndPlayStreamCommand front_end_play_stream_command_001;
extern FrontEndShowCelsCommand front_end_show_cels_command_133;
extern FrontEndCommandHeader *front_end_command_sequence_101[4];
extern FrontEndCelDescriptor front_end_cel_group_154[8];
extern FrontEndShowCelsCommand front_end_show_cels_command_134;
extern FrontEndCommandHeader *front_end_command_sequence_102[5];
extern FrontEndCelDescriptor front_end_cel_group_155[8];
extern FrontEndShowCelsCommand front_end_show_cels_command_135;
extern FrontEndCommandHeader *front_end_command_sequence_103[5];
extern FrontEndCelDescriptor front_end_cel_group_156[9];
extern FrontEndShowCelsCommand front_end_show_cels_command_136;
extern FrontEndCommandHeader *front_end_command_sequence_104[3];
extern FrontEndCelDescriptor front_end_cel_group_157[9];
extern FrontEndShowCelsCommand front_end_show_cels_command_137;
extern FrontEndCommandHeader *front_end_command_sequence_105[3];
extern FrontEndCelDescriptor front_end_cel_group_158[9];
extern FrontEndShowCelsCommand front_end_show_cels_command_138;
extern FrontEndCommandHeader *front_end_command_sequence_106[3];
extern FrontEndCelDescriptor front_end_cel_group_159[3];
extern FrontEndPlayStreamCommand front_end_play_stream_command_002;
extern FrontEndShowCelsCommand front_end_show_cels_command_139;
extern FrontEndCommandHeader *front_end_command_sequence_107[4];
extern FrontEndCelDescriptor front_end_cel_group_160[8];
extern FrontEndShowCelsCommand front_end_show_cels_command_140;
extern FrontEndCommandHeader *front_end_command_sequence_108[5];
extern FrontEndCelDescriptor front_end_cel_group_161[8];
extern FrontEndShowCelsCommand front_end_show_cels_command_141;
extern FrontEndCommandHeader *front_end_command_sequence_109[5];
extern FrontEndCelDescriptor front_end_cel_group_162[9];
extern FrontEndShowCelsCommand front_end_show_cels_command_142;
extern FrontEndCommandHeader *front_end_command_sequence_110[3];
extern FrontEndCelDescriptor front_end_cel_group_163[9];
extern FrontEndShowCelsCommand front_end_show_cels_command_143;
extern FrontEndCommandHeader *front_end_command_sequence_111[3];
extern FrontEndCelDescriptor front_end_cel_group_164[9];
extern FrontEndShowCelsCommand front_end_show_cels_command_144;
extern FrontEndCommandHeader *front_end_command_sequence_112[3];
extern FrontEndCelDescriptor front_end_cel_group_165[3];
extern FrontEndPlayStreamCommand front_end_play_stream_command_003;
extern FrontEndShowCelsCommand front_end_show_cels_command_145;
extern FrontEndCommandHeader *front_end_command_sequence_113[4];
extern FrontEndCelDescriptor front_end_cel_group_166[8];
extern FrontEndShowCelsCommand front_end_show_cels_command_146;
extern FrontEndCommandHeader *front_end_command_sequence_114[5];
extern FrontEndCelDescriptor front_end_cel_group_167[8];
extern FrontEndShowCelsCommand front_end_show_cels_command_147;
extern FrontEndCommandHeader *front_end_command_sequence_115[5];
extern FrontEndCelDescriptor front_end_cel_group_168[9];
extern FrontEndShowCelsCommand front_end_show_cels_command_148;
extern FrontEndCommandHeader *front_end_command_sequence_116[3];
extern FrontEndCelDescriptor front_end_cel_group_169[9];
extern FrontEndShowCelsCommand front_end_show_cels_command_149;
extern FrontEndCommandHeader *front_end_command_sequence_117[3];
extern FrontEndCelDescriptor front_end_cel_group_170[9];
extern FrontEndShowCelsCommand front_end_show_cels_command_150;
extern FrontEndCommandHeader *front_end_command_sequence_118[3];
extern FrontEndCelDescriptor front_end_cel_group_171[3];
extern FrontEndPlayStreamCommand front_end_play_stream_command_004;
extern FrontEndShowCelsCommand front_end_show_cels_command_151;
extern FrontEndCommandHeader *front_end_command_sequence_119[4];
extern FrontEndCelDescriptor front_end_cel_group_172[8];
extern FrontEndShowCelsCommand front_end_show_cels_command_152;
extern FrontEndCommandHeader *front_end_command_sequence_120[5];
extern FrontEndCelDescriptor front_end_cel_group_173[8];
extern FrontEndShowCelsCommand front_end_show_cels_command_153;
extern FrontEndCommandHeader *front_end_command_sequence_121[5];
extern FrontEndCelDescriptor front_end_cel_group_174[9];
extern FrontEndShowCelsCommand front_end_show_cels_command_154;
extern FrontEndCommandHeader *front_end_command_sequence_122[3];
extern FrontEndCelDescriptor front_end_cel_group_175[9];
extern FrontEndShowCelsCommand front_end_show_cels_command_155;
extern FrontEndCommandHeader *front_end_command_sequence_123[3];
extern FrontEndCelDescriptor front_end_cel_group_176[9];
extern FrontEndShowCelsCommand front_end_show_cels_command_156;
extern FrontEndCommandHeader *front_end_command_sequence_124[3];
extern FrontEndCallbackCommand front_end_callback_command_045;
extern FrontEndCallbackCommand front_end_callback_command_046;
extern FrontEndCallbackCommand front_end_callback_command_047;
extern FrontEndCallbackCommand front_end_callback_command_048;
extern FrontEndCallbackCommand front_end_callback_command_049;
extern FrontEndCallbackCommand front_end_callback_command_050;
extern FrontEndCallbackCommand front_end_callback_command_051;
extern FrontEndCallbackCommand front_end_callback_command_052;
extern FrontEndCallbackCommand front_end_callback_command_053;
extern FrontEndCallbackCommand front_end_callback_command_054;
extern FrontEndCelDescriptor front_end_cel_group_177[6];
extern FrontEndShowCelsCommand front_end_show_cels_command_157;
extern FrontEndCelDescriptor front_end_cel_group_178[10];
extern FrontEndShowCelsCommand front_end_show_cels_command_158;
extern FrontEndCommandHeader *front_end_command_sequence_125[7];
extern FrontEndCelDescriptor front_end_cel_group_179[6];
extern FrontEndShowCelsCommand front_end_show_cels_command_159;
extern FrontEndCelDescriptor front_end_cel_group_180[10];
extern FrontEndShowCelsCommand front_end_show_cels_command_160;
extern FrontEndCommandHeader *front_end_command_sequence_126[7];
extern FrontEndCelDescriptor front_end_cel_group_181[6];
extern FrontEndShowCelsCommand front_end_show_cels_command_161;
extern FrontEndCelDescriptor front_end_cel_group_182[10];
extern FrontEndShowCelsCommand front_end_show_cels_command_162;
extern FrontEndCommandHeader *front_end_command_sequence_127[7];
extern FrontEndCelDescriptor front_end_cel_group_183[6];
extern FrontEndShowCelsCommand front_end_show_cels_command_163;
extern FrontEndCelDescriptor front_end_cel_group_184[10];
extern FrontEndShowCelsCommand front_end_show_cels_command_164;
extern FrontEndCommandHeader *front_end_command_sequence_128[7];
extern FrontEndCelDescriptor front_end_cel_group_185[6];
extern FrontEndShowCelsCommand front_end_show_cels_command_165;
extern FrontEndCelDescriptor front_end_cel_group_186[10];
extern FrontEndShowCelsCommand front_end_show_cels_command_166;
extern FrontEndCommandHeader *front_end_command_sequence_129[7];
extern FrontEndCelDescriptor front_end_cel_group_187[6];
extern FrontEndShowCelsCommand front_end_show_cels_command_167;
extern FrontEndCelDescriptor front_end_cel_group_188[7];
extern FrontEndShowCelsCommand front_end_show_cels_command_168;
extern FrontEndCommandHeader *front_end_command_sequence_130[6];
extern FrontEndCelDescriptor front_end_cel_group_189[3];
extern FrontEndPlayStreamCommand front_end_play_stream_command_005;
extern FrontEndShowCelsCommand front_end_show_cels_command_169;
extern FrontEndCommandHeader *front_end_command_sequence_131[4];
extern FrontEndCelDescriptor front_end_cel_group_190[8];
extern FrontEndShowCelsCommand front_end_show_cels_command_170;
extern FrontEndCommandHeader *front_end_command_sequence_132[5];
extern FrontEndCelDescriptor front_end_cel_group_191[8];
extern FrontEndShowCelsCommand front_end_show_cels_command_171;
extern FrontEndCommandHeader *front_end_command_sequence_133[5];
extern FrontEndCelDescriptor front_end_cel_group_192[9];
extern FrontEndShowCelsCommand front_end_show_cels_command_172;
extern FrontEndCommandHeader *front_end_command_sequence_134[3];
extern FrontEndCelDescriptor front_end_cel_group_193[9];
extern FrontEndShowCelsCommand front_end_show_cels_command_173;
extern FrontEndCommandHeader *front_end_command_sequence_135[3];
extern FrontEndCelDescriptor front_end_cel_group_194[9];
extern FrontEndShowCelsCommand front_end_show_cels_command_174;
extern FrontEndCommandHeader *front_end_command_sequence_136[3];
extern FrontEndCelDescriptor front_end_cel_group_195[3];
extern FrontEndPlayStreamCommand front_end_play_stream_command_006;
extern FrontEndShowCelsCommand front_end_show_cels_command_175;
extern FrontEndCommandHeader *front_end_command_sequence_137[4];
extern FrontEndCelDescriptor front_end_cel_group_196[8];
extern FrontEndShowCelsCommand front_end_show_cels_command_176;
extern FrontEndCommandHeader *front_end_command_sequence_138[5];
extern FrontEndCelDescriptor front_end_cel_group_197[8];
extern FrontEndShowCelsCommand front_end_show_cels_command_177;
extern FrontEndCommandHeader *front_end_command_sequence_139[5];
extern FrontEndCelDescriptor front_end_cel_group_198[9];
extern FrontEndShowCelsCommand front_end_show_cels_command_178;
extern FrontEndCommandHeader *front_end_command_sequence_140[3];
extern FrontEndCelDescriptor front_end_cel_group_199[9];
extern FrontEndShowCelsCommand front_end_show_cels_command_179;
extern FrontEndCommandHeader *front_end_command_sequence_141[3];
extern FrontEndCelDescriptor front_end_cel_group_200[9];
extern FrontEndShowCelsCommand front_end_show_cels_command_180;
extern FrontEndCommandHeader *front_end_command_sequence_142[3];
extern FrontEndCelDescriptor front_end_cel_group_201[3];
extern FrontEndPlayStreamCommand front_end_play_stream_command_007;
extern FrontEndShowCelsCommand front_end_show_cels_command_181;
extern FrontEndCommandHeader *front_end_command_sequence_143[4];
extern FrontEndCelDescriptor front_end_cel_group_202[8];
extern FrontEndShowCelsCommand front_end_show_cels_command_182;
extern FrontEndCommandHeader *front_end_command_sequence_144[5];
extern FrontEndCelDescriptor front_end_cel_group_203[8];
extern FrontEndShowCelsCommand front_end_show_cels_command_183;
extern FrontEndCommandHeader *front_end_command_sequence_145[5];
extern FrontEndCelDescriptor front_end_cel_group_204[9];
extern FrontEndShowCelsCommand front_end_show_cels_command_184;
extern FrontEndCommandHeader *front_end_command_sequence_146[3];
extern FrontEndCelDescriptor front_end_cel_group_205[9];
extern FrontEndShowCelsCommand front_end_show_cels_command_185;
extern FrontEndCommandHeader *front_end_command_sequence_147[3];
extern FrontEndCelDescriptor front_end_cel_group_206[9];
extern FrontEndShowCelsCommand front_end_show_cels_command_186;
extern FrontEndCommandHeader *front_end_command_sequence_148[3];
extern FrontEndCelDescriptor front_end_cel_group_207[3];
extern FrontEndPlayStreamCommand front_end_play_stream_command_008;
extern FrontEndShowCelsCommand front_end_show_cels_command_187;
extern FrontEndCommandHeader *front_end_command_sequence_149[4];
extern FrontEndCelDescriptor front_end_cel_group_208[8];
extern FrontEndShowCelsCommand front_end_show_cels_command_188;
extern FrontEndCommandHeader *front_end_command_sequence_150[5];
extern FrontEndCelDescriptor front_end_cel_group_209[8];
extern FrontEndShowCelsCommand front_end_show_cels_command_189;
extern FrontEndCommandHeader *front_end_command_sequence_151[5];
extern FrontEndCelDescriptor front_end_cel_group_210[9];
extern FrontEndShowCelsCommand front_end_show_cels_command_190;
extern FrontEndCommandHeader *front_end_command_sequence_152[3];
extern FrontEndCelDescriptor front_end_cel_group_211[9];
extern FrontEndShowCelsCommand front_end_show_cels_command_191;
extern FrontEndCommandHeader *front_end_command_sequence_153[3];
extern FrontEndCelDescriptor front_end_cel_group_212[9];
extern FrontEndShowCelsCommand front_end_show_cels_command_192;
extern FrontEndCommandHeader *front_end_command_sequence_154[3];
extern FrontEndCelDescriptor front_end_cel_group_213[3];
extern FrontEndPlayStreamCommand front_end_play_stream_command_009;
extern FrontEndShowCelsCommand front_end_show_cels_command_193;
extern FrontEndCommandHeader *front_end_command_sequence_155[4];
extern FrontEndCelDescriptor front_end_cel_group_214[8];
extern FrontEndShowCelsCommand front_end_show_cels_command_194;
extern FrontEndCommandHeader *front_end_command_sequence_156[5];
extern FrontEndCelDescriptor front_end_cel_group_215[8];
extern FrontEndShowCelsCommand front_end_show_cels_command_195;
extern FrontEndCommandHeader *front_end_command_sequence_157[5];
extern FrontEndCelDescriptor front_end_cel_group_216[9];
extern FrontEndShowCelsCommand front_end_show_cels_command_196;
extern FrontEndCommandHeader *front_end_command_sequence_158[3];
extern FrontEndCelDescriptor front_end_cel_group_217[9];
extern FrontEndShowCelsCommand front_end_show_cels_command_197;
extern FrontEndCommandHeader *front_end_command_sequence_159[3];
extern FrontEndCelDescriptor front_end_cel_group_218[9];
extern FrontEndShowCelsCommand front_end_show_cels_command_198;
extern FrontEndCommandHeader *front_end_command_sequence_160[3];
extern FrontEndCallbackCommand front_end_callback_command_055;
extern FrontEndCallbackCommand front_end_callback_command_056;
extern FrontEndCallbackCommand front_end_callback_command_057;
extern FrontEndCallbackCommand front_end_callback_command_058;
extern FrontEndCallbackCommand front_end_callback_command_059;
extern FrontEndCallbackCommand front_end_callback_command_060;
extern FrontEndCallbackCommand front_end_callback_command_061;
extern FrontEndCallbackCommand front_end_callback_command_062;
extern FrontEndCallbackCommand front_end_callback_command_063;
extern FrontEndCallbackCommand front_end_callback_command_064;
extern FrontEndCelDescriptor front_end_cel_group_219[6];
extern FrontEndShowCelsCommand front_end_show_cels_command_199;
extern FrontEndCelDescriptor front_end_cel_group_220[10];
extern FrontEndShowCelsCommand front_end_show_cels_command_200;
extern FrontEndCommandHeader *front_end_command_sequence_161[7];
extern FrontEndCelDescriptor front_end_cel_group_221[6];
extern FrontEndShowCelsCommand front_end_show_cels_command_201;
extern FrontEndCelDescriptor front_end_cel_group_222[10];
extern FrontEndShowCelsCommand front_end_show_cels_command_202;
extern FrontEndCommandHeader *front_end_command_sequence_162[7];
extern FrontEndCelDescriptor front_end_cel_group_223[6];
extern FrontEndShowCelsCommand front_end_show_cels_command_203;
extern FrontEndCelDescriptor front_end_cel_group_224[10];
extern FrontEndShowCelsCommand front_end_show_cels_command_204;
extern FrontEndCommandHeader *front_end_command_sequence_163[7];
extern FrontEndCelDescriptor front_end_cel_group_225[6];
extern FrontEndShowCelsCommand front_end_show_cels_command_205;
extern FrontEndCelDescriptor front_end_cel_group_226[10];
extern FrontEndShowCelsCommand front_end_show_cels_command_206;
extern FrontEndCommandHeader *front_end_command_sequence_164[7];
extern FrontEndCelDescriptor front_end_cel_group_227[6];
extern FrontEndShowCelsCommand front_end_show_cels_command_207;
extern FrontEndCelDescriptor front_end_cel_group_228[10];
extern FrontEndShowCelsCommand front_end_show_cels_command_208;
extern FrontEndCommandHeader *front_end_command_sequence_165[7];
extern FrontEndCelDescriptor front_end_cel_group_229[6];
extern FrontEndShowCelsCommand front_end_show_cels_command_209;
extern FrontEndCelDescriptor front_end_cel_group_230[7];
extern FrontEndShowCelsCommand front_end_show_cels_command_210;
extern FrontEndCommandHeader *front_end_command_sequence_166[6];
extern FrontEndCelDescriptor front_end_cel_group_231[3];
extern FrontEndPlayStreamCommand front_end_play_stream_command_010;
extern FrontEndShowCelsCommand front_end_show_cels_command_211;
extern FrontEndCommandHeader *front_end_command_sequence_167[4];
extern FrontEndCelDescriptor front_end_cel_group_232[8];
extern FrontEndShowCelsCommand front_end_show_cels_command_212;
extern FrontEndCommandHeader *front_end_command_sequence_168[5];
extern FrontEndCelDescriptor front_end_cel_group_233[8];
extern FrontEndShowCelsCommand front_end_show_cels_command_213;
extern FrontEndCommandHeader *front_end_command_sequence_169[5];
extern FrontEndCelDescriptor front_end_cel_group_234[9];
extern FrontEndShowCelsCommand front_end_show_cels_command_214;
extern FrontEndCommandHeader *front_end_command_sequence_170[3];
extern FrontEndCelDescriptor front_end_cel_group_235[9];
extern FrontEndShowCelsCommand front_end_show_cels_command_215;
extern FrontEndCommandHeader *front_end_command_sequence_171[3];
extern FrontEndCelDescriptor front_end_cel_group_236[9];
extern FrontEndShowCelsCommand front_end_show_cels_command_216;
extern FrontEndCommandHeader *front_end_command_sequence_172[3];
extern FrontEndCelDescriptor front_end_cel_group_237[3];
extern FrontEndPlayStreamCommand front_end_play_stream_command_011;
extern FrontEndShowCelsCommand front_end_show_cels_command_217;
extern FrontEndCommandHeader *front_end_command_sequence_173[4];
extern FrontEndCelDescriptor front_end_cel_group_238[8];
extern FrontEndShowCelsCommand front_end_show_cels_command_218;
extern FrontEndCommandHeader *front_end_command_sequence_174[5];
extern FrontEndCelDescriptor front_end_cel_group_239[8];
extern FrontEndShowCelsCommand front_end_show_cels_command_219;
extern FrontEndCommandHeader *front_end_command_sequence_175[5];
extern FrontEndCelDescriptor front_end_cel_group_240[9];
extern FrontEndShowCelsCommand front_end_show_cels_command_220;
extern FrontEndCommandHeader *front_end_command_sequence_176[3];
extern FrontEndCelDescriptor front_end_cel_group_241[9];
extern FrontEndShowCelsCommand front_end_show_cels_command_221;
extern FrontEndCommandHeader *front_end_command_sequence_177[3];
extern FrontEndCelDescriptor front_end_cel_group_242[9];
extern FrontEndShowCelsCommand front_end_show_cels_command_222;
extern FrontEndCommandHeader *front_end_command_sequence_178[3];
extern FrontEndCelDescriptor front_end_cel_group_243[3];
extern FrontEndPlayStreamCommand front_end_play_stream_command_012;
extern FrontEndShowCelsCommand front_end_show_cels_command_223;
extern FrontEndCommandHeader *front_end_command_sequence_179[4];
extern FrontEndCelDescriptor front_end_cel_group_244[8];
extern FrontEndShowCelsCommand front_end_show_cels_command_224;
extern FrontEndCommandHeader *front_end_command_sequence_180[5];
extern FrontEndCelDescriptor front_end_cel_group_245[8];
extern FrontEndShowCelsCommand front_end_show_cels_command_225;
extern FrontEndCommandHeader *front_end_command_sequence_181[5];
extern FrontEndCelDescriptor front_end_cel_group_246[9];
extern FrontEndShowCelsCommand front_end_show_cels_command_226;
extern FrontEndCommandHeader *front_end_command_sequence_182[3];
extern FrontEndCelDescriptor front_end_cel_group_247[9];
extern FrontEndShowCelsCommand front_end_show_cels_command_227;
extern FrontEndCommandHeader *front_end_command_sequence_183[3];
extern FrontEndCelDescriptor front_end_cel_group_248[9];
extern FrontEndShowCelsCommand front_end_show_cels_command_228;
extern FrontEndCommandHeader *front_end_command_sequence_184[3];
extern FrontEndCelDescriptor front_end_cel_group_249[3];
extern FrontEndPlayStreamCommand front_end_play_stream_command_013;
extern FrontEndShowCelsCommand front_end_show_cels_command_229;
extern FrontEndCommandHeader *front_end_command_sequence_185[4];
extern FrontEndCelDescriptor front_end_cel_group_250[8];
extern FrontEndShowCelsCommand front_end_show_cels_command_230;
extern FrontEndCommandHeader *front_end_command_sequence_186[5];
extern FrontEndCelDescriptor front_end_cel_group_251[8];
extern FrontEndShowCelsCommand front_end_show_cels_command_231;
extern FrontEndCommandHeader *front_end_command_sequence_187[5];
extern FrontEndCelDescriptor front_end_cel_group_252[9];
extern FrontEndShowCelsCommand front_end_show_cels_command_232;
extern FrontEndCommandHeader *front_end_command_sequence_188[3];
extern FrontEndCelDescriptor front_end_cel_group_253[9];
extern FrontEndShowCelsCommand front_end_show_cels_command_233;
extern FrontEndCommandHeader *front_end_command_sequence_189[3];
extern FrontEndCelDescriptor front_end_cel_group_254[9];
extern FrontEndShowCelsCommand front_end_show_cels_command_234;
extern FrontEndCommandHeader *front_end_command_sequence_190[3];
extern FrontEndCelDescriptor front_end_cel_group_255[3];
extern FrontEndPlayStreamCommand front_end_play_stream_command_014;
extern FrontEndShowCelsCommand front_end_show_cels_command_235;
extern FrontEndCommandHeader *front_end_command_sequence_191[4];
extern FrontEndCelDescriptor front_end_cel_group_256[8];
extern FrontEndShowCelsCommand front_end_show_cels_command_236;
extern FrontEndCommandHeader *front_end_command_sequence_192[5];
extern FrontEndCelDescriptor front_end_cel_group_257[8];
extern FrontEndShowCelsCommand front_end_show_cels_command_237;
extern FrontEndCommandHeader *front_end_command_sequence_193[5];
extern FrontEndCelDescriptor front_end_cel_group_258[9];
extern FrontEndShowCelsCommand front_end_show_cels_command_238;
extern FrontEndCommandHeader *front_end_command_sequence_194[3];
extern FrontEndCelDescriptor front_end_cel_group_259[9];
extern FrontEndShowCelsCommand front_end_show_cels_command_239;
extern FrontEndCommandHeader *front_end_command_sequence_195[3];
extern FrontEndCelDescriptor front_end_cel_group_260[9];
extern FrontEndShowCelsCommand front_end_show_cels_command_240;
extern FrontEndCommandHeader *front_end_command_sequence_196[3];
extern FrontEndCelDescriptor front_end_cel_group_261[2];
extern FrontEndCelDescriptor front_end_cel_group_262[2];
extern FrontEndCelDescriptor front_end_cel_group_263[2];
extern FrontEndCelDescriptor front_end_cel_group_264[2];
extern FrontEndCelDescriptor front_end_cel_group_265[2];
extern FrontEndCelDescriptor front_end_cel_group_266[2];
extern FrontEndCelDescriptor front_end_cel_group_267[2];
extern FrontEndCelDescriptor front_end_cel_group_268[2];
extern FrontEndCelDescriptor front_end_cel_group_269[5];
extern FrontEndShowCelsCommand front_end_show_cels_command_241;
extern FrontEndCelDescriptor front_end_cel_group_270[8];
extern FrontEndShowCelsCommand front_end_show_cels_command_242;
extern FrontEndCallbackCommand front_end_callback_command_065;
extern FrontEndCommandHeader *front_end_command_sequence_197[6];
extern FrontEndCelDescriptor front_end_cel_group_271[5];
extern FrontEndShowCelsCommand front_end_show_cels_command_243;
extern FrontEndCelDescriptor front_end_cel_group_272[8];
extern FrontEndShowCelsCommand front_end_show_cels_command_244;
extern FrontEndCallbackCommand front_end_callback_command_066;
extern FrontEndCommandHeader *front_end_command_sequence_198[6];
extern FrontEndCelDescriptor front_end_cel_group_273[5];
extern FrontEndShowCelsCommand front_end_show_cels_command_245;
extern FrontEndCelDescriptor front_end_cel_group_274[7];
extern FrontEndShowCelsCommand front_end_show_cels_command_246;
extern FrontEndCommandHeader *front_end_command_sequence_199[6];
extern FrontEndCelDescriptor front_end_cel_group_275[5];
extern FrontEndShowCelsCommand front_end_show_cels_command_247;
extern FrontEndCelDescriptor front_end_cel_group_276[7];
extern FrontEndShowCelsCommand front_end_show_cels_command_248;
extern FrontEndCommandHeader *front_end_command_sequence_200[6];
extern FrontEndCelDescriptor front_end_cel_group_277[5];
extern FrontEndShowCelsCommand front_end_show_cels_command_249;
extern FrontEndCelDescriptor front_end_cel_group_278[7];
extern FrontEndShowCelsCommand front_end_show_cels_command_250;
extern FrontEndCommandHeader *front_end_command_sequence_201[6];
extern FrontEndCelDescriptor front_end_cel_group_279[5];
extern FrontEndShowCelsCommand front_end_show_cels_command_251;
extern FrontEndCelDescriptor front_end_cel_group_280[7];
extern FrontEndShowCelsCommand front_end_show_cels_command_252;
extern FrontEndCommandHeader *front_end_command_sequence_202[6];
extern FrontEndCallbackCommand front_end_callback_command_067;
extern FrontEndCelDescriptor front_end_cel_group_281[4];
extern FrontEndCelDescriptor front_end_cel_group_282[4];
extern FrontEndCelDescriptor front_end_cel_group_283[5];
extern FrontEndCelDescriptor front_end_cel_group_284[4];
extern FrontEndCelDescriptor front_end_cel_group_285[4];
extern FrontEndCallbackCommand front_end_callback_command_068;
extern FrontEndCommandHeader *front_end_command_sequence_203[4];
extern FrontEndCelDescriptor front_end_cel_group_286[5];
extern FrontEndCelDescriptor front_end_cel_group_287[2];
extern FrontEndCelDescriptor front_end_cel_group_288[5];
extern FrontEndCelDescriptor front_end_cel_group_289[4];
extern FrontEndCelDescriptor front_end_cel_group_290[4];
extern FrontEndCelDescriptor front_end_cel_group_291[4];
extern FrontEndCelDescriptor front_end_cel_group_292[4];
extern FrontEndCelDescriptor front_end_cel_group_293[4];
extern FrontEndCelDescriptor front_end_cel_group_294[4];
extern FrontEndCallbackCommand front_end_callback_command_069;
extern FrontEndCallbackCommand front_end_callback_command_070;
extern FrontEndCommandHeader *front_end_command_sequence_204[4];
extern FrontEndCelDescriptor front_end_cel_group_295[4];
extern FrontEndCelDescriptor front_end_cel_group_296[4];
extern FrontEndCelDescriptor front_end_cel_group_297[4];
extern FrontEndCelDescriptor front_end_cel_group_298[4];
extern FrontEndCelDescriptor front_end_cel_group_299[2];
extern FrontEndCallbackCommand front_end_callback_command_071;
extern FrontEndCommandHeader *front_end_command_sequence_205[3];
extern FrontEndCelDescriptor front_end_cel_group_300[2];
extern FrontEndCelDescriptor front_end_cel_group_301[2];
extern FrontEndCelDescriptor front_end_cel_group_302[1];
extern FrontEndAction front_end_actions[343];
extern FrontEndMenu front_end_menus[154];
extern FrontEndAssetState front_end_asset_state;
extern RaceModeState race_mode_state;
extern RiderProfile player_profiles[PLAYER_PROFILE_COUNT];
extern int profile_slot_rider_types[PROFILE_SLOT_COUNT];
extern RaceResultEntry race_results[4];
extern int course_distance_tenths[5][5];
extern int rider_style_purchase_prices[15];
extern int rider_style_resale_values[15];
extern int rider_starting_balances[10];
extern int race_cash_awards[15];
extern const char *front_end_stream_paths[43];
extern FrontEndPreviewState front_end_preview_state;
extern signed char opponent_choice_style_map[4];
extern int opponent_portrait_resource_ids[8][3];
extern int opponent_bike_resource_ids[8][3];
extern unsigned char random_preview_active[4];
extern FrontEndCelDescriptor profile_load_descriptor;
extern FrontEndCelDescriptor profile_save_descriptor;

extern TrackEffectPools track_effect_pools;
extern CCB *track_effect_resources[TRACK_EFFECT_RESOURCE_COUNT];
extern int track_effect_type_masks[TRACK_EFFECT_TYPE_COUNT];
extern TrackEffectCreationDescriptor track_effect_parent_descriptor;
extern TrackEffectCreationDescriptor track_effect_child_descriptor;
extern RaceInputLatchState race_input_latch_state;
extern GameAudioState game_audio_state;
extern unsigned char collision_positions_adjusted[4];
extern RaceObjectSchedulerState race_object_scheduler;
extern TrackVisibilityScanState track_visibility_scan;
extern RiderPhysicsTuning rider_physics_tuning;
extern RoadRendererState road_renderer_state;
extern int road_depth_input_distances[ROAD_DEPTH_INPUT_COUNT];
extern unsigned int cel_pixel_shifts[CEL_PIXEL_FORMAT_COUNT];
extern unsigned int cel_pixel_masks[CEL_PIXEL_FORMAT_COUNT];
extern signed char road_draw_order[ROAD_DRAW_ORDER_COUNT]
[ROAD_DRAW_COMMAND_COUNT];
extern AnimationResourcePair primary_animation_resource_pairs[2];
extern AnimationResourcePair secondary_animation_resource_pairs[2];
extern signed char animation_resource_pair_by_variant[4];
extern void *rider_animation_state_targets[RACE_RIDER_COUNT];
extern AnimationBindingSpec
  rider_animation_channels[ANIMATION_CHANNEL_SPEC_COUNT];
extern CelAnimationBinding
  rider_one_animation_bindings[RIDER_ONE_ANIMATION_BINDING_COUNT];
extern CelAnimationBinding *rider_one_animation_by_state
[RIDER_ONE_ANIMATION_STATE_COUNT];
extern CelAnimationBinding *rider_one_animation_state_sentinel;
extern CelAnimationBinding
  rider_two_animation_bindings[RIDER_TWO_ANIMATION_BINDING_COUNT];
extern CelAnimationBinding *rider_two_animation_by_state
[RIDER_TWO_ANIMATION_STATE_COUNT];
extern CelAnimationBinding **rider_animation_state_tables[3];
extern CarAnimationLoadState car_animation_load_state;
extern char car_resource_suffix[12];
extern int car_resource_suffix_alignment;
extern AnimationBindingSpec
  car_animation_selectors[CAR_ANIMATION_VARIANT_COUNT];
extern CarAnimationResourcePair
  car_animation_resources[CAR_ANIMATION_COUNT];
extern int car_animation_output_info[CAR_ANIMATION_COUNT];
extern CelAnimationBinding car_animation_bindings[CAR_ANIMATION_COUNT]
[CAR_ANIMATION_VARIANT_COUNT];
extern CelAnimationBinding *car_animation_variant_bindings
[CAR_ANIMATION_COUNT][CAR_ANIMATION_VARIANT_COUNT];
extern CelAnimationBinding
**car_animation_bindings_by_car[CAR_ANIMATION_COUNT];
extern SegmentRepairGlobals segment_repair_globals;
extern FamilyResourceGlobals family_resource_globals;
extern TrackSimulationGlobals track_simulation_globals;
extern signed char track_direction_modes[8];
extern RacerCreationTemplate track_object_template;
extern TrackObjectModeTemplate track_object_mode_templates[4];
extern DisplayRuntimeGlobals display_runtime_globals;
extern TrackRuntimeGlobals track_runtime_globals;
extern ResourceCatalogHandles resource_catalog_handles;
extern char resource_root_path[12];
extern char course_variant_names[COURSE_VARIANT_COUNT][10];
extern char course_resource_suffix[12];
extern ResourceCatalogEntry course_catalog[COURSE_CATALOG_COUNT];
extern ResourceCatalogEntry rash_options_catalog[RASH_OPTIONS_CATALOG_COUNT];
extern RuntimeFeatureFlags runtime_feature_flags;
extern unsigned int frame_tick;
extern int frame_time_scale;
extern BigEndianByteFlag frame_timer_enabled;
extern ResourceInfoState resource_info_state;
extern struct IntrusiveListStorage *rider_visual_pool;
extern RacerCreationTemplate rider_visual_template;
extern AnimationBindingSpec rider_motion_frames[6];
extern AnimationBindingSpec rider_standing_frames[1];
extern AnimationBindingSpec rider_shake_frames[2];
extern AnimationBindingSpec rider_fall_frames[6];
extern AnimationBindingSpec rider_attack_frames[2];
extern AnimationBindingSpec rider_fast_frames[6];
extern AnimationBindingSpec rider_flag_frames[6];
extern int rider_direction_vectors[4];
extern MoviePlaybackControl movie_playback_control;
extern TextRenderManager *text_manager;
extern unsigned int text_plut[8];
extern int text_render_workspace[8];
extern DisplaySystemGlobals display_system_globals;
extern unsigned int previous_control_pad_state[8];
extern ParticleResourceGlobals particle_resource_globals;
extern HudGlobals hud_globals;
extern unsigned char pixel_bit_shifts[8];
extern unsigned char pixel_repeat_counts[4];
extern RoadSurfaceRenderGlobals road_surface_render_globals;
extern int quad_join_offsets[16][2];
extern BigEndianByteFlag road_surface_render_enabled;
extern int road_surface_colors[2];
extern void *road_surface_dynamic_cels[2];
extern int road_surface_thresholds[3];
extern void *road_surface_cel_variants[3][3];
extern void *road_surface_palette_variants[3];
extern void *road_surface_cel_table_a[6];
extern void *road_surface_cel_table_b[6];
extern int stream_audio_reference_count;
extern AudioPlaybackState audio_playback_state;
extern AudioSourceDescriptor default_audio_source;
extern unsigned int stream_input_snapshots[2];
extern unsigned int stream_input_masks[2];
extern int async_stream_message_port;
extern unsigned int stream_frame_cel_terminator_flags;
extern int stream_runtime_workspace[16];
extern RwTagArgument stream_open_tags[16];
extern BackgroundAudioGlobals background_audio_globals;
extern RwTagArgument background_stream_tags[5];
extern RwTagArgument background_command_tags[2];
extern int background_stream_result;
extern int background_command_result;
extern MemoryAllocatorGlobals memory_allocator_globals;
extern ResourceManagerGlobals resource_manager_globals;
extern const char *mixer_gain_names[24];
extern AudioMixerTemplateDefaults audio_mixer_template_defaults;

#endif
