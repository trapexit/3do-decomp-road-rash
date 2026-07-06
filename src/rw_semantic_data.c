/*
 * Reconstructed initialized data with independently identified purpose.
 * The two mathematical tables are the exact Portfolio operamath tables used
 * by Atan2F16() and SinF32().
 */
#include "stddef.h"

#include "rw_semantic_data.h"

#include "audio_ro_data.h"
#include "front_end_callback_adapters.h"
#include "front_end_stream_paths.h"
#include "operamath.h"
#include "stream_audio_playback.h"
#include "track_effect_runtime.h"

#define SEMANTIC_SIZE_ASSERT(name, expression) \
        typedef char semantic_size_assert_ ## name[(expression) ? 1 : -1]

SEMANTIC_SIZE_ASSERT(race_boost_config, sizeof(RaceBoostConfig) == 0x1C);
SEMANTIC_SIZE_ASSERT(race_rider_state, sizeof(RaceRiderState) == 0x104);
SEMANTIC_SIZE_ASSERT(racer_creation_template,
                     sizeof(RacerCreationTemplate) == 0x64);
SEMANTIC_SIZE_ASSERT(racer_motion_defaults, sizeof(RacerMotionDefaults) == 0x18);
SEMANTIC_SIZE_ASSERT(rider_runtime_slot, sizeof(RiderRuntimeSlot) == 0x40);
SEMANTIC_SIZE_ASSERT(rider_runtime_resource_item_offset,
                     offsetof(RiderRuntimeSlot, resource_item) == 0x00);
SEMANTIC_SIZE_ASSERT(rider_runtime_maximum_health_offset,
                     offsetof(RiderRuntimeSlot, maximum_health) == 0x34);
SEMANTIC_SIZE_ASSERT(rider_runtime_recovery_health_offset,
                     offsetof(RiderRuntimeSlot, recovery_health_ceiling) == 0x38);
SEMANTIC_SIZE_ASSERT(rider_runtime_current_health_offset,
                     offsetof(RiderRuntimeSlot, current_health) == 0x3C);
SEMANTIC_SIZE_ASSERT(resource_ref_pair, sizeof(ResourceRefPair) == 0x08);
SEMANTIC_SIZE_ASSERT(input_chord_state, sizeof(InputChordState) == 0x0C);
SEMANTIC_SIZE_ASSERT(input_thread_state, sizeof(InputThreadState) == 0x44);
SEMANTIC_SIZE_ASSERT(opponent_choice_record,
                     sizeof(OpponentChoiceRecord) == 0x38);
SEMANTIC_SIZE_ASSERT(opponent_choice_level, sizeof(OpponentChoiceLevel) == 0x1C0);
SEMANTIC_SIZE_ASSERT(front_end_cel_descriptor,
                     sizeof(FrontEndCelDescriptor) == 0x0C);
SEMANTIC_SIZE_ASSERT(front_end_wait_command,
                     sizeof(FrontEndWaitCommand) == 0x08);
SEMANTIC_SIZE_ASSERT(front_end_show_cels_command,
                     sizeof(FrontEndShowCelsCommand) == 0x08);
SEMANTIC_SIZE_ASSERT(front_end_present_command,
                     sizeof(FrontEndPresentCommand) == 0x04);
SEMANTIC_SIZE_ASSERT(front_end_play_stream_command,
                     sizeof(FrontEndPlayStreamCommand) == 0x08);
SEMANTIC_SIZE_ASSERT(front_end_callback_command,
                     sizeof(FrontEndCallbackCommand) == 0x10);
SEMANTIC_SIZE_ASSERT(front_end_action, sizeof(FrontEndAction) == 0x30);
SEMANTIC_SIZE_ASSERT(front_end_menu, sizeof(FrontEndMenu) == 0x14);
SEMANTIC_SIZE_ASSERT(front_end_asset_state,
                     sizeof(FrontEndAssetState) == 0x10);
SEMANTIC_SIZE_ASSERT(race_mode_state, sizeof(RaceModeState) == 0x10);
SEMANTIC_SIZE_ASSERT(rider_profile, sizeof(RiderProfile) == 0x3C);
SEMANTIC_SIZE_ASSERT(race_result_entry, sizeof(RaceResultEntry) == 0x08);
SEMANTIC_SIZE_ASSERT(front_end_preview_state,
                     sizeof(FrontEndPreviewState) == 0x1C);
SEMANTIC_SIZE_ASSERT(track_effect_pools, sizeof(TrackEffectPools) == 0x0C);
SEMANTIC_SIZE_ASSERT(track_effect_creation_descriptor,
                     sizeof(TrackEffectCreationDescriptor) == 0x1C);
SEMANTIC_SIZE_ASSERT(race_input_latch_state,
                     sizeof(RaceInputLatchState) == 0x14);
SEMANTIC_SIZE_ASSERT(audio_voice_slot, sizeof(AudioVoiceSlot) == 0x08);
SEMANTIC_SIZE_ASSERT(audio_effect_channel,
                     sizeof(AudioEffectChannel) == 0x10);
SEMANTIC_SIZE_ASSERT(pursuit_voice_group,
                     sizeof(PursuitVoiceGroup) == 0x10);
SEMANTIC_SIZE_ASSERT(score_playback_state,
                     sizeof(ScorePlaybackState) == 0x14);
SEMANTIC_SIZE_ASSERT(game_audio_state, sizeof(GameAudioState) == 0x13C);
SEMANTIC_SIZE_ASSERT(game_audio_primary_effect_voice_offset,
                     offsetof(GameAudioState, effect_channels) +
                     offsetof(AudioEffectChannel, channel) +
                     offsetof(AudioVoiceSlot, voice_item) == 0x1C);
SEMANTIC_SIZE_ASSERT(game_audio_secondary_effect_voice_offset,
                     offsetof(GameAudioState, effect_channels) +
                     sizeof(AudioEffectChannel) +
                     offsetof(AudioEffectChannel, channel) +
                     offsetof(AudioVoiceSlot, voice_item) == 0x2C);
SEMANTIC_SIZE_ASSERT(game_audio_sound_samples_offset,
                     offsetof(GameAudioState, sound_sample_items) == 0x48);
SEMANTIC_SIZE_ASSERT(game_audio_initialized_offset,
                     offsetof(GameAudioState, initialized) == 0xB4);
SEMANTIC_SIZE_ASSERT(game_audio_boost_active_offset,
                     offsetof(GameAudioState, boost_sound_active) == 0xD0);
SEMANTIC_SIZE_ASSERT(game_audio_event_count_offset,
                     offsetof(GameAudioState, queued_event_count) == 0xD8);
SEMANTIC_SIZE_ASSERT(game_audio_event_write_offset,
                     offsetof(GameAudioState, queued_event_write_index) == 0xDC);
SEMANTIC_SIZE_ASSERT(game_audio_event_read_offset,
                     offsetof(GameAudioState, queued_event_read_index) == 0xE0);
SEMANTIC_SIZE_ASSERT(game_audio_transient_sample_offset,
                     offsetof(GameAudioState, transient_sample_item) == 0xE4);
SEMANTIC_SIZE_ASSERT(game_audio_voice_pitch_offset,
                     offsetof(GameAudioState, voice_pitch) == 0xE8);
SEMANTIC_SIZE_ASSERT(game_audio_pitch_step_offset,
                     offsetof(GameAudioState, pitch_step) == 0xFC);
SEMANTIC_SIZE_ASSERT(game_audio_voice_volume_offset,
                     offsetof(GameAudioState, voice_volume) == 0x10C);
SEMANTIC_SIZE_ASSERT(game_audio_voice_pan_offset,
                     offsetof(GameAudioState, voice_pan) == 0x11C);
SEMANTIC_SIZE_ASSERT(game_audio_event_countdown_offset,
                     offsetof(GameAudioState, repeated_event_countdown) == 0x138);
SEMANTIC_SIZE_ASSERT(race_object_scheduler_state,
                     sizeof(RaceObjectSchedulerState) == 0x34);
SEMANTIC_SIZE_ASSERT(track_visibility_scan_state,
                     sizeof(TrackVisibilityScanState) == 0x08);
SEMANTIC_SIZE_ASSERT(rider_physics_tuning,
                     sizeof(RiderPhysicsTuning) == 0xD4);
SEMANTIC_SIZE_ASSERT(renderer_motion_state,
                     sizeof(RendererMotionState) == 0x2C);
SEMANTIC_SIZE_ASSERT(display_easing_lookup_offset,
                     offsetof(DisplayRuntimeGlobals, easing_lookup) == 0x74);
SEMANTIC_SIZE_ASSERT(display_trig_lookup_low_offset,
                     offsetof(DisplayRuntimeGlobals, trig_lookup_low) == 0x178);
SEMANTIC_SIZE_ASSERT(display_trig_lookup_high_offset,
                     offsetof(DisplayRuntimeGlobals, trig_lookup_high) == 0x1F8);
SEMANTIC_SIZE_ASSERT(display_shading_mode_offset,
                     offsetof(DisplayRuntimeGlobals, shading_mode) == 0x400);
SEMANTIC_SIZE_ASSERT(display_shading_row_offset,
                     offsetof(DisplayRuntimeGlobals, shading_row_offset) == 0x404);
SEMANTIC_SIZE_ASSERT(display_random_seed_scratch_offset,
                     offsetof(DisplayRuntimeGlobals, random_seed_scratch) == 0x408);
SEMANTIC_SIZE_ASSERT(random_seed_scratch,
                     sizeof(RandomSeedScratch) == 0x08);
SEMANTIC_SIZE_ASSERT(road_renderer_state,
                     sizeof(RoadRendererState) == 0x174);
SEMANTIC_SIZE_ASSERT(animation_binding_spec,
                     sizeof(AnimationBindingSpec) == 0x08);
SEMANTIC_SIZE_ASSERT(audio_mixer_template_defaults,
                     sizeof(AudioMixerTemplateDefaults) == 0x14);
SEMANTIC_SIZE_ASSERT(car_animation_load_state,
                     sizeof(CarAnimationLoadState) == 0x08);
SEMANTIC_SIZE_ASSERT(animation_resource_pair,
                     sizeof(AnimationResourcePair) == 0x08);
SEMANTIC_SIZE_ASSERT(car_animation_resource_pair,
                     sizeof(CarAnimationResourcePair) == 0x08);
SEMANTIC_SIZE_ASSERT(segment_repair_globals,
                     sizeof(SegmentRepairGlobals) == 0x3C);
SEMANTIC_SIZE_ASSERT(family_resource_request,
                     sizeof(FamilyResourceRequest) == 0x0C);
SEMANTIC_SIZE_ASSERT(family_resource_globals,
                     sizeof(FamilyResourceGlobals) == 0xE0);
SEMANTIC_SIZE_ASSERT(track_simulation_globals,
                     sizeof(TrackSimulationGlobals) == 0x80);
SEMANTIC_SIZE_ASSERT(track_object_mode_template,
                     sizeof(TrackObjectModeTemplate) == 0x18);
SEMANTIC_SIZE_ASSERT(display_runtime_globals,
                     sizeof(DisplayRuntimeGlobals) == 0x410);
SEMANTIC_SIZE_ASSERT(race_object_creation_descriptor,
                     sizeof(RaceObjectCreationDescriptor) == 0x1C);
SEMANTIC_SIZE_ASSERT(track_runtime_globals,
                     sizeof(TrackRuntimeGlobals) == 0x5C);
SEMANTIC_SIZE_ASSERT(track_runtime_family_schedule_span_offset,
                     offsetof(TrackRuntimeGlobals,
                              family_schedule_window_span) == 0x00);
SEMANTIC_SIZE_ASSERT(track_runtime_family_schedule_enable_offset,
                     offsetof(TrackRuntimeGlobals,
                              family_schedule_events_enabled) == 0x10);
SEMANTIC_SIZE_ASSERT(track_runtime_async_family_load_offset,
                     offsetof(TrackRuntimeGlobals,
                              async_family_resource_loads_enabled) == 0x14);
SEMANTIC_SIZE_ASSERT(track_runtime_road_segment_pool_offset,
                     offsetof(TrackRuntimeGlobals, road_segment_pool) == 0x18);
SEMANTIC_SIZE_ASSERT(track_runtime_road_object_descriptor_offset,
                     offsetof(TrackRuntimeGlobals, road_object_descriptor) == 0x1C);
SEMANTIC_SIZE_ASSERT(track_runtime_allocation_pool_offset,
                     offsetof(TrackRuntimeGlobals, allocation_pool) == 0x38);
SEMANTIC_SIZE_ASSERT(resource_catalog_handles,
                     sizeof(ResourceCatalogHandles) == 0x10);
SEMANTIC_SIZE_ASSERT(resource_catalog_entry,
                     sizeof(ResourceCatalogEntry) == 0x14);
SEMANTIC_SIZE_ASSERT(big_endian_byte_flag,
                     sizeof(BigEndianByteFlag) == 0x04);
SEMANTIC_SIZE_ASSERT(runtime_feature_flags,
                     sizeof(RuntimeFeatureFlags) == 0x20);
SEMANTIC_SIZE_ASSERT(resource_info_state,
                     sizeof(ResourceInfoState) == 0x0C);
SEMANTIC_SIZE_ASSERT(movie_playback_control,
                     sizeof(MoviePlaybackControl) == 0x08);
SEMANTIC_SIZE_ASSERT(display_system_globals,
                     sizeof(DisplaySystemGlobals) == 0x48);
SEMANTIC_SIZE_ASSERT(particle_resource_globals,
                     sizeof(ParticleResourceGlobals) == 0x94);
SEMANTIC_SIZE_ASSERT(hud_coordinate, sizeof(HudCoordinate) == 0x08);
SEMANTIC_SIZE_ASSERT(hud_globals, sizeof(HudGlobals) == 0xF0);
SEMANTIC_SIZE_ASSERT(road_surface_render_globals,
                     sizeof(RoadSurfaceRenderGlobals) == 0x4C);
SEMANTIC_SIZE_ASSERT(audio_playback_state,
                     sizeof(AudioPlaybackState) == 0x28);
SEMANTIC_SIZE_ASSERT(audio_playback_voice_id_offset,
                     offsetof(AudioPlaybackState, playback_voice_id) == 0x0C);
SEMANTIC_SIZE_ASSERT(audio_playback_amplitude_offset,
                     offsetof(AudioPlaybackState, amplitude) == 0x24);
SEMANTIC_SIZE_ASSERT(audio_source_descriptor,
                     sizeof(AudioSourceDescriptor) == 0x40);
SEMANTIC_SIZE_ASSERT(background_audio_globals,
                     sizeof(BackgroundAudioGlobals) == 0x5C);
SEMANTIC_SIZE_ASSERT(memory_allocator_globals,
                     sizeof(MemoryAllocatorGlobals) == 0x38);
SEMANTIC_SIZE_ASSERT(resource_manager_globals,
                     sizeof(ResourceManagerGlobals) == 0x0C);
SEMANTIC_SIZE_ASSERT(music_library_globals,
                     sizeof(MusicLibraryGlobals) == 0x50);
SEMANTIC_SIZE_ASSERT(tag_argument, sizeof(RwTagArgument) == 0x08);
SEMANTIC_SIZE_ASSERT(memory_vectors, sizeof(RwMemoryVectors) == 0x08);
SEMANTIC_SIZE_ASSERT(saudio_template_record,
                     sizeof(SAudioTemplateRecord) == 0x0C);
SEMANTIC_SIZE_ASSERT(thread_creation_defaults,
                     sizeof(ThreadCreationDefaults) == 0x4C);
SEMANTIC_SIZE_ASSERT(block_file_api_globals,
                     sizeof(BlockFileApiGlobals) == 0x0C);
SEMANTIC_SIZE_ASSERT(linked_list_memory_api,
                     sizeof(LinkedListMemoryApi) == 0x0C);
SEMANTIC_SIZE_ASSERT(block_file_sector_cache,
                     sizeof(BlockFileSectorCache) == 0x18);
SEMANTIC_SIZE_ASSERT(cpak_subscriber_globals,
                     sizeof(CPakSubscriberGlobals) == 0x0C);
SEMANTIC_SIZE_ASSERT(event_broker_client,
                     sizeof(EventBrokerClientState) == 0x20);
SEMANTIC_SIZE_ASSERT(legacy_random_generator,
                     sizeof(LegacyRandomGeneratorState) == 0xE0);
SEMANTIC_SIZE_ASSERT(cam_stdio_runtime, sizeof(CamStdioRuntime) == 0x18);
SEMANTIC_SIZE_ASSERT(opponent_progression_record,
                     sizeof(OpponentProgressionRecord) == 0x1D8);
SEMANTIC_SIZE_ASSERT(cel_animation_binding,
                     sizeof(CelAnimationBinding) == 0x14);

int
block_file_allocate_memory(int size,
                           int memory_type);
void
block_file_free_memory(int memory,
                       int size);
int
byte_file_allocate_memory(int size,
                          int memory_type);
void
byte_file_free_memory(int memory,
                      int size);

int
racer_object_create();
int
racer_object_update();
int
ai_racer_control();
int
racer_object_destroy();
int
racer_state_create();
int
racer_state_update();
int
ai_racer_state_control();
int
player_rider_control();
int
racer_state_destroy();
void
racer_object_contact_response(RacerEntity *rider);
void
racer_state_contact_response(RacerEntity *state);
int
rider_update_mode_0(void *rider);
int
rider_update_mode_1(void *rider);
int
rider_update_mode_2(void *rider);
int
rider_update_mode_3(void *rider);
int
rider_update_mode_4(void *rider);
int
rider_update_mode_5(void *rider);
int
rider_update_mode_6(void *rider);
int
rider_update_mode_7(void *rider);
int
rider_update_mode_8(void *rider);
int
rider_update_mode_9(void *rider);

void
rider_visual_create(char *object,
                    int   configuration);
void
rider_visual_update(char *object);
void
rider_visual_destroy(char *object);
void
rider_visual_contact(RacerEntity *object);

#define front_end_advance_random_rider_preview \
        front_end_advance_random_rider_preview_command_adapter
#define front_end_draw_course_distance \
        front_end_draw_course_distance_command_adapter
#define front_end_draw_current_balance \
        front_end_draw_current_balance_command_adapter
#define front_end_draw_rider_bike_statistics \
        front_end_draw_rider_bike_statistics_command_adapter
#define front_end_draw_rider_style_affordability \
        front_end_draw_rider_style_affordability_command_adapter
#define front_end_draw_rider_style_price \
        front_end_draw_rider_style_price_command_adapter
#define front_end_draw_selected_bike_summary \
        front_end_draw_selected_bike_summary_command_adapter
#define front_end_draw_selected_rider_summary \
        front_end_draw_selected_rider_summary_command_adapter
#define front_end_finalize_race_setup \
        front_end_finalize_race_setup_command_adapter
#define front_end_load_mode_backdrop \
        front_end_load_mode_backdrop_command_adapter
#define front_end_noop front_end_noop_command_adapter
#define front_end_play_transition \
        front_end_play_transition_command_adapter
#define front_end_preload_selected_opponent \
        front_end_preload_selected_opponent_command_adapter
#define front_end_reset_random_rider_preview \
        front_end_reset_random_rider_preview_command_adapter
#define front_end_select_available_race_variant \
        front_end_select_available_race_variant_command_adapter
#define front_end_set_selected_level \
        front_end_set_selected_level_command_adapter
#define front_end_show_current_player_descriptor \
        front_end_show_current_player_descriptor_command_adapter
#define front_end_show_game_mode_descriptor \
        front_end_show_game_mode_descriptor_command_adapter
#define front_end_show_profile_slot_descriptor \
        front_end_show_profile_slot_descriptor_command_adapter
#define front_end_show_race_mode_descriptor \
        front_end_show_race_mode_descriptor_command_adapter
#define front_end_show_rider_type_descriptor \
        front_end_show_rider_type_descriptor_command_adapter
#define front_end_show_selected_level \
        front_end_show_selected_level_command_adapter
#define front_end_show_selected_player_descriptor \
        front_end_show_selected_player_descriptor_command_adapter
#define front_end_synchronize_current_player \
        front_end_synchronize_current_player_command_adapter
#define front_end_update_rider_progression \
        front_end_update_rider_progression_command_adapter

#define front_end_can_afford_rider_style \
        front_end_can_afford_rider_style_condition_adapter
#define front_end_choose_random_rider_preview \
        front_end_choose_random_rider_preview_condition_adapter
#define front_end_consume_pending_event \
        front_end_consume_pending_event_condition_adapter
#define front_end_has_pending_race_result \
        front_end_has_pending_race_result_condition_adapter
#define front_end_is_alternate_game_mode \
        front_end_is_alternate_game_mode_condition_adapter
#define front_end_is_player_configuration_valid \
        front_end_is_player_configuration_valid_condition_adapter
#define front_end_is_profile_slot_available \
        front_end_is_profile_slot_available_condition_adapter
#define front_end_is_selected_rider_style \
        front_end_is_selected_rider_style_condition_adapter
#define front_end_is_single_player \
        front_end_is_single_player_condition_adapter
#define front_end_is_special_mode_enabled \
        front_end_is_special_mode_enabled_condition_adapter
#define front_end_prepare_selected_opponents \
        front_end_prepare_selected_opponents_condition_adapter
#define front_end_set_player_descriptor_selection \
        front_end_set_player_descriptor_selection_condition_adapter
#define front_end_set_selected_finish_position \
        front_end_set_selected_finish_position_condition_adapter

extern const unsigned char bike_shop_rat_perro_stream_path[];
extern const unsigned char bike_shop_rat_corsair_stream_path[];
extern const unsigned char bike_shop_rat_banzai_stream_path[];
extern const unsigned char bike_shop_rat_killer_stream_path[];
extern const unsigned char bike_shop_rat_kamakazi_stream_path[];
extern const unsigned char bike_shop_sport_stiletto_stream_path[];
extern const unsigned char bike_shop_sport_diablo_stream_path[];
extern const unsigned char bike_shop_sport_perro_stream_path[];
extern const unsigned char bike_shop_sport_dmg_stream_path[];
extern const unsigned char bike_shop_sport_kamakazi_stream_path[];
extern const unsigned char bike_shop_super_stiletto_stream_path[];
extern const unsigned char bike_shop_super_diablo_stream_path[];
extern const unsigned char bike_shop_super_corsair_stream_path[];
extern const unsigned char bike_shop_super_banzai_stream_path[];
extern const unsigned char bike_shop_super_kamakazi_stream_path[];

void *kernel_vector_base = 0;

RaceBoostConfig race_boost_config =
{
  17,
  384,
  120,
  100,
  30,
  15,
  0
};

RaceRiderState race_rider_state =
{
  0, 16, 0, 0, 0, 0,
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  0, 0, 0, { 0, 0, 0 }, 0,
  { -60, -30, -15 },
  { 60, 30, 15 },
  { 95, 105 }
};

RacerCreationTemplate challenge_object_template =
{
  0x03000000U, 4, 4,
  racer_object_create, racer_object_update, ai_racer_control,
  racer_object_destroy,
  300, 256, 130, 8, 37, -2560, 256, 74, 4, 24,
  7680, 64, 8960, 192, 256, racer_object_contact_response, 1, 1
};

RacerCreationTemplate challenge_state_template =
{
  0, 8, 4,
  racer_state_create, racer_state_update, ai_racer_state_control,
  racer_state_destroy,
  300, 256, 130, 70, 37, -2560, 256, 74, 0, 24,
  6400, 16, 7680, 192, 25, racer_state_contact_response, 1, 1
};

RacerCreationTemplate opponent_object_template =
{
  0x03000000U, 4, 4,
  racer_object_create, racer_object_update, ai_racer_control,
  racer_object_destroy,
  300, 256, 130, 8, 37, -2560, 256, 74, 4, 24,
  7680, 64, 8960, 192, 256, racer_object_contact_response, 1, 1
};

RacerCreationTemplate opponent_state_template =
{
  0, 8, 4,
  racer_state_create, racer_state_update, ai_racer_state_control,
  racer_state_destroy,
  300, 256, 130, 70, 37, -2560, 256, 74, 0, 24,
  6400, 16, 7680, 192, 25, racer_state_contact_response, 1, 1
};

RacerCreationTemplate player_object_template =
{
  0x03000000U, 4, 1,
  racer_object_create, racer_object_update, ai_racer_control,
  racer_object_destroy,
  300, 256, 130, 8, 37, -2560, 256, 74, 4, 24,
  7680, 64, 8960, 192, 256, racer_object_contact_response, 1, 1
};

RacerCreationTemplate player_state_template =
{
  0, 8, 1,
  racer_state_create, player_rider_control, ai_racer_state_control,
  racer_state_destroy,
  300, 256, 130, 70, 37, -2560, 256, 74, 0, 24,
  6400, 16, 7680, 192, 25, racer_state_contact_response, 1, 1
};

RacerMotionDefaults upright_motion_defaults =
{
  0x1E00, 0x40, 0x2300, 0xC0, 0x100, racer_object_contact_response
};

RacerMotionDefaults impact_motion_defaults =
{
  0x1E00, 0x40, 0x0800, 0xC0, 0x100, racer_object_contact_response
};

RiderRuntimeSlot rider_runtime_slots[RACE_RIDER_COUNT] =
{
  { 0, 0x01000000U, 74, 180, 0, 0x04280000U,
    { 0, 0, 0, 0, 0, 0 }, 7, 2500, 2500, 2500 },
  { 0, 0x01000000U, 74, 215, 0, 0x04280000U,
    { 0, 0, 0, 0, 0, 0 }, 7, 3000, 3000, 3000 },
  { 0, 0, 63, 108, 0, 0x02640000U,
    { 0, 0, 0, 0, 0, 0 }, 5, 2000, 2000, 2000 },
  { 0, 0x01000000U, 74, 142, 0, 0x033C0000U,
    { 1, 1, 1, 1, 1, 1 }, 6, 2500, 2500, 2500 },
  { 0, 0, 67, 118, 0, 0x01460000U,
    { 2, 2, 2, 2, 2, 2 }, 5, 2500, 2500, 2500 },
  { 0, 0x01000000U, 74, 215, 0, 0x09280000U,
    { 1, 1, 1, 1, 1, 1 }, 7, 3500, 3500, 3500 },
  { 0, 0, 60, 145, 0, 0x06FF0000U,
    { 0, 0, 0, 0, 0, 0 }, 6, 3500, 3500, 3500 },
  { 0, 0x01000000U, 75, 230, 0, 0x05080000U,
    { 0, 0, 0, 0, 0, 0 }, 8, 4000, 4000, 4000 },
  { 0, 0x01000000U, 73, 183, 0, 0x05FF0000U,
    { 2, 2, 2, 2, 2, 2 }, 7, 3000, 3000, 3000 },
  { 0, 0x01000000U, 74, 215, 0, 0x04280000U,
    { 1, 1, 1, 1, 1, 1 }, 4, 2200, 2200, 2200 },
  { 0, 0x01000000U, 68, 108, 0, 0x02C80000U,
    { 2, 2, 2, 2, 2, 2 }, 2, 1600, 1600, 1600 },
  { 0, 0x01000000U, 74, 142, 0, 0x033C0000U,
    { 0, 0, 0, 0, 0, 0 }, 3, 2000, 2000, 2000 },
  { 0, 0x01000000U, 67, 118, 0, 0x01460000U,
    { 1, 1, 1, 1, 1, 1 }, 2, 1800, 1800, 1800 },
  { 0, 0x01000000U, 74, 215, 0, 0x09280000U,
    { 2, 2, 2, 2, 2, 2 }, 6, 2500, 2500, 2500 },
  { 0, 0x01000000U, 60, 145, 0, 0x06FF0000U,
    { 0, 0, 0, 0, 0, 0 }, 3, 2000, 2000, 2000 },
  { 0, 0x01000000U, 73, 171, 0, 0x02FF0000U,
    { 1, 1, 1, 1, 1, 1 }, 6, 2200, 2200, 2200 }
};

signed char rider_runtime_selectors[RACE_RIDER_COUNT] =
{
  0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15
};

ResourceRefPair rider_resource_references[2][RACE_RIDER_COUNT] =
{
  {
    { 0x4000, 0x3200U }, { 0x4000, 0x7B00U },
    { 0x4000, 0x3600U }, { 0x4000, 0x3800U },
    { 0xA000, 0x3400U }, { 0x4000, 0x6200U },
    { 0x2000, 0x6C00U }, { 0x9000, 0x7600U },
    { 0x5000, 0x5800U }, { 0xA000, 0x3300U },
    { 0xA000, 0x3500U }, { 0xA000, 0x3700U },
    { 0xA000, 0x3900U }, { 0x4000, 0x5300U },
    { 0xA000, 0x5D00U }, { 0, 0x5800U }
  },
  {
    { 0x4000, 0x3200U }, { 0x4000, 0x7600U },
    { 0x4000, 0x4400U }, { 0x4000, 0x4E00U },
    { 0xA000, 0x3600U }, { 0x4000, 0x6700U },
    { 0x2000, 0x6C00U }, { 0x9000, 0x7100U },
    { 0x5000, 0x5D00U }, { 0xA000, 0x3400U },
    { 0xA000, 0x3800U }, { 0xA000, 0x4900U },
    { 0xA000, 0x5300U }, { 0x4000, 0x5800U },
    { 0xA000, 0x6200U }, { 0, 0x5800U }
  }
};

void *rider_resource_items[RACE_RIDER_COUNT] =
{
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

void *challenge_resource_items[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };

char rider_names[RIDER_TYPE_COUNT_RW][RIDER_NAME_BYTES] =
{
  "Player", "Teflon", "Cydney", "Slim", "Pearl", "Milwaukee",
  "Rhonda", "Bose", "Axle", "", "", "", "", "", "", "Cop"
};

char opponent_names[OPPONENT_NAME_LEVELS]
[OPPONENTS_PER_LEVEL][RIDER_NAME_BYTES] =
{
  { "Butch", "Buzz", "Druid", "Hog", "Serge", "Slash" },
  { "Killjoy", "Max", "Ollie", "Sid", "Spike", "Zippy" },
  { "Angel", "Buffy", "Chino", "Felix", "Roz", "Slater" },
  { "Biff", "Ikira", "Jorg", "PENo1", "Stud", "Sven" },
  { "Boffo", "Dex", "Driller", "Grubb", "Gunther", "Spud" }
};

unsigned char opponent_names_alignment[2] = { 0, 0 };

unsigned char
  rider_style_defaults[RIDER_TYPE_COUNT_RW]
                      [RIDER_PROGRESSION_LEVEL_COUNT] =
{
  { 1, 0, 12, 6, 9 }, { 1, 3, 12, 8, 7 },
  { 2, 14, 10, 9, 7 }, { 4, 14, 6, 5, 7 },
  { 4, 3, 12, 8, 7 }, { 1, 3, 10, 5, 7 },
  { 1, 3, 12, 8, 7 }, { 2, 14, 6, 9, 7 },
  { 1, 3, 12, 8, 7 }, { 4, 0, 13, 6, 9 },
  { 1, 3, 12, 8, 7 }, { 4, 0, 12, 6, 9 },
  { 1, 3, 12, 8, 7 }, { 4, 0, 13, 6, 9 },
  { 1, 3, 12, 8, 7 }, { 4, 0, 13, 6, 9 }
};

int opponent_speed_percent[RIDER_PROGRESSION_LEVEL_COUNT] =
  { 116, 116, 114, 114, 114 };

int rider_animation_fallback_states[50] =
{
  2, 2, 2, 2, 2, 2, 2, 2, 2, 9,
  16, 17, 16, 17, 16, 17, 16, 17, 16, 17,
  16, 17, 16, 17, 16, 17, 16, 17, 28, 28,
  28, 28, 28, 33, 28, 33, 37, 37, 37, 33,
  33, 33, 33, 48, 48, 48, 46, 46, 48, 48
};

int opponent_resource_group_a[8] = { 22, 16, 10, 26, 24, 18, 12, 26 };
int opponent_resource_group_b[8] = { 23, 17, 11, 27, 25, 19, 13, 27 };
int opponent_resource_group_c[8] = { 22, 20, 14, 26, 23, 21, 15, 27 };

OpponentProgressionRecord rider_progression_defaults[RIDER_TYPE_COUNT] =
{
  { {
      0x00000000U, 0x00002000U, 0x00004000U, 0x00002000U, 0x00000000U, 0x00005000U,
      0x00000000U, 0x00001000U, 0x00007000U, 0x00000000U, 0x00002000U, 0x00002000U,
      0x00000000U, 0x00001000U, 0x00004000U, 0x00000000U, 0x00002000U, 0x00003000U,
      0x00002000U, 0x00000000U, 0x00006000U, 0x00000000U, 0x00000000U, 0x00001000U,
      0x00002000U, 0x00000000U, 0x00007000U, 0x00000000U, 0x00000000U, 0x00004000U,
      0x00000000U, 0x00000000U, 0x00004000U, 0x00000000U, 0x00000000U, 0x00005000U,
      0x00000000U, 0x00000000U, 0x00007000U, 0x00000000U, 0x00000000U, 0x00002000U,
      0x00000000U, 0x00000000U, 0x00004000U, 0x00000000U, 0x00000000U, 0x00003000U,
      0x00000000U, 0x00000000U, 0x00006000U, 0x00000000U, 0x00000000U, 0x00001000U,
      0x00000000U, 0x00000000U, 0x00007000U, 0x00000000U, 0x00000000U, 0x00004000U,
      0x00000400U, 0x00000400U, 0x00000080U, 0x00001000U, 0x00000800U, 0x00000100U,
      0x00002000U, 0x00002000U, 0x00000200U, 0x00000000U, 0x00000000U, 0x00000000U,
      0x00000000U, 0xFFFFE000U, 0xFFFFFC00U, 0x00000200U, 0x00000080U, 0x00000004U,
      0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U,
      0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U,
      0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U,
      0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U,
      0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000046U, 0x00000052U,
      0x0000005FU, 0x0000006EU, 0x0000007DU, 0x00000057U, 0x0000006BU, 0x0000007AU,
      0x00000091U, 0x000000AFU, 0x03040000U, 0x00000003U
    } },
  { {
      0x00000000U, 0x00000000U, 0x00007000U, 0x00000000U, 0x00000000U, 0x00000000U,
      0x00000000U, 0x00000000U, 0x00000100U, 0x00000000U, 0x00000000U, 0x00007000U,
      0x00000000U, 0x00000000U, 0x00008000U, 0x00000000U, 0x00000000U, 0x00006000U,
      0x00000000U, 0x00000000U, 0x00004000U, 0x00000000U, 0x00000000U, 0x00006000U,
      0x00000000U, 0x00000000U, 0x00005000U, 0x00000000U, 0x00000000U, 0x00006000U,
      0x00000000U, 0x00000000U, 0x00007000U, 0x00000000U, 0x00000000U, 0x00000000U,
      0x00000000U, 0x00000000U, 0x00000100U, 0x00000000U, 0x00000000U, 0x00007000U,
      0x00000000U, 0x00000000U, 0x00008000U, 0x00000000U, 0x00000000U, 0x00006000U,
      0x00000000U, 0x00000000U, 0x00004000U, 0x00000000U, 0x00000000U, 0x00006000U,
      0x00000000U, 0x00000000U, 0x00005000U, 0x00000000U, 0x00000000U, 0x00006000U,
      0x00000400U, 0x00000400U, 0x00000200U, 0x00001000U, 0x00000800U, 0x00000400U,
      0x00004000U, 0x00002000U, 0x00000400U, 0x00000000U, 0x00000000U, 0x00000000U,
      0x00000800U, 0x00000800U, 0xFFFFFC00U, 0x00000400U, 0x00000100U, 0x00000008U,
      0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U,
      0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00010000U,
      0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U,
      0x00000000U, 0x00000000U, 0x00000000U, 0x01000000U, 0x00000000U, 0x00000000U,
      0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x0000004BU, 0x00000055U,
      0x0000005FU, 0x0000006EU, 0x00000078U, 0x00000063U, 0x00000073U, 0x00000082U,
      0x00000096U, 0x000000B9U, 0x04020000U, 0x00000004U
    } },
  { {
      0x00000000U, 0x00000000U, 0x00003000U, 0x00000000U, 0x00000000U, 0x00007000U,
      0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000100U,
      0x00000000U, 0x00000000U, 0x00006000U, 0x00000000U, 0x00000000U, 0x00005000U,
      0x00000000U, 0x00000000U, 0x00005000U, 0x00000000U, 0x00000000U, 0x00008000U,
      0x00000000U, 0x00000000U, 0x00004000U, 0x00000000U, 0x00000000U, 0x00002000U,
      0x00000000U, 0x00000000U, 0x00003000U, 0x00000000U, 0x00000000U, 0x00007000U,
      0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000100U,
      0x00000000U, 0x00000000U, 0x00006000U, 0x00000000U, 0x00000000U, 0x00005000U,
      0x00000000U, 0x00000000U, 0x00005000U, 0x00000000U, 0x00000000U, 0x00008000U,
      0x00000000U, 0x00000000U, 0x00004000U, 0x00000000U, 0x00000000U, 0x00002000U,
      0x00000000U, 0x00000200U, 0x00000080U, 0x00000400U, 0x00000400U, 0x00000100U,
      0x00001000U, 0x00001000U, 0x00000200U, 0x00000000U, 0x00000000U, 0x00000000U,
      0x00000000U, 0xFFFFF800U, 0xFFFFFE00U, 0x00000200U, 0x00000100U, 0x00000004U,
      0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U,
      0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00020000U,
      0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U,
      0x00000000U, 0x00000000U, 0x00000000U, 0x02000000U, 0x00000000U, 0x00000000U,
      0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x0000005AU, 0x00000064U,
      0x0000006EU, 0x0000007DU, 0x0000008CU, 0x00000068U, 0x00000078U, 0x0000008CU,
      0x000000A0U, 0x000000BEU, 0x03040000U, 0x00000003U
    } },
  { {
      0x00000000U, 0x00000000U, 0x00003000U, 0x00000000U, 0x00000000U, 0x00003000U,
      0x00000000U, 0x00000000U, 0x00006000U, 0x00000000U, 0x00000000U, 0x00000000U,
      0x00000000U, 0x00000000U, 0x00004000U, 0x00000000U, 0x00000000U, 0x00002000U,
      0x00000000U, 0x00000000U, 0x00000100U, 0x00000000U, 0x00000000U, 0x00002000U,
      0x00000000U, 0x00000000U, 0x00003000U, 0x00000000U, 0x00000000U, 0x00004000U,
      0x00000000U, 0x00000000U, 0x00003000U, 0x00000000U, 0x00000000U, 0x00003000U,
      0x00000000U, 0x00000000U, 0x00006000U, 0x00000000U, 0x00000000U, 0x00000000U,
      0x00000000U, 0x00000000U, 0x00004000U, 0x00000000U, 0x00000000U, 0x00002000U,
      0x00000000U, 0x00000000U, 0x00000100U, 0x00000000U, 0x00000000U, 0x00002000U,
      0x00000000U, 0x00000000U, 0x00003000U, 0x00000000U, 0x00000000U, 0x00004000U,
      0x00000400U, 0x00000800U, 0x00000100U, 0x00000800U, 0x00001000U, 0x00000200U,
      0x00001000U, 0x00002000U, 0x00000200U, 0x00000000U, 0x00000000U, 0x00000000U,
      0xFFFFF000U, 0xFFFFF000U, 0xFFFFFE00U, 0x00000080U, 0x00000200U, 0x00000002U,
      0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U,
      0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00030000U,
      0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U,
      0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U,
      0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000055U, 0x0000006CU,
      0x00000078U, 0x00000082U, 0x0000008CU, 0x00000066U, 0x00000074U, 0x00000085U,
      0x0000009BU, 0x000000B9U, 0x02040000U, 0x00000003U
    } },
  { {
      0x00000000U, 0x00000000U, 0x00004000U, 0x00000000U, 0x00000000U, 0x00004000U,
      0x00000000U, 0x00000000U, 0x00006000U, 0x00000000U, 0x00000000U, 0x00005000U,
      0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00006000U,
      0x00000000U, 0x00000000U, 0x00007000U, 0x00000000U, 0x00000000U, 0x00003000U,
      0x00000000U, 0x00000000U, 0x00000100U, 0x00000000U, 0x00000000U, 0x00000100U,
      0x00000000U, 0x00000000U, 0x00004000U, 0x00000000U, 0x00000000U, 0x00004000U,
      0x00000000U, 0x00000000U, 0x00006000U, 0x00000000U, 0x00000000U, 0x00005000U,
      0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00006000U,
      0x00000000U, 0x00000000U, 0x00007000U, 0x00000000U, 0x00000000U, 0x00003000U,
      0x00000000U, 0x00000000U, 0x00000100U, 0x00000000U, 0x00000000U, 0x00000100U,
      0x00000100U, 0x00000200U, 0x00000100U, 0x00000400U, 0x00000800U, 0x00000400U,
      0x00001000U, 0x00002000U, 0x00000200U, 0x00000000U, 0x00000000U, 0x00000000U,
      0xFFFFF000U, 0xFFFFF000U, 0xFFFFFE00U, 0x00000080U, 0x00000200U, 0x00000006U,
      0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U,
      0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00040000U,
      0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U,
      0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U,
      0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000051U, 0x00000062U,
      0x0000006EU, 0x00000078U, 0x00000082U, 0x00000063U, 0x00000070U, 0x00000080U,
      0x00000094U, 0x000000B7U, 0x03030000U, 0x00000002U
    } },
  { {
      0x00000000U, 0x00000000U, 0x00004000U, 0x00000000U, 0x00000000U, 0x00005000U,
      0x00000000U, 0x00000000U, 0x00008000U, 0x00000000U, 0x00000000U, 0x00004000U,
      0x00000000U, 0x00000000U, 0x00003000U, 0x00000000U, 0x00000000U, 0x00000000U,
      0x00000000U, 0x00000000U, 0x00005000U, 0x00000000U, 0x00000000U, 0x00000100U,
      0x00000000U, 0x00000000U, 0x00006000U, 0x00000000U, 0x00000000U, 0x00006000U,
      0x00000000U, 0x00000000U, 0x00004000U, 0x00000000U, 0x00000000U, 0x00005000U,
      0x00000000U, 0x00000000U, 0x00008000U, 0x00000000U, 0x00000000U, 0x00004000U,
      0x00000000U, 0x00000000U, 0x00003000U, 0x00000000U, 0x00000000U, 0x00000000U,
      0x00000000U, 0x00000000U, 0x00005000U, 0x00000000U, 0x00000000U, 0x00000100U,
      0x00000000U, 0x00000000U, 0x00006000U, 0x00000000U, 0x00000000U, 0x00006000U,
      0x00000100U, 0x00000400U, 0x00000080U, 0x00000800U, 0x00000000U, 0x00000100U,
      0x00000800U, 0x00000800U, 0x00000200U, 0x00000000U, 0x00000000U, 0x00000000U,
      0x00000000U, 0xFFFFF800U, 0xFFFFFF00U, 0x00000400U, 0x00000100U, 0x00000002U,
      0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U,
      0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00050000U,
      0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U,
      0x00000000U, 0x00000000U, 0x00000000U, 0x01000000U, 0x00000000U, 0x00000000U,
      0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000054U, 0x0000005FU,
      0x00000069U, 0x00000073U, 0x0000007DU, 0x00000064U, 0x00000072U, 0x00000084U,
      0x0000009AU, 0x000000BFU, 0x04020000U, 0x00000003U
    } },
  { {
      0x00000000U, 0x00000000U, 0x00006000U, 0x00000000U, 0x00000000U, 0x00000100U,
      0x00000000U, 0x00000000U, 0x00007000U, 0x00000000U, 0x00000000U, 0x00008000U,
      0x00000000U, 0x00000000U, 0x00007000U, 0x00000000U, 0x00000000U, 0x00004000U,
      0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00003000U,
      0x00000000U, 0x00000000U, 0x00005000U, 0x00000000U, 0x00000000U, 0x00004000U,
      0x00000000U, 0x00000000U, 0x00006000U, 0x00000000U, 0x00000000U, 0x00000100U,
      0x00000000U, 0x00000000U, 0x00007000U, 0x00000000U, 0x00000000U, 0x00008000U,
      0x00000000U, 0x00000000U, 0x00007000U, 0x00000000U, 0x00000000U, 0x00004000U,
      0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00003000U,
      0x00000000U, 0x00000000U, 0x00005000U, 0x00000000U, 0x00000000U, 0x00004000U,
      0x00000000U, 0x00000800U, 0x00000080U, 0x00002000U, 0x00000000U, 0xFFFFFF00U,
      0x00000000U, 0x00001000U, 0x00000200U, 0x00000000U, 0x00000000U, 0x00000000U,
      0x00000000U, 0xFFFFFC00U, 0x00000100U, 0x00000200U, 0x00000200U, 0x00000004U,
      0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U,
      0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00060000U,
      0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U,
      0x00000000U, 0x00000000U, 0x00000000U, 0x02000000U, 0x00000000U, 0x00000000U,
      0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x0000004DU, 0x00000057U,
      0x00000061U, 0x0000006BU, 0x00000075U, 0x00000061U, 0x00000070U, 0x00000081U,
      0x00000094U, 0x000000B7U, 0x03020000U, 0x00000003U
    } },
  { {
      0x00000000U, 0x00000000U, 0x00002000U, 0x00000000U, 0x00000000U, 0x00004000U,
      0x00000000U, 0x00000000U, 0x00005000U, 0x00000000U, 0x00000000U, 0x00001000U,
      0x00000000U, 0x00000000U, 0x00003000U, 0x00000000U, 0x00000000U, 0x00000100U,
      0x00000000U, 0x00000000U, 0x00004000U, 0x00000000U, 0x00000000U, 0x00000000U,
      0x00000000U, 0x00000000U, 0x00008000U, 0x00000000U, 0x00000000U, 0x00000100U,
      0x00000000U, 0x00000000U, 0x00002000U, 0x00000000U, 0x00000000U, 0x00004000U,
      0x00000000U, 0x00000000U, 0x00005000U, 0x00000000U, 0x00000000U, 0x00001000U,
      0x00000000U, 0x00000000U, 0x00003000U, 0x00000000U, 0x00000000U, 0x00000100U,
      0x00000000U, 0x00000000U, 0x00004000U, 0x00000000U, 0x00000000U, 0x00000000U,
      0x00000000U, 0x00000000U, 0x00008000U, 0x00000000U, 0x00000000U, 0x00000100U,
      0x00000100U, 0x00000200U, 0x00000080U, 0x00000800U, 0x00001000U, 0x00000400U,
      0x00000400U, 0x00000400U, 0x00000100U, 0x00000000U, 0x00000000U, 0x00000000U,
      0x00000000U, 0xFFFFFE00U, 0xFFFFFF80U, 0x00000200U, 0x00000080U, 0x00000004U,
      0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U,
      0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00070000U,
      0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U,
      0x00000000U, 0x00000000U, 0x00000000U, 0x01000000U, 0x00000000U, 0x00000000U,
      0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000057U, 0x00000069U,
      0x00000078U, 0x00000082U, 0x0000008CU, 0x00000060U, 0x0000006EU, 0x0000007DU,
      0x00000093U, 0x000000BBU, 0x04010000U, 0x00000003U
    } },
  { {
      0x00000000U, 0x00000000U, 0x00007000U, 0x00000000U, 0x00000000U, 0x00005000U,
      0x00000000U, 0x00000000U, 0x00007000U, 0x00000000U, 0x00000000U, 0x00006000U,
      0x00000000U, 0x00000000U, 0x00000100U, 0x00000000U, 0x00000000U, 0x00008000U,
      0x00000000U, 0x00000000U, 0x00006000U, 0x00000000U, 0x00000000U, 0x00004000U,
      0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00005000U,
      0x00000000U, 0x00000000U, 0x00007000U, 0x00000000U, 0x00000000U, 0x00005000U,
      0x00000000U, 0x00000000U, 0x00007000U, 0x00000000U, 0x00000000U, 0x00006000U,
      0x00000000U, 0x00000000U, 0x00000100U, 0x00000000U, 0x00000000U, 0x00008000U,
      0x00000000U, 0x00000000U, 0x00006000U, 0x00000000U, 0x00000000U, 0x00004000U,
      0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00005000U,
      0x00000400U, 0x00000400U, 0x00000100U, 0x00001000U, 0x00000000U, 0xFFFFFC00U,
      0x00002000U, 0x00002000U, 0x00000400U, 0x00000000U, 0x00000000U, 0x00000000U,
      0x00000000U, 0xFFFFF800U, 0xFFFFFF00U, 0x00000080U, 0x00000100U, 0x00000008U,
      0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U,
      0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00080000U,
      0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U,
      0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U,
      0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000046U, 0x00000052U,
      0x0000005FU, 0x00000069U, 0x0000006EU, 0x0000005FU, 0x00000069U, 0x0000007BU,
      0x00000094U, 0x000000B9U, 0x03020000U, 0x00000004U
    } },
  { {
      0x00000000U, 0x00000000U, 0x00003F00U, 0x00000000U, 0x00000000U, 0x00003F00U,
      0x00000000U, 0x00000000U, 0x00003F00U, 0x00000000U, 0x00000000U, 0x00003F00U,
      0x00000000U, 0x00000000U, 0x00003F00U, 0x00000000U, 0x00000000U, 0x00003F00U,
      0x00000000U, 0x00000000U, 0x00003F00U, 0x00000000U, 0x00000000U, 0x00003F00U,
      0x00000000U, 0x00000000U, 0x00003F00U, 0x00000000U, 0x00000000U, 0x00003F00U,
      0x00000000U, 0x00000000U, 0x00003F00U, 0x00000000U, 0x00000000U, 0x00003F00U,
      0x00000000U, 0x00000000U, 0x00003F00U, 0x00000000U, 0x00000000U, 0x00003F00U,
      0x00000000U, 0x00000000U, 0x00003F00U, 0x00000000U, 0x00000000U, 0x00003F00U,
      0x00000000U, 0x00000000U, 0x00003F00U, 0x00000000U, 0x00000000U, 0x00003F00U,
      0x00000000U, 0x00000000U, 0x00003F00U, 0x00000000U, 0x00000000U, 0x00000000U,
      0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U,
      0x00000200U, 0x00000800U, 0x00000200U, 0x00000000U, 0x00000000U, 0x00000000U,
      0x00000000U, 0x00000000U, 0x00000000U, 0x00000400U, 0x00000000U, 0x00000080U,
      0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U,
      0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00090000U,
      0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U,
      0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U,
      0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U,
      0x00000000U, 0x00000000U, 0x00000000U, 0x00000078U, 0x00000082U, 0x0000008CU,
      0x000000A0U, 0x000000AFU, 0x7F7F0000U, 0x00000004U
    } }
};

RiderUpdateCallback rider_update_handlers[RIDER_UPDATE_HANDLER_COUNT] =
{
  rider_update_mode_0,
  rider_update_mode_1,
  rider_update_mode_2,
  rider_update_mode_3,
  rider_update_mode_4,
  rider_update_mode_5,
  rider_update_mode_6,
  rider_update_mode_7,
  rider_update_mode_8,
  rider_update_mode_9,
  0
};

int rider_pacing_factor_adjustments[20] =
{
  15, 15, 15, 15, 15, 14, 13, 12, 11, 10,
  9, 8, 7, 6, 4, 2, 0, 0, -1, -1
};

int rider_contact_scale_by_mode[4] = { 0x100, 0x140, 0x180, 0x40 };

int rider_contact_delay_thresholds[5] = { 4, 8, 25, 64, 256 };

InputThreadState input_thread_state =
{
  1, { 0, 0, 0 },
  1, { 0, 0, 0 },
  0, { 0, 0, 0 },
  0,
  0,
  0,
  { 0x01000000U, 1, { 0, 0, 0 }, 2 },
  { 0x00800000U, 1, { 0, 0, 0 }, 2 },
  0,
  0,
  0,
  0,
  0
};

OpponentChoiceLevel opponent_choice_levels[OPPONENT_CHOICE_LEVEL_COUNT] =
{
  { {
      { { { 828, 829 }, { 836, 837 }, { 832, 833 }, { 830, 831 }, { 838, 839 }, { 834, 835 } },
        { { 2, 4 }, { 2, 4 }, { 2, 4 } }, { 0, 0 } },
      { { { 502, 503 }, { 510, 511 }, { 506, 507 }, { 504, 505 }, { 512, 513 }, { 508, 509 } },
        { { 7, 3 }, { 7, 3 }, { 7, 3 } }, { 0, 0 } },
      { { { 763, 764 }, { 771, 772 }, { 767, 768 }, { 765, 766 }, { 773, 774 }, { 769, 770 } },
        { { 6, 1 }, { 6, 1 }, { 6, 1 } }, { 0, 0 } },
      { { { 633, 634 }, { 641, 642 }, { 637, 638 }, { 635, 636 }, { 643, 644 }, { 639, 640 } },
        { { 8, 6 }, { 8, 6 }, { 8, 6 } }, { 0, 0 } },
      { { { 568, 569 }, { 576, 577 }, { 572, 573 }, { 570, 571 }, { 578, 579 }, { 574, 575 } },
        { { 7, 2 }, { 7, 2 }, { 7, 2 } }, { 0, 0 } },
      { { { 698, 699 }, { 706, 707 }, { 702, 703 }, { 700, 701 }, { 708, 709 }, { 704, 705 } },
        { { 3, 1 }, { 3, 1 }, { 3, 1 } }, { 0, 0 } },
      { { { 437, 438 }, { 445, 446 }, { 441, 442 }, { 439, 440 }, { 447, 448 }, { 443, 444 } },
        { { 8, 5 }, { 8, 5 }, { 8, 5 } }, { 0, 0 } },
      { { { 372, 373 }, { 380, 381 }, { 376, 377 }, { 374, 375 }, { 382, 383 }, { 378, 379 } },
        { { 5, 4 }, { 5, 4 }, { 5, 4 } }, { 0, 0 } }
    } },
  { {
      { { { 840, 841 }, { 848, 849 }, { 844, 845 }, { 842, 843 }, { 850, 851 }, { 846, 847 } },
        { { 2, 4 }, { 2, 4 }, { 2, 4 } }, { 0, 0 } },
      { { { 514, 515 }, { 522, 523 }, { 518, 519 }, { 516, 517 }, { 524, 525 }, { 520, 521 } },
        { { 7, 3 }, { 7, 3 }, { 7, 3 } }, { 0, 0 } },
      { { { 775, 776 }, { 783, 784 }, { 779, 780 }, { 777, 778 }, { 785, 786 }, { 781, 782 } },
        { { 6, 1 }, { 6, 1 }, { 6, 1 } }, { 0, 0 } },
      { { { 645, 646 }, { 653, 654 }, { 649, 650 }, { 647, 648 }, { 655, 656 }, { 651, 652 } },
        { { 8, 6 }, { 8, 6 }, { 8, 6 } }, { 0, 0 } },
      { { { 580, 581 }, { 588, 589 }, { 584, 585 }, { 582, 583 }, { 590, 591 }, { 586, 587 } },
        { { 7, 2 }, { 7, 2 }, { 7, 2 } }, { 0, 0 } },
      { { { 710, 711 }, { 718, 719 }, { 714, 715 }, { 712, 713 }, { 720, 721 }, { 716, 717 } },
        { { 3, 1 }, { 3, 1 }, { 3, 1 } }, { 0, 0 } },
      { { { 449, 450 }, { 457, 458 }, { 453, 454 }, { 451, 452 }, { 459, 460 }, { 455, 456 } },
        { { 8, 5 }, { 8, 5 }, { 8, 5 } }, { 0, 0 } },
      { { { 384, 385 }, { 392, 393 }, { 388, 389 }, { 386, 387 }, { 394, 395 }, { 390, 391 } },
        { { 5, 4 }, { 5, 4 }, { 5, 4 } }, { 0, 0 } }
    } },
  { {
      { { { 852, 853 }, { 860, 861 }, { 856, 857 }, { 854, 855 }, { 862, 863 }, { 858, 859 } },
        { { 2, 4 }, { 2, 4 }, { 2, 4 } }, { 0, 0 } },
      { { { 526, 527 }, { 534, 535 }, { 530, 531 }, { 528, 529 }, { 536, 537 }, { 532, 533 } },
        { { 7, 3 }, { 7, 3 }, { 7, 3 } }, { 0, 0 } },
      { { { 787, 788 }, { 795, 796 }, { 791, 792 }, { 789, 790 }, { 797, 798 }, { 793, 794 } },
        { { 6, 1 }, { 6, 1 }, { 6, 1 } }, { 0, 0 } },
      { { { 657, 658 }, { 665, 666 }, { 661, 662 }, { 659, 660 }, { 667, 668 }, { 663, 664 } },
        { { 8, 6 }, { 8, 6 }, { 8, 6 } }, { 0, 0 } },
      { { { 592, 593 }, { 600, 601 }, { 596, 597 }, { 594, 595 }, { 602, 603 }, { 598, 599 } },
        { { 7, 2 }, { 7, 2 }, { 7, 2 } }, { 0, 0 } },
      { { { 722, 723 }, { 730, 731 }, { 726, 727 }, { 724, 725 }, { 732, 733 }, { 728, 729 } },
        { { 3, 1 }, { 3, 1 }, { 3, 1 } }, { 0, 0 } },
      { { { 461, 462 }, { 469, 470 }, { 465, 466 }, { 463, 464 }, { 471, 472 }, { 467, 468 } },
        { { 8, 5 }, { 8, 5 }, { 8, 5 } }, { 0, 0 } },
      { { { 396, 397 }, { 404, 405 }, { 400, 401 }, { 398, 399 }, { 406, 407 }, { 402, 403 } },
        { { 5, 4 }, { 5, 4 }, { 5, 4 } }, { 0, 0 } }
    } },
  { {
      { { { 864, 865 }, { 872, 873 }, { 868, 869 }, { 866, 867 }, { 874, 875 }, { 870, 871 } },
        { { 2, 4 }, { 2, 4 }, { 2, 4 } }, { 0, 0 } },
      { { { 538, 539 }, { 546, 547 }, { 542, 543 }, { 540, 541 }, { 548, 549 }, { 544, 545 } },
        { { 7, 3 }, { 7, 3 }, { 7, 3 } }, { 0, 0 } },
      { { { 799, 800 }, { 807, 808 }, { 803, 804 }, { 801, 802 }, { 809, 810 }, { 805, 806 } },
        { { 6, 1 }, { 6, 1 }, { 6, 1 } }, { 0, 0 } },
      { { { 669, 670 }, { 677, 678 }, { 673, 674 }, { 671, 672 }, { 679, 680 }, { 675, 676 } },
        { { 8, 6 }, { 8, 6 }, { 8, 6 } }, { 0, 0 } },
      { { { 604, 605 }, { 612, 613 }, { 608, 609 }, { 606, 607 }, { 614, 615 }, { 610, 611 } },
        { { 7, 2 }, { 7, 2 }, { 7, 2 } }, { 0, 0 } },
      { { { 734, 735 }, { 742, 743 }, { 738, 739 }, { 736, 737 }, { 744, 745 }, { 740, 741 } },
        { { 3, 1 }, { 3, 1 }, { 3, 1 } }, { 0, 0 } },
      { { { 473, 474 }, { 481, 482 }, { 477, 478 }, { 475, 476 }, { 483, 484 }, { 479, 480 } },
        { { 8, 5 }, { 8, 5 }, { 8, 5 } }, { 0, 0 } },
      { { { 408, 409 }, { 416, 417 }, { 412, 413 }, { 410, 411 }, { 418, 419 }, { 414, 415 } },
        { { 5, 4 }, { 5, 4 }, { 5, 4 } }, { 0, 0 } }
    } },
  { {
      { { { 876, 877 }, { 884, 885 }, { 880, 881 }, { 878, 879 }, { 886, 887 }, { 882, 883 } },
        { { 2, 4 }, { 2, 4 }, { 2, 4 } }, { 0, 0 } },
      { { { 550, 551 }, { 558, 559 }, { 554, 555 }, { 552, 553 }, { 560, 561 }, { 556, 557 } },
        { { 7, 3 }, { 7, 3 }, { 7, 3 } }, { 0, 0 } },
      { { { 811, 812 }, { 819, 820 }, { 815, 816 }, { 813, 814 }, { 821, 822 }, { 817, 818 } },
        { { 6, 1 }, { 6, 1 }, { 6, 1 } }, { 0, 0 } },
      { { { 681, 682 }, { 689, 690 }, { 685, 686 }, { 683, 684 }, { 691, 692 }, { 687, 688 } },
        { { 8, 6 }, { 8, 6 }, { 8, 6 } }, { 0, 0 } },
      { { { 616, 617 }, { 624, 625 }, { 620, 621 }, { 618, 619 }, { 626, 627 }, { 622, 623 } },
        { { 7, 2 }, { 7, 2 }, { 7, 2 } }, { 0, 0 } },
      { { { 746, 747 }, { 754, 755 }, { 750, 751 }, { 748, 749 }, { 756, 757 }, { 752, 753 } },
        { { 3, 1 }, { 3, 1 }, { 3, 1 } }, { 0, 0 } },
      { { { 485, 486 }, { 493, 494 }, { 489, 490 }, { 487, 488 }, { 495, 496 }, { 491, 492 } },
        { { 8, 5 }, { 8, 5 }, { 8, 5 } }, { 0, 0 } },
      { { { 420, 421 }, { 428, 429 }, { 424, 425 }, { 422, 423 }, { 430, 431 }, { 426, 427 } },
        { { 5, 4 }, { 5, 4 }, { 5, 4 } }, { 0, 0 } }
    } }
};

FrontEndCelDescriptor front_end_cel_group_000[2] =
{
  { 12, -1, 0, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_001[2] =
{
  { 13, -1, 0, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_002[2] =
{
  { 14, -1, 0, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_003[14] =
{
  { 16, -1, 0, { 0, 0, 0 } },
  { 17, -1, 0, { 0, 0, 0 } },
  { 18, -1, 0, { 0, 0, 0 } },
  { 19, -1, 0, { 0, 0, 0 } },
  { 20, -1, 0, { 0, 0, 0 } },
  { 21, -1, 0, { 0, 0, 0 } },
  { 22, -1, 0, { 0, 0, 0 } },
  { 23, -1, 0, { 0, 0, 0 } },
  { 24, -1, 0, { 0, 0, 0 } },
  { 25, -1, 0, { 0, 0, 0 } },
  { 26, -1, 0, { 0, 0, 0 } },
  { 27, -1, 0, { 0, 0, 0 } },
  { 28, -1, 0, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_004[2] =
{
  { 15, -1, 0, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_005[2] =
{
  { 154, -1, 0, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_006[2] =
{
  { 155, -1, 0, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_007[5] =
{
  { 0, -1, 0, { 0, 0, 0 } },
  { 1, -1, 0, { 0, 0, 0 } },
  { 4, -1, 0, { 0, 0, 0 } },
  { 11, 1, 2, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_008[6] =
{
  { 0, -1, 0, { 0, 0, 0 } },
  { 2, -1, 0, { 0, 0, 0 } },
  { 3, -1, 0, { 0, 0, 0 } },
  { 4, -1, 0, { 0, 0, 0 } },
  { 11, 2, 2, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_009[5] =
{
  { 0, -1, 0, { 0, 0, 0 } },
  { 1, -1, 0, { 0, 0, 0 } },
  { 10, -1, 0, { 0, 0, 0 } },
  { 11, 1, 1, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_010[6] =
{
  { 0, -1, 0, { 0, 0, 0 } },
  { 8, -1, 0, { 0, 0, 0 } },
  { 9, -1, 0, { 0, 0, 0 } },
  { 10, -1, 0, { 0, 0, 0 } },
  { 11, 8, 1, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_011[5] =
{
  { 0, -1, 0, { 0, 0, 0 } },
  { 1, -1, 0, { 0, 0, 0 } },
  { 7, -1, 0, { 0, 0, 0 } },
  { 11, 1, 3, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_012[6] =
{
  { 0, -1, 0, { 0, 0, 0 } },
  { 5, -1, 0, { 0, 0, 0 } },
  { 6, -1, 0, { 0, 0, 0 } },
  { 7, -1, 0, { 0, 0, 0 } },
  { 11, 5, 3, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_013[2] =
{
  { 226, -1, 1, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_014[6] =
{
  { 188, -1, 1, { 0, 0, 0 } },
  { 189, -1, 1, { 0, 0, 0 } },
  { 192, -1, 1, { 0, 0, 0 } },
  { 191, -1, 1, { 0, 0, 0 } },
  { 11, 189, 1, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_015[8] =
{
  { 188, -1, 1, { 0, 0, 0 } },
  { 192, -1, 1, { 0, 0, 0 } },
  { 193, -1, 1, { 0, 0, 0 } },
  { 194, -1, 1, { 0, 0, 0 } },
  { 195, -1, 1, { 0, 0, 0 } },
  { 225, -1, 1, { 0, 0, 0 } },
  { 11, 193, 1, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_016[6] =
{
  { 188, -1, 1, { 0, 0, 0 } },
  { 189, -1, 1, { 0, 0, 0 } },
  { 197, -1, 1, { 0, 0, 0 } },
  { 191, -1, 1, { 0, 0, 0 } },
  { 11, 189, 2, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_017[8] =
{
  { 188, -1, 1, { 0, 0, 0 } },
  { 197, -1, 1, { 0, 0, 0 } },
  { 198, -1, 1, { 0, 0, 0 } },
  { 199, -1, 1, { 0, 0, 0 } },
  { 200, -1, 1, { 0, 0, 0 } },
  { 225, -1, 1, { 0, 0, 0 } },
  { 11, 198, 2, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_018[6] =
{
  { 188, -1, 1, { 0, 0, 0 } },
  { 189, -1, 1, { 0, 0, 0 } },
  { 202, -1, 1, { 0, 0, 0 } },
  { 191, -1, 1, { 0, 0, 0 } },
  { 11, 189, 3, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_019[8] =
{
  { 188, -1, 1, { 0, 0, 0 } },
  { 202, -1, 1, { 0, 0, 0 } },
  { 203, -1, 1, { 0, 0, 0 } },
  { 204, -1, 1, { 0, 0, 0 } },
  { 205, -1, 1, { 0, 0, 0 } },
  { 225, -1, 1, { 0, 0, 0 } },
  { 11, 203, 3, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_020[6] =
{
  { 188, -1, 1, { 0, 0, 0 } },
  { 189, -1, 1, { 0, 0, 0 } },
  { 207, -1, 1, { 0, 0, 0 } },
  { 191, -1, 1, { 0, 0, 0 } },
  { 11, 189, 4, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_021[8] =
{
  { 188, -1, 1, { 0, 0, 0 } },
  { 207, -1, 1, { 0, 0, 0 } },
  { 208, -1, 1, { 0, 0, 0 } },
  { 209, -1, 1, { 0, 0, 0 } },
  { 210, -1, 1, { 0, 0, 0 } },
  { 225, -1, 1, { 0, 0, 0 } },
  { 11, 208, 4, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_022[6] =
{
  { 188, -1, 1, { 0, 0, 0 } },
  { 189, -1, 1, { 0, 0, 0 } },
  { 212, -1, 1, { 0, 0, 0 } },
  { 191, -1, 1, { 0, 0, 0 } },
  { 11, 189, 5, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_023[8] =
{
  { 188, -1, 1, { 0, 0, 0 } },
  { 212, -1, 1, { 0, 0, 0 } },
  { 213, -1, 1, { 0, 0, 0 } },
  { 214, -1, 1, { 0, 0, 0 } },
  { 215, -1, 1, { 0, 0, 0 } },
  { 225, -1, 1, { 0, 0, 0 } },
  { 11, 213, 5, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_024[6] =
{
  { 188, -1, 1, { 0, 0, 0 } },
  { 189, -1, 1, { 0, 0, 0 } },
  { 217, -1, 1, { 0, 0, 0 } },
  { 191, -1, 1, { 0, 0, 0 } },
  { 11, 189, 6, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_025[8] =
{
  { 188, -1, 1, { 0, 0, 0 } },
  { 217, -1, 1, { 0, 0, 0 } },
  { 218, -1, 1, { 0, 0, 0 } },
  { 219, -1, 1, { 0, 0, 0 } },
  { 220, -1, 1, { 0, 0, 0 } },
  { 225, -1, 1, { 0, 0, 0 } },
  { 11, 218, 6, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_026[6] =
{
  { 188, -1, 1, { 0, 0, 0 } },
  { 189, -1, 1, { 0, 0, 0 } },
  { 192, -1, 1, { 0, 0, 0 } },
  { 190, -1, 1, { 0, 0, 0 } },
  { 11, 189, 1, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_027[8] =
{
  { 188, -1, 1, { 0, 0, 0 } },
  { 192, -1, 1, { 0, 0, 0 } },
  { 193, -1, 1, { 0, 0, 0 } },
  { 194, -1, 1, { 0, 0, 0 } },
  { 196, -1, 1, { 0, 0, 0 } },
  { 11, 193, 1, { 0, 0, 0 } },
  { 225, -1, 1, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_028[6] =
{
  { 188, -1, 1, { 0, 0, 0 } },
  { 189, -1, 1, { 0, 0, 0 } },
  { 197, -1, 1, { 0, 0, 0 } },
  { 190, -1, 1, { 0, 0, 0 } },
  { 11, 189, 2, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_029[8] =
{
  { 188, -1, 1, { 0, 0, 0 } },
  { 197, -1, 1, { 0, 0, 0 } },
  { 198, -1, 1, { 0, 0, 0 } },
  { 199, -1, 1, { 0, 0, 0 } },
  { 201, -1, 1, { 0, 0, 0 } },
  { 11, 198, 2, { 0, 0, 0 } },
  { 225, -1, 1, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_030[6] =
{
  { 188, -1, 1, { 0, 0, 0 } },
  { 189, -1, 1, { 0, 0, 0 } },
  { 202, -1, 1, { 0, 0, 0 } },
  { 190, -1, 1, { 0, 0, 0 } },
  { 11, 189, 3, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_031[8] =
{
  { 188, -1, 1, { 0, 0, 0 } },
  { 202, -1, 1, { 0, 0, 0 } },
  { 203, -1, 1, { 0, 0, 0 } },
  { 204, -1, 1, { 0, 0, 0 } },
  { 206, -1, 1, { 0, 0, 0 } },
  { 11, 203, 3, { 0, 0, 0 } },
  { 225, -1, 1, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_032[6] =
{
  { 188, -1, 1, { 0, 0, 0 } },
  { 189, -1, 1, { 0, 0, 0 } },
  { 207, -1, 1, { 0, 0, 0 } },
  { 190, -1, 1, { 0, 0, 0 } },
  { 11, 189, 4, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_033[8] =
{
  { 188, -1, 1, { 0, 0, 0 } },
  { 207, -1, 1, { 0, 0, 0 } },
  { 208, -1, 1, { 0, 0, 0 } },
  { 209, -1, 1, { 0, 0, 0 } },
  { 211, -1, 1, { 0, 0, 0 } },
  { 11, 208, 4, { 0, 0, 0 } },
  { 225, -1, 1, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_034[6] =
{
  { 188, -1, 1, { 0, 0, 0 } },
  { 189, -1, 1, { 0, 0, 0 } },
  { 212, -1, 1, { 0, 0, 0 } },
  { 190, -1, 1, { 0, 0, 0 } },
  { 11, 189, 5, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_035[8] =
{
  { 188, -1, 1, { 0, 0, 0 } },
  { 212, -1, 1, { 0, 0, 0 } },
  { 213, -1, 1, { 0, 0, 0 } },
  { 214, -1, 1, { 0, 0, 0 } },
  { 216, -1, 1, { 0, 0, 0 } },
  { 11, 213, 5, { 0, 0, 0 } },
  { 225, -1, 1, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_036[6] =
{
  { 188, -1, 1, { 0, 0, 0 } },
  { 189, -1, 1, { 0, 0, 0 } },
  { 221, -1, 1, { 0, 0, 0 } },
  { 190, -1, 1, { 0, 0, 0 } },
  { 11, 189, 6, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_037[8] =
{
  { 188, -1, 1, { 0, 0, 0 } },
  { 221, -1, 1, { 0, 0, 0 } },
  { 222, -1, 1, { 0, 0, 0 } },
  { 223, -1, 1, { 0, 0, 0 } },
  { 224, -1, 1, { 0, 0, 0 } },
  { 11, 222, 6, { 0, 0, 0 } },
  { 225, -1, 1, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_038[6] =
{
  { 156, -1, 0, { 0, 0, 0 } },
  { 157, -1, 0, { 0, 0, 0 } },
  { 158, -1, 0, { 0, 0, 0 } },
  { 159, -1, 0, { 0, 0, 0 } },
  { 11, 157, 1, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_039[6] =
{
  { 160, -1, 0, { 0, 0, 0 } },
  { 161, -1, 0, { 0, 0, 0 } },
  { 162, -1, 0, { 0, 0, 0 } },
  { 163, -1, 0, { 0, 0, 0 } },
  { 11, 161, 2, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_040[6] =
{
  { 164, -1, 0, { 0, 0, 0 } },
  { 165, -1, 0, { 0, 0, 0 } },
  { 166, -1, 0, { 0, 0, 0 } },
  { 167, -1, 0, { 0, 0, 0 } },
  { 11, 165, 3, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_041[6] =
{
  { 168, -1, 0, { 0, 0, 0 } },
  { 169, -1, 0, { 0, 0, 0 } },
  { 170, -1, 0, { 0, 0, 0 } },
  { 171, -1, 0, { 0, 0, 0 } },
  { 11, 169, 4, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_042[6] =
{
  { 172, -1, 0, { 0, 0, 0 } },
  { 173, -1, 0, { 0, 0, 0 } },
  { 174, -1, 0, { 0, 0, 0 } },
  { 175, -1, 0, { 0, 0, 0 } },
  { 11, 173, 5, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_043[6] =
{
  { 176, -1, 0, { 0, 0, 0 } },
  { 177, -1, 0, { 0, 0, 0 } },
  { 178, -1, 0, { 0, 0, 0 } },
  { 179, -1, 0, { 0, 0, 0 } },
  { 11, 177, 6, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_044[6] =
{
  { 180, -1, 0, { 0, 0, 0 } },
  { 181, -1, 0, { 0, 0, 0 } },
  { 182, -1, 0, { 0, 0, 0 } },
  { 183, -1, 0, { 0, 0, 0 } },
  { 11, 181, 7, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_045[6] =
{
  { 184, -1, 0, { 0, 0, 0 } },
  { 185, -1, 0, { 0, 0, 0 } },
  { 186, -1, 0, { 0, 0, 0 } },
  { 187, -1, 0, { 0, 0, 0 } },
  { 11, 185, 8, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_046[4] =
{
  { 29, -1, 0, { 0, 0, 0 } },
  { 35, -1, 0, { 0, 0, 0 } },
  { 36, -1, 0, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_047[2] =
{
  { 29, -1, 0, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_048[3] =
{
  { 37, -1, 0, { 0, 0, 0 } },
  { 11, 35, 1, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_049[5] =
{
  { 40, -1, 0, { 0, 0, 0 } },
  { 41, -1, 0, { 0, 0, 0 } },
  { 42, -1, 0, { 0, 0, 0 } },
  { 11, 35, 1, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_050[5] =
{
  { 40, -1, 0, { 0, 0, 0 } },
  { 99, -1, 0, { 0, 0, 0 } },
  { 38, -1, 0, { 0, 0, 0 } },
  { 11, 99, 1, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_051[5] =
{
  { 40, -1, 0, { 0, 0, 0 } },
  { 99, -1, 0, { 0, 0, 0 } },
  { 39, -1, 0, { 0, 0, 0 } },
  { 11, 99, 2, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_052[3] =
{
  { 37, -1, 0, { 0, 0, 0 } },
  { 11, 35, 2, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_053[5] =
{
  { 43, -1, 0, { 0, 0, 0 } },
  { 44, -1, 0, { 0, 0, 0 } },
  { 45, -1, 0, { 0, 0, 0 } },
  { 11, 35, 2, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_054[3] =
{
  { 37, -1, 0, { 0, 0, 0 } },
  { 11, 35, 3, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_055[5] =
{
  { 46, -1, 0, { 0, 0, 0 } },
  { 47, -1, 0, { 0, 0, 0 } },
  { 48, -1, 0, { 0, 0, 0 } },
  { 11, 35, 3, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_056[3] =
{
  { 37, -1, 0, { 0, 0, 0 } },
  { 11, 35, 4, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_057[5] =
{
  { 49, -1, 0, { 0, 0, 0 } },
  { 50, -1, 0, { 0, 0, 0 } },
  { 51, -1, 0, { 0, 0, 0 } },
  { 11, 35, 4, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_058[3] =
{
  { 37, -1, 0, { 0, 0, 0 } },
  { 11, 35, 5, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_059[5] =
{
  { 53, -1, 0, { 0, 0, 0 } },
  { 54, -1, 0, { 0, 0, 0 } },
  { 55, -1, 0, { 0, 0, 0 } },
  { 11, 35, 5, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_060[3] =
{
  { 37, -1, 0, { 0, 0, 0 } },
  { 11, 35, 6, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_061[5] =
{
  { 56, -1, 0, { 0, 0, 0 } },
  { 57, -1, 0, { 0, 0, 0 } },
  { 58, -1, 0, { 0, 0, 0 } },
  { 11, 35, 6, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_062[3] =
{
  { 37, -1, 0, { 0, 0, 0 } },
  { 11, 35, 7, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_063[5] =
{
  { 59, -1, 0, { 0, 0, 0 } },
  { 60, -1, 0, { 0, 0, 0 } },
  { 61, -1, 0, { 0, 0, 0 } },
  { 11, 35, 7, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_064[3] =
{
  { 68, -1, 0, { 0, 0, 0 } },
  { 69, -1, 0, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_065[3] =
{
  { 70, -1, 0, { 0, 0, 0 } },
  { 71, -1, 0, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_066[3] =
{
  { 72, -1, 0, { 0, 0, 0 } },
  { 73, -1, 0, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_067[3] =
{
  { 74, -1, 0, { 0, 0, 0 } },
  { 75, -1, 0, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_068[3] =
{
  { 76, -1, 0, { 0, 0, 0 } },
  { 77, -1, 0, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_069[3] =
{
  { 78, -1, 0, { 0, 0, 0 } },
  { 79, -1, 0, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_070[3] =
{
  { 80, -1, 0, { 0, 0, 0 } },
  { 81, -1, 0, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_071[3] =
{
  { 82, -1, 0, { 0, 0, 0 } },
  { 83, -1, 0, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_072[3] =
{
  { 84, -1, 0, { 0, 0, 0 } },
  { 85, -1, 0, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_073[3] =
{
  { 86, -1, 0, { 0, 0, 0 } },
  { 87, -1, 0, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_074[3] =
{
  { 88, -1, 0, { 0, 0, 0 } },
  { 89, -1, 0, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_075[3] =
{
  { 90, -1, 0, { 0, 0, 0 } },
  { 91, -1, 0, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_076[3] =
{
  { 92, -1, 0, { 0, 0, 0 } },
  { 93, -1, 0, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_077[3] =
{
  { 94, -1, 0, { 0, 0, 0 } },
  { 95, -1, 0, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_078[3] =
{
  { 37, -1, 0, { 0, 0, 0 } },
  { 11, 35, 8, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_079[5] =
{
  { 62, -1, 0, { 0, 0, 0 } },
  { 63, -1, 0, { 0, 0, 0 } },
  { 64, -1, 0, { 0, 0, 0 } },
  { 11, 35, 8, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_080[3] =
{
  { 37, -1, 0, { 0, 0, 0 } },
  { 11, 35, 9, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_081[5] =
{
  { 65, -1, 0, { 0, 0, 0 } },
  { 66, -1, 0, { 0, 0, 0 } },
  { 67, -1, 0, { 0, 0, 0 } },
  { 11, 35, 9, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_082[7] =
{
  { 117, -1, 0, { 0, 0, 0 } },
  { 118, -1, 0, { 0, 0, 0 } },
  { 119, -1, 0, { 0, 0, 0 } },
  { 120, -1, 0, { 0, 0, 0 } },
  { 121, -1, 0, { 0, 0, 0 } },
  { 11, 117, 1, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_083[7] =
{
  { 117, -1, 0, { 0, 0, 0 } },
  { 118, -1, 0, { 0, 0, 0 } },
  { 119, -1, 0, { 0, 0, 0 } },
  { 120, -1, 0, { 0, 0, 0 } },
  { 122, -1, 0, { 0, 0, 0 } },
  { 11, 120, 1, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_084[7] =
{
  { 117, -1, 0, { 0, 0, 0 } },
  { 118, -1, 0, { 0, 0, 0 } },
  { 119, -1, 0, { 0, 0, 0 } },
  { 120, -1, 0, { 0, 0, 0 } },
  { 123, -1, 0, { 0, 0, 0 } },
  { 11, 120, 2, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_085[7] =
{
  { 117, -1, 0, { 0, 0, 0 } },
  { 118, -1, 0, { 0, 0, 0 } },
  { 124, -1, 0, { 0, 0, 0 } },
  { 125, -1, 0, { 0, 0, 0 } },
  { 126, -1, 0, { 0, 0, 0 } },
  { 11, 117, 2, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_086[7] =
{
  { 117, -1, 0, { 0, 0, 0 } },
  { 118, -1, 0, { 0, 0, 0 } },
  { 124, -1, 0, { 0, 0, 0 } },
  { 125, -1, 0, { 0, 0, 0 } },
  { 127, -1, 0, { 0, 0, 0 } },
  { 11, 125, 1, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_087[7] =
{
  { 117, -1, 0, { 0, 0, 0 } },
  { 118, -1, 0, { 0, 0, 0 } },
  { 128, -1, 0, { 0, 0, 0 } },
  { 129, -1, 0, { 0, 0, 0 } },
  { 130, -1, 0, { 0, 0, 0 } },
  { 11, 117, 3, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_088[7] =
{
  { 117, -1, 0, { 0, 0, 0 } },
  { 118, -1, 0, { 0, 0, 0 } },
  { 128, -1, 0, { 0, 0, 0 } },
  { 129, -1, 0, { 0, 0, 0 } },
  { 131, -1, 0, { 0, 0, 0 } },
  { 11, 129, 1, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_089[7] =
{
  { 117, -1, 0, { 0, 0, 0 } },
  { 118, -1, 0, { 0, 0, 0 } },
  { 128, -1, 0, { 0, 0, 0 } },
  { 129, -1, 0, { 0, 0, 0 } },
  { 132, -1, 0, { 0, 0, 0 } },
  { 11, 129, 2, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_090[7] =
{
  { 117, -1, 0, { 0, 0, 0 } },
  { 118, -1, 0, { 0, 0, 0 } },
  { 128, -1, 0, { 0, 0, 0 } },
  { 129, -1, 0, { 0, 0, 0 } },
  { 133, -1, 0, { 0, 0, 0 } },
  { 11, 129, 3, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_091[7] =
{
  { 117, -1, 0, { 0, 0, 0 } },
  { 118, -1, 0, { 0, 0, 0 } },
  { 134, -1, 0, { 0, 0, 0 } },
  { 135, -1, 0, { 0, 0, 0 } },
  { 136, -1, 0, { 0, 0, 0 } },
  { 11, 117, 4, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_092[7] =
{
  { 117, -1, 0, { 0, 0, 0 } },
  { 118, -1, 0, { 0, 0, 0 } },
  { 134, -1, 0, { 0, 0, 0 } },
  { 135, -1, 0, { 0, 0, 0 } },
  { 137, -1, 0, { 0, 0, 0 } },
  { 11, 135, 1, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_093[7] =
{
  { 117, -1, 0, { 0, 0, 0 } },
  { 118, -1, 0, { 0, 0, 0 } },
  { 134, -1, 0, { 0, 0, 0 } },
  { 135, -1, 0, { 0, 0, 0 } },
  { 138, -1, 0, { 0, 0, 0 } },
  { 11, 135, 2, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_094[7] =
{
  { 117, -1, 0, { 0, 0, 0 } },
  { 118, -1, 0, { 0, 0, 0 } },
  { 134, -1, 0, { 0, 0, 0 } },
  { 135, -1, 0, { 0, 0, 0 } },
  { 139, -1, 0, { 0, 0, 0 } },
  { 11, 135, 3, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_095[7] =
{
  { 117, -1, 0, { 0, 0, 0 } },
  { 118, -1, 0, { 0, 0, 0 } },
  { 134, -1, 0, { 0, 0, 0 } },
  { 135, -1, 0, { 0, 0, 0 } },
  { 140, -1, 0, { 0, 0, 0 } },
  { 11, 135, 4, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_096[7] =
{
  { 117, -1, 0, { 0, 0, 0 } },
  { 118, -1, 0, { 0, 0, 0 } },
  { 141, -1, 0, { 0, 0, 0 } },
  { 142, -1, 0, { 0, 0, 0 } },
  { 143, -1, 0, { 0, 0, 0 } },
  { 11, 117, 5, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_097[7] =
{
  { 117, -1, 0, { 0, 0, 0 } },
  { 118, -1, 0, { 0, 0, 0 } },
  { 141, -1, 0, { 0, 0, 0 } },
  { 142, -1, 0, { 0, 0, 0 } },
  { 144, -1, 0, { 0, 0, 0 } },
  { 11, 142, 1, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_098[7] =
{
  { 117, -1, 0, { 0, 0, 0 } },
  { 118, -1, 0, { 0, 0, 0 } },
  { 141, -1, 0, { 0, 0, 0 } },
  { 142, -1, 0, { 0, 0, 0 } },
  { 145, -1, 0, { 0, 0, 0 } },
  { 11, 142, 2, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_099[7] =
{
  { 117, -1, 0, { 0, 0, 0 } },
  { 118, -1, 0, { 0, 0, 0 } },
  { 146, -1, 0, { 0, 0, 0 } },
  { 147, -1, 0, { 0, 0, 0 } },
  { 148, -1, 0, { 0, 0, 0 } },
  { 11, 117, 6, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_100[7] =
{
  { 117, -1, 0, { 0, 0, 0 } },
  { 118, -1, 0, { 0, 0, 0 } },
  { 146, -1, 0, { 0, 0, 0 } },
  { 147, -1, 0, { 0, 0, 0 } },
  { 149, -1, 0, { 0, 0, 0 } },
  { 11, 147, 1, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_101[7] =
{
  { 117, -1, 0, { 0, 0, 0 } },
  { 118, -1, 0, { 0, 0, 0 } },
  { 146, -1, 0, { 0, 0, 0 } },
  { 147, -1, 0, { 0, 0, 0 } },
  { 150, -1, 0, { 0, 0, 0 } },
  { 11, 147, 2, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_102[5] =
{
  { 117, -1, 0, { 0, 0, 0 } },
  { 118, -1, 0, { 0, 0, 0 } },
  { 151, -1, 0, { 0, 0, 0 } },
  { 11, 117, 7, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_103[5] =
{
  { 46, -1, 0, { 0, 0, 0 } },
  { 96, -1, 0, { 0, 0, 0 } },
  { 97, -1, 1, { 0, 0, 0 } },
  { 11, 96, 2, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_104[5] =
{
  { 46, -1, 0, { 0, 0, 0 } },
  { 96, -1, 0, { 0, 0, 0 } },
  { 98, -1, 1, { 0, 0, 0 } },
  { 11, 96, 1, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_105[5] =
{
  { 49, -1, 0, { 0, 0, 0 } },
  { 99, -1, 0, { 0, 0, 0 } },
  { 100, -1, 0, { 0, 0, 0 } },
  { 11, 99, 1, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_106[5] =
{
  { 49, -1, 0, { 0, 0, 0 } },
  { 99, -1, 0, { 0, 0, 0 } },
  { 101, -1, 0, { 0, 0, 0 } },
  { 11, 99, 2, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_107[5] =
{
  { 53, -1, 0, { 0, 0, 0 } },
  { 102, -1, 0, { 0, 0, 0 } },
  { 103, -1, 1, { 0, 0, 0 } },
  { 11, 102, 1, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_108[5] =
{
  { 53, -1, 0, { 0, 0, 0 } },
  { 102, -1, 0, { 0, 0, 0 } },
  { 104, -1, 1, { 0, 0, 0 } },
  { 11, 102, 2, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_109[5] =
{
  { 53, -1, 0, { 0, 0, 0 } },
  { 102, -1, 0, { 0, 0, 0 } },
  { 105, -1, 1, { 0, 0, 0 } },
  { 11, 102, 3, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_110[5] =
{
  { 53, -1, 0, { 0, 0, 0 } },
  { 102, -1, 0, { 0, 0, 0 } },
  { 106, -1, 1, { 0, 0, 0 } },
  { 11, 102, 4, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_111[5] =
{
  { 53, -1, 0, { 0, 0, 0 } },
  { 102, -1, 0, { 0, 0, 0 } },
  { 107, -1, 1, { 0, 0, 0 } },
  { 11, 102, 5, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_112[5] =
{
  { 56, -1, 0, { 0, 0, 0 } },
  { 108, -1, 0, { 0, 0, 0 } },
  { 109, -1, 1, { 0, 0, 0 } },
  { 11, 108, 1, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_113[5] =
{
  { 56, -1, 0, { 0, 0, 0 } },
  { 108, -1, 0, { 0, 0, 0 } },
  { 110, -1, 1, { 0, 0, 0 } },
  { 11, 108, 2, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_114[7] =
{
  { 59, -1, 0, { 0, 0, 0 } },
  { 111, -1, 0, { 0, 0, 0 } },
  { 114, -1, 1, { 0, 0, 0 } },
  { 115, -1, 1, { 0, 0, 0 } },
  { 116, -1, 1, { 0, 0, 0 } },
  { 11, 111, 2, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_115[5] =
{
  { 59, -1, 0, { 0, 0, 0 } },
  { 111, -1, 0, { 0, 0, 0 } },
  { 112, -1, 1, { 0, 0, 0 } },
  { 11, 111, 1, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_116[5] =
{
  { 59, -1, 0, { 0, 0, 0 } },
  { 111, -1, 0, { 0, 0, 0 } },
  { 113, -1, 1, { 0, 0, 0 } },
  { 11, 111, 2, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_117[2] =
{
  { 152, -1, 0, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_118[2] =
{
  { 11, 152, 1, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_119[2] =
{
  { 11, 152, 2, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_120[2] =
{
  { 11, 152, 3, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_121[2] =
{
  { 11, 152, 4, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_122[2] =
{
  { 11, 152, 5, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_123[2] =
{
  { 11, 152, 6, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_124[2] =
{
  { 11, 152, 7, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_125[2] =
{
  { 11, 152, 8, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_126[1] =
{
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_127[5] =
{
  { 237, -1, 0, { 0, 0, 0 } },
  { 238, -1, 0, { 0, 0, 0 } },
  { 245, -1, 0, { 0, 0, 0 } },
  { 11, 238, 1, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_128[6] =
{
  { 237, -1, 0, { 0, 0, 0 } },
  { 243, -1, 0, { 0, 0, 0 } },
  { 244, -1, 0, { 0, 0, 0 } },
  { 245, -1, 0, { 0, 0, 0 } },
  { 11, 243, 1, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_129[5] =
{
  { 237, -1, 0, { 0, 0, 0 } },
  { 238, -1, 0, { 0, 0, 0 } },
  { 248, -1, 0, { 0, 0, 0 } },
  { 11, 238, 2, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_130[6] =
{
  { 237, -1, 0, { 0, 0, 0 } },
  { 246, -1, 0, { 0, 0, 0 } },
  { 247, -1, 0, { 0, 0, 0 } },
  { 248, -1, 0, { 0, 0, 0 } },
  { 11, 246, 2, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_131[5] =
{
  { 237, -1, 0, { 0, 0, 0 } },
  { 238, -1, 0, { 0, 0, 0 } },
  { 251, -1, 0, { 0, 0, 0 } },
  { 11, 238, 3, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_132[6] =
{
  { 237, -1, 0, { 0, 0, 0 } },
  { 246, -1, 0, { 0, 0, 0 } },
  { 250, -1, 0, { 0, 0, 0 } },
  { 251, -1, 0, { 0, 0, 0 } },
  { 11, 246, 3, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_133[5] =
{
  { 237, -1, 0, { 0, 0, 0 } },
  { 238, -1, 0, { 0, 0, 0 } },
  { 254, -1, 0, { 0, 0, 0 } },
  { 11, 238, 4, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_134[6] =
{
  { 237, -1, 0, { 0, 0, 0 } },
  { 246, -1, 0, { 0, 0, 0 } },
  { 253, -1, 0, { 0, 0, 0 } },
  { 254, -1, 0, { 0, 0, 0 } },
  { 11, 246, 4, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_135[6] =
{
  { 263, -1, 0, { 0, 0, 0 } },
  { 264, -1, 0, { 0, 0, 0 } },
  { 265, -1, 0, { 0, 0, 0 } },
  { 266, -1, 0, { 0, 0, 0 } },
  { 11, 264, 4, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_136[10] =
{
  { 263, -1, 0, { 0, 0, 0 } },
  { 264, -1, 0, { 0, 0, 0 } },
  { 265, -1, 0, { 0, 0, 0 } },
  { 267, -1, 0, { 0, 0, 0 } },
  { 268, -1, 0, { 0, 0, 0 } },
  { 269, -1, 0, { 0, 0, 0 } },
  { 270, -1, 0, { 0, 0, 0 } },
  { 11, 264, 4, { 0, 0, 0 } },
  { 225, -1, 1, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_137[6] =
{
  { 263, -1, 0, { 0, 0, 0 } },
  { 264, -1, 0, { 0, 0, 0 } },
  { 265, -1, 0, { 0, 0, 0 } },
  { 266, -1, 0, { 0, 0, 0 } },
  { 11, 264, 1, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_138[10] =
{
  { 263, -1, 0, { 0, 0, 0 } },
  { 264, -1, 0, { 0, 0, 0 } },
  { 265, -1, 0, { 0, 0, 0 } },
  { 279, -1, 0, { 0, 0, 0 } },
  { 280, -1, 0, { 0, 0, 0 } },
  { 281, -1, 0, { 0, 0, 0 } },
  { 282, -1, 0, { 0, 0, 0 } },
  { 11, 264, 1, { 0, 0, 0 } },
  { 225, -1, 1, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_139[6] =
{
  { 263, -1, 0, { 0, 0, 0 } },
  { 264, -1, 0, { 0, 0, 0 } },
  { 265, -1, 0, { 0, 0, 0 } },
  { 266, -1, 0, { 0, 0, 0 } },
  { 11, 264, 2, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_140[10] =
{
  { 263, -1, 0, { 0, 0, 0 } },
  { 264, -1, 0, { 0, 0, 0 } },
  { 265, -1, 0, { 0, 0, 0 } },
  { 271, -1, 0, { 0, 0, 0 } },
  { 272, -1, 0, { 0, 0, 0 } },
  { 273, -1, 0, { 0, 0, 0 } },
  { 274, -1, 0, { 0, 0, 0 } },
  { 11, 264, 2, { 0, 0, 0 } },
  { 225, -1, 1, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_141[6] =
{
  { 263, -1, 0, { 0, 0, 0 } },
  { 264, -1, 0, { 0, 0, 0 } },
  { 265, -1, 0, { 0, 0, 0 } },
  { 266, -1, 0, { 0, 0, 0 } },
  { 11, 264, 3, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_142[10] =
{
  { 263, -1, 0, { 0, 0, 0 } },
  { 264, -1, 0, { 0, 0, 0 } },
  { 265, -1, 0, { 0, 0, 0 } },
  { 275, -1, 0, { 0, 0, 0 } },
  { 276, -1, 0, { 0, 0, 0 } },
  { 277, -1, 0, { 0, 0, 0 } },
  { 278, -1, 0, { 0, 0, 0 } },
  { 11, 264, 3, { 0, 0, 0 } },
  { 225, -1, 1, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_143[6] =
{
  { 263, -1, 0, { 0, 0, 0 } },
  { 264, -1, 0, { 0, 0, 0 } },
  { 265, -1, 0, { 0, 0, 0 } },
  { 266, -1, 0, { 0, 0, 0 } },
  { 11, 264, 5, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_144[10] =
{
  { 263, -1, 0, { 0, 0, 0 } },
  { 264, -1, 0, { 0, 0, 0 } },
  { 265, -1, 0, { 0, 0, 0 } },
  { 283, -1, 0, { 0, 0, 0 } },
  { 284, -1, 0, { 0, 0, 0 } },
  { 285, -1, 0, { 0, 0, 0 } },
  { 286, -1, 0, { 0, 0, 0 } },
  { 11, 264, 5, { 0, 0, 0 } },
  { 225, -1, 1, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_145[6] =
{
  { 263, -1, 0, { 0, 0, 0 } },
  { 264, -1, 0, { 0, 0, 0 } },
  { 265, -1, 0, { 0, 0, 0 } },
  { 266, -1, 0, { 0, 0, 0 } },
  { 11, 264, 6, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_146[7] =
{
  { 263, -1, 0, { 0, 0, 0 } },
  { 264, -1, 0, { 0, 0, 0 } },
  { 265, -1, 0, { 0, 0, 0 } },
  { 287, -1, 0, { 0, 0, 0 } },
  { 11, 264, 6, { 0, 0, 0 } },
  { 225, -1, 1, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_147[3] =
{
  { 288, -1, 0, { 0, 0, 0 } },
  { 289, -1, 0, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_148[8] =
{
  { 288, -1, 0, { 0, 0, 0 } },
  { 289, -1, 0, { 0, 0, 0 } },
  { 242, -1, 0, { 0, 0, 0 } },
  { 262, -1, 0, { 0, 0, 0 } },
  { 259, -1, 0, { 0, 0, 0 } },
  { 11, 262, 1, { 0, 0, 0 } },
  { 225, -1, 1, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_149[8] =
{
  { 288, -1, 0, { 0, 0, 0 } },
  { 289, -1, 0, { 0, 0, 0 } },
  { 242, -1, 0, { 0, 0, 0 } },
  { 262, -1, 0, { 0, 0, 0 } },
  { 260, -1, 0, { 0, 0, 0 } },
  { 11, 262, 2, { 0, 0, 0 } },
  { 225, -1, 1, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_150[9] =
{
  { 288, -1, 0, { 0, 0, 0 } },
  { 289, -1, 0, { 0, 0, 0 } },
  { 242, -1, 0, { 0, 0, 0 } },
  { 255, -1, 0, { 0, 0, 0 } },
  { 256, -1, 0, { 0, 0, 0 } },
  { 262, -1, 0, { 0, 0, 0 } },
  { 261, -1, 0, { 0, 0, 0 } },
  { 11, 262, 2, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_151[9] =
{
  { 288, -1, 0, { 0, 0, 0 } },
  { 289, -1, 0, { 0, 0, 0 } },
  { 242, -1, 0, { 0, 0, 0 } },
  { 255, -1, 0, { 0, 0, 0 } },
  { 257, -1, 0, { 0, 0, 0 } },
  { 262, -1, 0, { 0, 0, 0 } },
  { 261, -1, 0, { 0, 0, 0 } },
  { 11, 262, 2, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_152[9] =
{
  { 288, -1, 0, { 0, 0, 0 } },
  { 289, -1, 0, { 0, 0, 0 } },
  { 242, -1, 0, { 0, 0, 0 } },
  { 255, -1, 0, { 0, 0, 0 } },
  { 258, -1, 0, { 0, 0, 0 } },
  { 262, -1, 0, { 0, 0, 0 } },
  { 261, -1, 0, { 0, 0, 0 } },
  { 11, 262, 2, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_153[3] =
{
  { 290, -1, 0, { 0, 0, 0 } },
  { 291, -1, 0, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_154[8] =
{
  { 290, -1, 0, { 0, 0, 0 } },
  { 291, -1, 0, { 0, 0, 0 } },
  { 242, -1, 0, { 0, 0, 0 } },
  { 262, -1, 0, { 0, 0, 0 } },
  { 259, -1, 0, { 0, 0, 0 } },
  { 11, 262, 1, { 0, 0, 0 } },
  { 225, -1, 1, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_155[8] =
{
  { 290, -1, 0, { 0, 0, 0 } },
  { 291, -1, 0, { 0, 0, 0 } },
  { 242, -1, 0, { 0, 0, 0 } },
  { 262, -1, 0, { 0, 0, 0 } },
  { 260, -1, 0, { 0, 0, 0 } },
  { 11, 262, 2, { 0, 0, 0 } },
  { 225, -1, 1, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_156[9] =
{
  { 290, -1, 0, { 0, 0, 0 } },
  { 291, -1, 0, { 0, 0, 0 } },
  { 242, -1, 0, { 0, 0, 0 } },
  { 255, -1, 0, { 0, 0, 0 } },
  { 256, -1, 0, { 0, 0, 0 } },
  { 262, -1, 0, { 0, 0, 0 } },
  { 261, -1, 0, { 0, 0, 0 } },
  { 11, 262, 2, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_157[9] =
{
  { 290, -1, 0, { 0, 0, 0 } },
  { 291, -1, 0, { 0, 0, 0 } },
  { 242, -1, 0, { 0, 0, 0 } },
  { 255, -1, 0, { 0, 0, 0 } },
  { 257, -1, 0, { 0, 0, 0 } },
  { 262, -1, 0, { 0, 0, 0 } },
  { 261, -1, 0, { 0, 0, 0 } },
  { 11, 262, 2, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_158[9] =
{
  { 290, -1, 0, { 0, 0, 0 } },
  { 291, -1, 0, { 0, 0, 0 } },
  { 242, -1, 0, { 0, 0, 0 } },
  { 255, -1, 0, { 0, 0, 0 } },
  { 258, -1, 0, { 0, 0, 0 } },
  { 262, -1, 0, { 0, 0, 0 } },
  { 261, -1, 0, { 0, 0, 0 } },
  { 11, 262, 2, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_159[3] =
{
  { 292, -1, 0, { 0, 0, 0 } },
  { 293, -1, 0, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_160[8] =
{
  { 292, -1, 0, { 0, 0, 0 } },
  { 293, -1, 0, { 0, 0, 0 } },
  { 242, -1, 0, { 0, 0, 0 } },
  { 262, -1, 0, { 0, 0, 0 } },
  { 259, -1, 0, { 0, 0, 0 } },
  { 11, 262, 1, { 0, 0, 0 } },
  { 225, -1, 1, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_161[8] =
{
  { 292, -1, 0, { 0, 0, 0 } },
  { 293, -1, 0, { 0, 0, 0 } },
  { 242, -1, 0, { 0, 0, 0 } },
  { 262, -1, 0, { 0, 0, 0 } },
  { 260, -1, 0, { 0, 0, 0 } },
  { 11, 262, 2, { 0, 0, 0 } },
  { 225, -1, 1, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_162[9] =
{
  { 292, -1, 0, { 0, 0, 0 } },
  { 293, -1, 0, { 0, 0, 0 } },
  { 242, -1, 0, { 0, 0, 0 } },
  { 255, -1, 0, { 0, 0, 0 } },
  { 256, -1, 0, { 0, 0, 0 } },
  { 262, -1, 0, { 0, 0, 0 } },
  { 261, -1, 0, { 0, 0, 0 } },
  { 11, 262, 2, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_163[9] =
{
  { 292, -1, 0, { 0, 0, 0 } },
  { 293, -1, 0, { 0, 0, 0 } },
  { 242, -1, 0, { 0, 0, 0 } },
  { 255, -1, 0, { 0, 0, 0 } },
  { 257, -1, 0, { 0, 0, 0 } },
  { 262, -1, 0, { 0, 0, 0 } },
  { 261, -1, 0, { 0, 0, 0 } },
  { 11, 262, 2, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_164[9] =
{
  { 292, -1, 0, { 0, 0, 0 } },
  { 293, -1, 0, { 0, 0, 0 } },
  { 242, -1, 0, { 0, 0, 0 } },
  { 255, -1, 0, { 0, 0, 0 } },
  { 258, -1, 0, { 0, 0, 0 } },
  { 262, -1, 0, { 0, 0, 0 } },
  { 261, -1, 0, { 0, 0, 0 } },
  { 11, 262, 2, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_165[3] =
{
  { 294, -1, 0, { 0, 0, 0 } },
  { 295, -1, 0, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_166[8] =
{
  { 294, -1, 0, { 0, 0, 0 } },
  { 295, -1, 0, { 0, 0, 0 } },
  { 242, -1, 0, { 0, 0, 0 } },
  { 262, -1, 0, { 0, 0, 0 } },
  { 259, -1, 0, { 0, 0, 0 } },
  { 11, 262, 1, { 0, 0, 0 } },
  { 225, -1, 1, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_167[8] =
{
  { 294, -1, 0, { 0, 0, 0 } },
  { 295, -1, 0, { 0, 0, 0 } },
  { 242, -1, 0, { 0, 0, 0 } },
  { 262, -1, 0, { 0, 0, 0 } },
  { 260, -1, 0, { 0, 0, 0 } },
  { 11, 262, 2, { 0, 0, 0 } },
  { 225, -1, 1, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_168[9] =
{
  { 294, -1, 0, { 0, 0, 0 } },
  { 295, -1, 0, { 0, 0, 0 } },
  { 242, -1, 0, { 0, 0, 0 } },
  { 255, -1, 0, { 0, 0, 0 } },
  { 256, -1, 0, { 0, 0, 0 } },
  { 262, -1, 0, { 0, 0, 0 } },
  { 261, -1, 0, { 0, 0, 0 } },
  { 11, 262, 2, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_169[9] =
{
  { 294, -1, 0, { 0, 0, 0 } },
  { 295, -1, 0, { 0, 0, 0 } },
  { 242, -1, 0, { 0, 0, 0 } },
  { 255, -1, 0, { 0, 0, 0 } },
  { 257, -1, 0, { 0, 0, 0 } },
  { 262, -1, 0, { 0, 0, 0 } },
  { 261, -1, 0, { 0, 0, 0 } },
  { 11, 262, 2, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_170[9] =
{
  { 294, -1, 0, { 0, 0, 0 } },
  { 295, -1, 0, { 0, 0, 0 } },
  { 242, -1, 0, { 0, 0, 0 } },
  { 255, -1, 0, { 0, 0, 0 } },
  { 258, -1, 0, { 0, 0, 0 } },
  { 262, -1, 0, { 0, 0, 0 } },
  { 261, -1, 0, { 0, 0, 0 } },
  { 11, 262, 2, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_171[3] =
{
  { 296, -1, 0, { 0, 0, 0 } },
  { 297, -1, 0, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_172[8] =
{
  { 296, -1, 0, { 0, 0, 0 } },
  { 297, -1, 0, { 0, 0, 0 } },
  { 242, -1, 0, { 0, 0, 0 } },
  { 262, -1, 0, { 0, 0, 0 } },
  { 259, -1, 0, { 0, 0, 0 } },
  { 11, 262, 1, { 0, 0, 0 } },
  { 225, -1, 1, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_173[8] =
{
  { 296, -1, 0, { 0, 0, 0 } },
  { 297, -1, 0, { 0, 0, 0 } },
  { 242, -1, 0, { 0, 0, 0 } },
  { 262, -1, 0, { 0, 0, 0 } },
  { 260, -1, 0, { 0, 0, 0 } },
  { 11, 262, 2, { 0, 0, 0 } },
  { 225, -1, 1, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_174[9] =
{
  { 296, -1, 0, { 0, 0, 0 } },
  { 297, -1, 0, { 0, 0, 0 } },
  { 242, -1, 0, { 0, 0, 0 } },
  { 255, -1, 0, { 0, 0, 0 } },
  { 256, -1, 0, { 0, 0, 0 } },
  { 262, -1, 0, { 0, 0, 0 } },
  { 261, -1, 0, { 0, 0, 0 } },
  { 11, 262, 2, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_175[9] =
{
  { 296, -1, 0, { 0, 0, 0 } },
  { 297, -1, 0, { 0, 0, 0 } },
  { 242, -1, 0, { 0, 0, 0 } },
  { 255, -1, 0, { 0, 0, 0 } },
  { 257, -1, 0, { 0, 0, 0 } },
  { 262, -1, 0, { 0, 0, 0 } },
  { 261, -1, 0, { 0, 0, 0 } },
  { 11, 262, 2, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_176[9] =
{
  { 296, -1, 0, { 0, 0, 0 } },
  { 297, -1, 0, { 0, 0, 0 } },
  { 242, -1, 0, { 0, 0, 0 } },
  { 255, -1, 0, { 0, 0, 0 } },
  { 258, -1, 0, { 0, 0, 0 } },
  { 262, -1, 0, { 0, 0, 0 } },
  { 261, -1, 0, { 0, 0, 0 } },
  { 11, 262, 2, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_177[6] =
{
  { 298, -1, 0, { 0, 0, 0 } },
  { 299, -1, 0, { 0, 0, 0 } },
  { 300, -1, 0, { 0, 0, 0 } },
  { 301, -1, 0, { 0, 0, 0 } },
  { 11, 299, 5, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_178[10] =
{
  { 298, -1, 0, { 0, 0, 0 } },
  { 299, -1, 0, { 0, 0, 0 } },
  { 300, -1, 0, { 0, 0, 0 } },
  { 302, -1, 0, { 0, 0, 0 } },
  { 303, -1, 0, { 0, 0, 0 } },
  { 304, -1, 0, { 0, 0, 0 } },
  { 305, -1, 0, { 0, 0, 0 } },
  { 11, 299, 5, { 0, 0, 0 } },
  { 225, -1, 1, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_179[6] =
{
  { 298, -1, 0, { 0, 0, 0 } },
  { 299, -1, 0, { 0, 0, 0 } },
  { 300, -1, 0, { 0, 0, 0 } },
  { 301, -1, 0, { 0, 0, 0 } },
  { 11, 299, 1, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_180[10] =
{
  { 298, -1, 0, { 0, 0, 0 } },
  { 299, -1, 0, { 0, 0, 0 } },
  { 300, -1, 0, { 0, 0, 0 } },
  { 306, -1, 0, { 0, 0, 0 } },
  { 307, -1, 0, { 0, 0, 0 } },
  { 308, -1, 0, { 0, 0, 0 } },
  { 309, -1, 0, { 0, 0, 0 } },
  { 11, 299, 1, { 0, 0, 0 } },
  { 225, -1, 1, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_181[6] =
{
  { 298, -1, 0, { 0, 0, 0 } },
  { 299, -1, 0, { 0, 0, 0 } },
  { 300, -1, 0, { 0, 0, 0 } },
  { 301, -1, 0, { 0, 0, 0 } },
  { 11, 299, 4, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_182[10] =
{
  { 298, -1, 0, { 0, 0, 0 } },
  { 299, -1, 0, { 0, 0, 0 } },
  { 300, -1, 0, { 0, 0, 0 } },
  { 310, -1, 0, { 0, 0, 0 } },
  { 311, -1, 0, { 0, 0, 0 } },
  { 312, -1, 0, { 0, 0, 0 } },
  { 313, -1, 0, { 0, 0, 0 } },
  { 11, 299, 4, { 0, 0, 0 } },
  { 225, -1, 1, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_183[6] =
{
  { 298, -1, 0, { 0, 0, 0 } },
  { 299, -1, 0, { 0, 0, 0 } },
  { 300, -1, 0, { 0, 0, 0 } },
  { 301, -1, 0, { 0, 0, 0 } },
  { 11, 299, 2, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_184[10] =
{
  { 298, -1, 0, { 0, 0, 0 } },
  { 299, -1, 0, { 0, 0, 0 } },
  { 300, -1, 0, { 0, 0, 0 } },
  { 314, -1, 0, { 0, 0, 0 } },
  { 315, -1, 0, { 0, 0, 0 } },
  { 316, -1, 0, { 0, 0, 0 } },
  { 317, -1, 0, { 0, 0, 0 } },
  { 11, 299, 2, { 0, 0, 0 } },
  { 225, -1, 1, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_185[6] =
{
  { 298, -1, 0, { 0, 0, 0 } },
  { 299, -1, 0, { 0, 0, 0 } },
  { 300, -1, 0, { 0, 0, 0 } },
  { 301, -1, 0, { 0, 0, 0 } },
  { 11, 299, 3, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_186[10] =
{
  { 298, -1, 0, { 0, 0, 0 } },
  { 299, -1, 0, { 0, 0, 0 } },
  { 300, -1, 0, { 0, 0, 0 } },
  { 318, -1, 0, { 0, 0, 0 } },
  { 319, -1, 0, { 0, 0, 0 } },
  { 320, -1, 0, { 0, 0, 0 } },
  { 321, -1, 0, { 0, 0, 0 } },
  { 11, 299, 3, { 0, 0, 0 } },
  { 225, -1, 1, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_187[6] =
{
  { 298, -1, 0, { 0, 0, 0 } },
  { 299, -1, 0, { 0, 0, 0 } },
  { 300, -1, 0, { 0, 0, 0 } },
  { 301, -1, 0, { 0, 0, 0 } },
  { 11, 299, 6, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_188[7] =
{
  { 298, -1, 0, { 0, 0, 0 } },
  { 299, -1, 0, { 0, 0, 0 } },
  { 300, -1, 0, { 0, 0, 0 } },
  { 322, -1, 0, { 0, 0, 0 } },
  { 11, 299, 6, { 0, 0, 0 } },
  { 225, -1, 1, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_189[3] =
{
  { 323, -1, 0, { 0, 0, 0 } },
  { 324, -1, 0, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_190[8] =
{
  { 323, -1, 0, { 0, 0, 0 } },
  { 324, -1, 0, { 0, 0, 0 } },
  { 242, -1, 0, { 0, 0, 0 } },
  { 262, -1, 0, { 0, 0, 0 } },
  { 259, -1, 0, { 0, 0, 0 } },
  { 11, 262, 1, { 0, 0, 0 } },
  { 225, -1, 1, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_191[8] =
{
  { 323, -1, 0, { 0, 0, 0 } },
  { 324, -1, 0, { 0, 0, 0 } },
  { 242, -1, 0, { 0, 0, 0 } },
  { 262, -1, 0, { 0, 0, 0 } },
  { 260, -1, 0, { 0, 0, 0 } },
  { 11, 262, 2, { 0, 0, 0 } },
  { 225, -1, 1, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_192[9] =
{
  { 323, -1, 0, { 0, 0, 0 } },
  { 324, -1, 0, { 0, 0, 0 } },
  { 242, -1, 0, { 0, 0, 0 } },
  { 255, -1, 0, { 0, 0, 0 } },
  { 256, -1, 0, { 0, 0, 0 } },
  { 262, -1, 0, { 0, 0, 0 } },
  { 261, -1, 0, { 0, 0, 0 } },
  { 11, 262, 2, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_193[9] =
{
  { 323, -1, 0, { 0, 0, 0 } },
  { 324, -1, 0, { 0, 0, 0 } },
  { 242, -1, 0, { 0, 0, 0 } },
  { 255, -1, 0, { 0, 0, 0 } },
  { 257, -1, 0, { 0, 0, 0 } },
  { 262, -1, 0, { 0, 0, 0 } },
  { 261, -1, 0, { 0, 0, 0 } },
  { 11, 262, 2, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_194[9] =
{
  { 323, -1, 0, { 0, 0, 0 } },
  { 324, -1, 0, { 0, 0, 0 } },
  { 242, -1, 0, { 0, 0, 0 } },
  { 255, -1, 0, { 0, 0, 0 } },
  { 258, -1, 0, { 0, 0, 0 } },
  { 262, -1, 0, { 0, 0, 0 } },
  { 261, -1, 0, { 0, 0, 0 } },
  { 11, 262, 2, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_195[3] =
{
  { 325, -1, 0, { 0, 0, 0 } },
  { 326, -1, 0, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_196[8] =
{
  { 325, -1, 0, { 0, 0, 0 } },
  { 326, -1, 0, { 0, 0, 0 } },
  { 242, -1, 0, { 0, 0, 0 } },
  { 262, -1, 0, { 0, 0, 0 } },
  { 259, -1, 0, { 0, 0, 0 } },
  { 11, 262, 1, { 0, 0, 0 } },
  { 225, -1, 1, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_197[8] =
{
  { 325, -1, 0, { 0, 0, 0 } },
  { 326, -1, 0, { 0, 0, 0 } },
  { 242, -1, 0, { 0, 0, 0 } },
  { 262, -1, 0, { 0, 0, 0 } },
  { 260, -1, 0, { 0, 0, 0 } },
  { 11, 262, 2, { 0, 0, 0 } },
  { 225, -1, 1, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_198[9] =
{
  { 325, -1, 0, { 0, 0, 0 } },
  { 326, -1, 0, { 0, 0, 0 } },
  { 242, -1, 0, { 0, 0, 0 } },
  { 255, -1, 0, { 0, 0, 0 } },
  { 256, -1, 0, { 0, 0, 0 } },
  { 262, -1, 0, { 0, 0, 0 } },
  { 261, -1, 0, { 0, 0, 0 } },
  { 11, 262, 2, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_199[9] =
{
  { 325, -1, 0, { 0, 0, 0 } },
  { 326, -1, 0, { 0, 0, 0 } },
  { 242, -1, 0, { 0, 0, 0 } },
  { 255, -1, 0, { 0, 0, 0 } },
  { 257, -1, 0, { 0, 0, 0 } },
  { 262, -1, 0, { 0, 0, 0 } },
  { 261, -1, 0, { 0, 0, 0 } },
  { 11, 262, 2, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_200[9] =
{
  { 325, -1, 0, { 0, 0, 0 } },
  { 326, -1, 0, { 0, 0, 0 } },
  { 242, -1, 0, { 0, 0, 0 } },
  { 255, -1, 0, { 0, 0, 0 } },
  { 258, -1, 0, { 0, 0, 0 } },
  { 262, -1, 0, { 0, 0, 0 } },
  { 261, -1, 0, { 0, 0, 0 } },
  { 11, 262, 2, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_201[3] =
{
  { 327, -1, 0, { 0, 0, 0 } },
  { 328, -1, 0, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_202[8] =
{
  { 327, -1, 0, { 0, 0, 0 } },
  { 328, -1, 0, { 0, 0, 0 } },
  { 242, -1, 0, { 0, 0, 0 } },
  { 262, -1, 0, { 0, 0, 0 } },
  { 259, -1, 0, { 0, 0, 0 } },
  { 11, 262, 1, { 0, 0, 0 } },
  { 225, -1, 1, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_203[8] =
{
  { 327, -1, 0, { 0, 0, 0 } },
  { 328, -1, 0, { 0, 0, 0 } },
  { 242, -1, 0, { 0, 0, 0 } },
  { 262, -1, 0, { 0, 0, 0 } },
  { 260, -1, 0, { 0, 0, 0 } },
  { 11, 262, 2, { 0, 0, 0 } },
  { 225, -1, 1, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_204[9] =
{
  { 327, -1, 0, { 0, 0, 0 } },
  { 328, -1, 0, { 0, 0, 0 } },
  { 242, -1, 0, { 0, 0, 0 } },
  { 255, -1, 0, { 0, 0, 0 } },
  { 256, -1, 0, { 0, 0, 0 } },
  { 262, -1, 0, { 0, 0, 0 } },
  { 261, -1, 0, { 0, 0, 0 } },
  { 11, 262, 2, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_205[9] =
{
  { 327, -1, 0, { 0, 0, 0 } },
  { 328, -1, 0, { 0, 0, 0 } },
  { 242, -1, 0, { 0, 0, 0 } },
  { 255, -1, 0, { 0, 0, 0 } },
  { 257, -1, 0, { 0, 0, 0 } },
  { 262, -1, 0, { 0, 0, 0 } },
  { 261, -1, 0, { 0, 0, 0 } },
  { 11, 262, 2, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_206[9] =
{
  { 327, -1, 0, { 0, 0, 0 } },
  { 328, -1, 0, { 0, 0, 0 } },
  { 242, -1, 0, { 0, 0, 0 } },
  { 255, -1, 0, { 0, 0, 0 } },
  { 258, -1, 0, { 0, 0, 0 } },
  { 262, -1, 0, { 0, 0, 0 } },
  { 261, -1, 0, { 0, 0, 0 } },
  { 11, 262, 2, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_207[3] =
{
  { 329, -1, 0, { 0, 0, 0 } },
  { 330, -1, 0, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_208[8] =
{
  { 329, -1, 0, { 0, 0, 0 } },
  { 330, -1, 0, { 0, 0, 0 } },
  { 242, -1, 0, { 0, 0, 0 } },
  { 262, -1, 0, { 0, 0, 0 } },
  { 259, -1, 0, { 0, 0, 0 } },
  { 11, 262, 1, { 0, 0, 0 } },
  { 225, -1, 1, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_209[8] =
{
  { 329, -1, 0, { 0, 0, 0 } },
  { 330, -1, 0, { 0, 0, 0 } },
  { 242, -1, 0, { 0, 0, 0 } },
  { 262, -1, 0, { 0, 0, 0 } },
  { 260, -1, 0, { 0, 0, 0 } },
  { 11, 262, 2, { 0, 0, 0 } },
  { 225, -1, 1, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_210[9] =
{
  { 329, -1, 0, { 0, 0, 0 } },
  { 330, -1, 0, { 0, 0, 0 } },
  { 242, -1, 0, { 0, 0, 0 } },
  { 255, -1, 0, { 0, 0, 0 } },
  { 256, -1, 0, { 0, 0, 0 } },
  { 262, -1, 0, { 0, 0, 0 } },
  { 261, -1, 0, { 0, 0, 0 } },
  { 11, 262, 2, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_211[9] =
{
  { 329, -1, 0, { 0, 0, 0 } },
  { 330, -1, 0, { 0, 0, 0 } },
  { 242, -1, 0, { 0, 0, 0 } },
  { 255, -1, 0, { 0, 0, 0 } },
  { 257, -1, 0, { 0, 0, 0 } },
  { 262, -1, 0, { 0, 0, 0 } },
  { 261, -1, 0, { 0, 0, 0 } },
  { 11, 262, 2, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_212[9] =
{
  { 329, -1, 0, { 0, 0, 0 } },
  { 330, -1, 0, { 0, 0, 0 } },
  { 242, -1, 0, { 0, 0, 0 } },
  { 255, -1, 0, { 0, 0, 0 } },
  { 258, -1, 0, { 0, 0, 0 } },
  { 262, -1, 0, { 0, 0, 0 } },
  { 261, -1, 0, { 0, 0, 0 } },
  { 11, 262, 2, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_213[3] =
{
  { 331, -1, 0, { 0, 0, 0 } },
  { 332, -1, 0, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_214[8] =
{
  { 331, -1, 0, { 0, 0, 0 } },
  { 332, -1, 0, { 0, 0, 0 } },
  { 242, -1, 0, { 0, 0, 0 } },
  { 262, -1, 0, { 0, 0, 0 } },
  { 259, -1, 0, { 0, 0, 0 } },
  { 11, 262, 1, { 0, 0, 0 } },
  { 225, -1, 1, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_215[8] =
{
  { 331, -1, 0, { 0, 0, 0 } },
  { 332, -1, 0, { 0, 0, 0 } },
  { 242, -1, 0, { 0, 0, 0 } },
  { 262, -1, 0, { 0, 0, 0 } },
  { 260, -1, 0, { 0, 0, 0 } },
  { 11, 262, 2, { 0, 0, 0 } },
  { 225, -1, 1, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_216[9] =
{
  { 331, -1, 0, { 0, 0, 0 } },
  { 332, -1, 0, { 0, 0, 0 } },
  { 242, -1, 0, { 0, 0, 0 } },
  { 255, -1, 0, { 0, 0, 0 } },
  { 256, -1, 0, { 0, 0, 0 } },
  { 262, -1, 0, { 0, 0, 0 } },
  { 261, -1, 0, { 0, 0, 0 } },
  { 11, 262, 2, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_217[9] =
{
  { 331, -1, 0, { 0, 0, 0 } },
  { 332, -1, 0, { 0, 0, 0 } },
  { 242, -1, 0, { 0, 0, 0 } },
  { 255, -1, 0, { 0, 0, 0 } },
  { 257, -1, 0, { 0, 0, 0 } },
  { 262, -1, 0, { 0, 0, 0 } },
  { 261, -1, 0, { 0, 0, 0 } },
  { 11, 262, 2, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_218[9] =
{
  { 331, -1, 0, { 0, 0, 0 } },
  { 332, -1, 0, { 0, 0, 0 } },
  { 242, -1, 0, { 0, 0, 0 } },
  { 255, -1, 0, { 0, 0, 0 } },
  { 258, -1, 0, { 0, 0, 0 } },
  { 262, -1, 0, { 0, 0, 0 } },
  { 261, -1, 0, { 0, 0, 0 } },
  { 11, 262, 2, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_219[6] =
{
  { 333, -1, 0, { 0, 0, 0 } },
  { 334, -1, 0, { 0, 0, 0 } },
  { 335, -1, 0, { 0, 0, 0 } },
  { 336, -1, 0, { 0, 0, 0 } },
  { 11, 334, 5, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_220[10] =
{
  { 333, -1, 0, { 0, 0, 0 } },
  { 334, -1, 0, { 0, 0, 0 } },
  { 335, -1, 0, { 0, 0, 0 } },
  { 337, -1, 0, { 0, 0, 0 } },
  { 338, -1, 0, { 0, 0, 0 } },
  { 339, -1, 0, { 0, 0, 0 } },
  { 340, -1, 0, { 0, 0, 0 } },
  { 11, 334, 5, { 0, 0, 0 } },
  { 225, -1, 1, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_221[6] =
{
  { 333, -1, 0, { 0, 0, 0 } },
  { 334, -1, 0, { 0, 0, 0 } },
  { 335, -1, 0, { 0, 0, 0 } },
  { 336, -1, 0, { 0, 0, 0 } },
  { 11, 334, 3, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_222[10] =
{
  { 333, -1, 0, { 0, 0, 0 } },
  { 334, -1, 0, { 0, 0, 0 } },
  { 335, -1, 0, { 0, 0, 0 } },
  { 341, -1, 0, { 0, 0, 0 } },
  { 342, -1, 0, { 0, 0, 0 } },
  { 343, -1, 0, { 0, 0, 0 } },
  { 344, -1, 0, { 0, 0, 0 } },
  { 11, 334, 3, { 0, 0, 0 } },
  { 225, -1, 1, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_223[6] =
{
  { 333, -1, 0, { 0, 0, 0 } },
  { 334, -1, 0, { 0, 0, 0 } },
  { 335, -1, 0, { 0, 0, 0 } },
  { 336, -1, 0, { 0, 0, 0 } },
  { 11, 334, 2, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_224[10] =
{
  { 333, -1, 0, { 0, 0, 0 } },
  { 334, -1, 0, { 0, 0, 0 } },
  { 335, -1, 0, { 0, 0, 0 } },
  { 345, -1, 0, { 0, 0, 0 } },
  { 346, -1, 0, { 0, 0, 0 } },
  { 347, -1, 0, { 0, 0, 0 } },
  { 348, -1, 0, { 0, 0, 0 } },
  { 11, 334, 2, { 0, 0, 0 } },
  { 225, -1, 1, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_225[6] =
{
  { 333, -1, 0, { 0, 0, 0 } },
  { 334, -1, 0, { 0, 0, 0 } },
  { 335, -1, 0, { 0, 0, 0 } },
  { 336, -1, 0, { 0, 0, 0 } },
  { 11, 334, 1, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_226[10] =
{
  { 333, -1, 0, { 0, 0, 0 } },
  { 334, -1, 0, { 0, 0, 0 } },
  { 335, -1, 0, { 0, 0, 0 } },
  { 349, -1, 0, { 0, 0, 0 } },
  { 350, -1, 0, { 0, 0, 0 } },
  { 351, -1, 0, { 0, 0, 0 } },
  { 352, -1, 0, { 0, 0, 0 } },
  { 11, 334, 1, { 0, 0, 0 } },
  { 225, -1, 1, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_227[6] =
{
  { 333, -1, 0, { 0, 0, 0 } },
  { 334, -1, 0, { 0, 0, 0 } },
  { 335, -1, 0, { 0, 0, 0 } },
  { 336, -1, 0, { 0, 0, 0 } },
  { 11, 334, 4, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_228[10] =
{
  { 333, -1, 0, { 0, 0, 0 } },
  { 334, -1, 0, { 0, 0, 0 } },
  { 335, -1, 0, { 0, 0, 0 } },
  { 353, -1, 0, { 0, 0, 0 } },
  { 354, -1, 0, { 0, 0, 0 } },
  { 355, -1, 0, { 0, 0, 0 } },
  { 356, -1, 0, { 0, 0, 0 } },
  { 11, 334, 4, { 0, 0, 0 } },
  { 225, -1, 1, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_229[6] =
{
  { 333, -1, 0, { 0, 0, 0 } },
  { 334, -1, 0, { 0, 0, 0 } },
  { 335, -1, 0, { 0, 0, 0 } },
  { 336, -1, 0, { 0, 0, 0 } },
  { 11, 334, 6, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_230[7] =
{
  { 333, -1, 0, { 0, 0, 0 } },
  { 334, -1, 0, { 0, 0, 0 } },
  { 335, -1, 0, { 0, 0, 0 } },
  { 357, -1, 0, { 0, 0, 0 } },
  { 11, 334, 6, { 0, 0, 0 } },
  { 225, -1, 1, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_231[3] =
{
  { 358, -1, 0, { 0, 0, 0 } },
  { 359, -1, 0, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_232[8] =
{
  { 358, -1, 0, { 0, 0, 0 } },
  { 359, -1, 0, { 0, 0, 0 } },
  { 242, -1, 0, { 0, 0, 0 } },
  { 262, -1, 0, { 0, 0, 0 } },
  { 259, -1, 0, { 0, 0, 0 } },
  { 11, 262, 1, { 0, 0, 0 } },
  { 225, -1, 1, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_233[8] =
{
  { 358, -1, 0, { 0, 0, 0 } },
  { 359, -1, 0, { 0, 0, 0 } },
  { 242, -1, 0, { 0, 0, 0 } },
  { 262, -1, 0, { 0, 0, 0 } },
  { 260, -1, 0, { 0, 0, 0 } },
  { 11, 262, 2, { 0, 0, 0 } },
  { 225, -1, 1, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_234[9] =
{
  { 358, -1, 0, { 0, 0, 0 } },
  { 359, -1, 0, { 0, 0, 0 } },
  { 242, -1, 0, { 0, 0, 0 } },
  { 255, -1, 0, { 0, 0, 0 } },
  { 256, -1, 0, { 0, 0, 0 } },
  { 262, -1, 0, { 0, 0, 0 } },
  { 261, -1, 0, { 0, 0, 0 } },
  { 11, 262, 2, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_235[9] =
{
  { 358, -1, 0, { 0, 0, 0 } },
  { 359, -1, 0, { 0, 0, 0 } },
  { 242, -1, 0, { 0, 0, 0 } },
  { 255, -1, 0, { 0, 0, 0 } },
  { 257, -1, 0, { 0, 0, 0 } },
  { 262, -1, 0, { 0, 0, 0 } },
  { 261, -1, 0, { 0, 0, 0 } },
  { 11, 262, 2, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_236[9] =
{
  { 358, -1, 0, { 0, 0, 0 } },
  { 359, -1, 0, { 0, 0, 0 } },
  { 242, -1, 0, { 0, 0, 0 } },
  { 255, -1, 0, { 0, 0, 0 } },
  { 258, -1, 0, { 0, 0, 0 } },
  { 262, -1, 0, { 0, 0, 0 } },
  { 261, -1, 0, { 0, 0, 0 } },
  { 11, 262, 2, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_237[3] =
{
  { 360, -1, 0, { 0, 0, 0 } },
  { 361, -1, 0, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_238[8] =
{
  { 360, -1, 0, { 0, 0, 0 } },
  { 361, -1, 0, { 0, 0, 0 } },
  { 242, -1, 0, { 0, 0, 0 } },
  { 262, -1, 0, { 0, 0, 0 } },
  { 259, -1, 0, { 0, 0, 0 } },
  { 11, 262, 1, { 0, 0, 0 } },
  { 225, -1, 1, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_239[8] =
{
  { 360, -1, 0, { 0, 0, 0 } },
  { 361, -1, 0, { 0, 0, 0 } },
  { 242, -1, 0, { 0, 0, 0 } },
  { 262, -1, 0, { 0, 0, 0 } },
  { 260, -1, 0, { 0, 0, 0 } },
  { 11, 262, 2, { 0, 0, 0 } },
  { 225, -1, 1, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_240[9] =
{
  { 360, -1, 0, { 0, 0, 0 } },
  { 361, -1, 0, { 0, 0, 0 } },
  { 242, -1, 0, { 0, 0, 0 } },
  { 255, -1, 0, { 0, 0, 0 } },
  { 256, -1, 0, { 0, 0, 0 } },
  { 262, -1, 0, { 0, 0, 0 } },
  { 261, -1, 0, { 0, 0, 0 } },
  { 11, 262, 2, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_241[9] =
{
  { 360, -1, 0, { 0, 0, 0 } },
  { 361, -1, 0, { 0, 0, 0 } },
  { 242, -1, 0, { 0, 0, 0 } },
  { 255, -1, 0, { 0, 0, 0 } },
  { 257, -1, 0, { 0, 0, 0 } },
  { 262, -1, 0, { 0, 0, 0 } },
  { 261, -1, 0, { 0, 0, 0 } },
  { 11, 262, 2, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_242[9] =
{
  { 360, -1, 0, { 0, 0, 0 } },
  { 361, -1, 0, { 0, 0, 0 } },
  { 242, -1, 0, { 0, 0, 0 } },
  { 255, -1, 0, { 0, 0, 0 } },
  { 258, -1, 0, { 0, 0, 0 } },
  { 262, -1, 0, { 0, 0, 0 } },
  { 261, -1, 0, { 0, 0, 0 } },
  { 11, 262, 2, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_243[3] =
{
  { 362, -1, 0, { 0, 0, 0 } },
  { 363, -1, 0, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_244[8] =
{
  { 362, -1, 0, { 0, 0, 0 } },
  { 363, -1, 0, { 0, 0, 0 } },
  { 242, -1, 0, { 0, 0, 0 } },
  { 262, -1, 0, { 0, 0, 0 } },
  { 259, -1, 0, { 0, 0, 0 } },
  { 11, 262, 1, { 0, 0, 0 } },
  { 225, -1, 1, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_245[8] =
{
  { 362, -1, 0, { 0, 0, 0 } },
  { 363, -1, 0, { 0, 0, 0 } },
  { 242, -1, 0, { 0, 0, 0 } },
  { 262, -1, 0, { 0, 0, 0 } },
  { 260, -1, 0, { 0, 0, 0 } },
  { 11, 262, 2, { 0, 0, 0 } },
  { 225, -1, 1, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_246[9] =
{
  { 362, -1, 0, { 0, 0, 0 } },
  { 363, -1, 0, { 0, 0, 0 } },
  { 242, -1, 0, { 0, 0, 0 } },
  { 255, -1, 0, { 0, 0, 0 } },
  { 256, -1, 0, { 0, 0, 0 } },
  { 262, -1, 0, { 0, 0, 0 } },
  { 261, -1, 0, { 0, 0, 0 } },
  { 11, 262, 2, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_247[9] =
{
  { 362, -1, 0, { 0, 0, 0 } },
  { 363, -1, 0, { 0, 0, 0 } },
  { 242, -1, 0, { 0, 0, 0 } },
  { 255, -1, 0, { 0, 0, 0 } },
  { 257, -1, 0, { 0, 0, 0 } },
  { 262, -1, 0, { 0, 0, 0 } },
  { 261, -1, 0, { 0, 0, 0 } },
  { 11, 262, 2, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_248[9] =
{
  { 362, -1, 0, { 0, 0, 0 } },
  { 363, -1, 0, { 0, 0, 0 } },
  { 242, -1, 0, { 0, 0, 0 } },
  { 255, -1, 0, { 0, 0, 0 } },
  { 258, -1, 0, { 0, 0, 0 } },
  { 262, -1, 0, { 0, 0, 0 } },
  { 261, -1, 0, { 0, 0, 0 } },
  { 11, 262, 2, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_249[3] =
{
  { 364, -1, 0, { 0, 0, 0 } },
  { 365, -1, 0, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_250[8] =
{
  { 364, -1, 0, { 0, 0, 0 } },
  { 365, -1, 0, { 0, 0, 0 } },
  { 242, -1, 0, { 0, 0, 0 } },
  { 262, -1, 0, { 0, 0, 0 } },
  { 259, -1, 0, { 0, 0, 0 } },
  { 11, 262, 1, { 0, 0, 0 } },
  { 225, -1, 1, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_251[8] =
{
  { 364, -1, 0, { 0, 0, 0 } },
  { 365, -1, 0, { 0, 0, 0 } },
  { 242, -1, 0, { 0, 0, 0 } },
  { 262, -1, 0, { 0, 0, 0 } },
  { 260, -1, 0, { 0, 0, 0 } },
  { 11, 262, 2, { 0, 0, 0 } },
  { 225, -1, 1, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_252[9] =
{
  { 364, -1, 0, { 0, 0, 0 } },
  { 365, -1, 0, { 0, 0, 0 } },
  { 242, -1, 0, { 0, 0, 0 } },
  { 255, -1, 0, { 0, 0, 0 } },
  { 256, -1, 0, { 0, 0, 0 } },
  { 262, -1, 0, { 0, 0, 0 } },
  { 261, -1, 0, { 0, 0, 0 } },
  { 11, 262, 2, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_253[9] =
{
  { 364, -1, 0, { 0, 0, 0 } },
  { 365, -1, 0, { 0, 0, 0 } },
  { 242, -1, 0, { 0, 0, 0 } },
  { 255, -1, 0, { 0, 0, 0 } },
  { 257, -1, 0, { 0, 0, 0 } },
  { 262, -1, 0, { 0, 0, 0 } },
  { 261, -1, 0, { 0, 0, 0 } },
  { 11, 262, 2, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_254[9] =
{
  { 364, -1, 0, { 0, 0, 0 } },
  { 365, -1, 0, { 0, 0, 0 } },
  { 242, -1, 0, { 0, 0, 0 } },
  { 255, -1, 0, { 0, 0, 0 } },
  { 258, -1, 0, { 0, 0, 0 } },
  { 262, -1, 0, { 0, 0, 0 } },
  { 261, -1, 0, { 0, 0, 0 } },
  { 11, 262, 2, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_255[3] =
{
  { 366, -1, 0, { 0, 0, 0 } },
  { 367, -1, 0, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_256[8] =
{
  { 366, -1, 0, { 0, 0, 0 } },
  { 367, -1, 0, { 0, 0, 0 } },
  { 242, -1, 0, { 0, 0, 0 } },
  { 262, -1, 0, { 0, 0, 0 } },
  { 259, -1, 0, { 0, 0, 0 } },
  { 11, 262, 1, { 0, 0, 0 } },
  { 225, -1, 1, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_257[8] =
{
  { 366, -1, 0, { 0, 0, 0 } },
  { 367, -1, 0, { 0, 0, 0 } },
  { 242, -1, 0, { 0, 0, 0 } },
  { 262, -1, 0, { 0, 0, 0 } },
  { 260, -1, 0, { 0, 0, 0 } },
  { 11, 262, 2, { 0, 0, 0 } },
  { 225, -1, 1, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_258[9] =
{
  { 366, -1, 0, { 0, 0, 0 } },
  { 367, -1, 0, { 0, 0, 0 } },
  { 242, -1, 0, { 0, 0, 0 } },
  { 255, -1, 0, { 0, 0, 0 } },
  { 256, -1, 0, { 0, 0, 0 } },
  { 262, -1, 0, { 0, 0, 0 } },
  { 261, -1, 0, { 0, 0, 0 } },
  { 11, 262, 2, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_259[9] =
{
  { 366, -1, 0, { 0, 0, 0 } },
  { 367, -1, 0, { 0, 0, 0 } },
  { 242, -1, 0, { 0, 0, 0 } },
  { 255, -1, 0, { 0, 0, 0 } },
  { 257, -1, 0, { 0, 0, 0 } },
  { 262, -1, 0, { 0, 0, 0 } },
  { 261, -1, 0, { 0, 0, 0 } },
  { 11, 262, 2, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_260[9] =
{
  { 366, -1, 0, { 0, 0, 0 } },
  { 367, -1, 0, { 0, 0, 0 } },
  { 242, -1, 0, { 0, 0, 0 } },
  { 255, -1, 0, { 0, 0, 0 } },
  { 258, -1, 0, { 0, 0, 0 } },
  { 262, -1, 0, { 0, 0, 0 } },
  { 261, -1, 0, { 0, 0, 0 } },
  { 11, 262, 2, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_261[2] =
{
  { 948, -1, 0, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_262[2] =
{
  { 949, -1, 0, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_263[2] =
{
  { 950, -1, 0, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_264[2] =
{
  { 951, -1, 0, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_265[2] =
{
  { 952, -1, 0, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_266[2] =
{
  { 953, -1, 0, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_267[2] =
{
  { 954, -1, 0, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_268[2] =
{
  { 955, -1, 0, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_269[5] =
{
  { 938, -1, 0, { 0, 0, 0 } },
  { 939, -1, 0, { 0, 0, 0 } },
  { 947, -1, 0, { 0, 0, 0 } },
  { 11, 939, 1, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_270[8] =
{
  { 938, -1, 0, { 0, 0, 0 } },
  { 944, -1, 0, { 0, 0, 0 } },
  { 945, -1, 0, { 0, 0, 0 } },
  { 946, -1, 0, { 0, 0, 0 } },
  { 947, -1, 0, { 0, 0, 0 } },
  { 11, 945, 1, { 0, 0, 0 } },
  { 225, -1, 1, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_271[5] =
{
  { 938, -1, 0, { 0, 0, 0 } },
  { 939, -1, 0, { 0, 0, 0 } },
  { 943, -1, 0, { 0, 0, 0 } },
  { 11, 939, 2, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_272[8] =
{
  { 938, -1, 0, { 0, 0, 0 } },
  { 940, -1, 0, { 0, 0, 0 } },
  { 941, -1, 0, { 0, 0, 0 } },
  { 942, -1, 0, { 0, 0, 0 } },
  { 943, -1, 0, { 0, 0, 0 } },
  { 11, 941, 2, { 0, 0, 0 } },
  { 225, -1, 1, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_273[5] =
{
  { 956, -1, 0, { 0, 0, 0 } },
  { 957, -1, 0, { 0, 0, 0 } },
  { 959, -1, 0, { 0, 0, 0 } },
  { 11, 957, 1, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_274[7] =
{
  { 956, -1, 0, { 0, 0, 0 } },
  { 958, -1, 0, { 0, 0, 0 } },
  { 959, -1, 0, { 0, 0, 0 } },
  { 960, -1, 0, { 0, 0, 0 } },
  { 11, 958, 1, { 0, 0, 0 } },
  { 225, -1, 1, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_275[5] =
{
  { 956, -1, 0, { 0, 0, 0 } },
  { 957, -1, 0, { 0, 0, 0 } },
  { 962, -1, 0, { 0, 0, 0 } },
  { 11, 957, 2, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_276[7] =
{
  { 956, -1, 0, { 0, 0, 0 } },
  { 961, -1, 0, { 0, 0, 0 } },
  { 962, -1, 0, { 0, 0, 0 } },
  { 963, -1, 0, { 0, 0, 0 } },
  { 11, 961, 2, { 0, 0, 0 } },
  { 225, -1, 1, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_277[5] =
{
  { 956, -1, 0, { 0, 0, 0 } },
  { 957, -1, 0, { 0, 0, 0 } },
  { 965, -1, 0, { 0, 0, 0 } },
  { 11, 957, 3, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_278[7] =
{
  { 956, -1, 0, { 0, 0, 0 } },
  { 964, -1, 0, { 0, 0, 0 } },
  { 965, -1, 0, { 0, 0, 0 } },
  { 966, -1, 0, { 0, 0, 0 } },
  { 11, 964, 3, { 0, 0, 0 } },
  { 225, -1, 1, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_279[5] =
{
  { 956, -1, 0, { 0, 0, 0 } },
  { 957, -1, 0, { 0, 0, 0 } },
  { 968, -1, 0, { 0, 0, 0 } },
  { 11, 957, 4, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_280[7] =
{
  { 956, -1, 0, { 0, 0, 0 } },
  { 967, -1, 0, { 0, 0, 0 } },
  { 968, -1, 0, { 0, 0, 0 } },
  { 969, -1, 0, { 0, 0, 0 } },
  { 11, 967, 4, { 0, 0, 0 } },
  { 225, -1, 1, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_281[4] =
{
  { 888, -1, 0, { 0, 0, 0 } },
  { 974, -1, 0, { 0, 0, 0 } },
  { 975, -1, 0, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_282[4] =
{
  { 976, -1, 0, { 0, 0, 0 } },
  { 977, -1, 0, { 0, 0, 0 } },
  { 978, -1, 0, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_283[5] =
{
  { 894, -1, 0, { 0, 0, 0 } },
  { 1004, -1, 0, { 0, 0, 0 } },
  { 1005, -1, 0, { 0, 0, 0 } },
  { 1006, -1, 0, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_284[4] =
{
  { 900, -1, 0, { 0, 0, 0 } },
  { 1007, -1, 0, { 0, 0, 0 } },
  { 1008, -1, 0, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_285[4] =
{
  { 915, -1, 0, { 0, 0, 0 } },
  { 1009, -1, 0, { 0, 0, 0 } },
  { 1010, -1, 0, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_286[5] =
{
  { 888, -1, 0, { 0, 0, 0 } },
  { 970, -1, 0, { 0, 0, 0 } },
  { 972, -1, 0, { 0, 0, 0 } },
  { 973, -1, 0, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_287[2] =
{
  { 976, -1, 0, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_288[5] =
{
  { 894, -1, 0, { 0, 0, 0 } },
  { 970, -1, 0, { 0, 0, 0 } },
  { 972, -1, 0, { 0, 0, 0 } },
  { 973, -1, 0, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_289[4] =
{
  { 900, -1, 0, { 0, 0, 0 } },
  { 971, -1, 0, { 0, 0, 0 } },
  { 914, -1, 0, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_290[4] =
{
  { 900, -1, 0, { 0, 0, 0 } },
  { 971, -1, 0, { 0, 0, 0 } },
  { 906, -1, 0, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_291[4] =
{
  { 900, -1, 0, { 0, 0, 0 } },
  { 971, -1, 0, { 0, 0, 0 } },
  { 921, -1, 0, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_292[4] =
{
  { 915, -1, 0, { 0, 0, 0 } },
  { 971, -1, 0, { 0, 0, 0 } },
  { 922, -1, 0, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_293[4] =
{
  { 915, -1, 0, { 0, 0, 0 } },
  { 971, -1, 0, { 0, 0, 0 } },
  { 923, -1, 0, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_294[4] =
{
  { 915, -1, 0, { 0, 0, 0 } },
  { 971, -1, 0, { 0, 0, 0 } },
  { 921, -1, 0, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_295[4] =
{
  { 933, -1, 0, { 0, 0, 0 } },
  { 931, -1, 0, { 0, 0, 0 } },
  { 932, -1, 0, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_296[4] =
{
  { 934, -1, 0, { 0, 0, 0 } },
  { 931, -1, 0, { 0, 0, 0 } },
  { 932, -1, 0, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_297[4] =
{
  { 935, -1, 0, { 0, 0, 0 } },
  { 931, -1, 0, { 0, 0, 0 } },
  { 932, -1, 0, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_298[4] =
{
  { 936, -1, 0, { 0, 0, 0 } },
  { 931, -1, 0, { 0, 0, 0 } },
  { 932, -1, 0, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_299[2] =
{
  { 976, -1, 0, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_300[2] =
{
  { 976, -1, 0, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_301[2] =
{
  { 979, -1, 0, { 0, 0, 0 } },
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndCelDescriptor front_end_cel_group_302[1] =
{
  { -1, -1, 0, { 0, 0, 0 } }
};

FrontEndPresentCommand front_end_present_command_000 = { 2 };
FrontEndWaitCommand front_end_wait_command_000 = { 0, 0 };
FrontEndShowCelsCommand front_end_show_cels_command_000 = { 1, front_end_cel_group_000 };
FrontEndShowCelsCommand front_end_show_cels_command_001 = { 1, front_end_cel_group_001 };
FrontEndShowCelsCommand front_end_show_cels_command_002 = { 1, front_end_cel_group_002 };
FrontEndWaitCommand front_end_wait_command_001 = { 0, 120 };
FrontEndCallbackCommand front_end_callback_command_000 = { 7, front_end_play_transition, 0, 0 };
FrontEndShowCelsCommand front_end_show_cels_command_003 = { 1, front_end_cel_group_004 };
FrontEndCallbackCommand front_end_callback_command_001 = { 7,
                                                           front_end_show_current_player_descriptor,
                                                           0, 0 };
FrontEndCallbackCommand front_end_callback_command_002 = { 7, front_end_draw_selected_rider_summary,
                                                           0, 0 };
FrontEndCallbackCommand front_end_callback_command_003 = { 7, front_end_draw_selected_bike_summary,
                                                           0, 0 };
FrontEndShowCelsCommand front_end_show_cels_command_004 = { 1, front_end_cel_group_007 };
FrontEndShowCelsCommand front_end_show_cels_command_005 = { 1, front_end_cel_group_008 };
FrontEndShowCelsCommand front_end_show_cels_command_006 = { 1, front_end_cel_group_009 };
FrontEndShowCelsCommand front_end_show_cels_command_007 = { 1, front_end_cel_group_010 };
FrontEndShowCelsCommand front_end_show_cels_command_008 = { 1, front_end_cel_group_011 };
FrontEndShowCelsCommand front_end_show_cels_command_009 = { 1, front_end_cel_group_012 };
FrontEndCallbackCommand front_end_callback_command_004 = { 7, front_end_draw_course_distance, 0,
                                                           0 };
FrontEndCallbackCommand front_end_callback_command_005 = { 7, front_end_draw_course_distance, 1,
                                                           0 };
FrontEndCallbackCommand front_end_callback_command_006 = { 7, front_end_draw_course_distance, 2,
                                                           0 };
FrontEndCallbackCommand front_end_callback_command_007 = { 7, front_end_draw_course_distance, 4,
                                                           0 };
FrontEndCallbackCommand front_end_callback_command_008 = { 7, front_end_draw_course_distance, 3,
                                                           0 };
FrontEndCallbackCommand front_end_callback_command_009 = { 7,
                                                           front_end_select_available_race_variant,
                                                           0, 0 };
FrontEndShowCelsCommand front_end_show_cels_command_010 = { 1, front_end_cel_group_014 };
FrontEndShowCelsCommand front_end_show_cels_command_011 = { 1, front_end_cel_group_015 };
FrontEndShowCelsCommand front_end_show_cels_command_012 = { 1, front_end_cel_group_016 };
FrontEndShowCelsCommand front_end_show_cels_command_013 = { 1, front_end_cel_group_017 };
FrontEndShowCelsCommand front_end_show_cels_command_014 = { 1, front_end_cel_group_018 };
FrontEndShowCelsCommand front_end_show_cels_command_015 = { 1, front_end_cel_group_019 };
FrontEndShowCelsCommand front_end_show_cels_command_016 = { 1, front_end_cel_group_020 };
FrontEndShowCelsCommand front_end_show_cels_command_017 = { 1, front_end_cel_group_021 };
FrontEndShowCelsCommand front_end_show_cels_command_018 = { 1, front_end_cel_group_022 };
FrontEndShowCelsCommand front_end_show_cels_command_019 = { 1, front_end_cel_group_023 };
FrontEndShowCelsCommand front_end_show_cels_command_020 = { 1, front_end_cel_group_024 };
FrontEndShowCelsCommand front_end_show_cels_command_021 = { 1, front_end_cel_group_025 };
FrontEndShowCelsCommand front_end_show_cels_command_022 = { 1, front_end_cel_group_026 };
FrontEndShowCelsCommand front_end_show_cels_command_023 = { 1, front_end_cel_group_027 };
FrontEndShowCelsCommand front_end_show_cels_command_024 = { 1, front_end_cel_group_028 };
FrontEndShowCelsCommand front_end_show_cels_command_025 = { 1, front_end_cel_group_029 };
FrontEndShowCelsCommand front_end_show_cels_command_026 = { 1, front_end_cel_group_030 };
FrontEndShowCelsCommand front_end_show_cels_command_027 = { 1, front_end_cel_group_031 };
FrontEndShowCelsCommand front_end_show_cels_command_028 = { 1, front_end_cel_group_032 };
FrontEndShowCelsCommand front_end_show_cels_command_029 = { 1, front_end_cel_group_033 };
FrontEndShowCelsCommand front_end_show_cels_command_030 = { 1, front_end_cel_group_034 };
FrontEndShowCelsCommand front_end_show_cels_command_031 = { 1, front_end_cel_group_035 };
FrontEndShowCelsCommand front_end_show_cels_command_032 = { 1, front_end_cel_group_036 };
FrontEndShowCelsCommand front_end_show_cels_command_033 = { 1, front_end_cel_group_037 };
FrontEndCallbackCommand front_end_callback_command_010 = { 7, front_end_synchronize_current_player,
                                                           0, 0 };
FrontEndCallbackCommand front_end_callback_command_011 = { 7, front_end_draw_rider_bike_statistics,
                                                           8, 0 };
FrontEndCallbackCommand front_end_callback_command_012 = { 7, front_end_draw_rider_bike_statistics,
                                                           7, 0 };
FrontEndCallbackCommand front_end_callback_command_013 = { 7, front_end_draw_rider_bike_statistics,
                                                           2, 0 };
FrontEndCallbackCommand front_end_callback_command_014 = { 7, front_end_draw_rider_bike_statistics,
                                                           5, 0 };
FrontEndCallbackCommand front_end_callback_command_015 = { 7, front_end_draw_rider_bike_statistics,
                                                           4, 0 };
FrontEndCallbackCommand front_end_callback_command_016 = { 7, front_end_draw_rider_bike_statistics,
                                                           6, 0 };
FrontEndCallbackCommand front_end_callback_command_017 = { 7, front_end_draw_rider_bike_statistics,
                                                           3, 0 };
FrontEndCallbackCommand front_end_callback_command_018 = { 7, front_end_draw_rider_bike_statistics,
                                                           1, 0 };
FrontEndShowCelsCommand front_end_show_cels_command_034 = { 1, front_end_cel_group_038 };
FrontEndShowCelsCommand front_end_show_cels_command_035 = { 1, front_end_cel_group_039 };
FrontEndShowCelsCommand front_end_show_cels_command_036 = { 1, front_end_cel_group_040 };
FrontEndShowCelsCommand front_end_show_cels_command_037 = { 1, front_end_cel_group_041 };
FrontEndShowCelsCommand front_end_show_cels_command_038 = { 1, front_end_cel_group_042 };
FrontEndShowCelsCommand front_end_show_cels_command_039 = { 1, front_end_cel_group_043 };
FrontEndShowCelsCommand front_end_show_cels_command_040 = { 1, front_end_cel_group_044 };
FrontEndShowCelsCommand front_end_show_cels_command_041 = { 1, front_end_cel_group_045 };
FrontEndCallbackCommand front_end_callback_command_019 = { 7, front_end_reset_random_rider_preview,
                                                           0, 0 };
FrontEndCallbackCommand front_end_callback_command_020 = { 7,
                                                           front_end_advance_random_rider_preview,
                                                           0, 0 };
FrontEndShowCelsCommand front_end_show_cels_command_042 = { 1, front_end_cel_group_048 };
FrontEndShowCelsCommand front_end_show_cels_command_043 = { 1, front_end_cel_group_049 };
FrontEndShowCelsCommand front_end_show_cels_command_044 = { 1, front_end_cel_group_050 };
FrontEndShowCelsCommand front_end_show_cels_command_045 = { 1, front_end_cel_group_051 };
FrontEndShowCelsCommand front_end_show_cels_command_046 = { 1, front_end_cel_group_052 };
FrontEndShowCelsCommand front_end_show_cels_command_047 = { 1, front_end_cel_group_053 };
FrontEndShowCelsCommand front_end_show_cels_command_048 = { 1, front_end_cel_group_054 };
FrontEndShowCelsCommand front_end_show_cels_command_049 = { 1, front_end_cel_group_055 };
FrontEndShowCelsCommand front_end_show_cels_command_050 = { 1, front_end_cel_group_056 };
FrontEndShowCelsCommand front_end_show_cels_command_051 = { 1, front_end_cel_group_057 };
FrontEndShowCelsCommand front_end_show_cels_command_052 = { 1, front_end_cel_group_058 };
FrontEndShowCelsCommand front_end_show_cels_command_053 = { 1, front_end_cel_group_059 };
FrontEndShowCelsCommand front_end_show_cels_command_054 = { 1, front_end_cel_group_060 };
FrontEndShowCelsCommand front_end_show_cels_command_055 = { 1, front_end_cel_group_061 };
FrontEndShowCelsCommand front_end_show_cels_command_056 = { 1, front_end_cel_group_062 };
FrontEndShowCelsCommand front_end_show_cels_command_057 = { 1, front_end_cel_group_063 };
FrontEndShowCelsCommand front_end_show_cels_command_058 = { 1, front_end_cel_group_078 };
FrontEndShowCelsCommand front_end_show_cels_command_059 = { 1, front_end_cel_group_079 };
FrontEndCallbackCommand front_end_callback_command_021 = { 7, front_end_show_game_mode_descriptor,
                                                           0, 0 };
FrontEndShowCelsCommand front_end_show_cels_command_060 = { 1, front_end_cel_group_080 };
FrontEndShowCelsCommand front_end_show_cels_command_061 = { 1, front_end_cel_group_081 };
FrontEndShowCelsCommand front_end_show_cels_command_062 = { 1, front_end_cel_group_082 };
FrontEndShowCelsCommand front_end_show_cels_command_063 = { 1, front_end_cel_group_083 };
FrontEndShowCelsCommand front_end_show_cels_command_064 = { 1, front_end_cel_group_084 };
FrontEndShowCelsCommand front_end_show_cels_command_065 = { 1, front_end_cel_group_085 };
FrontEndShowCelsCommand front_end_show_cels_command_066 = { 1, front_end_cel_group_086 };
FrontEndShowCelsCommand front_end_show_cels_command_067 = { 1, front_end_cel_group_087 };
FrontEndShowCelsCommand front_end_show_cels_command_068 = { 1, front_end_cel_group_088 };
FrontEndShowCelsCommand front_end_show_cels_command_069 = { 1, front_end_cel_group_089 };
FrontEndShowCelsCommand front_end_show_cels_command_070 = { 1, front_end_cel_group_090 };
FrontEndShowCelsCommand front_end_show_cels_command_071 = { 1, front_end_cel_group_091 };
FrontEndShowCelsCommand front_end_show_cels_command_072 = { 1, front_end_cel_group_092 };
FrontEndShowCelsCommand front_end_show_cels_command_073 = { 1, front_end_cel_group_093 };
FrontEndShowCelsCommand front_end_show_cels_command_074 = { 1, front_end_cel_group_094 };
FrontEndShowCelsCommand front_end_show_cels_command_075 = { 1, front_end_cel_group_095 };
FrontEndShowCelsCommand front_end_show_cels_command_076 = { 1, front_end_cel_group_096 };
FrontEndShowCelsCommand front_end_show_cels_command_077 = { 1, front_end_cel_group_097 };
FrontEndShowCelsCommand front_end_show_cels_command_078 = { 1, front_end_cel_group_098 };
FrontEndShowCelsCommand front_end_show_cels_command_079 = { 1, front_end_cel_group_099 };
FrontEndShowCelsCommand front_end_show_cels_command_080 = { 1, front_end_cel_group_100 };
FrontEndShowCelsCommand front_end_show_cels_command_081 = { 1, front_end_cel_group_101 };
FrontEndShowCelsCommand front_end_show_cels_command_082 = { 1, front_end_cel_group_102 };
FrontEndShowCelsCommand front_end_show_cels_command_083 = { 1, front_end_cel_group_103 };
FrontEndShowCelsCommand front_end_show_cels_command_084 = { 1, front_end_cel_group_104 };
FrontEndShowCelsCommand front_end_show_cels_command_085 = { 1, front_end_cel_group_105 };
FrontEndShowCelsCommand front_end_show_cels_command_086 = { 1, front_end_cel_group_106 };
FrontEndShowCelsCommand front_end_show_cels_command_087 = { 1, front_end_cel_group_107 };
FrontEndCallbackCommand front_end_callback_command_022 = { 7, front_end_set_selected_level, 0, 0 };
FrontEndShowCelsCommand front_end_show_cels_command_088 = { 1, front_end_cel_group_108 };
FrontEndCallbackCommand front_end_callback_command_023 = { 7, front_end_set_selected_level, 1, 0 };
FrontEndShowCelsCommand front_end_show_cels_command_089 = { 1, front_end_cel_group_109 };
FrontEndCallbackCommand front_end_callback_command_024 = { 7, front_end_set_selected_level, 2, 0 };
FrontEndShowCelsCommand front_end_show_cels_command_090 = { 1, front_end_cel_group_110 };
FrontEndCallbackCommand front_end_callback_command_025 = { 7, front_end_set_selected_level, 3, 0 };
FrontEndShowCelsCommand front_end_show_cels_command_091 = { 1, front_end_cel_group_111 };
FrontEndCallbackCommand front_end_callback_command_026 = { 7, front_end_set_selected_level, 4, 0 };
FrontEndShowCelsCommand front_end_show_cels_command_092 = { 1, front_end_cel_group_112 };
FrontEndShowCelsCommand front_end_show_cels_command_093 = { 1, front_end_cel_group_113 };
FrontEndShowCelsCommand front_end_show_cels_command_094 = { 1, front_end_cel_group_114 };
FrontEndShowCelsCommand front_end_show_cels_command_095 = { 1, front_end_cel_group_115 };
FrontEndShowCelsCommand front_end_show_cels_command_096 = { 1, front_end_cel_group_116 };
FrontEndCallbackCommand front_end_callback_command_027 = { 7,
                                                           front_end_show_profile_slot_descriptor,
                                                           0, 0 };
FrontEndCallbackCommand front_end_callback_command_028 = { 7,
                                                           front_end_show_selected_player_descriptor,
                                                           0, 0 };
FrontEndShowCelsCommand front_end_show_cels_command_097 = { 1, front_end_cel_group_117 };
FrontEndShowCelsCommand front_end_show_cels_command_098 = { 1, front_end_cel_group_118 };
FrontEndShowCelsCommand front_end_show_cels_command_099 = { 1, front_end_cel_group_119 };
FrontEndShowCelsCommand front_end_show_cels_command_100 = { 1, front_end_cel_group_120 };
FrontEndShowCelsCommand front_end_show_cels_command_101 = { 1, front_end_cel_group_121 };
FrontEndShowCelsCommand front_end_show_cels_command_102 = { 1, front_end_cel_group_122 };
FrontEndShowCelsCommand front_end_show_cels_command_103 = { 1, front_end_cel_group_123 };
FrontEndShowCelsCommand front_end_show_cels_command_104 = { 1, front_end_cel_group_124 };
FrontEndShowCelsCommand front_end_show_cels_command_105 = { 1, front_end_cel_group_125 };
FrontEndShowCelsCommand front_end_show_cels_command_106 = { 1, front_end_cel_group_126 };
FrontEndCallbackCommand front_end_callback_command_029 = { 7, front_end_noop, 0, 0 };
FrontEndShowCelsCommand front_end_show_cels_command_107 = { 1, front_end_cel_group_127 };
FrontEndShowCelsCommand front_end_show_cels_command_108 = { 1, front_end_cel_group_128 };
FrontEndShowCelsCommand front_end_show_cels_command_109 = { 1, front_end_cel_group_129 };
FrontEndShowCelsCommand front_end_show_cels_command_110 = { 1, front_end_cel_group_130 };
FrontEndShowCelsCommand front_end_show_cels_command_111 = { 1, front_end_cel_group_131 };
FrontEndShowCelsCommand front_end_show_cels_command_112 = { 1, front_end_cel_group_132 };
FrontEndShowCelsCommand front_end_show_cels_command_113 = { 1, front_end_cel_group_133 };
FrontEndShowCelsCommand front_end_show_cels_command_114 = { 1, front_end_cel_group_134 };
FrontEndCallbackCommand front_end_callback_command_030 = { 7, front_end_preload_selected_opponent,
                                                           0, 0 };
FrontEndCallbackCommand front_end_callback_command_031 = { 7, front_end_preload_selected_opponent,
                                                           1, 0 };
FrontEndCallbackCommand front_end_callback_command_032 = { 7, front_end_preload_selected_opponent,
                                                           2, 0 };
FrontEndCallbackCommand front_end_callback_command_033 = { 7, front_end_preload_selected_opponent,
                                                           3, 0 };
FrontEndCallbackCommand front_end_callback_command_034 = { 7,
                                                           front_end_draw_rider_style_affordability,
                                                           0, 0 };
FrontEndCallbackCommand front_end_callback_command_035 = { 7,
                                                           front_end_draw_rider_style_affordability,
                                                           1, 0 };
FrontEndCallbackCommand front_end_callback_command_036 = { 7,
                                                           front_end_draw_rider_style_affordability,
                                                           2, 0 };
FrontEndCallbackCommand front_end_callback_command_037 = { 7,
                                                           front_end_draw_rider_style_affordability,
                                                           3, 0 };
FrontEndCallbackCommand front_end_callback_command_038 = { 7,
                                                           front_end_draw_rider_style_affordability,
                                                           4, 0 };
FrontEndCallbackCommand front_end_callback_command_039 = { 7, front_end_draw_rider_style_price, 0,
                                                           0 };
FrontEndCallbackCommand front_end_callback_command_040 = { 7, front_end_draw_rider_style_price, 1,
                                                           0 };
FrontEndCallbackCommand front_end_callback_command_041 = { 7, front_end_draw_rider_style_price, 2,
                                                           0 };
FrontEndCallbackCommand front_end_callback_command_042 = { 7, front_end_draw_rider_style_price, 3,
                                                           0 };
FrontEndCallbackCommand front_end_callback_command_043 = { 7, front_end_draw_rider_style_price, 4,
                                                           0 };
FrontEndCallbackCommand front_end_callback_command_044 = { 7, front_end_draw_current_balance, 0,
                                                           0 };
FrontEndShowCelsCommand front_end_show_cels_command_115 = { 1, front_end_cel_group_135 };
FrontEndShowCelsCommand front_end_show_cels_command_116 = { 1, front_end_cel_group_136 };
FrontEndShowCelsCommand front_end_show_cels_command_117 = { 1, front_end_cel_group_137 };
FrontEndShowCelsCommand front_end_show_cels_command_118 = { 1, front_end_cel_group_138 };
FrontEndShowCelsCommand front_end_show_cels_command_119 = { 1, front_end_cel_group_139 };
FrontEndShowCelsCommand front_end_show_cels_command_120 = { 1, front_end_cel_group_140 };
FrontEndShowCelsCommand front_end_show_cels_command_121 = { 1, front_end_cel_group_141 };
FrontEndShowCelsCommand front_end_show_cels_command_122 = { 1, front_end_cel_group_142 };
FrontEndShowCelsCommand front_end_show_cels_command_123 = { 1, front_end_cel_group_143 };
FrontEndShowCelsCommand front_end_show_cels_command_124 = { 1, front_end_cel_group_144 };
FrontEndShowCelsCommand front_end_show_cels_command_125 = { 1, front_end_cel_group_145 };
FrontEndShowCelsCommand front_end_show_cels_command_126 = { 1, front_end_cel_group_146 };
FrontEndPlayStreamCommand front_end_play_stream_command_000 = { 4,
                                                                bike_shop_rat_perro_stream_path };
FrontEndShowCelsCommand front_end_show_cels_command_127 = { 1, front_end_cel_group_147 };
FrontEndShowCelsCommand front_end_show_cels_command_128 = { 1, front_end_cel_group_148 };
FrontEndShowCelsCommand front_end_show_cels_command_129 = { 1, front_end_cel_group_149 };
FrontEndShowCelsCommand front_end_show_cels_command_130 = { 1, front_end_cel_group_150 };
FrontEndShowCelsCommand front_end_show_cels_command_131 = { 1, front_end_cel_group_151 };
FrontEndShowCelsCommand front_end_show_cels_command_132 = { 1, front_end_cel_group_152 };
FrontEndPlayStreamCommand front_end_play_stream_command_001 = { 4,
                                                                bike_shop_rat_corsair_stream_path };
FrontEndShowCelsCommand front_end_show_cels_command_133 = { 1, front_end_cel_group_153 };
FrontEndShowCelsCommand front_end_show_cels_command_134 = { 1, front_end_cel_group_154 };
FrontEndShowCelsCommand front_end_show_cels_command_135 = { 1, front_end_cel_group_155 };
FrontEndShowCelsCommand front_end_show_cels_command_136 = { 1, front_end_cel_group_156 };
FrontEndShowCelsCommand front_end_show_cels_command_137 = { 1, front_end_cel_group_157 };
FrontEndShowCelsCommand front_end_show_cels_command_138 = { 1, front_end_cel_group_158 };
FrontEndPlayStreamCommand front_end_play_stream_command_002 = { 4,
                                                                bike_shop_rat_banzai_stream_path };
FrontEndShowCelsCommand front_end_show_cels_command_139 = { 1, front_end_cel_group_159 };
FrontEndShowCelsCommand front_end_show_cels_command_140 = { 1, front_end_cel_group_160 };
FrontEndShowCelsCommand front_end_show_cels_command_141 = { 1, front_end_cel_group_161 };
FrontEndShowCelsCommand front_end_show_cels_command_142 = { 1, front_end_cel_group_162 };
FrontEndShowCelsCommand front_end_show_cels_command_143 = { 1, front_end_cel_group_163 };
FrontEndShowCelsCommand front_end_show_cels_command_144 = { 1, front_end_cel_group_164 };
FrontEndPlayStreamCommand front_end_play_stream_command_003 = { 4,
                                                                bike_shop_rat_killer_stream_path };
FrontEndShowCelsCommand front_end_show_cels_command_145 = { 1, front_end_cel_group_165 };
FrontEndShowCelsCommand front_end_show_cels_command_146 = { 1, front_end_cel_group_166 };
FrontEndShowCelsCommand front_end_show_cels_command_147 = { 1, front_end_cel_group_167 };
FrontEndShowCelsCommand front_end_show_cels_command_148 = { 1, front_end_cel_group_168 };
FrontEndShowCelsCommand front_end_show_cels_command_149 = { 1, front_end_cel_group_169 };
FrontEndShowCelsCommand front_end_show_cels_command_150 = { 1, front_end_cel_group_170 };
FrontEndPlayStreamCommand front_end_play_stream_command_004 = { 4,
                                                                bike_shop_rat_kamakazi_stream_path };
FrontEndShowCelsCommand front_end_show_cels_command_151 = { 1, front_end_cel_group_171 };
FrontEndShowCelsCommand front_end_show_cels_command_152 = { 1, front_end_cel_group_172 };
FrontEndShowCelsCommand front_end_show_cels_command_153 = { 1, front_end_cel_group_173 };
FrontEndShowCelsCommand front_end_show_cels_command_154 = { 1, front_end_cel_group_174 };
FrontEndShowCelsCommand front_end_show_cels_command_155 = { 1, front_end_cel_group_175 };
FrontEndShowCelsCommand front_end_show_cels_command_156 = { 1, front_end_cel_group_176 };
FrontEndCallbackCommand front_end_callback_command_045 = { 7,
                                                           front_end_draw_rider_style_affordability,
                                                           10, 0 };
FrontEndCallbackCommand front_end_callback_command_046 = { 7,
                                                           front_end_draw_rider_style_affordability,
                                                           11, 0 };
FrontEndCallbackCommand front_end_callback_command_047 = { 7,
                                                           front_end_draw_rider_style_affordability,
                                                           12, 0 };
FrontEndCallbackCommand front_end_callback_command_048 = { 7,
                                                           front_end_draw_rider_style_affordability,
                                                           13, 0 };
FrontEndCallbackCommand front_end_callback_command_049 = { 7,
                                                           front_end_draw_rider_style_affordability,
                                                           14, 0 };
FrontEndCallbackCommand front_end_callback_command_050 = { 7, front_end_draw_rider_style_price, 10,
                                                           0 };
FrontEndCallbackCommand front_end_callback_command_051 = { 7, front_end_draw_rider_style_price, 11,
                                                           0 };
FrontEndCallbackCommand front_end_callback_command_052 = { 7, front_end_draw_rider_style_price, 12,
                                                           0 };
FrontEndCallbackCommand front_end_callback_command_053 = { 7, front_end_draw_rider_style_price, 13,
                                                           0 };
FrontEndCallbackCommand front_end_callback_command_054 = { 7, front_end_draw_rider_style_price, 14,
                                                           0 };
FrontEndShowCelsCommand front_end_show_cels_command_157 = { 1, front_end_cel_group_177 };
FrontEndShowCelsCommand front_end_show_cels_command_158 = { 1, front_end_cel_group_178 };
FrontEndShowCelsCommand front_end_show_cels_command_159 = { 1, front_end_cel_group_179 };
FrontEndShowCelsCommand front_end_show_cels_command_160 = { 1, front_end_cel_group_180 };
FrontEndShowCelsCommand front_end_show_cels_command_161 = { 1, front_end_cel_group_181 };
FrontEndShowCelsCommand front_end_show_cels_command_162 = { 1, front_end_cel_group_182 };
FrontEndShowCelsCommand front_end_show_cels_command_163 = { 1, front_end_cel_group_183 };
FrontEndShowCelsCommand front_end_show_cels_command_164 = { 1, front_end_cel_group_184 };
FrontEndShowCelsCommand front_end_show_cels_command_165 = { 1, front_end_cel_group_185 };
FrontEndShowCelsCommand front_end_show_cels_command_166 = { 1, front_end_cel_group_186 };
FrontEndShowCelsCommand front_end_show_cels_command_167 = { 1, front_end_cel_group_187 };
FrontEndShowCelsCommand front_end_show_cels_command_168 = { 1, front_end_cel_group_188 };
FrontEndPlayStreamCommand front_end_play_stream_command_005 = { 4,
                                                                bike_shop_sport_stiletto_stream_path };
FrontEndShowCelsCommand front_end_show_cels_command_169 = { 1, front_end_cel_group_189 };
FrontEndShowCelsCommand front_end_show_cels_command_170 = { 1, front_end_cel_group_190 };
FrontEndShowCelsCommand front_end_show_cels_command_171 = { 1, front_end_cel_group_191 };
FrontEndShowCelsCommand front_end_show_cels_command_172 = { 1, front_end_cel_group_192 };
FrontEndShowCelsCommand front_end_show_cels_command_173 = { 1, front_end_cel_group_193 };
FrontEndShowCelsCommand front_end_show_cels_command_174 = { 1, front_end_cel_group_194 };
FrontEndPlayStreamCommand front_end_play_stream_command_006 = { 4,
                                                                bike_shop_sport_diablo_stream_path };
FrontEndShowCelsCommand front_end_show_cels_command_175 = { 1, front_end_cel_group_195 };
FrontEndShowCelsCommand front_end_show_cels_command_176 = { 1, front_end_cel_group_196 };
FrontEndShowCelsCommand front_end_show_cels_command_177 = { 1, front_end_cel_group_197 };
FrontEndShowCelsCommand front_end_show_cels_command_178 = { 1, front_end_cel_group_198 };
FrontEndShowCelsCommand front_end_show_cels_command_179 = { 1, front_end_cel_group_199 };
FrontEndShowCelsCommand front_end_show_cels_command_180 = { 1, front_end_cel_group_200 };
FrontEndPlayStreamCommand front_end_play_stream_command_007 = { 4,
                                                                bike_shop_sport_perro_stream_path };
FrontEndShowCelsCommand front_end_show_cels_command_181 = { 1, front_end_cel_group_201 };
FrontEndShowCelsCommand front_end_show_cels_command_182 = { 1, front_end_cel_group_202 };
FrontEndShowCelsCommand front_end_show_cels_command_183 = { 1, front_end_cel_group_203 };
FrontEndShowCelsCommand front_end_show_cels_command_184 = { 1, front_end_cel_group_204 };
FrontEndShowCelsCommand front_end_show_cels_command_185 = { 1, front_end_cel_group_205 };
FrontEndShowCelsCommand front_end_show_cels_command_186 = { 1, front_end_cel_group_206 };
FrontEndPlayStreamCommand front_end_play_stream_command_008 = { 4,
                                                                bike_shop_sport_dmg_stream_path };
FrontEndShowCelsCommand front_end_show_cels_command_187 = { 1, front_end_cel_group_207 };
FrontEndShowCelsCommand front_end_show_cels_command_188 = { 1, front_end_cel_group_208 };
FrontEndShowCelsCommand front_end_show_cels_command_189 = { 1, front_end_cel_group_209 };
FrontEndShowCelsCommand front_end_show_cels_command_190 = { 1, front_end_cel_group_210 };
FrontEndShowCelsCommand front_end_show_cels_command_191 = { 1, front_end_cel_group_211 };
FrontEndShowCelsCommand front_end_show_cels_command_192 = { 1, front_end_cel_group_212 };
FrontEndPlayStreamCommand front_end_play_stream_command_009 = { 4,
                                                                bike_shop_sport_kamakazi_stream_path };
FrontEndShowCelsCommand front_end_show_cels_command_193 = { 1, front_end_cel_group_213 };
FrontEndShowCelsCommand front_end_show_cels_command_194 = { 1, front_end_cel_group_214 };
FrontEndShowCelsCommand front_end_show_cels_command_195 = { 1, front_end_cel_group_215 };
FrontEndShowCelsCommand front_end_show_cels_command_196 = { 1, front_end_cel_group_216 };
FrontEndShowCelsCommand front_end_show_cels_command_197 = { 1, front_end_cel_group_217 };
FrontEndShowCelsCommand front_end_show_cels_command_198 = { 1, front_end_cel_group_218 };
FrontEndCallbackCommand front_end_callback_command_055 = { 7,
                                                           front_end_draw_rider_style_affordability,
                                                           5, 0 };
FrontEndCallbackCommand front_end_callback_command_056 = { 7,
                                                           front_end_draw_rider_style_affordability,
                                                           6, 0 };
FrontEndCallbackCommand front_end_callback_command_057 = { 7,
                                                           front_end_draw_rider_style_affordability,
                                                           7, 0 };
FrontEndCallbackCommand front_end_callback_command_058 = { 7,
                                                           front_end_draw_rider_style_affordability,
                                                           8, 0 };
FrontEndCallbackCommand front_end_callback_command_059 = { 7,
                                                           front_end_draw_rider_style_affordability,
                                                           9, 0 };
FrontEndCallbackCommand front_end_callback_command_060 = { 7, front_end_draw_rider_style_price, 5,
                                                           0 };
FrontEndCallbackCommand front_end_callback_command_061 = { 7, front_end_draw_rider_style_price, 6,
                                                           0 };
FrontEndCallbackCommand front_end_callback_command_062 = { 7, front_end_draw_rider_style_price, 7,
                                                           0 };
FrontEndCallbackCommand front_end_callback_command_063 = { 7, front_end_draw_rider_style_price, 8,
                                                           0 };
FrontEndCallbackCommand front_end_callback_command_064 = { 7, front_end_draw_rider_style_price, 9,
                                                           0 };
FrontEndShowCelsCommand front_end_show_cels_command_199 = { 1, front_end_cel_group_219 };
FrontEndShowCelsCommand front_end_show_cels_command_200 = { 1, front_end_cel_group_220 };
FrontEndShowCelsCommand front_end_show_cels_command_201 = { 1, front_end_cel_group_221 };
FrontEndShowCelsCommand front_end_show_cels_command_202 = { 1, front_end_cel_group_222 };
FrontEndShowCelsCommand front_end_show_cels_command_203 = { 1, front_end_cel_group_223 };
FrontEndShowCelsCommand front_end_show_cels_command_204 = { 1, front_end_cel_group_224 };
FrontEndShowCelsCommand front_end_show_cels_command_205 = { 1, front_end_cel_group_225 };
FrontEndShowCelsCommand front_end_show_cels_command_206 = { 1, front_end_cel_group_226 };
FrontEndShowCelsCommand front_end_show_cels_command_207 = { 1, front_end_cel_group_227 };
FrontEndShowCelsCommand front_end_show_cels_command_208 = { 1, front_end_cel_group_228 };
FrontEndShowCelsCommand front_end_show_cels_command_209 = { 1, front_end_cel_group_229 };
FrontEndShowCelsCommand front_end_show_cels_command_210 = { 1, front_end_cel_group_230 };
FrontEndPlayStreamCommand front_end_play_stream_command_010 = { 4,
                                                                bike_shop_super_stiletto_stream_path };
FrontEndShowCelsCommand front_end_show_cels_command_211 = { 1, front_end_cel_group_231 };
FrontEndShowCelsCommand front_end_show_cels_command_212 = { 1, front_end_cel_group_232 };
FrontEndShowCelsCommand front_end_show_cels_command_213 = { 1, front_end_cel_group_233 };
FrontEndShowCelsCommand front_end_show_cels_command_214 = { 1, front_end_cel_group_234 };
FrontEndShowCelsCommand front_end_show_cels_command_215 = { 1, front_end_cel_group_235 };
FrontEndShowCelsCommand front_end_show_cels_command_216 = { 1, front_end_cel_group_236 };
FrontEndPlayStreamCommand front_end_play_stream_command_011 = { 4,
                                                                bike_shop_super_diablo_stream_path };
FrontEndShowCelsCommand front_end_show_cels_command_217 = { 1, front_end_cel_group_237 };
FrontEndShowCelsCommand front_end_show_cels_command_218 = { 1, front_end_cel_group_238 };
FrontEndShowCelsCommand front_end_show_cels_command_219 = { 1, front_end_cel_group_239 };
FrontEndShowCelsCommand front_end_show_cels_command_220 = { 1, front_end_cel_group_240 };
FrontEndShowCelsCommand front_end_show_cels_command_221 = { 1, front_end_cel_group_241 };
FrontEndShowCelsCommand front_end_show_cels_command_222 = { 1, front_end_cel_group_242 };
FrontEndPlayStreamCommand front_end_play_stream_command_012 = { 4,
                                                                bike_shop_super_corsair_stream_path };
FrontEndShowCelsCommand front_end_show_cels_command_223 = { 1, front_end_cel_group_243 };
FrontEndShowCelsCommand front_end_show_cels_command_224 = { 1, front_end_cel_group_244 };
FrontEndShowCelsCommand front_end_show_cels_command_225 = { 1, front_end_cel_group_245 };
FrontEndShowCelsCommand front_end_show_cels_command_226 = { 1, front_end_cel_group_246 };
FrontEndShowCelsCommand front_end_show_cels_command_227 = { 1, front_end_cel_group_247 };
FrontEndShowCelsCommand front_end_show_cels_command_228 = { 1, front_end_cel_group_248 };
FrontEndPlayStreamCommand front_end_play_stream_command_013 = { 4,
                                                                bike_shop_super_banzai_stream_path };
FrontEndShowCelsCommand front_end_show_cels_command_229 = { 1, front_end_cel_group_249 };
FrontEndShowCelsCommand front_end_show_cels_command_230 = { 1, front_end_cel_group_250 };
FrontEndShowCelsCommand front_end_show_cels_command_231 = { 1, front_end_cel_group_251 };
FrontEndShowCelsCommand front_end_show_cels_command_232 = { 1, front_end_cel_group_252 };
FrontEndShowCelsCommand front_end_show_cels_command_233 = { 1, front_end_cel_group_253 };
FrontEndShowCelsCommand front_end_show_cels_command_234 = { 1, front_end_cel_group_254 };
FrontEndPlayStreamCommand front_end_play_stream_command_014 = { 4,
                                                                bike_shop_super_kamakazi_stream_path };
FrontEndShowCelsCommand front_end_show_cels_command_235 = { 1, front_end_cel_group_255 };
FrontEndShowCelsCommand front_end_show_cels_command_236 = { 1, front_end_cel_group_256 };
FrontEndShowCelsCommand front_end_show_cels_command_237 = { 1, front_end_cel_group_257 };
FrontEndShowCelsCommand front_end_show_cels_command_238 = { 1, front_end_cel_group_258 };
FrontEndShowCelsCommand front_end_show_cels_command_239 = { 1, front_end_cel_group_259 };
FrontEndShowCelsCommand front_end_show_cels_command_240 = { 1, front_end_cel_group_260 };
FrontEndShowCelsCommand front_end_show_cels_command_241 = { 1, front_end_cel_group_269 };
FrontEndShowCelsCommand front_end_show_cels_command_242 = { 1, front_end_cel_group_270 };
FrontEndCallbackCommand front_end_callback_command_065 = { 7, front_end_show_rider_type_descriptor,
                                                           0, 0 };
FrontEndShowCelsCommand front_end_show_cels_command_243 = { 1, front_end_cel_group_271 };
FrontEndShowCelsCommand front_end_show_cels_command_244 = { 1, front_end_cel_group_272 };
FrontEndCallbackCommand front_end_callback_command_066 = { 7, front_end_show_rider_type_descriptor,
                                                           0, 0 };
FrontEndShowCelsCommand front_end_show_cels_command_245 = { 1, front_end_cel_group_273 };
FrontEndShowCelsCommand front_end_show_cels_command_246 = { 1, front_end_cel_group_274 };
FrontEndShowCelsCommand front_end_show_cels_command_247 = { 1, front_end_cel_group_275 };
FrontEndShowCelsCommand front_end_show_cels_command_248 = { 1, front_end_cel_group_276 };
FrontEndShowCelsCommand front_end_show_cels_command_249 = { 1, front_end_cel_group_277 };
FrontEndShowCelsCommand front_end_show_cels_command_250 = { 1, front_end_cel_group_278 };
FrontEndShowCelsCommand front_end_show_cels_command_251 = { 1, front_end_cel_group_279 };
FrontEndShowCelsCommand front_end_show_cels_command_252 = { 1, front_end_cel_group_280 };
FrontEndCallbackCommand front_end_callback_command_067 = { 7, front_end_load_mode_backdrop, 0, 0 };
FrontEndCallbackCommand front_end_callback_command_068 = { 7, front_end_finalize_race_setup, 0, 0 };
FrontEndCallbackCommand front_end_callback_command_069 = { 7, front_end_show_race_mode_descriptor,
                                                           0, 0 };
FrontEndCallbackCommand front_end_callback_command_070 = { 7, front_end_update_rider_progression, 0,
                                                           0 };
FrontEndCallbackCommand front_end_callback_command_071 = { 7, front_end_show_selected_level, 0, 0 };

FrontEndCommandHeader *front_end_command_sequence_000[8] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_001,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  (FrontEndCommandHeader *)&front_end_wait_command_001,
  (FrontEndCommandHeader *)&front_end_show_cels_command_002,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  (FrontEndCommandHeader *)&front_end_wait_command_001,
  (FrontEndCommandHeader *)&front_end_callback_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_001[5] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_004,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  (FrontEndCommandHeader *)&front_end_show_cels_command_005,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_002[5] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_006,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  (FrontEndCommandHeader *)&front_end_show_cels_command_007,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_003[5] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_008,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  (FrontEndCommandHeader *)&front_end_show_cels_command_009,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_004[2] =
{
  (FrontEndCommandHeader *)&front_end_callback_command_009,
  0
};

FrontEndCommandHeader *front_end_command_sequence_005[7] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_010,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  (FrontEndCommandHeader *)&front_end_show_cels_command_011,
  (FrontEndCommandHeader *)&front_end_callback_command_004,
  (FrontEndCommandHeader *)&front_end_callback_command_002,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_006[7] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_012,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  (FrontEndCommandHeader *)&front_end_show_cels_command_013,
  (FrontEndCommandHeader *)&front_end_callback_command_005,
  (FrontEndCommandHeader *)&front_end_callback_command_002,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_007[7] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_014,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  (FrontEndCommandHeader *)&front_end_show_cels_command_015,
  (FrontEndCommandHeader *)&front_end_callback_command_006,
  (FrontEndCommandHeader *)&front_end_callback_command_002,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_008[7] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_016,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  (FrontEndCommandHeader *)&front_end_show_cels_command_017,
  (FrontEndCommandHeader *)&front_end_callback_command_008,
  (FrontEndCommandHeader *)&front_end_callback_command_002,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_009[7] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_018,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  (FrontEndCommandHeader *)&front_end_show_cels_command_019,
  (FrontEndCommandHeader *)&front_end_callback_command_007,
  (FrontEndCommandHeader *)&front_end_callback_command_002,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_010[6] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_020,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  (FrontEndCommandHeader *)&front_end_show_cels_command_021,
  (FrontEndCommandHeader *)&front_end_callback_command_002,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_011[7] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_022,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  (FrontEndCommandHeader *)&front_end_show_cels_command_023,
  (FrontEndCommandHeader *)&front_end_callback_command_004,
  (FrontEndCommandHeader *)&front_end_callback_command_002,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_012[7] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_024,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  (FrontEndCommandHeader *)&front_end_show_cels_command_025,
  (FrontEndCommandHeader *)&front_end_callback_command_005,
  (FrontEndCommandHeader *)&front_end_callback_command_002,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_013[7] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_026,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  (FrontEndCommandHeader *)&front_end_show_cels_command_027,
  (FrontEndCommandHeader *)&front_end_callback_command_006,
  (FrontEndCommandHeader *)&front_end_callback_command_002,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_014[7] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_028,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  (FrontEndCommandHeader *)&front_end_show_cels_command_029,
  (FrontEndCommandHeader *)&front_end_callback_command_008,
  (FrontEndCommandHeader *)&front_end_callback_command_002,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_015[7] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_030,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  (FrontEndCommandHeader *)&front_end_show_cels_command_031,
  (FrontEndCommandHeader *)&front_end_callback_command_007,
  (FrontEndCommandHeader *)&front_end_callback_command_002,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_016[6] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_032,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  (FrontEndCommandHeader *)&front_end_show_cels_command_033,
  (FrontEndCommandHeader *)&front_end_callback_command_002,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_017[5] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_034,
  (FrontEndCommandHeader *)&front_end_callback_command_010,
  (FrontEndCommandHeader *)&front_end_callback_command_011,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_018[5] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_035,
  (FrontEndCommandHeader *)&front_end_callback_command_010,
  (FrontEndCommandHeader *)&front_end_callback_command_012,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_019[5] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_036,
  (FrontEndCommandHeader *)&front_end_callback_command_010,
  (FrontEndCommandHeader *)&front_end_callback_command_013,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_020[5] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_037,
  (FrontEndCommandHeader *)&front_end_callback_command_010,
  (FrontEndCommandHeader *)&front_end_callback_command_014,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_021[5] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_038,
  (FrontEndCommandHeader *)&front_end_callback_command_010,
  (FrontEndCommandHeader *)&front_end_callback_command_015,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_022[5] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_039,
  (FrontEndCommandHeader *)&front_end_callback_command_010,
  (FrontEndCommandHeader *)&front_end_callback_command_016,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_023[5] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_040,
  (FrontEndCommandHeader *)&front_end_callback_command_010,
  (FrontEndCommandHeader *)&front_end_callback_command_017,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_024[5] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_041,
  (FrontEndCommandHeader *)&front_end_callback_command_010,
  (FrontEndCommandHeader *)&front_end_callback_command_018,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_025[7] =
{
  (FrontEndCommandHeader *)&front_end_callback_command_019,
  (FrontEndCommandHeader *)&front_end_show_cels_command_042,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  (FrontEndCommandHeader *)&front_end_callback_command_019,
  (FrontEndCommandHeader *)&front_end_show_cels_command_043,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_026[4] =
{
  (FrontEndCommandHeader *)&front_end_callback_command_019,
  (FrontEndCommandHeader *)&front_end_show_cels_command_044,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_027[4] =
{
  (FrontEndCommandHeader *)&front_end_callback_command_019,
  (FrontEndCommandHeader *)&front_end_show_cels_command_045,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_028[7] =
{
  (FrontEndCommandHeader *)&front_end_callback_command_019,
  (FrontEndCommandHeader *)&front_end_show_cels_command_046,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  (FrontEndCommandHeader *)&front_end_callback_command_019,
  (FrontEndCommandHeader *)&front_end_show_cels_command_047,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_029[7] =
{
  (FrontEndCommandHeader *)&front_end_callback_command_019,
  (FrontEndCommandHeader *)&front_end_show_cels_command_048,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  (FrontEndCommandHeader *)&front_end_callback_command_019,
  (FrontEndCommandHeader *)&front_end_show_cels_command_049,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_030[7] =
{
  (FrontEndCommandHeader *)&front_end_callback_command_019,
  (FrontEndCommandHeader *)&front_end_show_cels_command_050,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  (FrontEndCommandHeader *)&front_end_callback_command_019,
  (FrontEndCommandHeader *)&front_end_show_cels_command_051,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_031[7] =
{
  (FrontEndCommandHeader *)&front_end_callback_command_019,
  (FrontEndCommandHeader *)&front_end_show_cels_command_052,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  (FrontEndCommandHeader *)&front_end_callback_command_019,
  (FrontEndCommandHeader *)&front_end_show_cels_command_053,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_032[7] =
{
  (FrontEndCommandHeader *)&front_end_callback_command_019,
  (FrontEndCommandHeader *)&front_end_show_cels_command_054,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  (FrontEndCommandHeader *)&front_end_callback_command_019,
  (FrontEndCommandHeader *)&front_end_show_cels_command_055,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_033[7] =
{
  (FrontEndCommandHeader *)&front_end_callback_command_019,
  (FrontEndCommandHeader *)&front_end_show_cels_command_056,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  (FrontEndCommandHeader *)&front_end_callback_command_019,
  (FrontEndCommandHeader *)&front_end_show_cels_command_057,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_034[8] =
{
  (FrontEndCommandHeader *)&front_end_callback_command_019,
  (FrontEndCommandHeader *)&front_end_show_cels_command_058,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  (FrontEndCommandHeader *)&front_end_callback_command_019,
  (FrontEndCommandHeader *)&front_end_show_cels_command_059,
  (FrontEndCommandHeader *)&front_end_callback_command_021,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_035[7] =
{
  (FrontEndCommandHeader *)&front_end_callback_command_019,
  (FrontEndCommandHeader *)&front_end_show_cels_command_060,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  (FrontEndCommandHeader *)&front_end_callback_command_019,
  (FrontEndCommandHeader *)&front_end_show_cels_command_061,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_036[4] =
{
  (FrontEndCommandHeader *)&front_end_callback_command_020,
  (FrontEndCommandHeader *)&front_end_show_cels_command_062,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_037[4] =
{
  (FrontEndCommandHeader *)&front_end_callback_command_020,
  (FrontEndCommandHeader *)&front_end_show_cels_command_063,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_038[4] =
{
  (FrontEndCommandHeader *)&front_end_callback_command_020,
  (FrontEndCommandHeader *)&front_end_show_cels_command_064,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_039[4] =
{
  (FrontEndCommandHeader *)&front_end_callback_command_020,
  (FrontEndCommandHeader *)&front_end_show_cels_command_065,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_040[4] =
{
  (FrontEndCommandHeader *)&front_end_callback_command_020,
  (FrontEndCommandHeader *)&front_end_show_cels_command_066,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_041[4] =
{
  (FrontEndCommandHeader *)&front_end_callback_command_020,
  (FrontEndCommandHeader *)&front_end_show_cels_command_067,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_042[4] =
{
  (FrontEndCommandHeader *)&front_end_callback_command_020,
  (FrontEndCommandHeader *)&front_end_show_cels_command_068,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_043[4] =
{
  (FrontEndCommandHeader *)&front_end_callback_command_020,
  (FrontEndCommandHeader *)&front_end_show_cels_command_069,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_044[4] =
{
  (FrontEndCommandHeader *)&front_end_callback_command_020,
  (FrontEndCommandHeader *)&front_end_show_cels_command_070,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_045[4] =
{
  (FrontEndCommandHeader *)&front_end_callback_command_020,
  (FrontEndCommandHeader *)&front_end_show_cels_command_071,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_046[4] =
{
  (FrontEndCommandHeader *)&front_end_callback_command_020,
  (FrontEndCommandHeader *)&front_end_show_cels_command_072,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_047[4] =
{
  (FrontEndCommandHeader *)&front_end_callback_command_020,
  (FrontEndCommandHeader *)&front_end_show_cels_command_073,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_048[4] =
{
  (FrontEndCommandHeader *)&front_end_callback_command_020,
  (FrontEndCommandHeader *)&front_end_show_cels_command_074,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_049[4] =
{
  (FrontEndCommandHeader *)&front_end_callback_command_020,
  (FrontEndCommandHeader *)&front_end_show_cels_command_075,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_050[4] =
{
  (FrontEndCommandHeader *)&front_end_callback_command_020,
  (FrontEndCommandHeader *)&front_end_show_cels_command_076,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_051[4] =
{
  (FrontEndCommandHeader *)&front_end_callback_command_020,
  (FrontEndCommandHeader *)&front_end_show_cels_command_077,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_052[4] =
{
  (FrontEndCommandHeader *)&front_end_callback_command_020,
  (FrontEndCommandHeader *)&front_end_show_cels_command_078,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_053[4] =
{
  (FrontEndCommandHeader *)&front_end_callback_command_020,
  (FrontEndCommandHeader *)&front_end_show_cels_command_079,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_054[4] =
{
  (FrontEndCommandHeader *)&front_end_callback_command_020,
  (FrontEndCommandHeader *)&front_end_show_cels_command_080,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_055[4] =
{
  (FrontEndCommandHeader *)&front_end_callback_command_020,
  (FrontEndCommandHeader *)&front_end_show_cels_command_081,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_056[4] =
{
  (FrontEndCommandHeader *)&front_end_callback_command_020,
  (FrontEndCommandHeader *)&front_end_show_cels_command_082,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_057[4] =
{
  (FrontEndCommandHeader *)&front_end_callback_command_019,
  (FrontEndCommandHeader *)&front_end_show_cels_command_083,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_058[4] =
{
  (FrontEndCommandHeader *)&front_end_callback_command_019,
  (FrontEndCommandHeader *)&front_end_show_cels_command_084,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_059[4] =
{
  (FrontEndCommandHeader *)&front_end_callback_command_019,
  (FrontEndCommandHeader *)&front_end_show_cels_command_085,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_060[4] =
{
  (FrontEndCommandHeader *)&front_end_callback_command_019,
  (FrontEndCommandHeader *)&front_end_show_cels_command_086,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_061[5] =
{
  (FrontEndCommandHeader *)&front_end_callback_command_019,
  (FrontEndCommandHeader *)&front_end_callback_command_022,
  (FrontEndCommandHeader *)&front_end_show_cels_command_087,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_062[5] =
{
  (FrontEndCommandHeader *)&front_end_callback_command_019,
  (FrontEndCommandHeader *)&front_end_callback_command_023,
  (FrontEndCommandHeader *)&front_end_show_cels_command_088,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_063[5] =
{
  (FrontEndCommandHeader *)&front_end_callback_command_019,
  (FrontEndCommandHeader *)&front_end_callback_command_024,
  (FrontEndCommandHeader *)&front_end_show_cels_command_089,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_064[5] =
{
  (FrontEndCommandHeader *)&front_end_callback_command_019,
  (FrontEndCommandHeader *)&front_end_callback_command_025,
  (FrontEndCommandHeader *)&front_end_show_cels_command_090,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_065[5] =
{
  (FrontEndCommandHeader *)&front_end_callback_command_019,
  (FrontEndCommandHeader *)&front_end_callback_command_026,
  (FrontEndCommandHeader *)&front_end_show_cels_command_091,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_066[4] =
{
  (FrontEndCommandHeader *)&front_end_callback_command_019,
  (FrontEndCommandHeader *)&front_end_show_cels_command_092,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_067[4] =
{
  (FrontEndCommandHeader *)&front_end_callback_command_019,
  (FrontEndCommandHeader *)&front_end_show_cels_command_093,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_068[4] =
{
  (FrontEndCommandHeader *)&front_end_callback_command_019,
  (FrontEndCommandHeader *)&front_end_show_cels_command_094,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_069[4] =
{
  (FrontEndCommandHeader *)&front_end_callback_command_019,
  (FrontEndCommandHeader *)&front_end_show_cels_command_095,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_070[4] =
{
  (FrontEndCommandHeader *)&front_end_callback_command_019,
  (FrontEndCommandHeader *)&front_end_show_cels_command_096,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_071[5] =
{
  (FrontEndCommandHeader *)&front_end_callback_command_019,
  (FrontEndCommandHeader *)&front_end_callback_command_027,
  (FrontEndCommandHeader *)&front_end_show_cels_command_097,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_072[4] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_098,
  (FrontEndCommandHeader *)&front_end_callback_command_028,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_073[4] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_099,
  (FrontEndCommandHeader *)&front_end_callback_command_028,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_074[4] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_100,
  (FrontEndCommandHeader *)&front_end_callback_command_028,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_075[4] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_101,
  (FrontEndCommandHeader *)&front_end_callback_command_028,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_076[4] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_102,
  (FrontEndCommandHeader *)&front_end_callback_command_028,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_077[4] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_103,
  (FrontEndCommandHeader *)&front_end_callback_command_028,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_078[4] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_104,
  (FrontEndCommandHeader *)&front_end_callback_command_028,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_079[4] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_105,
  (FrontEndCommandHeader *)&front_end_callback_command_028,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_080[4] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_106,
  (FrontEndCommandHeader *)&front_end_callback_command_029,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_081[5] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_107,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  (FrontEndCommandHeader *)&front_end_show_cels_command_108,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_082[5] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_109,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  (FrontEndCommandHeader *)&front_end_show_cels_command_110,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_083[5] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_111,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  (FrontEndCommandHeader *)&front_end_show_cels_command_112,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_084[5] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_113,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  (FrontEndCommandHeader *)&front_end_show_cels_command_114,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_085[3] =
{
  (FrontEndCommandHeader *)&front_end_callback_command_030,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_086[3] =
{
  (FrontEndCommandHeader *)&front_end_callback_command_031,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_087[3] =
{
  (FrontEndCommandHeader *)&front_end_callback_command_032,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_088[3] =
{
  (FrontEndCommandHeader *)&front_end_callback_command_033,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_089[7] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_115,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  (FrontEndCommandHeader *)&front_end_show_cels_command_116,
  (FrontEndCommandHeader *)&front_end_callback_command_003,
  (FrontEndCommandHeader *)&front_end_callback_command_043,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_090[7] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_117,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  (FrontEndCommandHeader *)&front_end_show_cels_command_118,
  (FrontEndCommandHeader *)&front_end_callback_command_003,
  (FrontEndCommandHeader *)&front_end_callback_command_039,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_091[7] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_119,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  (FrontEndCommandHeader *)&front_end_show_cels_command_120,
  (FrontEndCommandHeader *)&front_end_callback_command_003,
  (FrontEndCommandHeader *)&front_end_callback_command_040,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_092[7] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_121,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  (FrontEndCommandHeader *)&front_end_show_cels_command_122,
  (FrontEndCommandHeader *)&front_end_callback_command_003,
  (FrontEndCommandHeader *)&front_end_callback_command_041,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_093[7] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_123,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  (FrontEndCommandHeader *)&front_end_show_cels_command_124,
  (FrontEndCommandHeader *)&front_end_callback_command_003,
  (FrontEndCommandHeader *)&front_end_callback_command_042,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_094[6] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_125,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  (FrontEndCommandHeader *)&front_end_show_cels_command_126,
  (FrontEndCommandHeader *)&front_end_callback_command_003,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_095[4] =
{
  (FrontEndCommandHeader *)&front_end_play_stream_command_000,
  (FrontEndCommandHeader *)&front_end_show_cels_command_127,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_096[5] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_128,
  (FrontEndCommandHeader *)&front_end_callback_command_003,
  (FrontEndCommandHeader *)&front_end_callback_command_038,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_097[5] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_129,
  (FrontEndCommandHeader *)&front_end_callback_command_003,
  (FrontEndCommandHeader *)&front_end_callback_command_038,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_098[3] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_130,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_099[3] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_131,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_100[3] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_132,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_101[4] =
{
  (FrontEndCommandHeader *)&front_end_play_stream_command_001,
  (FrontEndCommandHeader *)&front_end_show_cels_command_133,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_102[5] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_134,
  (FrontEndCommandHeader *)&front_end_callback_command_003,
  (FrontEndCommandHeader *)&front_end_callback_command_035,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_103[5] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_135,
  (FrontEndCommandHeader *)&front_end_callback_command_003,
  (FrontEndCommandHeader *)&front_end_callback_command_035,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_104[3] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_136,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_105[3] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_137,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_106[3] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_138,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_107[4] =
{
  (FrontEndCommandHeader *)&front_end_play_stream_command_002,
  (FrontEndCommandHeader *)&front_end_show_cels_command_139,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_108[5] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_140,
  (FrontEndCommandHeader *)&front_end_callback_command_003,
  (FrontEndCommandHeader *)&front_end_callback_command_034,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_109[5] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_141,
  (FrontEndCommandHeader *)&front_end_callback_command_003,
  (FrontEndCommandHeader *)&front_end_callback_command_034,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_110[3] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_142,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_111[3] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_143,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_112[3] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_144,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_113[4] =
{
  (FrontEndCommandHeader *)&front_end_play_stream_command_003,
  (FrontEndCommandHeader *)&front_end_show_cels_command_145,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_114[5] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_146,
  (FrontEndCommandHeader *)&front_end_callback_command_003,
  (FrontEndCommandHeader *)&front_end_callback_command_037,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_115[5] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_147,
  (FrontEndCommandHeader *)&front_end_callback_command_003,
  (FrontEndCommandHeader *)&front_end_callback_command_037,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_116[3] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_148,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_117[3] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_149,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_118[3] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_150,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_119[4] =
{
  (FrontEndCommandHeader *)&front_end_play_stream_command_004,
  (FrontEndCommandHeader *)&front_end_show_cels_command_151,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_120[5] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_152,
  (FrontEndCommandHeader *)&front_end_callback_command_003,
  (FrontEndCommandHeader *)&front_end_callback_command_036,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_121[5] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_153,
  (FrontEndCommandHeader *)&front_end_callback_command_003,
  (FrontEndCommandHeader *)&front_end_callback_command_036,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_122[3] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_154,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_123[3] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_155,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_124[3] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_156,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_125[7] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_157,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  (FrontEndCommandHeader *)&front_end_show_cels_command_158,
  (FrontEndCommandHeader *)&front_end_callback_command_003,
  (FrontEndCommandHeader *)&front_end_callback_command_054,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_126[7] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_159,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  (FrontEndCommandHeader *)&front_end_show_cels_command_160,
  (FrontEndCommandHeader *)&front_end_callback_command_003,
  (FrontEndCommandHeader *)&front_end_callback_command_050,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_127[7] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_161,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  (FrontEndCommandHeader *)&front_end_show_cels_command_162,
  (FrontEndCommandHeader *)&front_end_callback_command_003,
  (FrontEndCommandHeader *)&front_end_callback_command_053,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_128[7] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_163,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  (FrontEndCommandHeader *)&front_end_show_cels_command_164,
  (FrontEndCommandHeader *)&front_end_callback_command_003,
  (FrontEndCommandHeader *)&front_end_callback_command_051,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_129[7] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_165,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  (FrontEndCommandHeader *)&front_end_show_cels_command_166,
  (FrontEndCommandHeader *)&front_end_callback_command_003,
  (FrontEndCommandHeader *)&front_end_callback_command_052,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_130[6] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_167,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  (FrontEndCommandHeader *)&front_end_show_cels_command_168,
  (FrontEndCommandHeader *)&front_end_callback_command_003,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_131[4] =
{
  (FrontEndCommandHeader *)&front_end_play_stream_command_005,
  (FrontEndCommandHeader *)&front_end_show_cels_command_169,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_132[5] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_170,
  (FrontEndCommandHeader *)&front_end_callback_command_003,
  (FrontEndCommandHeader *)&front_end_callback_command_049,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_133[5] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_171,
  (FrontEndCommandHeader *)&front_end_callback_command_003,
  (FrontEndCommandHeader *)&front_end_callback_command_049,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_134[3] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_172,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_135[3] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_173,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_136[3] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_174,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_137[4] =
{
  (FrontEndCommandHeader *)&front_end_play_stream_command_006,
  (FrontEndCommandHeader *)&front_end_show_cels_command_175,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_138[5] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_176,
  (FrontEndCommandHeader *)&front_end_callback_command_003,
  (FrontEndCommandHeader *)&front_end_callback_command_045,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_139[5] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_177,
  (FrontEndCommandHeader *)&front_end_callback_command_003,
  (FrontEndCommandHeader *)&front_end_callback_command_045,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_140[3] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_178,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_141[3] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_179,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_142[3] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_180,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_143[4] =
{
  (FrontEndCommandHeader *)&front_end_play_stream_command_007,
  (FrontEndCommandHeader *)&front_end_show_cels_command_181,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_144[5] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_182,
  (FrontEndCommandHeader *)&front_end_callback_command_003,
  (FrontEndCommandHeader *)&front_end_callback_command_048,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_145[5] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_183,
  (FrontEndCommandHeader *)&front_end_callback_command_003,
  (FrontEndCommandHeader *)&front_end_callback_command_048,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_146[3] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_184,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_147[3] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_185,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_148[3] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_186,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_149[4] =
{
  (FrontEndCommandHeader *)&front_end_play_stream_command_008,
  (FrontEndCommandHeader *)&front_end_show_cels_command_187,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_150[5] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_188,
  (FrontEndCommandHeader *)&front_end_callback_command_003,
  (FrontEndCommandHeader *)&front_end_callback_command_046,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_151[5] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_189,
  (FrontEndCommandHeader *)&front_end_callback_command_003,
  (FrontEndCommandHeader *)&front_end_callback_command_046,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_152[3] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_190,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_153[3] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_191,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_154[3] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_192,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_155[4] =
{
  (FrontEndCommandHeader *)&front_end_play_stream_command_009,
  (FrontEndCommandHeader *)&front_end_show_cels_command_193,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_156[5] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_194,
  (FrontEndCommandHeader *)&front_end_callback_command_003,
  (FrontEndCommandHeader *)&front_end_callback_command_047,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_157[5] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_195,
  (FrontEndCommandHeader *)&front_end_callback_command_003,
  (FrontEndCommandHeader *)&front_end_callback_command_047,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_158[3] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_196,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_159[3] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_197,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_160[3] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_198,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_161[7] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_199,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  (FrontEndCommandHeader *)&front_end_show_cels_command_200,
  (FrontEndCommandHeader *)&front_end_callback_command_003,
  (FrontEndCommandHeader *)&front_end_callback_command_064,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_162[7] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_201,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  (FrontEndCommandHeader *)&front_end_show_cels_command_202,
  (FrontEndCommandHeader *)&front_end_callback_command_003,
  (FrontEndCommandHeader *)&front_end_callback_command_062,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_163[7] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_203,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  (FrontEndCommandHeader *)&front_end_show_cels_command_204,
  (FrontEndCommandHeader *)&front_end_callback_command_003,
  (FrontEndCommandHeader *)&front_end_callback_command_061,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_164[7] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_205,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  (FrontEndCommandHeader *)&front_end_show_cels_command_206,
  (FrontEndCommandHeader *)&front_end_callback_command_003,
  (FrontEndCommandHeader *)&front_end_callback_command_060,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_165[7] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_207,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  (FrontEndCommandHeader *)&front_end_show_cels_command_208,
  (FrontEndCommandHeader *)&front_end_callback_command_003,
  (FrontEndCommandHeader *)&front_end_callback_command_063,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_166[6] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_209,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  (FrontEndCommandHeader *)&front_end_show_cels_command_210,
  (FrontEndCommandHeader *)&front_end_callback_command_003,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_167[4] =
{
  (FrontEndCommandHeader *)&front_end_play_stream_command_010,
  (FrontEndCommandHeader *)&front_end_show_cels_command_211,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_168[5] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_212,
  (FrontEndCommandHeader *)&front_end_callback_command_003,
  (FrontEndCommandHeader *)&front_end_callback_command_059,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_169[5] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_213,
  (FrontEndCommandHeader *)&front_end_callback_command_003,
  (FrontEndCommandHeader *)&front_end_callback_command_059,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_170[3] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_214,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_171[3] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_215,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_172[3] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_216,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_173[4] =
{
  (FrontEndCommandHeader *)&front_end_play_stream_command_011,
  (FrontEndCommandHeader *)&front_end_show_cels_command_217,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_174[5] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_218,
  (FrontEndCommandHeader *)&front_end_callback_command_003,
  (FrontEndCommandHeader *)&front_end_callback_command_057,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_175[5] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_219,
  (FrontEndCommandHeader *)&front_end_callback_command_003,
  (FrontEndCommandHeader *)&front_end_callback_command_057,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_176[3] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_220,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_177[3] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_221,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_178[3] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_222,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_179[4] =
{
  (FrontEndCommandHeader *)&front_end_play_stream_command_012,
  (FrontEndCommandHeader *)&front_end_show_cels_command_223,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_180[5] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_224,
  (FrontEndCommandHeader *)&front_end_callback_command_003,
  (FrontEndCommandHeader *)&front_end_callback_command_056,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_181[5] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_225,
  (FrontEndCommandHeader *)&front_end_callback_command_003,
  (FrontEndCommandHeader *)&front_end_callback_command_056,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_182[3] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_226,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_183[3] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_227,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_184[3] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_228,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_185[4] =
{
  (FrontEndCommandHeader *)&front_end_play_stream_command_013,
  (FrontEndCommandHeader *)&front_end_show_cels_command_229,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_186[5] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_230,
  (FrontEndCommandHeader *)&front_end_callback_command_003,
  (FrontEndCommandHeader *)&front_end_callback_command_055,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_187[5] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_231,
  (FrontEndCommandHeader *)&front_end_callback_command_003,
  (FrontEndCommandHeader *)&front_end_callback_command_055,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_188[3] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_232,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_189[3] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_233,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_190[3] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_234,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_191[4] =
{
  (FrontEndCommandHeader *)&front_end_play_stream_command_014,
  (FrontEndCommandHeader *)&front_end_show_cels_command_235,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_192[5] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_236,
  (FrontEndCommandHeader *)&front_end_callback_command_003,
  (FrontEndCommandHeader *)&front_end_callback_command_058,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_193[5] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_237,
  (FrontEndCommandHeader *)&front_end_callback_command_003,
  (FrontEndCommandHeader *)&front_end_callback_command_058,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_194[3] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_238,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_195[3] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_239,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_196[3] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_240,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_197[6] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_241,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  (FrontEndCommandHeader *)&front_end_show_cels_command_242,
  (FrontEndCommandHeader *)&front_end_callback_command_002,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_198[6] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_243,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  (FrontEndCommandHeader *)&front_end_show_cels_command_244,
  (FrontEndCommandHeader *)&front_end_callback_command_002,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_199[6] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_245,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  (FrontEndCommandHeader *)&front_end_show_cels_command_246,
  (FrontEndCommandHeader *)&front_end_callback_command_002,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_200[6] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_247,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  (FrontEndCommandHeader *)&front_end_show_cels_command_248,
  (FrontEndCommandHeader *)&front_end_callback_command_002,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_201[6] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_249,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  (FrontEndCommandHeader *)&front_end_show_cels_command_250,
  (FrontEndCommandHeader *)&front_end_callback_command_002,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_202[6] =
{
  (FrontEndCommandHeader *)&front_end_show_cels_command_251,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  (FrontEndCommandHeader *)&front_end_show_cels_command_252,
  (FrontEndCommandHeader *)&front_end_callback_command_002,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_203[4] =
{
  (FrontEndCommandHeader *)&front_end_callback_command_067,
  (FrontEndCommandHeader *)&front_end_callback_command_068,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_204[4] =
{
  (FrontEndCommandHeader *)&front_end_callback_command_069,
  (FrontEndCommandHeader *)&front_end_callback_command_070,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndCommandHeader *front_end_command_sequence_205[3] =
{
  (FrontEndCommandHeader *)&front_end_callback_command_071,
  (FrontEndCommandHeader *)&front_end_present_command_000,
  0
};

FrontEndAction front_end_actions[343] =
{
  { 1, 0, 0, -1, -1, -1, -1,
    front_end_consume_pending_event, 0, 0, 4, 497 },
  { 8, front_end_command_sequence_002, 0, -1, -1, -1, 7,
    0, 0, 0, 242, -1 },
  { 7, front_end_command_sequence_001, 0, -1, -1, 8, 9,
    0, 0, 0, 226, -1 },
  { 9, front_end_command_sequence_003, 0, -1, -1, 7, -1,
    0, 0, 0, 10, -1 },
  { 11, front_end_command_sequence_025, 0, 18, 12, -1, -1,
    front_end_is_alternate_game_mode, 0, 0, 10, 222 },
  { 12, front_end_command_sequence_029, 0, 11, 13, -1, -1,
    0, 0, 0, 19, -1 },
  { 13, front_end_command_sequence_030, 0, 12, 14, -1, -1,
    0, 0, 0, 22, -1 },
  { 14, front_end_command_sequence_031, 0, 13, 15, -1, -1,
    front_end_is_alternate_game_mode, 0, 0, 25, 10 },
  { 15, front_end_command_sequence_032, 0, 14, 16, -1, -1,
    0, 0, 0, 32, -1 },
  { 16, front_end_command_sequence_033, 0, 15, 17, -1, -1,
    0, 0, 0, 35, -1 },
  { 17, front_end_command_sequence_034, 0, 16, 18, -1, -1,
    0, 0, 0, 107, -1 },
  { 18, front_end_command_sequence_035, 0, 17, 11, -1, -1,
    front_end_choose_random_rider_preview, 0, 0, 6, 6 },
  { 20, front_end_command_sequence_058, 0, -1, 21, -1, -1,
    0, 0, 0, 10, -1 },
  { 21, front_end_command_sequence_057, 0, 20, -1, -1, -1,
    0, 0, 0, 10, -1 },
  { 23, front_end_command_sequence_059, 0, -1, 24, -1, -1,
    0, 0, 0, 10, -1 },
  { 24, front_end_command_sequence_060, 0, 23, -1, -1, -1,
    0, 0, 0, 10, -1 },
  { 26, front_end_command_sequence_061, 0, -1, 27, -1, -1,
    0, 0, 0, 10, -1 },
  { 27, front_end_command_sequence_062, 0, 26, 28, -1, -1,
    0, 0, 0, 10, -1 },
  { 28, front_end_command_sequence_063, 0, 27, 29, -1, -1,
    0, 0, 0, 10, -1 },
  { 29, front_end_command_sequence_064, 0, 28, 30, -1, -1,
    0, 0, 0, 10, -1 },
  { 30, front_end_command_sequence_065, 0, 29, -1, -1, -1,
    0, 0, 0, 10, -1 },
  { 33, front_end_command_sequence_066, 0, -1, 34, -1, -1,
    0, 0, 0, 10, -1 },
  { 34, front_end_command_sequence_067, 0, 33, -1, -1, -1,
    0, 0, 0, 10, -1 },
  { 36, 0, 0, -1, -1, -1, -1,
    front_end_is_special_mode_enabled, 0, 0, 37, 40 },
  { 41, front_end_command_sequence_068, 0, -1, -1, -1, -1,
    0, 0, 0, 10, -1 },
  { 38, front_end_command_sequence_069, 0, -1, 39, -1, -1,
    0, 0, 0, 10, -1 },
  { 39, front_end_command_sequence_070, 0, 38, -1, -1, -1,
    0, 0, 0, 10, -1 },
  { 43, front_end_command_sequence_025, 0, 51, 44, -1, -1,
    front_end_is_alternate_game_mode, 0, 0, 42, 186 },
  { 44, front_end_command_sequence_028, 0, 43, 45, -1, -1,
    front_end_is_alternate_game_mode, 0, 0, 42, 188 },
  { 45, front_end_command_sequence_029, 0, 44, 46, -1, -1,
    0, 0, 0, 52, -1 },
  { 46, front_end_command_sequence_030, 0, 45, 47, -1, -1,
    0, 0, 0, 55, -1 },
  { 47, front_end_command_sequence_031, 0, 46, 48, -1, -1,
    front_end_is_alternate_game_mode, 0, 0, 58, 42 },
  { 48, front_end_command_sequence_032, 0, 47, 49, -1, -1,
    0, 0, 0, 65, -1 },
  { 49, front_end_command_sequence_033, 0, 48, 50, -1, -1,
    0, 0, 0, 68, -1 },
  { 50, front_end_command_sequence_034, 0, 49, 51, -1, -1,
    0, 0, 0, 129, -1 },
  { 51, front_end_command_sequence_035, 0, 50, 43, -1, -1,
    front_end_choose_random_rider_preview, 0, 0, 482, 482 },
  { 53, front_end_command_sequence_058, 0, -1, 54, -1, -1,
    0, 0, 0, 42, -1 },
  { 54, front_end_command_sequence_057, 0, 53, -1, -1, -1,
    0, 0, 0, 42, -1 },
  { 56, front_end_command_sequence_059, 0, -1, 57, -1, -1,
    0, 0, 0, 42, -1 },
  { 57, front_end_command_sequence_060, 0, 56, -1, -1, -1,
    0, 0, 0, 42, -1 },
  { 59, front_end_command_sequence_061, 0, -1, 60, -1, -1,
    0, 0, 0, 42, -1 },
  { 60, front_end_command_sequence_062, 0, 59, 61, -1, -1,
    0, 0, 0, 42, -1 },
  { 61, front_end_command_sequence_063, 0, 60, 62, -1, -1,
    0, 0, 0, 42, -1 },
  { 62, front_end_command_sequence_064, 0, 61, 63, -1, -1,
    0, 0, 0, 42, -1 },
  { 63, front_end_command_sequence_065, 0, 62, -1, -1, -1,
    0, 0, 0, 42, -1 },
  { 66, front_end_command_sequence_066, 0, -1, 67, -1, -1,
    0, 0, 0, 42, -1 },
  { 67, front_end_command_sequence_067, 0, 66, -1, -1, -1,
    0, 0, 0, 42, -1 },
  { 69, 0, 0, -1, -1, -1, -1,
    front_end_is_special_mode_enabled, 0, 0, 70, 73 },
  { 74, front_end_command_sequence_068, 0, -1, -1, -1, -1,
    0, 0, 0, 42, -1 },
  { 71, front_end_command_sequence_069, 0, -1, 72, -1, -1,
    0, 0, 0, 42, -1 },
  { 72, front_end_command_sequence_070, 0, 71, -1, -1, -1,
    0, 0, 0, 42, -1 },
  { 76, front_end_command_sequence_025, 0, 83, 77, -1, -1,
    front_end_is_single_player, 0, 0, 192, 202 },
  { 77, front_end_command_sequence_029, 0, 76, 78, -1, -1,
    0, 0, 0, 84, -1 },
  { 78, front_end_command_sequence_030, 0, 77, 79, -1, -1,
    0, 0, 0, 87, -1 },
  { 79, front_end_command_sequence_031, 0, 78, 80, -1, -1,
    front_end_is_alternate_game_mode, 0, 0, 90, 75 },
  { 80, front_end_command_sequence_032, 0, 79, 81, -1, -1,
    0, 0, 0, 97, -1 },
  { 81, front_end_command_sequence_033, 0, 80, 82, -1, -1,
    0, 0, 0, 100, -1 },
  { 82, front_end_command_sequence_034, 0, 81, 83, -1, -1,
    0, 0, 0, 151, -1 },
  { 83, front_end_command_sequence_035, 0, 82, 76, -1, -1,
    front_end_choose_random_rider_preview, 0, 0, 242, 242 },
  { 85, front_end_command_sequence_058, 0, -1, 86, -1, -1,
    0, 0, 0, 75, -1 },
  { 86, front_end_command_sequence_057, 0, 85, -1, -1, -1,
    0, 0, 0, 75, -1 },
  { 88, front_end_command_sequence_059, 0, -1, 89, -1, -1,
    0, 0, 0, 75, -1 },
  { 89, front_end_command_sequence_060, 0, 88, -1, -1, -1,
    0, 0, 0, 75, -1 },
  { 91, front_end_command_sequence_061, 0, -1, 92, -1, -1,
    0, 0, 0, 75, -1 },
  { 92, front_end_command_sequence_062, 0, 91, 93, -1, -1,
    0, 0, 0, 75, -1 },
  { 93, front_end_command_sequence_063, 0, 92, 94, -1, -1,
    0, 0, 0, 75, -1 },
  { 94, front_end_command_sequence_064, 0, 93, 95, -1, -1,
    0, 0, 0, 75, -1 },
  { 95, front_end_command_sequence_065, 0, 94, -1, -1, -1,
    0, 0, 0, 75, -1 },
  { 98, front_end_command_sequence_066, 0, -1, 99, -1, -1,
    0, 0, 0, 75, -1 },
  { 99, front_end_command_sequence_067, 0, 98, -1, -1, -1,
    0, 0, 0, 75, -1 },
  { 101, 0, 0, -1, -1, -1, -1,
    front_end_is_special_mode_enabled, 0, 0, 102, 105 },
  { 106, front_end_command_sequence_068, 0, -1, -1, -1, -1,
    0, 0, 0, 75, -1 },
  { 103, front_end_command_sequence_069, 0, -1, 104, -1, -1,
    0, 0, 0, 75, -1 },
  { 104, front_end_command_sequence_070, 0, 103, -1, -1, -1,
    0, 0, 0, 75, -1 },
  { 108, front_end_command_sequence_036, 0, -1, 111, -1, 109,
    0, 0, 0, -1, -1 },
  { 109, front_end_command_sequence_037, 0, -1, 110, 108, -1,
    front_end_set_selected_finish_position, 0, 0, 10, -1 },
  { 110, front_end_command_sequence_038, 0, 109, -1, 108, -1,
    front_end_set_selected_finish_position, 1, 0, 10, -1 },
  { 111, front_end_command_sequence_039, 0, 108, 113, -1, 112,
    0, 0, 0, -1, -1 },
  { 112, front_end_command_sequence_040, 0, -1, -1, 111, -1,
    front_end_set_selected_finish_position, 2, 0, 10, -1 },
  { 113, front_end_command_sequence_041, 0, 111, 117, -1, 114,
    0, 0, 0, -1, -1 },
  { 114, front_end_command_sequence_042, 0, -1, 115, 113, -1,
    front_end_set_selected_finish_position, 3, 0, 10, -1 },
  { 115, front_end_command_sequence_043, 0, 114, 116, 113, -1,
    front_end_set_selected_finish_position, 4, 0, 10, -1 },
  { 116, front_end_command_sequence_044, 0, 115, -1, 113, -1,
    front_end_set_selected_finish_position, 5, 0, 10, -1 },
  { 117, front_end_command_sequence_045, 0, 113, 122, -1, 118,
    0, 0, 0, -1, -1 },
  { 118, front_end_command_sequence_046, 0, -1, 119, 117, -1,
    front_end_set_selected_finish_position, 6, 0, 10, -1 },
  { 119, front_end_command_sequence_047, 0, 118, 120, 117, -1,
    front_end_set_selected_finish_position, 7, 0, 10, -1 },
  { 120, front_end_command_sequence_048, 0, 119, 121, 117, -1,
    front_end_set_selected_finish_position, 8, 0, 10, -1 },
  { 121, front_end_command_sequence_049, 0, 120, -1, 117, -1,
    front_end_set_selected_finish_position, 9, 0, 10, -1 },
  { 122, front_end_command_sequence_050, 0, 117, 125, -1, 123,
    0, 0, 0, -1, -1 },
  { 123, front_end_command_sequence_051, 0, -1, 124, 122, -1,
    front_end_set_selected_finish_position, 10, 0, 10, -1 },
  { 124, front_end_command_sequence_052, 0, 123, -1, 122, -1,
    front_end_set_selected_finish_position, 11, 0, 10, -1 },
  { 125, front_end_command_sequence_053, 0, 122, 128, -1, 126,
    0, 0, 0, -1, -1 },
  { 126, front_end_command_sequence_054, 0, -1, 127, 125, -1,
    front_end_set_selected_finish_position, 12, 0, 10, -1 },
  { 127, front_end_command_sequence_055, 0, 126, -1, 125, -1,
    front_end_set_selected_finish_position, 13, 0, 10, -1 },
  { 128, front_end_command_sequence_056, 0, 125, -1, -1, -1,
    0, 0, 0, 10, -1 },
  { 130, front_end_command_sequence_036, 0, -1, 133, -1, 131,
    0, 0, 0, -1, -1 },
  { 131, front_end_command_sequence_037, 0, -1, 132, 130, -1,
    front_end_set_selected_finish_position, 0, 0, 42, -1 },
  { 132, front_end_command_sequence_038, 0, 131, -1, 130, -1,
    front_end_set_selected_finish_position, 1, 0, 42, -1 },
  { 133, front_end_command_sequence_039, 0, 130, 135, -1, 134,
    0, 0, 0, -1, -1 },
  { 134, front_end_command_sequence_040, 0, -1, -1, 133, -1,
    front_end_set_selected_finish_position, 2, 0, 42, -1 },
  { 135, front_end_command_sequence_041, 0, 133, 139, -1, 136,
    0, 0, 0, -1, -1 },
  { 136, front_end_command_sequence_042, 0, -1, 137, 135, -1,
    front_end_set_selected_finish_position, 3, 0, 42, -1 },
  { 137, front_end_command_sequence_043, 0, 136, 138, 135, -1,
    front_end_set_selected_finish_position, 4, 0, 42, -1 },
  { 138, front_end_command_sequence_044, 0, 137, -1, 135, -1,
    front_end_set_selected_finish_position, 5, 0, 42, -1 },
  { 139, front_end_command_sequence_045, 0, 135, 144, -1, 140,
    0, 0, 0, -1, -1 },
  { 140, front_end_command_sequence_046, 0, -1, 141, 139, -1,
    front_end_set_selected_finish_position, 6, 0, 42, -1 },
  { 141, front_end_command_sequence_047, 0, 140, 142, 139, -1,
    front_end_set_selected_finish_position, 7, 0, 42, -1 },
  { 142, front_end_command_sequence_048, 0, 141, 143, 139, -1,
    front_end_set_selected_finish_position, 8, 0, 42, -1 },
  { 143, front_end_command_sequence_049, 0, 142, -1, 139, -1,
    front_end_set_selected_finish_position, 9, 0, 42, -1 },
  { 144, front_end_command_sequence_050, 0, 139, 147, -1, 145,
    0, 0, 0, -1, -1 },
  { 145, front_end_command_sequence_051, 0, -1, 146, 144, -1,
    front_end_set_selected_finish_position, 10, 0, 42, -1 },
  { 146, front_end_command_sequence_052, 0, 145, -1, 144, -1,
    front_end_set_selected_finish_position, 11, 0, 42, -1 },
  { 147, front_end_command_sequence_053, 0, 144, 150, -1, 148,
    0, 0, 0, -1, -1 },
  { 148, front_end_command_sequence_054, 0, -1, 149, 147, -1,
    front_end_set_selected_finish_position, 12, 0, 42, -1 },
  { 149, front_end_command_sequence_055, 0, 148, -1, 147, -1,
    front_end_set_selected_finish_position, 13, 0, 42, -1 },
  { 150, front_end_command_sequence_056, 0, 149, -1, -1, -1,
    0, 0, 0, 42, -1 },
  { 152, front_end_command_sequence_036, 0, -1, 155, -1, 153,
    0, 0, 0, -1, -1 },
  { 153, front_end_command_sequence_037, 0, -1, 154, 152, -1,
    front_end_set_selected_finish_position, 0, 0, 75, -1 },
  { 154, front_end_command_sequence_038, 0, 153, -1, 152, -1,
    front_end_set_selected_finish_position, 1, 0, 75, -1 },
  { 155, front_end_command_sequence_039, 0, 152, 157, -1, 156,
    0, 0, 0, -1, -1 },
  { 156, front_end_command_sequence_040, 0, -1, -1, 155, -1,
    front_end_set_selected_finish_position, 2, 0, 75, -1 },
  { 157, front_end_command_sequence_041, 0, 155, 161, -1, 158,
    0, 0, 0, -1, -1 },
  { 158, front_end_command_sequence_042, 0, -1, 159, 157, -1,
    front_end_set_selected_finish_position, 3, 0, 75, -1 },
  { 159, front_end_command_sequence_043, 0, 158, 160, 157, -1,
    front_end_set_selected_finish_position, 4, 0, 75, -1 },
  { 160, front_end_command_sequence_044, 0, 159, -1, 157, -1,
    front_end_set_selected_finish_position, 5, 0, 75, -1 },
  { 161, front_end_command_sequence_045, 0, 157, 166, -1, 162,
    0, 0, 0, -1, -1 },
  { 162, front_end_command_sequence_046, 0, -1, 163, 161, -1,
    front_end_set_selected_finish_position, 6, 0, 75, -1 },
  { 163, front_end_command_sequence_047, 0, 162, 164, 161, -1,
    front_end_set_selected_finish_position, 7, 0, 75, -1 },
  { 164, front_end_command_sequence_048, 0, 163, 165, 161, -1,
    front_end_set_selected_finish_position, 8, 0, 75, -1 },
  { 165, front_end_command_sequence_049, 0, 164, -1, 161, -1,
    front_end_set_selected_finish_position, 9, 0, 75, -1 },
  { 166, front_end_command_sequence_050, 0, 161, 169, -1, 167,
    0, 0, 0, -1, -1 },
  { 167, front_end_command_sequence_051, 0, -1, 168, 166, -1,
    front_end_set_selected_finish_position, 10, 0, 75, -1 },
  { 168, front_end_command_sequence_052, 0, 167, -1, 166, -1,
    front_end_set_selected_finish_position, 11, 0, 75, -1 },
  { 169, front_end_command_sequence_053, 0, 166, 172, -1, 170,
    0, 0, 0, -1, -1 },
  { 170, front_end_command_sequence_054, 0, -1, 171, 169, -1,
    front_end_set_selected_finish_position, 12, 0, 75, -1 },
  { 171, front_end_command_sequence_055, 0, 170, -1, 169, -1,
    front_end_set_selected_finish_position, 13, 0, 75, -1 },
  { 172, front_end_command_sequence_056, 0, 171, -1, -1, -1,
    0, 0, 0, 75, -1 },
  { 191, 0, 0, -1, -1, -1, -1,
    front_end_is_profile_slot_available, 0, 0, 42, 42 },
  { 184, front_end_command_sequence_026, 0, -1, 185, -1, -1,
    front_end_set_player_descriptor_selection, 0, 0, 173, -1 },
  { 185, front_end_command_sequence_027, 0, 184, -1, -1, -1,
    front_end_set_player_descriptor_selection, 1, 0, 173, -1 },
  { 187, 0, 0, -1, -1, -1, -1,
    front_end_is_single_player, 0, 0, 173, 183 },
  { 189, 0, 0, -1, -1, -1, -1,
    front_end_is_single_player, 0, 0, 173, 183 },
  { 174, front_end_command_sequence_072, front_end_command_sequence_071, -1, 175, -1, -1,
    front_end_is_profile_slot_available, 0, 0, 42, 190 },
  { 175, front_end_command_sequence_073, front_end_command_sequence_071, 174, 176, -1, -1,
    front_end_is_profile_slot_available, 0, 0, 42, 190 },
  { 176, front_end_command_sequence_074, front_end_command_sequence_071, 175, 177, -1, -1,
    front_end_is_profile_slot_available, 0, 0, 42, 190 },
  { 177, front_end_command_sequence_075, front_end_command_sequence_071, 176, 178, -1, -1,
    front_end_is_profile_slot_available, 0, 0, 42, 190 },
  { 178, front_end_command_sequence_076, front_end_command_sequence_071, 177, 179, -1, -1,
    front_end_is_profile_slot_available, 0, 0, 42, 190 },
  { 179, front_end_command_sequence_077, front_end_command_sequence_071, 178, 180, -1, -1,
    front_end_is_profile_slot_available, 0, 0, 42, 190 },
  { 180, front_end_command_sequence_078, front_end_command_sequence_071, 179, 181, -1, -1,
    front_end_is_profile_slot_available, 0, 0, 42, 190 },
  { 181, front_end_command_sequence_079, front_end_command_sequence_071, 180, -1, 43, -1,
    0, 0, 0, 42, -1 },
  { 182, front_end_command_sequence_080, front_end_command_sequence_071, -1, -1, -1, 174,
    0, 0, 0, 42, -1 },
  { 208, 0, 0, -1, -1, -1, -1,
    front_end_is_profile_slot_available, 0, 0, 75, 75 },
  { 203, front_end_command_sequence_026, 0, -1, 204, -1, -1,
    front_end_set_player_descriptor_selection, 0, 0, 192, -1 },
  { 204, front_end_command_sequence_027, 0, 203, -1, -1, -1,
    front_end_set_player_descriptor_selection, 1, 0, 192, -1 },
  { 206, 0, 0, -1, -1, -1, -1,
    front_end_is_single_player, 0, 0, 192, 202 },
  { 193, front_end_command_sequence_072, front_end_command_sequence_071, -1, 194, -1, -1,
    front_end_is_profile_slot_available, 0, 0, 75, 207 },
  { 194, front_end_command_sequence_073, front_end_command_sequence_071, 193, 195, -1, -1,
    front_end_is_profile_slot_available, 0, 0, 75, 207 },
  { 195, front_end_command_sequence_074, front_end_command_sequence_071, 194, 196, -1, -1,
    front_end_is_profile_slot_available, 0, 0, 75, 207 },
  { 196, front_end_command_sequence_075, front_end_command_sequence_071, 195, 197, -1, -1,
    front_end_is_profile_slot_available, 0, 0, 75, 207 },
  { 197, front_end_command_sequence_076, front_end_command_sequence_071, 196, 198, -1, -1,
    front_end_is_profile_slot_available, 0, 0, 75, 207 },
  { 198, front_end_command_sequence_077, front_end_command_sequence_071, 197, 199, -1, -1,
    front_end_is_profile_slot_available, 0, 0, 75, 207 },
  { 199, front_end_command_sequence_078, front_end_command_sequence_071, 198, 200, -1, -1,
    front_end_is_profile_slot_available, 0, 0, 75, 207 },
  { 200, front_end_command_sequence_079, front_end_command_sequence_071, 199, -1, -1, -1,
    0, 0, 0, 75, -1 },
  { 201, front_end_command_sequence_080, front_end_command_sequence_071, -1, -1, -1, 193,
    0, 0, 0, 75, -1 },
  { 225, 0, 0, -1, -1, -1, -1,
    front_end_is_profile_slot_available, 0, 0, 10, 10 },
  { 220, front_end_command_sequence_026, 0, -1, 221, -1, -1,
    front_end_set_player_descriptor_selection, 0, 0, 209, -1 },
  { 221, front_end_command_sequence_027, 0, 220, -1, -1, -1,
    front_end_set_player_descriptor_selection, 1, 0, 209, -1 },
  { 223, 0, 0, -1, -1, -1, -1,
    front_end_is_single_player, 0, 0, 209, 219 },
  { 210, front_end_command_sequence_072, front_end_command_sequence_071, -1, 211, -1, -1,
    front_end_is_profile_slot_available, 0, 0, 10, 224 },
  { 211, front_end_command_sequence_073, front_end_command_sequence_071, 210, 212, -1, -1,
    front_end_is_profile_slot_available, 0, 0, 10, 224 },
  { 212, front_end_command_sequence_074, front_end_command_sequence_071, 211, 213, -1, -1,
    front_end_is_profile_slot_available, 0, 0, 10, 224 },
  { 213, front_end_command_sequence_075, front_end_command_sequence_071, 212, 214, -1, -1,
    front_end_is_profile_slot_available, 0, 0, 10, 224 },
  { 214, front_end_command_sequence_076, front_end_command_sequence_071, 213, 215, -1, -1,
    front_end_is_profile_slot_available, 0, 0, 10, 224 },
  { 215, front_end_command_sequence_077, front_end_command_sequence_071, 214, 216, -1, -1,
    front_end_is_profile_slot_available, 0, 0, 10, 224 },
  { 216, front_end_command_sequence_078, front_end_command_sequence_071, 215, 217, -1, -1,
    front_end_is_profile_slot_available, 0, 0, 10, 224 },
  { 217, front_end_command_sequence_079, front_end_command_sequence_071, 216, -1, -1, -1,
    0, 0, 0, 10, -1 },
  { 218, front_end_command_sequence_080, front_end_command_sequence_071, -1, -1, -1, 210,
    0, 0, 0, 10, -1 },
  { 234, front_end_command_sequence_017, 0, -1, 233, -1, 231,
    front_end_is_player_configuration_valid, 0, 0, 478, 226 },
  { 233, front_end_command_sequence_018, 0, 234, 228, -1, 230,
    front_end_is_player_configuration_valid, 0, 0, 478, 226 },
  { 228, front_end_command_sequence_019, 0, 233, -1, -1, 232,
    front_end_is_player_configuration_valid, 0, 0, 478, 226 },
  { 231, front_end_command_sequence_020, 0, -1, 230, 234, 229,
    front_end_is_player_configuration_valid, 0, 0, 478, 226 },
  { 230, front_end_command_sequence_021, 0, 231, 232, 233, 227,
    front_end_is_player_configuration_valid, 0, 0, 478, 226 },
  { 232, front_end_command_sequence_022, 0, 230, -1, 228, -1,
    front_end_is_player_configuration_valid, 0, 0, 478, 226 },
  { 229, front_end_command_sequence_023, 0, -1, 227, 231, -1,
    front_end_is_player_configuration_valid, 0, 0, 478, 226 },
  { 227, front_end_command_sequence_024, 0, 229, -1, 230, -1,
    front_end_is_player_configuration_valid, 0, 0, 478, 226 },
  { 236, front_end_command_sequence_005, 0, -1, 237, -1, 238,
    0, 0, 0, 499, -1 },
  { 237, front_end_command_sequence_006, 0, 236, -1, -1, 239,
    0, 0, 0, 499, -1 },
  { 238, front_end_command_sequence_007, 0, -1, 239, 236, 240,
    0, 0, 0, 499, -1 },
  { 239, front_end_command_sequence_008, 0, 238, -1, 237, 241,
    0, 0, 0, 499, -1 },
  { 240, front_end_command_sequence_009, 0, -1, 241, 238, -1,
    0, 0, 0, 499, -1 },
  { 241, front_end_command_sequence_010, 0, 240, -1, 239, -1,
    0, 0, 0, 482, -1 },
  { 243, front_end_command_sequence_011, 0, -1, 244, -1, 245,
    0, 0, 0, 499, -1 },
  { 244, front_end_command_sequence_012, 0, 243, -1, -1, 246,
    0, 0, 0, 499, -1 },
  { 245, front_end_command_sequence_013, 0, -1, 246, 243, 247,
    0, 0, 0, 499, -1 },
  { 246, front_end_command_sequence_014, 0, 245, -1, 244, 248,
    0, 0, 0, 499, -1 },
  { 247, front_end_command_sequence_015, 0, -1, 248, 245, -1,
    0, 0, 0, 499, -1 },
  { 248, front_end_command_sequence_016, 0, 247, -1, 246, -1,
    0, 0, 0, 75, -1 },
  { 250, front_end_command_sequence_081, 0, -1, 251, -1, 252,
    0, 0, 0, 254, -1 },
  { 251, front_end_command_sequence_082, 0, 250, -1, -1, 253,
    0, 0, 0, 326, -1 },
  { 252, front_end_command_sequence_083, 0, -1, 253, 250, -1,
    0, 0, 0, 398, -1 },
  { 253, front_end_command_sequence_084, 0, 252, -1, 251, -1,
    0, 0, 0, 478, -1 },
  { 471, 0, 0, -1, -1, -1, -1,
    0, 0, 0, 472, -1 },
  { 473, 0, 0, -1, -1, -1, -1,
    0, 0, 0, 474, -1 },
  { 475, 0, 0, -1, -1, -1, -1,
    0, 0, 0, 476, -1 },
  { 477, 0, 0, -1, -1, -1, -1,
    0, 0, 0, 482, -1 },
  { 255, front_end_command_sequence_089, 0, 259, -1, 257, 260,
    0, 0, 0, 261, -1 },
  { 256, front_end_command_sequence_090, 0, -1, 257, -1, 259,
    0, 0, 0, 274, -1 },
  { 257, front_end_command_sequence_091, 0, 256, -1, -1, 255,
    0, 0, 0, 287, -1 },
  { 258, front_end_command_sequence_093, 0, -1, 260, 259, -1,
    0, 0, 0, 300, -1 },
  { 259, front_end_command_sequence_092, 0, -1, 255, 256, 258,
    0, 0, 0, 313, -1 },
  { 260, front_end_command_sequence_094, 0, 258, -1, 255, -1,
    0, 0, 0, 249, -1 },
  { 265, 0, 0, -1, -1, -1, -1,
    front_end_can_afford_rider_style, 4, 0, 266, 268 },
  { 267, 0, 0, -1, -1, -1, -1,
    front_end_is_selected_rider_style, 4, 0, 270, 272 },
  { 262, front_end_command_sequence_096, 0, -1, -1, -1, 263,
    0, 0, 0, 264, -1 },
  { 263, front_end_command_sequence_097, 0, -1, -1, 262, -1,
    0, 0, 0, 254, -1 },
  { 269, front_end_command_sequence_098, 0, -1, -1, -1, -1,
    0, 0, 0, 249, -1 },
  { 271, 0, 0, -1, -1, -1, -1,
    0, 0, 0, 254, -1 },
  { 273, 0, 0, -1, -1, -1, -1,
    0, 0, 0, 254, -1 },
  { 291, 0, 0, -1, -1, -1, -1,
    front_end_can_afford_rider_style, 1, 0, 292, 294 },
  { 293, 0, 0, -1, -1, -1, -1,
    front_end_is_selected_rider_style, 1, 0, 296, 298 },
  { 288, front_end_command_sequence_102, 0, -1, -1, -1, 289,
    0, 0, 0, 290, -1 },
  { 289, front_end_command_sequence_103, 0, -1, -1, 288, -1,
    0, 0, 0, 254, -1 },
  { 295, front_end_command_sequence_104, 0, -1, -1, -1, -1,
    0, 0, 0, 249, -1 },
  { 297, 0, 0, -1, -1, -1, -1,
    0, 0, 0, 254, -1 },
  { 299, 0, 0, -1, -1, -1, -1,
    0, 0, 0, 254, -1 },
  { 278, 0, 0, -1, -1, -1, -1,
    front_end_can_afford_rider_style, 0, 0, 279, 281 },
  { 280, 0, 0, -1, -1, -1, -1,
    front_end_is_selected_rider_style, 0, 0, 283, 285 },
  { 275, front_end_command_sequence_108, 0, -1, -1, -1, 276,
    0, 0, 0, 277, -1 },
  { 276, front_end_command_sequence_109, 0, -1, -1, 275, -1,
    0, 0, 0, 254, -1 },
  { 282, front_end_command_sequence_110, 0, -1, -1, -1, -1,
    0, 0, 0, 249, -1 },
  { 284, 0, 0, -1, -1, -1, -1,
    0, 0, 0, 254, -1 },
  { 286, 0, 0, -1, -1, -1, -1,
    0, 0, 0, 254, -1 },
  { 304, 0, 0, -1, -1, -1, -1,
    front_end_can_afford_rider_style, 3, 0, 305, 307 },
  { 306, 0, 0, -1, -1, -1, -1,
    front_end_is_selected_rider_style, 3, 0, 309, 311 },
  { 301, front_end_command_sequence_114, 0, -1, -1, -1, 302,
    0, 0, 0, 303, -1 },
  { 302, front_end_command_sequence_115, 0, -1, -1, 301, -1,
    0, 0, 0, 254, -1 },
  { 308, front_end_command_sequence_116, 0, -1, -1, -1, -1,
    0, 0, 0, 249, -1 },
  { 310, 0, 0, -1, -1, -1, -1,
    0, 0, 0, 254, -1 },
  { 312, 0, 0, -1, -1, -1, -1,
    0, 0, 0, 254, -1 },
  { 317, 0, 0, -1, -1, -1, -1,
    front_end_can_afford_rider_style, 2, 0, 318, 320 },
  { 319, 0, 0, -1, -1, -1, -1,
    front_end_is_selected_rider_style, 2, 0, 322, 324 },
  { 314, front_end_command_sequence_120, 0, -1, -1, -1, 315,
    0, 0, 0, 316, -1 },
  { 315, front_end_command_sequence_121, 0, -1, -1, 314, -1,
    0, 0, 0, 254, -1 },
  { 321, front_end_command_sequence_122, 0, -1, -1, -1, -1,
    0, 0, 0, 249, -1 },
  { 323, 0, 0, -1, -1, -1, -1,
    0, 0, 0, 254, -1 },
  { 325, 0, 0, -1, -1, -1, -1,
    0, 0, 0, 254, -1 },
  { 327, front_end_command_sequence_125, 0, -1, 332, 331, -1,
    0, 0, 0, 333, -1 },
  { 328, front_end_command_sequence_126, 0, -1, 330, -1, 331,
    0, 0, 0, 346, -1 },
  { 329, front_end_command_sequence_127, 0, 331, -1, 330, 332,
    0, 0, 0, 359, -1 },
  { 330, front_end_command_sequence_128, 0, 328, -1, -1, 329,
    0, 0, 0, 372, -1 },
  { 331, front_end_command_sequence_129, 0, -1, 329, 328, 327,
    0, 0, 0, 385, -1 },
  { 332, front_end_command_sequence_130, 0, 327, -1, 329, -1,
    0, 0, 0, 249, -1 },
  { 337, 0, 0, -1, -1, -1, -1,
    front_end_can_afford_rider_style, 14, 0, 338, 340 },
  { 339, 0, 0, -1, -1, -1, -1,
    front_end_is_selected_rider_style, 14, 0, 342, 344 },
  { 334, front_end_command_sequence_132, 0, -1, -1, -1, 335,
    0, 0, 0, 336, -1 },
  { 335, front_end_command_sequence_133, 0, -1, -1, 334, -1,
    0, 0, 0, 326, -1 },
  { 341, front_end_command_sequence_134, 0, -1, -1, -1, -1,
    0, 0, 0, 249, -1 },
  { 343, 0, 0, -1, -1, -1, -1,
    0, 0, 0, 326, -1 },
  { 345, 0, 0, -1, -1, -1, -1,
    0, 0, 0, 326, -1 },
  { 350, 0, 0, -1, -1, -1, -1,
    front_end_can_afford_rider_style, 10, 0, 351, 353 },
  { 352, 0, 0, -1, -1, -1, -1,
    front_end_is_selected_rider_style, 10, 0, 355, 357 },
  { 347, front_end_command_sequence_138, 0, -1, -1, -1, 348,
    0, 0, 0, 349, -1 },
  { 348, front_end_command_sequence_139, 0, -1, -1, 347, -1,
    0, 0, 0, 326, -1 },
  { 354, front_end_command_sequence_140, 0, -1, -1, -1, -1,
    0, 0, 0, 249, -1 },
  { 356, 0, 0, -1, -1, -1, -1,
    0, 0, 0, 326, -1 },
  { 358, 0, 0, -1, -1, -1, -1,
    0, 0, 0, 326, -1 },
  { 363, 0, 0, -1, -1, -1, -1,
    front_end_can_afford_rider_style, 13, 0, 364, 366 },
  { 365, 0, 0, -1, -1, -1, -1,
    front_end_is_selected_rider_style, 13, 0, 368, 370 },
  { 360, front_end_command_sequence_144, 0, -1, -1, -1, 361,
    0, 0, 0, 362, -1 },
  { 361, front_end_command_sequence_145, 0, -1, -1, 360, -1,
    0, 0, 0, 326, -1 },
  { 367, front_end_command_sequence_146, 0, -1, -1, -1, -1,
    0, 0, 0, 249, -1 },
  { 369, 0, 0, -1, -1, -1, -1,
    0, 0, 0, 326, -1 },
  { 371, 0, 0, -1, -1, -1, -1,
    0, 0, 0, 326, -1 },
  { 376, 0, 0, -1, -1, -1, -1,
    front_end_can_afford_rider_style, 11, 0, 377, 379 },
  { 378, 0, 0, -1, -1, -1, -1,
    front_end_is_selected_rider_style, 11, 0, 381, 383 },
  { 373, front_end_command_sequence_150, 0, -1, -1, -1, 374,
    0, 0, 0, 375, -1 },
  { 374, front_end_command_sequence_151, 0, -1, -1, 373, -1,
    0, 0, 0, 326, -1 },
  { 380, front_end_command_sequence_152, 0, -1, -1, -1, -1,
    0, 0, 0, 249, -1 },
  { 382, 0, 0, -1, -1, -1, -1,
    0, 0, 0, 326, -1 },
  { 384, 0, 0, -1, -1, -1, -1,
    0, 0, 0, 326, -1 },
  { 389, 0, 0, -1, -1, -1, -1,
    front_end_can_afford_rider_style, 12, 0, 390, 392 },
  { 391, 0, 0, -1, -1, -1, -1,
    front_end_is_selected_rider_style, 12, 0, 394, 396 },
  { 386, front_end_command_sequence_156, 0, -1, -1, -1, 387,
    0, 0, 0, 388, -1 },
  { 387, front_end_command_sequence_157, 0, -1, -1, 386, -1,
    0, 0, 0, 326, -1 },
  { 393, front_end_command_sequence_158, 0, -1, -1, -1, -1,
    0, 0, 0, 249, -1 },
  { 395, 0, 0, -1, -1, -1, -1,
    0, 0, 0, 326, -1 },
  { 397, 0, 0, -1, -1, -1, -1,
    0, 0, 0, 326, -1 },
  { 399, front_end_command_sequence_161, 0, -1, 404, 400, -1,
    0, 0, 0, 405, -1 },
  { 400, front_end_command_sequence_162, 0, -1, 403, 402, 399,
    0, 0, 0, 418, -1 },
  { 401, front_end_command_sequence_163, 0, 402, -1, -1, 403,
    0, 0, 0, 431, -1 },
  { 402, front_end_command_sequence_164, 0, -1, 401, -1, 400,
    0, 0, 0, 444, -1 },
  { 403, front_end_command_sequence_165, 0, 400, -1, 401, 404,
    0, 0, 0, 457, -1 },
  { 404, front_end_command_sequence_166, 0, 399, -1, 403, -1,
    0, 0, 0, 249, -1 },
  { 409, 0, 0, -1, -1, -1, -1,
    front_end_can_afford_rider_style, 9, 0, 410, 412 },
  { 411, 0, 0, -1, -1, -1, -1,
    front_end_is_selected_rider_style, 9, 0, 414, 416 },
  { 406, front_end_command_sequence_168, 0, -1, -1, -1, 407,
    0, 0, 0, 408, -1 },
  { 407, front_end_command_sequence_169, 0, -1, -1, 406, -1,
    0, 0, 0, 398, -1 },
  { 413, front_end_command_sequence_170, 0, -1, -1, -1, -1,
    0, 0, 0, 249, -1 },
  { 415, 0, 0, -1, -1, -1, -1,
    0, 0, 0, 398, -1 },
  { 417, 0, 0, -1, -1, -1, -1,
    0, 0, 0, 398, -1 },
  { 422, 0, 0, -1, -1, -1, -1,
    front_end_can_afford_rider_style, 7, 0, 423, 425 },
  { 424, 0, 0, -1, -1, -1, -1,
    front_end_is_selected_rider_style, 7, 0, 427, 429 },
  { 419, front_end_command_sequence_174, 0, -1, -1, -1, 420,
    0, 0, 0, 421, -1 },
  { 420, front_end_command_sequence_175, 0, -1, -1, 419, -1,
    0, 0, 0, 398, -1 },
  { 426, front_end_command_sequence_176, 0, -1, -1, -1, -1,
    0, 0, 0, 249, -1 },
  { 428, 0, 0, -1, -1, -1, -1,
    0, 0, 0, 398, -1 },
  { 430, 0, 0, -1, -1, -1, -1,
    0, 0, 0, 398, -1 },
  { 435, 0, 0, -1, -1, -1, -1,
    front_end_can_afford_rider_style, 6, 0, 436, 438 },
  { 437, 0, 0, -1, -1, -1, -1,
    front_end_is_selected_rider_style, 6, 0, 440, 442 },
  { 432, front_end_command_sequence_180, 0, -1, -1, -1, 433,
    0, 0, 0, 434, -1 },
  { 433, front_end_command_sequence_181, 0, -1, -1, 432, -1,
    0, 0, 0, 398, -1 },
  { 439, front_end_command_sequence_182, 0, -1, -1, -1, -1,
    0, 0, 0, 249, -1 },
  { 441, 0, 0, -1, -1, -1, -1,
    0, 0, 0, 398, -1 },
  { 443, 0, 0, -1, -1, -1, -1,
    0, 0, 0, 398, -1 },
  { 448, 0, 0, -1, -1, -1, -1,
    front_end_can_afford_rider_style, 5, 0, 449, 451 },
  { 450, 0, 0, -1, -1, -1, -1,
    front_end_is_selected_rider_style, 5, 0, 453, 455 },
  { 445, front_end_command_sequence_186, 0, -1, -1, -1, 446,
    0, 0, 0, 447, -1 },
  { 446, front_end_command_sequence_187, 0, -1, -1, 445, -1,
    0, 0, 0, 398, -1 },
  { 452, front_end_command_sequence_188, 0, -1, -1, -1, -1,
    0, 0, 0, 249, -1 },
  { 454, 0, 0, -1, -1, -1, -1,
    0, 0, 0, 398, -1 },
  { 456, 0, 0, -1, -1, -1, -1,
    0, 0, 0, 398, -1 },
  { 461, 0, 0, -1, -1, -1, -1,
    front_end_can_afford_rider_style, 8, 0, 462, 464 },
  { 463, 0, 0, -1, -1, -1, -1,
    front_end_is_selected_rider_style, 8, 0, 466, 468 },
  { 458, front_end_command_sequence_192, 0, -1, -1, -1, 459,
    0, 0, 0, 460, -1 },
  { 459, front_end_command_sequence_193, 0, -1, -1, 458, -1,
    0, 0, 0, 398, -1 },
  { 465, front_end_command_sequence_194, 0, -1, -1, -1, -1,
    0, 0, 0, 249, -1 },
  { 467, 0, 0, -1, -1, -1, -1,
    0, 0, 0, 398, -1 },
  { 469, 0, 0, -1, -1, -1, -1,
    0, 0, 0, 398, -1 },
  { 479, front_end_command_sequence_197, 0, -1, -1, -1, 480,
    0, 0, 0, 482, -1 },
  { 480, front_end_command_sequence_198, 0, -1, -1, 479, -1,
    0, 0, 0, 249, -1 },
  { 484, front_end_command_sequence_199, 0, -1, 485, -1, 483,
    0, 0, 0, 470, -1 },
  { 485, front_end_command_sequence_200, 0, 484, -1, -1, 486,
    0, 0, 0, 235, -1 },
  { 483, front_end_command_sequence_201, 0, -1, 486, 484, -1,
    0, 0, 0, 42, -1 },
  { 486, front_end_command_sequence_202, 0, 483, -1, 485, -1,
    0, 0, 0, 478, -1 },
  { 488, 0, 0, -1, -1, -1, -1,
    0, 0, 0, 489, -1 },
  { 490, 0, 0, -1, -1, -1, -1,
    0, 0, 0, 491, 491 },
  { 492, 0, 0, -1, -1, -1, -1,
    front_end_prepare_selected_opponents, 0, 0, 493, 495 },
  { 494, 0, 0, -1, -1, -1, -1,
    front_end_is_alternate_game_mode, 0, 0, 242, 478 },
  { 496, 0, 0, -1, -1, -1, -1,
    front_end_is_alternate_game_mode, 0, 0, 242, 478 },
  { 498, 0, 0, -1, -1, -1, -1,
    front_end_has_pending_race_result, 0, 0, 487, 6 },
  { 3, 0, 0, -1, -1, -1, -1,
    0, 0, 0, 4, -1 },
  { 5, front_end_command_sequence_000, 0, -1, -1, -1, -1,
    0, 0, 0, 497, -1 },
  { -1, 0, 0, -1, -1, -1, -1,
    0, 0, 0, -1, -1 }
};

FrontEndMenu front_end_menus[154] =
{
  { 0, 0, 0, 1, 1, { 0, 0, 0 } },
  { 6, 0, 0, 8, 0, { 0, 0, 0 } },
  { 10, 0, 0, 11, 0, { 0, 0, 0 } },
  { 224, 0, 0, 225, 1, { 0, 0, 0 } },
  { 19, 0, 0, 20, 0, { 0, 0, 0 } },
  { 22, 0, 0, 23, 0, { 0, 0, 0 } },
  { 25, 0, 0, 26, 0, { 0, 0, 0 } },
  { 32, 0, 0, 33, 0, { 0, 0, 0 } },
  { 35, 0, 0, 36, 1, { 0, 0, 0 } },
  { 37, 0, 0, 38, 0, { 0, 0, 0 } },
  { 40, 0, 0, 41, 0, { 0, 0, 0 } },
  { 42, 0, 0, 43, 0, { 0, 0, 0 } },
  { 190, 0, 0, 191, 1, { 0, 0, 0 } },
  { 52, 0, 0, 54, 0, { 0, 0, 0 } },
  { 55, 0, 0, 56, 0, { 0, 0, 0 } },
  { 58, 0, 0, 59, 0, { 0, 0, 0 } },
  { 65, 0, 0, 66, 0, { 0, 0, 0 } },
  { 68, 0, 0, 69, 1, { 0, 0, 0 } },
  { 70, 0, 0, 71, 0, { 0, 0, 0 } },
  { 73, 0, 0, 74, 0, { 0, 0, 0 } },
  { 75, 0, 0, 76, 0, { 0, 0, 0 } },
  { 207, 0, 0, 208, 1, { 0, 0, 0 } },
  { 84, 0, 0, 85, 0, { 0, 0, 0 } },
  { 87, 0, 0, 88, 0, { 0, 0, 0 } },
  { 90, 0, 0, 91, 0, { 0, 0, 0 } },
  { 97, 0, 0, 98, 0, { 0, 0, 0 } },
  { 100, 0, 0, 101, 1, { 0, 0, 0 } },
  { 102, 0, 0, 103, 0, { 0, 0, 0 } },
  { 105, 0, 0, 106, 0, { 0, 0, 0 } },
  { 226, 0, 0, 234, 0, { 0, 0, 0 } },
  { 235, front_end_command_sequence_004, 0, 236, 0, { 0, 0, 0 } },
  { 242, front_end_command_sequence_004, 0, 243, 0, { 0, 0, 0 } },
  { 249, 0, 0, 250, 0, { 0, 0, 0 } },
  { 470, front_end_command_sequence_085, 0, 471, 0, { 0, 0, 0 } },
  { 472, front_end_command_sequence_086, 0, 473, 0, { 0, 0, 0 } },
  { 474, front_end_command_sequence_087, 0, 475, 0, { 0, 0, 0 } },
  { 476, front_end_command_sequence_088, 0, 477, 0, { 0, 0, 0 } },
  { 254, 0, 0, 256, 0, { 0, 0, 0 } },
  { 261, front_end_command_sequence_095, 0, 262, 0, { 0, 0, 0 } },
  { 264, 0, 0, 265, 1, { 0, 0, 0 } },
  { 266, 0, 0, 267, 1, { 0, 0, 0 } },
  { 268, 0, 0, 269, 0, { 0, 0, 0 } },
  { 270, front_end_command_sequence_099, 0, 271, 0, { 0, 0, 0 } },
  { 272, front_end_command_sequence_100, 0, 273, 0, { 0, 0, 0 } },
  { 287, front_end_command_sequence_101, 0, 288, 0, { 0, 0, 0 } },
  { 290, 0, 0, 291, 1, { 0, 0, 0 } },
  { 292, 0, 0, 293, 1, { 0, 0, 0 } },
  { 294, 0, 0, 295, 0, { 0, 0, 0 } },
  { 296, front_end_command_sequence_105, 0, 297, 0, { 0, 0, 0 } },
  { 298, front_end_command_sequence_106, 0, 299, 0, { 0, 0, 0 } },
  { 274, front_end_command_sequence_107, 0, 275, 0, { 0, 0, 0 } },
  { 277, 0, 0, 278, 1, { 0, 0, 0 } },
  { 279, 0, 0, 280, 1, { 0, 0, 0 } },
  { 281, 0, 0, 282, 0, { 0, 0, 0 } },
  { 283, front_end_command_sequence_111, 0, 284, 0, { 0, 0, 0 } },
  { 285, front_end_command_sequence_112, 0, 286, 0, { 0, 0, 0 } },
  { 300, front_end_command_sequence_113, 0, 301, 0, { 0, 0, 0 } },
  { 303, 0, 0, 304, 1, { 0, 0, 0 } },
  { 305, 0, 0, 306, 1, { 0, 0, 0 } },
  { 307, 0, 0, 308, 0, { 0, 0, 0 } },
  { 309, front_end_command_sequence_117, 0, 310, 0, { 0, 0, 0 } },
  { 311, front_end_command_sequence_118, 0, 312, 0, { 0, 0, 0 } },
  { 313, front_end_command_sequence_119, 0, 314, 0, { 0, 0, 0 } },
  { 316, 0, 0, 317, 1, { 0, 0, 0 } },
  { 318, 0, 0, 319, 1, { 0, 0, 0 } },
  { 320, 0, 0, 321, 0, { 0, 0, 0 } },
  { 322, front_end_command_sequence_123, 0, 323, 0, { 0, 0, 0 } },
  { 324, front_end_command_sequence_124, 0, 325, 0, { 0, 0, 0 } },
  { 326, 0, 0, 328, 0, { 0, 0, 0 } },
  { 333, front_end_command_sequence_131, 0, 334, 0, { 0, 0, 0 } },
  { 336, 0, 0, 337, 1, { 0, 0, 0 } },
  { 338, 0, 0, 339, 1, { 0, 0, 0 } },
  { 340, 0, 0, 341, 0, { 0, 0, 0 } },
  { 342, front_end_command_sequence_135, 0, 343, 0, { 0, 0, 0 } },
  { 344, front_end_command_sequence_136, 0, 345, 0, { 0, 0, 0 } },
  { 346, front_end_command_sequence_137, 0, 347, 0, { 0, 0, 0 } },
  { 349, 0, 0, 350, 1, { 0, 0, 0 } },
  { 351, 0, 0, 352, 1, { 0, 0, 0 } },
  { 353, 0, 0, 354, 0, { 0, 0, 0 } },
  { 355, front_end_command_sequence_141, 0, 356, 0, { 0, 0, 0 } },
  { 357, front_end_command_sequence_142, 0, 358, 0, { 0, 0, 0 } },
  { 359, front_end_command_sequence_143, 0, 360, 0, { 0, 0, 0 } },
  { 362, 0, 0, 363, 1, { 0, 0, 0 } },
  { 364, 0, 0, 365, 1, { 0, 0, 0 } },
  { 366, 0, 0, 367, 0, { 0, 0, 0 } },
  { 368, front_end_command_sequence_147, 0, 369, 0, { 0, 0, 0 } },
  { 370, front_end_command_sequence_148, 0, 371, 0, { 0, 0, 0 } },
  { 372, front_end_command_sequence_149, 0, 373, 0, { 0, 0, 0 } },
  { 375, 0, 0, 376, 1, { 0, 0, 0 } },
  { 377, 0, 0, 378, 1, { 0, 0, 0 } },
  { 379, 0, 0, 380, 0, { 0, 0, 0 } },
  { 381, front_end_command_sequence_153, 0, 382, 0, { 0, 0, 0 } },
  { 383, front_end_command_sequence_154, 0, 384, 0, { 0, 0, 0 } },
  { 385, front_end_command_sequence_155, 0, 386, 0, { 0, 0, 0 } },
  { 388, 0, 0, 389, 1, { 0, 0, 0 } },
  { 390, 0, 0, 391, 1, { 0, 0, 0 } },
  { 392, 0, 0, 393, 0, { 0, 0, 0 } },
  { 394, front_end_command_sequence_159, 0, 395, 0, { 0, 0, 0 } },
  { 396, front_end_command_sequence_160, 0, 397, 0, { 0, 0, 0 } },
  { 398, 0, 0, 402, 0, { 0, 0, 0 } },
  { 405, front_end_command_sequence_167, 0, 409, 1, { 0, 0, 0 } },
  { 408, 0, 0, 411, 1, { 0, 0, 0 } },
  { 410, 0, 0, 406, 0, { 0, 0, 0 } },
  { 412, 0, 0, 413, 0, { 0, 0, 0 } },
  { 414, front_end_command_sequence_171, 0, 415, 0, { 0, 0, 0 } },
  { 416, front_end_command_sequence_172, 0, 417, 0, { 0, 0, 0 } },
  { 418, front_end_command_sequence_173, 0, 419, 0, { 0, 0, 0 } },
  { 421, 0, 0, 422, 1, { 0, 0, 0 } },
  { 423, 0, 0, 424, 1, { 0, 0, 0 } },
  { 425, 0, 0, 426, 0, { 0, 0, 0 } },
  { 427, front_end_command_sequence_177, 0, 428, 0, { 0, 0, 0 } },
  { 429, front_end_command_sequence_178, 0, 430, 0, { 0, 0, 0 } },
  { 431, front_end_command_sequence_179, 0, 432, 0, { 0, 0, 0 } },
  { 434, 0, 0, 435, 1, { 0, 0, 0 } },
  { 436, 0, 0, 437, 1, { 0, 0, 0 } },
  { 438, 0, 0, 439, 0, { 0, 0, 0 } },
  { 440, front_end_command_sequence_183, 0, 441, 0, { 0, 0, 0 } },
  { 442, front_end_command_sequence_184, 0, 443, 0, { 0, 0, 0 } },
  { 444, front_end_command_sequence_185, 0, 445, 0, { 0, 0, 0 } },
  { 447, 0, 0, 448, 1, { 0, 0, 0 } },
  { 449, 0, 0, 450, 1, { 0, 0, 0 } },
  { 451, 0, 0, 452, 0, { 0, 0, 0 } },
  { 453, front_end_command_sequence_189, 0, 454, 0, { 0, 0, 0 } },
  { 455, front_end_command_sequence_190, 0, 456, 0, { 0, 0, 0 } },
  { 457, front_end_command_sequence_191, 0, 458, 0, { 0, 0, 0 } },
  { 460, 0, 0, 461, 1, { 0, 0, 0 } },
  { 462, 0, 0, 463, 1, { 0, 0, 0 } },
  { 464, 0, 0, 465, 0, { 0, 0, 0 } },
  { 466, front_end_command_sequence_195, 0, 467, 0, { 0, 0, 0 } },
  { 468, front_end_command_sequence_196, 0, 469, 0, { 0, 0, 0 } },
  { 478, 0, 0, 479, 0, { 0, 0, 0 } },
  { 482, 0, 0, 484, 0, { 0, 0, 0 } },
  { 107, 0, 0, 108, 0, { 0, 0, 0 } },
  { 129, 0, 0, 130, 0, { 0, 0, 0 } },
  { 151, 0, 0, 152, 0, { 0, 0, 0 } },
  { 173, front_end_command_sequence_071, 0, 174, 0, { 0, 0, 0 } },
  { 183, 0, 0, 184, 0, { 0, 0, 0 } },
  { 186, 0, 0, 187, 1, { 0, 0, 0 } },
  { 188, 0, 0, 189, 1, { 0, 0, 0 } },
  { 192, front_end_command_sequence_071, 0, 193, 0, { 0, 0, 0 } },
  { 202, 0, 0, 203, 0, { 0, 0, 0 } },
  { 205, 0, 0, 206, 1, { 0, 0, 0 } },
  { 209, front_end_command_sequence_071, 0, 210, 0, { 0, 0, 0 } },
  { 219, 0, 0, 220, 0, { 0, 0, 0 } },
  { 222, 0, 0, 223, 1, { 0, 0, 0 } },
  { 487, front_end_command_sequence_203, 0, 488, 0, { 0, 0, 0 } },
  { 489, front_end_command_sequence_204, 0, 490, 0, { 0, 0, 0 } },
  { 491, 0, 0, 492, 1, { 0, 0, 0 } },
  { 493, front_end_command_sequence_205, 0, 494, 0, { 0, 0, 0 } },
  { 495, 0, 0, 496, 1, { 0, 0, 0 } },
  { 497, 0, 0, 498, 1, { 0, 0, 0 } },
  { 2, 0, 0, 3, 0, { 0, 0, 0 } },
  { 4, 0, 0, 5, 0, { 0, 0, 0 } },
  { -1, 0, 0, -1, 0, { 0, 0, 0 } }
};

FrontEndAssetState front_end_asset_state = { 0, 0, 0, -1 };

RaceModeState race_mode_state =
{
  1, 1, 0, 1, 1, 0, { 0, 0 },
  0,
  0, 0, { 0, 0 }
};

RiderProfile player_profiles[PLAYER_PROFILE_COUNT] =
{
  {
    0, 4, 0, 0, 0, 0, { 0, 0 }, 500,
    { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }
  },
  {
    9, 4, 0, 0, 0, 0, { 0, 0 }, 500,
    { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }
  }
};

int profile_slot_rider_types[PROFILE_SLOT_COUNT] =
{
  0, 0, 0, 0, 0, 0, 0, 0
};

RaceResultEntry race_results[4] =
{
  { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }
};

int course_distance_tenths[5][5] =
{
  { 83, 132, 277, 309, 355 },
  { 83, 121, 152, 297, 369 },
  { 83, 128, 272, 336, 360 },
  { 80, 118, 262, 296, 389 },
  { 85, 129, 272, 294, 353 }
};

int rider_style_purchase_prices[15] =
{
  4495, 3249, 3497, 5489, 2999,
  29998, 18999, 40000, 21789, 34888,
  13796, 16875, 11988, 9199, 6994
};

int rider_style_resale_values[15] =
{
  2247, 1624, 1748, 2744, 1499,
  14999, 9499, 20000, 10894, 17444,
  6898, 8437, 5994, 4599, 3497
};

int rider_starting_balances[10] =
{
  500, 500, 1000, 600, 750, 400, 800, 500, 200, 500
};

int race_cash_awards[15] =
{
  100, 75, 50, 40, 30, 25, 20, 16, 13, 10, 7, 5, 3, 2, 1
};

const char *front_end_stream_paths[43] =
{
  front_end_intro_stream_path,
  front_end_inter_jessie_stream_path,
  front_end_inter_duel_stream_path,
  front_end_win_1_stream_path,
  front_end_win_2_stream_path,
  front_end_win_3_stream_path,
  front_end_win_4_stream_path,
  front_end_win_5_stream_path,
  front_end_win_6_stream_path,
  front_end_lose_1_stream_path,
  front_end_lose_2_stream_path,
  front_end_lose_3_stream_path,
  front_end_lose_4_stream_path,
  front_end_lose_5_stream_path,
  front_end_lose_6_stream_path,
  front_end_lose_7_stream_path,
  front_end_lose_8_stream_path,
  front_end_lose_9_stream_path,
  front_end_lose_10_stream_path,
  front_end_busted_1_stream_path,
  front_end_busted_2_stream_path,
  front_end_busted_3_stream_path,
  front_end_busted_4_stream_path,
  front_end_busted_5_stream_path,
  front_end_busted_6_stream_path,
  front_end_start_1_stream_path,
  front_end_start_2_stream_path,
  front_end_start_3_stream_path,
  front_end_start_4_stream_path,
  front_end_start_5_stream_path,
  front_end_start_6_stream_path,
  front_end_wrecked_1_stream_path,
  front_end_wrecked_2_stream_path,
  front_end_wrecked_3_stream_path,
  front_end_wrecked_4_stream_path,
  front_end_wrecked_5_stream_path,
  front_end_wrecked_6_stream_path,
  front_end_new_level_2_stream_path,
  front_end_new_level_1_stream_path,
  front_end_new_level_5_stream_path,
  front_end_new_level_6_stream_path,
  front_end_new_level_3_stream_path,
  0
};

FrontEndPreviewState front_end_preview_state =
{
  0,
  1,
  1, { 0, 0, 0 },
  0, { 0, 0, 0 },
  0, { 0, 0, 0 },
  0,
  -1
};

signed char opponent_choice_style_map[4] = { 1, 0, 1, 2 };

int opponent_portrait_resource_ids[8][3] =
{
  { 823, 825, 824 }, { 497, 499, 498 },
  { 758, 760, 759 }, { 628, 630, 629 },
  { 562, 564, 563 }, { 693, 695, 694 },
  { 432, 434, 433 }, { 368, 370, 369 }
};

int opponent_bike_resource_ids[8][3] =
{
  { 826, 827, 826 }, { 500, 500, 501 },
  { 761, 761, 762 }, { 631, 632, 631 },
  { 565, 567, 566 }, { 696, 696, 697 },
  { 435, 436, 436 }, { 371, 371, 371 }
};

unsigned char random_preview_active[4] = { 1, 0, 0, 0 };

FrontEndCelDescriptor profile_load_descriptor =
{
  40, -1, 0, { 0, 0, 0 }
};

FrontEndCelDescriptor profile_save_descriptor =
{
  43, -1, 0, { 0, 0, 0 }
};

TrackEffectPools track_effect_pools = { 0, 0, 0 };

CCB *track_effect_resources[TRACK_EFFECT_RESOURCE_COUNT] = { 0, 0, 0 };

int track_effect_type_masks[TRACK_EFFECT_TYPE_COUNT] =
{
  0x40, 0x80, 0x100, 0x200
};

TrackEffectCreationDescriptor track_effect_parent_descriptor =
{
  0, { 0, 0, 0 }, 0x400, 0x1E, 0, track_effect_parent_update, 0,
  track_effect_parent_destroy
};

TrackEffectCreationDescriptor track_effect_child_descriptor =
{
  0, { 0, 0, 0 }, 0x40, 0x1E, track_effect_child_draw, 0, 0,
  track_effect_child_destroy
};

RaceInputLatchState race_input_latch_state =
{
  0, { 0, 0, 0 }, 0, 0, 0, 0
};

GameAudioState game_audio_state =
{
  { { 0, 0 }, { 0, 0 }, { 0, 0 } },
  { { 0, { 0, 0 }, 0 }, { 0, { 0, 0 }, 0 } },
  { 0, 0, 0, 0 },
  {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0
  },
  { 0, 0, 0, 0, 0 },
  0, { 0, 0, 0 },
  0, { 0, 0, 0 },
  0,
  0, { 0, 0, 0 },
  0, { 0, 0, 0 },
  ".SCON",
  ".eams",
  0,
  0, { 0, 0, 0 },
  0, { 0, 0, 0 },
  0, 0, 0, 0,
  { 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0 },
  { 0, 0, 0, 0 },
  { 0, 0, 0, 0 },
  0, 0x7C6, 0, 0
};

unsigned char collision_positions_adjusted[4] = { 0, 0, 0, 0 };

RaceObjectSchedulerState race_object_scheduler =
{
  0, 0, 0, 0, { 0, 0, 0 }, 0, 0, 0,
  0, { 0, 0, 0 }, 0, 0, 0, 0, 0x1E0
};

TrackVisibilityScanState track_visibility_scan = { 0, 0 };

RiderPhysicsTuning rider_physics_tuning =
{
  0x4A,
  1, { 0, 0, 0 },
  0,
  { 0, 2, 0x14, 0x1E },
  { 0x100, 0xF0, 0x80, 0x64 },
  { 0xE6, 0xE6, 0x80, 0x64 },
  { 6, 5, 1, 1 },
  { 0x0C, 0x0C, 0, 0 },
  {
    { 0x100, 0x100, 0xF7, 0xF2 },
    { 0x100, 0x100, 0x108, 0x10D },
    { 0x100, 0x100, 0xF9, 0xF4 },
    { 0x100, 0x100, 0x10C, 0x111 },
    { 0x100, 0x100, 0xF3, 0xEE },
    { 0x100, 0x100, 0x10C, 0x111 },
    { 0x100, 0x100, 0xF5, 0xF0 },
    { 0x100, 0x100, 0x108, 0x10D },
  },
  0
};

RoadRendererState road_renderer_state =
{
  { 0, 0, 0, 0 },
  { 0, 0, 0 },
  0, { 0, 0, 0 },
  0,
  0,
  {
    0, 0, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3,
    4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
    5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
    5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
    6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
    6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
    6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
    6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7
  },
  3, { 0, 0, 0 },
  0, 0, 0,
  0, { 0, 0, 0 },
  0,
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, { 0, { 0, 0, 0 } } },
  0,
  0
};

int road_depth_input_distances[ROAD_DEPTH_INPUT_COUNT] =
{
  0xBB8, 0x9C4, 0x898, 0x7D0, 0x708, 0x640, 0x578, 0x4B0,
  0x3E8, 0x320, 0x2BC, 0x2BC, 0x2BC, 0x2BC, 0x2BC, 0x2BC,
  0x2BC, 0x2BC, 0x2BC, 0x2BC, 0x2BC, 0x2BC, 0x2BC, 0x2BC,
  0x2BC, 0x2BC, 0x2BC, 0x2BC, 0x2BC, 0x2BC, 0x2BC, 0x2BC,
  0x2BC, 0x2BC
};

unsigned int cel_pixel_shifts[CEL_PIXEL_FORMAT_COUNT] =
{
  0, 5, 4, 3, (unsigned int)-1, 2, 1
};

unsigned int cel_pixel_masks[CEL_PIXEL_FORMAT_COUNT] =
{
  0, 0x0F, 0x0F, 7, (unsigned int)-1, 3, 1
};

signed char road_draw_order[ROAD_DRAW_ORDER_COUNT]
[ROAD_DRAW_COMMAND_COUNT] =
{
  { 4, 3, 2, 1, 0, 0, 0, 0 },
  { 0, 1, 4, 3, 2, 0, 0, 0 },
  { 0, 1, 2, 3, 4, 0, 0, 0 }
};

AnimationResourcePair primary_animation_resource_pairs[2] =
{
  { 0, 0 }, { 0, 0 }
};

AnimationResourcePair secondary_animation_resource_pairs[2] =
{
  { 0, 0 }, { 0, 0 }
};

signed char animation_resource_pair_by_variant[4] = { 0, 0, 1, 0 };

void *rider_animation_state_targets[RACE_RIDER_COUNT] =
{
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

AnimationBindingSpec rider_animation_channels[ANIMATION_CHANNEL_SPEC_COUNT] =
{
  { 0x536D6C31U, 0, 1, 0x0000 },
  { 0x536D6C32U, 0, 1, 0x0000 },
  { 0x52696465U, 0, 1, 0x0000 },
  { 0x5475636BU, 0, 1, 0x0000 },
  { 0x57686565U, 0, 1, 0x0000 },
  { 0x41726D30U, 0, 0, 0x0000 },
  { 0x41726D31U, 0, 1, 0x0000 },
  { 0x31417230U, 0, 0, 0x0000 },
  { 0x31417231U, 0, 1, 0x0000 },
  { 0x41726D42U, 0, 1, 0x0000 },
  { 0x53746F70U, 0, 1, 0x0000 },
  { 0x48697430U, 0, 0, 0x0000 },
  { 0x48697431U, 0, 2, 0x0000 },
  { 0x43684C30U, 0, 0, 0x0000 },
  { 0x43684C31U, 0, 0, 0x0000 },
  { 0x43684C32U, 0, 0, 0x0000 },
  { 0x43684C33U, 0, 0, 0x0000 },
  { 0x43684C34U, 0, 2, 0x0000 },
  { 0x43685230U, 0, 0, 0x0000 },
  { 0x43685231U, 0, 0, 0x0000 },
  { 0x43685232U, 0, 0, 0x0000 },
  { 0x43685233U, 0, 0, 0x0000 },
  { 0x43685234U, 0, 2, 0x0000 },
  { 0x63684C30U, 0, 0, 0x0000 },
  { 0x63684C31U, 0, 0, 0x0000 },
  { 0x63684C32U, 0, 0, 0x0000 },
  { 0x63684C33U, 0, 0, 0x0000 },
  { 0x63684C34U, 0, 2, 0x0000 },
  { 0x63685230U, 0, 0, 0x0000 },
  { 0x63685231U, 0, 0, 0x0000 },
  { 0x63685232U, 0, 0, 0x0000 },
  { 0x63685233U, 0, 0, 0x0000 },
  { 0x63685234U, 0, 2, 0x0000 },
  { 0x53634C30U, 0, 0, 0x0000 },
  { 0x53634C31U, 0, 2, 0x0000 },
  { 0x53635230U, 0, 0, 0x0000 },
  { 0x53635231U, 0, 2, 0x0000 },
  { 0x436C4C30U, 0, 0, 0x0000 },
  { 0x436C4C31U, 0, 0, 0x0000 },
  { 0x436C4C32U, 0, 2, 0x0000 },
  { 0x436C5230U, 0, 0, 0x0000 },
  { 0x436C5231U, 0, 0, 0x0000 },
  { 0x436C5232U, 0, 2, 0x0000 },
  { 0x636C4C30U, 0, 0, 0x0000 },
  { 0x636C4C31U, 0, 0, 0x0000 },
  { 0x636C4C32U, 0, 2, 0x0000 },
  { 0x636C5230U, 0, 0, 0x0000 },
  { 0x636C5231U, 0, 0, 0x0000 },
  { 0x636C5232U, 0, 2, 0x0000 },
  { 0x53634C30U, 0, 0, 0x0000 },
  { 0x53634C31U, 0, 2, 0x0000 },
  { 0x53635230U, 0, 0, 0x0000 },
  { 0x53635231U, 0, 2, 0x0000 },
  { 0x50754C30U, 0, 0, 0x0000 },
  { 0x50754C31U, 0, 0, 0x0000 },
  { 0x50754C32U, 0, 2, 0x0000 },
  { 0x50755230U, 0, 0, 0x0000 },
  { 0x50755231U, 0, 0, 0x0000 },
  { 0x50755232U, 0, 2, 0x0000 },
  { 0x70754C30U, 0, 0, 0x0000 },
  { 0x70754C31U, 0, 0, 0x0000 },
  { 0x70754C32U, 0, 2, 0x0000 },
  { 0x70755230U, 0, 0, 0x0000 },
  { 0x70755231U, 0, 0, 0x0000 },
  { 0x70755232U, 0, 2, 0x0000 },
  { 0x4B694C30U, 0, 0, 0x0000 },
  { 0x4B694C31U, 0, 2, 0x0000 },
  { 0x4B695230U, 0, 0, 0x0000 },
  { 0x4B695231U, 0, 2, 0x0000 },
  { 0x46466C30U, 0, 0, 0x0000 },
  { 0x46466C31U, 0, 0, 0x0000 },
  { 0x46466C32U, 0, 0, 0x0000 },
  { 0x46466C33U, 0, 0, 0x0000 },
  { 0x46466C34U, 0, 0, 0x0000 },
  { 0x46466C35U, 0, 0, 0x0000 },
  { 0x46466C36U, 0, 1, 0x0000 },
  { 0x526F6C30U, 0, 0, 0x0000 },
  { 0x526F6C31U, 0, 0, 0x0000 },
  { 0x526F6C32U, 0, 0, 0x0000 },
  { 0x526F6C33U, 0, 0, 0x0000 },
  { 0x526F6C34U, 0, 0, 0x0000 },
  { 0x526F6C35U, 0, 0, 0x0000 },
  { 0x526F6C36U, 0, 0, 0x0000 },
  { 0x526F6C37U, 0, 1, 0x0000 },
  { 0x53526F30U, 0, 0, 0x0000 },
  { 0x53526F31U, 0, 0, 0x0000 },
  { 0x53526F32U, 0, 0, 0x0000 },
  { 0x53526F33U, 0, 1, 0x0000 },
  { 0x53537030U, 0, 0, 0x0000 },
  { 0x53537031U, 0, 0, 0x0000 },
  { 0x53537032U, 0, 0, 0x0000 },
  { 0x53537033U, 0, 1, 0x0000 },
  { 0x42537030U, 0, 0, 0x0000 },
  { 0x42537031U, 0, 0, 0x0000 },
  { 0x42537032U, 0, 0, 0x0000 },
  { 0x42537033U, 0, 1, 0x0000 },
  { 0x466C7930U, 0, 0, 0x0000 },
  { 0x466C7931U, 0, 0, 0x0000 },
  { 0x466C7932U, 0, 0, 0x0000 },
  { 0x466C7933U, 0, 0, 0x0000 },
  { 0x466C7934U, 0, 1, 0x0000 },
  { 0x4C616E30U, 0, 0, 0x0000 },
  { 0x4C616E31U, 0, 0, 0x0000 },
  { 0x4C616E32U, 0, 0, 0x0000 },
  { 0x4C616E33U, 0, 2, 0x0000 },
  { 0x46616C30U, 0, 0, 0x0000 },
  { 0x46616C31U, 0, 2, 0x0000 },
  { 0x53746130U, 0, 0, 0x0000 },
  { 0x53746131U, 0, 0, 0x0000 },
  { 0x53746132U, 0, 0, 0x0000 },
  { 0x53746133U, 0, 0, 0x0000 },
  { 0x53746134U, 0, 2, 0x0000 },
  { 0x5370726BU, 0, 0, 0x0000 },
  { 0x5370726BU, 1, 0, 0x0000 },
  { 0x5370726BU, 2, 0, 0x0000 },
  { 0x5370726BU, 3, 1, 0x0000 },
  { 0x52756E30U, 0, 0, 0x0000 },
  { 0x52756E31U, 0, 0, 0x0000 },
  { 0x52756E32U, 0, 0, 0x0000 },
  { 0x52756E33U, 0, 0, 0x0000 },
  { 0x52756E34U, 0, 0, 0x0000 },
  { 0x52756E35U, 0, 1, 0x0000 },
  { 0x5055704CU, 0, 0, 0x0000 },
  { 0x5055704CU, 1, 0, 0x0000 },
  { 0x5055704CU, 2, 0, 0x0000 },
  { 0x5055704CU, 3, 0, 0x0000 },
  { 0x5055704CU, 4, 2, 0x0000 },
  { 0x50557052U, 0, 0, 0x0000 },
  { 0x50557052U, 1, 0, 0x0000 },
  { 0x50557052U, 2, 0, 0x0000 },
  { 0x50557052U, 3, 0, 0x0000 },
  { 0x50557052U, 4, 2, 0x0000 },
  { 0x4D6E744CU, 0, 0, 0x0000 },
  { 0x4D6E744CU, 1, 0, 0x0000 },
  { 0x4D6E744CU, 2, 0, 0x0000 },
  { 0x4D6E744CU, 3, 0, 0x0000 },
  { 0x4D6E744CU, 4, 2, 0x0000 },
  { 0x4D6E7452U, 0, 0, 0x0000 },
  { 0x4D6E7452U, 1, 0, 0x0000 },
  { 0x4D6E7452U, 2, 0, 0x0000 },
  { 0x4D6E7452U, 3, 0, 0x0000 },
  { 0x4D6E7452U, 4, 2, 0x0000 },
  { 0x42466C30U, 0, 0, 0x0000 },
  { 0x42466C31U, 0, 0, 0x0000 },
  { 0x42466C32U, 0, 0, 0x0000 },
  { 0x42466C33U, 0, 0, 0x0000 },
  { 0x42466C34U, 0, 1, 0x0000 },
  { 0x4253704CU, 0, 0, 0x0000 },
  { 0x4253704CU, 1, 0, 0x0000 },
  { 0x4253704CU, 2, 0, 0x0000 },
  { 0x4253704CU, 3, 1, 0x0000 },
  { 0x42537052U, 0, 0, 0x0000 },
  { 0x42537052U, 1, 0, 0x0000 },
  { 0x42537052U, 2, 0, 0x0000 },
  { 0x42537052U, 3, 1, 0x0000 },
  { 0x4246614CU, 0, 0, 0x0000 },
  { 0x4246614CU, 1, 0, 0x0000 },
  { 0x4246614CU, 2, 2, 0x0000 },
  { 0x42466152U, 0, 0, 0x0000 },
  { 0x42466152U, 1, 0, 0x0000 },
  { 0x42466152U, 2, 2, 0x0000 },
  { 0x4253744CU, 0, 1, 0x0000 },
  { 0x42537452U, 0, 1, 0x0000 }
};

CelAnimationBinding
  rider_one_animation_bindings[RIDER_ONE_ANIMATION_BINDING_COUNT] =
{ { 0 } };

CelAnimationBinding *rider_one_animation_by_state
[RIDER_ONE_ANIMATION_STATE_COUNT] =
{
  &rider_one_animation_bindings[0],
  &rider_one_animation_bindings[1],
  &rider_one_animation_bindings[2],
  &rider_one_animation_bindings[3],
  &rider_one_animation_bindings[4],
  &rider_one_animation_bindings[5],
  &rider_one_animation_bindings[7],
  &rider_one_animation_bindings[9],
  &rider_one_animation_bindings[10],
  &rider_one_animation_bindings[11],
  &rider_one_animation_bindings[13],
  &rider_one_animation_bindings[18],
  &rider_one_animation_bindings[23],
  &rider_one_animation_bindings[28],
  &rider_one_animation_bindings[33],
  &rider_one_animation_bindings[35],
  &rider_one_animation_bindings[37],
  &rider_one_animation_bindings[40],
  &rider_one_animation_bindings[43],
  &rider_one_animation_bindings[46],
  &rider_one_animation_bindings[49],
  &rider_one_animation_bindings[51],
  &rider_one_animation_bindings[53],
  &rider_one_animation_bindings[56],
  &rider_one_animation_bindings[59],
  &rider_one_animation_bindings[62],
  &rider_one_animation_bindings[65],
  &rider_one_animation_bindings[67],
  gPlayerOneFrontFlipBindings,
  gPlayerOneRollBindings,
  &rider_one_animation_bindings[69],
  &rider_one_animation_bindings[73],
  &rider_one_animation_bindings[77],
  &rider_one_animation_bindings[81],
  &rider_one_animation_bindings[86],
  &rider_one_animation_bindings[90],
  &rider_one_animation_bindings[92],
  &rider_one_animation_bindings[97],
  gPlayerOneRunBindings,
  &rider_one_animation_bindings[101],
  &rider_one_animation_bindings[106],
  &rider_one_animation_bindings[111],
  &rider_one_animation_bindings[116],
  &rider_one_animation_bindings[121],
  &rider_one_animation_bindings[126],
  &rider_one_animation_bindings[130],
  &rider_one_animation_bindings[134],
  &rider_one_animation_bindings[137],
  &rider_one_animation_bindings[140],
  &rider_one_animation_bindings[141]
};

CelAnimationBinding *rider_one_animation_state_sentinel = 0;

CelAnimationBinding
  rider_two_animation_bindings[RIDER_TWO_ANIMATION_BINDING_COUNT] =
{ { 0 } };

CelAnimationBinding *rider_two_animation_by_state
[RIDER_TWO_ANIMATION_STATE_COUNT] =
{
  0,
  0,
  &rider_two_animation_bindings[0],
  0,
  0,
  0,
  0,
  0,
  &rider_two_animation_bindings[7],
  &rider_two_animation_bindings[8],
  0,
  0,
  0,
  0,
  0,
  0,
  &rider_two_animation_bindings[1],
  &rider_two_animation_bindings[4],
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  gPlayerTwoFrontFlipBindings,
  0,
  0,
  0,
  0,
  &rider_two_animation_bindings[15],
  0,
  0,
  0,
  &rider_two_animation_bindings[24],
  0,
  0,
  0,
  0,
  0,
  &rider_two_animation_bindings[10],
  0,
  0,
  &rider_two_animation_bindings[21],
  0,
  &rider_two_animation_bindings[20],
  0,
  0
};

CelAnimationBinding **rider_animation_state_tables[3] =
{
  rider_one_animation_by_state,
  rider_two_animation_by_state,
  0
};

int
car_animation_render();
int
track_object_update();
int
update_passive_racer_motion_and_collisions();
void
track_object_mode_update(RacerEntity *object);

CarAnimationLoadState car_animation_load_state =
{
  0, { 0, 0, 0 }, 0
};

char car_resource_suffix[12] = ".Cars.RSRC";

int car_resource_suffix_alignment = 0;

AnimationBindingSpec car_animation_selectors[CAR_ANIMATION_VARIANT_COUNT] =
{
  { 0x43617273U, 0, 1, 0 },
  { 0x43617231U, 0, 1, 0 },
  { 0x43617232U, 0, 1, 0 }
};

CarAnimationResourcePair
  car_animation_resources[CAR_ANIMATION_COUNT] = { { 0, 0 } };

int car_animation_output_info[CAR_ANIMATION_COUNT] =
{
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

CelAnimationBinding car_animation_bindings[CAR_ANIMATION_COUNT]
[CAR_ANIMATION_VARIANT_COUNT] =
{ { { 0 } } };

CelAnimationBinding *car_animation_variant_bindings[CAR_ANIMATION_COUNT]
[CAR_ANIMATION_VARIANT_COUNT] =
{
  { &car_animation_bindings[0][0], &car_animation_bindings[0][1], &car_animation_bindings[0][2] },
  { &car_animation_bindings[1][0], &car_animation_bindings[1][1], &car_animation_bindings[1][2] },
  { &car_animation_bindings[2][0], &car_animation_bindings[2][1], &car_animation_bindings[2][2] },
  { &car_animation_bindings[3][0], &car_animation_bindings[3][1], &car_animation_bindings[3][2] },
  { &car_animation_bindings[4][0], &car_animation_bindings[4][1], &car_animation_bindings[4][2] },
  { &car_animation_bindings[5][0], &car_animation_bindings[5][1], &car_animation_bindings[5][2] },
  { &car_animation_bindings[6][0], &car_animation_bindings[6][1], &car_animation_bindings[6][2] },
  { &car_animation_bindings[7][0], &car_animation_bindings[7][1], &car_animation_bindings[7][2] },
  { &car_animation_bindings[8][0], &car_animation_bindings[8][1], &car_animation_bindings[8][2] },
  { &car_animation_bindings[9][0], &car_animation_bindings[9][1], &car_animation_bindings[9][2] },
  { &car_animation_bindings[10][0], &car_animation_bindings[10][1],
    &car_animation_bindings[10][2] },
  { &car_animation_bindings[11][0], &car_animation_bindings[11][1],
    &car_animation_bindings[11][2] },
  { &car_animation_bindings[12][0], &car_animation_bindings[12][1],
    &car_animation_bindings[12][2] },
  { &car_animation_bindings[13][0], &car_animation_bindings[13][1],
    &car_animation_bindings[13][2] },
  { &car_animation_bindings[14][0], &car_animation_bindings[14][1], &car_animation_bindings[14][2] }
};

CelAnimationBinding **car_animation_bindings_by_car[CAR_ANIMATION_COUNT] =
{
  car_animation_variant_bindings[0],
  car_animation_variant_bindings[1],
  car_animation_variant_bindings[2],
  car_animation_variant_bindings[3],
  car_animation_variant_bindings[4],
  car_animation_variant_bindings[5],
  car_animation_variant_bindings[6],
  car_animation_variant_bindings[7],
  car_animation_variant_bindings[8],
  car_animation_variant_bindings[9],
  car_animation_variant_bindings[10],
  car_animation_variant_bindings[11],
  car_animation_variant_bindings[12],
  car_animation_variant_bindings[13],
  car_animation_variant_bindings[14]
};

SegmentRepairGlobals segment_repair_globals =
{
  0, 0, 0, 0,
  { 0, 0, 0, 0, 0, 0, 0 },
  3, 0, 4, 0
};

FamilyResourceGlobals family_resource_globals =
{
  "Families/",
  "Family",
  0, 0, 0,
  0, { 0, 0, 0 },
  {
    { 0, 0, 0 },
    { 0, 0, 0 },
    { 0, 0, 0 },
    { 0, 0, 0 },
    { 0, 0, 0 },
    { 0, 0, 0 },
    { 0, 0, 0 },
    { 0, 0, 0 }
  },
  0, 0,
  { 0, 0, 0, 0 },
  { 0, 0, 0, 0 },
  { 0, 0, 0, 0 },
  4, 0x29810, 0x5000,
  { 0, 0, 0, 0 },
  0
};

TrackSimulationGlobals track_simulation_globals =
{
  { 0, { 0, 0, 0 } },
  0, { 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, { 0, { 0, 0, 0 } } },
  0, 0, 0, 0,
  0, { 0, 0, 0 },
  0, 2, 3, 0x1C0, 0,
  { 0, 0, 0, 0, 0, 0, 0 },
  0, { 0, 0, 0 },
  0
};

signed char track_direction_modes[8] = { 0, 1, 0, -1, 1, 0, -1, 0 };

RacerCreationTemplate track_object_template =
{
  0x03000000U, 0x10, 0x0A,
  car_animation_render, track_object_update,
  update_passive_racer_motion_and_collisions, 0,
  0xE8, 0xBC, 0x32, 0x0A, 0x6F, -0xF00, 0x400, 0x4A,
  1, 0x80, 0x4000, 0x40, 0x2300, 0xC0, 0x708,
  track_object_mode_update, 0, 0
};

TrackObjectModeTemplate track_object_mode_templates[4] =
{
  { 0x4000, 0x40, 0x2300, 0xC0, 0x708, track_object_mode_update },
  { 0xA000, 0x40, 0x2300, 0xC0, 0x708, track_object_mode_update },
  { 0x4000, 0x40, 0x2300, 0xC0, 0x708, track_object_mode_update },
  { 0xA000, 0x40, 0x2300, 0xC0, 0x708, track_object_mode_update }
};

DisplayRuntimeGlobals display_runtime_globals =
{
  { 0, 0 },
  { 0, 0 },
  0,
  { 0, 0 },
  { 0, 0 },
  { 0, 0 },
  0, 0, 0,
  1, { 0, 0, 0 },
  0, 0, 0, 0,
  0, { 0, 0, 0 },
  0,
  0x89696798U,
  0, 0, 0, 0,
  0, 0,
  0, { 0, 0, 0 },
  {
    0, 0, 1, 2, 2, 3, 4, 4, 5, 6, 6, 7, 7, 8, 9, 9,
    10, 11, 11, 12, 13, 13, 14, 14, 15, 16, 16, 17, 18, 18, 19, 19,
    20, 21, 21, 22, 23, 23, 24, 24, 25, 26, 26, 27, 28, 28, 29, 29,
    30, 31, 31, 32, 32, 33, 34, 34, 35, 36, 36, 37, 37, 38, 39, 39,
    40, 40, 41, 42, 42, 43, 43, 44, 44, 45, 46, 46, 47, 47, 48, 49,
    49, 50, 50, 51, 51, 52, 53, 53, 54, 54, 55, 55, 56, 57, 57, 58,
    58, 59, 59, 60, 60, 61, 62, 62, 63, 63, 64, 64, 65, 65, 66, 66,
    67, 68, 68, 69, 69, 70, 70, 71, 71, 72, 72, 73, 73, 74, 74, 75,
    75, 76, 76, 77, 77, 78, 78, 79, 79, 80, 80, 81, 81, 82, 82, 83,
    83, 84, 84, 85, 85, 86, 86, 87, 87, 88, 88, 88, 89, 89, 90, 90,
    91, 91, 92, 92, 93, 93, 93, 94, 94, 95, 95, 96, 96, 97, 97, 97,
    98, 98, 99, 99, 100, 100, 100, 101, 101, 102, 102, 103, 103, 103, 104, 104,
    105, 105, 105, 106, 106, 107, 107, 107, 108, 108, 109, 109, 109, 110, 110, 111,
    111, 111, 112, 112, 112, 113, 113, 114, 114, 114, 115, 115, 115, 116, 116, 116,
    117, 117, 118, 118, 118, 119, 119, 119, 120, 120, 120, 121, 121, 121, 122, 122,
    122, 123, 123, 123, 124, 124, 124, 125, 125, 125, 126, 126, 126, 127, 127, 127,
    128
  },
  { 0, 0, 0 },
  {
    0, 1, 3, 4, 6, 7, 9, 11, 12, 14, 15, 17, 18, 20, 22, 23,
    25, 26, 28, 29, 31, 33, 34, 36, 37, 39, 41, 42, 44, 46, 47, 49,
    50, 52, 54, 55, 57, 59, 60, 62, 64, 65, 67, 69, 70, 72, 74, 75,
    77, 79, 81, 82, 84, 86, 88, 89, 91, 93, 95, 96, 98, 100, 102, 104,
    106, 107, 109, 111, 113, 115, 117, 119, 121, 123, 124, 126, 128, 130, 132, 134,
    136, 138, 140, 142, 145, 147, 149, 151, 153, 155, 157, 159, 162, 164, 166, 168,
    171, 173, 175, 177, 180, 182, 185, 187, 189, 192, 194, 197, 199, 202, 204, 207,
    210, 212, 215, 218, 220, 223, 226, 229, 232, 234, 237, 240, 243, 246, 249, 252
  },
  {
    0x100, 0x103, 0x106, 0x109, 0x10C, 0x110, 0x113, 0x116,
    0x11A, 0x11D, 0x121, 0x125, 0x128, 0x12C, 0x130, 0x134,
    0x137, 0x13B, 0x13F, 0x143, 0x148, 0x14C, 0x150, 0x154,
    0x159, 0x15D, 0x162, 0x166, 0x16B, 0x170, 0x175, 0x17A,
    0x17F, 0x184, 0x189, 0x18E, 0x194, 0x199, 0x19F, 0x1A5,
    0x1AB, 0x1B1, 0x1B7, 0x1BD, 0x1C3, 0x1CA, 0x1D1, 0x1D7,
    0x1DE, 0x1E6, 0x1ED, 0x1F4, 0x1FC, 0x204, 0x20C, 0x214,
    0x21D, 0x225, 0x22E, 0x238, 0x241, 0x24B, 0x255, 0x25F,
    0x26A, 0x274, 0x280, 0x28B, 0x297, 0x2A3, 0x2B0, 0x2BD,
    0x2CB, 0x2D9, 0x2E8, 0x2F7, 0x306, 0x317, 0x328, 0x339,
    0x34B, 0x35E, 0x372, 0x387, 0x39D, 0x3B3, 0x3CB, 0x3E4,
    0x3FE, 0x419, 0x435, 0x454, 0x474, 0x495, 0x4B9, 0x4DE,
    0x506, 0x531, 0x55E, 0x58F, 0x5C3, 0x5FB, 0x637, 0x677,
    0x6BD, 0x709, 0x75C, 0x7B7, 0x81B, 0x889, 0x904, 0x98D,
    0xA27, 0xAD5, 0xB9C, 0xC82, 0xD8E, 0xECB, 0x1046, 0x1216,
    0x145A, 0x1744, 0x1B26, 0x2095, 0x28BB, 0x3650, 0x5179, 0xA2F0,
    0x7FFFFFFF
  },
  0, { 0, 0, 0 },
  0, 0,
  { 0, 0 }
};

TrackRuntimeGlobals track_runtime_globals =
{
  10, 0, 0,
  0, { 0, 0, 0 },
  0, { 0, 0, 0 },
  0, { 0, 0, 0 },
  0,
  { 0, { 0, 0, 0 }, 1, 20, 0, 0, 0, 0 },
  0,
  { 0, 0 },
  0, 0, 0, 0,
  0, { 0, 0, 0 },
  0
};

#define RESOURCE_CHUNK_SEGMENT 0x53475320U
#define RESOURCE_CHUNK_NODE 0x4E4F4420U
#define RESOURCE_CHUNK_CEL 0x43454C20U
#define RESOURCE_CHUNK_ANIMATION 0x414E494DU
#define RESOURCE_CHUNK_PALETTE 0x50414C20U

ResourceCatalogHandles resource_catalog_handles = { 0, 0, 0, 0 };

char resource_root_path[12] = "$boot/Rash/";

char course_variant_names[COURSE_VARIANT_COUNT][10] =
{
  "City", "Highway", "Medley", "Napa", "Canyon", "City"
};

char course_resource_suffix[12] = "Opt.Rsrc";

ResourceCatalogEntry course_catalog[COURSE_CATALOG_COUNT] =
{
  { &segment_repair_globals.resource_destination,
    RESOURCE_CHUNK_SEGMENT, 0, 0, 0 },
  { &segment_repair_globals.current_segment,
    RESOURCE_CHUNK_NODE, 0, 0, 0 },
  { &road_renderer_state.road_layers[0], RESOURCE_CHUNK_CEL, 0, 0, 0 },
  { &road_renderer_state.road_layers[1], RESOURCE_CHUNK_CEL, 1, 0, 0 },
  { &road_renderer_state.road_layers[2], RESOURCE_CHUNK_CEL, 2, 0, 0 },
  { &rider_resource_items[9], RESOURCE_CHUNK_CEL, 3, 0, 0 },
  { &rider_resource_items[10], RESOURCE_CHUNK_CEL, 4, 0, 0 },
  { &rider_resource_items[11], RESOURCE_CHUNK_CEL, 5, 0, 0 },
  { &rider_resource_items[12], RESOURCE_CHUNK_CEL, 6, 0, 0 },
  { &rider_resource_items[13], RESOURCE_CHUNK_CEL, 7, 0, 0 },
  { &rider_resource_items[14], RESOURCE_CHUNK_CEL, 8, 0, 0 },
  { &challenge_resource_items[0], RESOURCE_CHUNK_CEL, 9, 0, 0 },
  { &challenge_resource_items[1], RESOURCE_CHUNK_CEL, 10, 0, 0 },
  { &challenge_resource_items[2], RESOURCE_CHUNK_CEL, 11, 0, 0 },
  { &challenge_resource_items[3], RESOURCE_CHUNK_CEL, 12, 0, 0 },
  { &challenge_resource_items[4], RESOURCE_CHUNK_CEL, 13, 0, 0 },
  { &challenge_resource_items[5], RESOURCE_CHUNK_CEL, 14, 0, 0 },
  { &challenge_resource_items[6], RESOURCE_CHUNK_CEL, 15, 0, 0 },
  { &challenge_resource_items[7], RESOURCE_CHUNK_CEL, 16, 0, 0 },
  { 0, 0, 0, 0, 0 }
};

ResourceCatalogEntry rash_options_catalog[RASH_OPTIONS_CATALOG_COUNT] =
{
  { &gRoadSurfaceCelVariants[0][0], RESOURCE_CHUNK_CEL, 0, 0, 0 },
  { &gRoadSurfaceCelVariants[1][0], RESOURCE_CHUNK_CEL, 1, 0, 0 },
  { &gRoadSurfaceCelVariants[2][0], RESOURCE_CHUNK_CEL, 2, 0, 0 },
  { &gRoadSurfaceCelVariants[3][0], RESOURCE_CHUNK_CEL, 3, 0, 0 },
  { &gRoadSurfaceCelVariants[4][0], RESOURCE_CHUNK_CEL, 4, 0, 0 },
  { &gRoadSurfaceCelVariants[5][0], RESOURCE_CHUNK_CEL, 5, 0, 0 },
  { &gRoadSurfaceCelVariants[6][0], RESOURCE_CHUNK_CEL, 6, 0, 0 },
  { &gRoadSurfaceCelVariants[7][0], RESOURCE_CHUNK_CEL, 7, 0, 0 },
  { &gRoadSurfaceCelVariants[8][0], RESOURCE_CHUNK_CEL, 8, 0, 0 },
  { &gRoadSurfaceCelVariants[0][1], RESOURCE_CHUNK_CEL, 9, 0, 0 },
  { &gRoadSurfaceCelVariants[1][1], RESOURCE_CHUNK_CEL, 10, 0, 0 },
  { &gRoadSurfaceCelVariants[2][1], RESOURCE_CHUNK_CEL, 11, 0, 0 },
  { &gRoadSurfaceCelVariants[3][1], RESOURCE_CHUNK_CEL, 12, 0, 0 },
  { &gRoadSurfaceCelVariants[4][1], RESOURCE_CHUNK_CEL, 13, 0, 0 },
  { &gRoadSurfaceCelVariants[5][1], RESOURCE_CHUNK_CEL, 14, 0, 0 },
  { &gRoadSurfaceCelVariants[6][1], RESOURCE_CHUNK_CEL, 15, 0, 0 },
  { &gRoadSurfaceCelVariants[7][1], RESOURCE_CHUNK_CEL, 16, 0, 0 },
  { &gRoadSurfaceCelVariants[8][1], RESOURCE_CHUNK_CEL, 17, 0, 0 },
  { &gRoadSurfaceCelVariants[0][2], RESOURCE_CHUNK_CEL, 18, 0, 0 },
  { &gRoadSurfaceCelVariants[1][2], RESOURCE_CHUNK_CEL, 19, 0, 0 },
  { &gRoadSurfaceCelVariants[2][2], RESOURCE_CHUNK_CEL, 20, 0, 0 },
  { &gRoadSurfaceCelVariants[3][2], RESOURCE_CHUNK_CEL, 21, 0, 0 },
  { &gRoadSurfaceCelVariants[4][2], RESOURCE_CHUNK_CEL, 22, 0, 0 },
  { &gRoadSurfaceCelVariants[5][2], RESOURCE_CHUNK_CEL, 23, 0, 0 },
  { &gRoadSurfaceCelVariants[6][2], RESOURCE_CHUNK_CEL, 24, 0, 0 },
  { &gRoadSurfaceCelVariants[7][2], RESOURCE_CHUNK_CEL, 25, 0, 0 },
  { &gRoadSurfaceCelVariants[8][2], RESOURCE_CHUNK_CEL, 26, 0, 0 },
  { &road_surface_cel_variants[0][0], RESOURCE_CHUNK_CEL, 27, 0, 0 },
  { &road_surface_cel_variants[0][2], RESOURCE_CHUNK_CEL, 28, 0, 0 },
  { &road_surface_cel_variants[1][0], RESOURCE_CHUNK_CEL, 29, 0, 0 },
  { &road_surface_cel_variants[1][2], RESOURCE_CHUNK_CEL, 30, 0, 0 },
  { &road_surface_cel_variants[2][0], RESOURCE_CHUNK_CEL, 31, 0, 0 },
  { &road_surface_cel_variants[2][2], RESOURCE_CHUNK_CEL, 32, 0, 0 },
  { &road_surface_palette_variants[0], RESOURCE_CHUNK_CEL, 33, 0, 0 },
  { &road_surface_palette_variants[1], RESOURCE_CHUNK_CEL, 34, 0, 0 },
  { &road_surface_palette_variants[2], RESOURCE_CHUNK_CEL, 35, 0, 0 },
  { &road_surface_cel_table_a[0], RESOURCE_CHUNK_CEL, 36, 0, 0 },
  { &road_surface_cel_table_a[1], RESOURCE_CHUNK_CEL, 37, 0, 0 },
  { &road_surface_cel_table_a[2], RESOURCE_CHUNK_CEL, 38, 0, 0 },
  { &road_surface_cel_table_a[3], RESOURCE_CHUNK_CEL, 39, 0, 0 },
  { &road_surface_cel_table_a[4], RESOURCE_CHUNK_CEL, 40, 0, 0 },
  { &road_surface_cel_table_a[5], RESOURCE_CHUNK_CEL, 41, 0, 0 },
  { &road_surface_cel_table_b[0], RESOURCE_CHUNK_CEL, 42, 0, 0 },
  { &road_surface_cel_table_b[1], RESOURCE_CHUNK_CEL, 43, 0, 0 },
  { &road_surface_cel_table_b[2], RESOURCE_CHUNK_CEL, 44, 0, 0 },
  { &road_surface_cel_table_b[3], RESOURCE_CHUNK_CEL, 45, 0, 0 },
  { &road_surface_cel_table_b[4], RESOURCE_CHUNK_CEL, 46, 0, 0 },
  { &road_surface_cel_table_b[5], RESOURCE_CHUNK_CEL, 47, 0, 0 },
  { &road_surface_dynamic_cels[0], RESOURCE_CHUNK_CEL, 48, 0, 0 },
  { &road_surface_dynamic_cels[1], RESOURCE_CHUNK_CEL, 49, 0, 0 },
  { &track_effect_pools.cel_source, RESOURCE_CHUNK_CEL, 50, 0, 0 },
  { &track_effect_resources[0], RESOURCE_CHUNK_CEL, 51, 0, 0 },
  { &track_effect_resources[1], RESOURCE_CHUNK_CEL, 52, 0, 0 },
  { &track_effect_resources[2], RESOURCE_CHUNK_CEL, 53, 0, 0 },
  { &hud_globals.primary_hud_cel, RESOURCE_CHUNK_CEL, 54, 0, 0 },
  { &hud_globals.gauge_animation_item,
    RESOURCE_CHUNK_ANIMATION, 0, 0, 0 },
  { &hud_globals.boost_bar_ccb, RESOURCE_CHUNK_CEL, 55, 0, 0 },
  { &hud_globals.speedometer_needle_ccb, RESOURCE_CHUNK_CEL, 56, 0, 0 },
  { &hud_globals.health_gauge_needle_ccb, RESOURCE_CHUNK_CEL, 57, 0, 0 },
  { &hud_globals.rider_resource_items[0], RESOURCE_CHUNK_CEL, 58, 0, 0 },
  { &hud_globals.rider_resource_items[1], RESOURCE_CHUNK_CEL, 59, 0, 0 },
  { &rider_resource_items[1], RESOURCE_CHUNK_CEL, 60, 0, 0 },
  { &rider_resource_items[2], RESOURCE_CHUNK_CEL, 61, 0, 0 },
  { &rider_resource_items[3], RESOURCE_CHUNK_CEL, 62, 0, 0 },
  { &rider_resource_items[4], RESOURCE_CHUNK_CEL, 63, 0, 0 },
  { &rider_resource_items[5], RESOURCE_CHUNK_CEL, 64, 0, 0 },
  { &rider_resource_items[6], RESOURCE_CHUNK_CEL, 65, 0, 0 },
  { &rider_resource_items[7], RESOURCE_CHUNK_CEL, 66, 0, 0 },
  { &rider_resource_items[8], RESOURCE_CHUNK_CEL, 67, 0, 0 },
  { &road_renderer_state.overlay_resource_items[0],
    RESOURCE_CHUNK_CEL, 68, 0, 0 },
  { &road_renderer_state.overlay_resource_items[1],
    RESOURCE_CHUNK_CEL, 69, 0, 0 },
  { &road_renderer_state.overlay_resource_items[2],
    RESOURCE_CHUNK_CEL, 70, 0, 0 },
  { &input_thread_state.overlay_resource_item,
    RESOURCE_CHUNK_CEL, 71, 0, 0 },
  { &rider_animation_state_targets[0], RESOURCE_CHUNK_PALETTE, 1, 0, 0 },
  { &rider_animation_state_targets[1], RESOURCE_CHUNK_PALETTE, 2, 0, 0 },
  { &rider_animation_state_targets[2], RESOURCE_CHUNK_PALETTE, 3, 0, 0 },
  { &rider_animation_state_targets[3], RESOURCE_CHUNK_PALETTE, 4, 0, 0 },
  { &rider_animation_state_targets[4], RESOURCE_CHUNK_PALETTE, 5, 0, 0 },
  { &rider_animation_state_targets[5], RESOURCE_CHUNK_PALETTE, 6, 0, 0 },
  { &rider_animation_state_targets[6], RESOURCE_CHUNK_PALETTE, 7, 0, 0 },
  { &rider_animation_state_targets[7], RESOURCE_CHUNK_PALETTE, 8, 0, 0 },
  { &rider_animation_state_targets[8], RESOURCE_CHUNK_PALETTE, 9, 0, 0 },
  { &rider_animation_state_targets[9], RESOURCE_CHUNK_PALETTE, 10, 0, 0 },
  { &rider_animation_state_targets[10], RESOURCE_CHUNK_PALETTE, 11, 0, 0 },
  { &rider_animation_state_targets[11], RESOURCE_CHUNK_PALETTE, 12, 0, 0 },
  { &rider_animation_state_targets[12], RESOURCE_CHUNK_PALETTE, 13, 0, 0 },
  { &rider_animation_state_targets[13], RESOURCE_CHUNK_PALETTE, 14, 0, 0 },
  { &rider_animation_state_targets[14], RESOURCE_CHUNK_PALETTE, 15, 0, 0 },
  { 0, 0, 0, 0, 0 }
};

RuntimeFeatureFlags runtime_feature_flags =
{
  { 0, { 0, 0, 0 } },
  { 1, { 0, 0, 0 } },
  { 1, { 0, 0, 0 } },
  { 1, { 0, 0, 0 } },
  { 1, { 0, 0, 0 } },
  { 1, { 0, 0, 0 } },
  { 1, { 0, 0, 0 } },
  { 1, { 0, 0, 0 } }
};

unsigned int frame_tick = 0;

int frame_time_scale = 0x9000;

BigEndianByteFlag frame_timer_enabled = { 1, { 0, 0, 0 } };

ResourceInfoState resource_info_state = { 0, -1, 0 };

struct IntrusiveListStorage *rider_visual_pool = 0;

RacerCreationTemplate rider_visual_template =
{
  0x03000000U, 0x20, 0x3C,
  (RacerLifecycleCallback)rider_visual_create,
  (RacerLifecycleCallback)rider_visual_update,
  (RacerLifecycleCallback)rider_visual_destroy, 0,
  0, 0, 0, 5, 0x25, -0x200, 0x100, 0x4A, 1,
  0xC0, 0x1000, 0x10, 0x1E00, 0xC0, 0x64,
  rider_visual_contact, 0, 0
};

AnimationBindingSpec rider_motion_frames[6] =
{
  { -0x4D6F7631, 0, 0, 0 },
  { 0x4D6F7632, 0, 0, 0 },
  { 0x4D6F7633, 0, 0, 0 },
  { 0x4D6F7634, 0, 0, 0 },
  { 0x4D6F7635, 0, 0, 0 },
  { 0x4D6F7636, 0, 1, 0 }
};

AnimationBindingSpec rider_standing_frames[1] =
{
  { 0x53746E64, 0, 1, 0 }
};

AnimationBindingSpec rider_shake_frames[2] =
{
  { -0x53686B31, 0, 0, 0 },
  { 0x53686B32, 0, 2, 0 }
};

AnimationBindingSpec rider_fall_frames[6] =
{
  { -0x46616C31, 0, 0, 0 },
  { 0x46616C32, 0, 0, 0 },
  { 0x46616C33, 0, 0, 0 },
  { 0x46616C34, 0, 0, 0 },
  { 0x46616C35, 0, 0, 0 },
  { 0x46616C36, 0, 0x51, 0 }
};

AnimationBindingSpec rider_attack_frames[2] =
{
  { -0x41746B31, 0, 0, 0 },
  { 0x41746B32, 0, 2, 0 }
};

AnimationBindingSpec rider_fast_frames[6] =
{
  { -0x46737431, 0, 0, 0 },
  { 0x46737432, 0, 0, 0 },
  { 0x46737433, 0, 0, 0 },
  { 0x46737434, 0, 0, 0 },
  { 0x46737435, 0, 0, 0 },
  { 0x46737436, 0, 1, 0 }
};

AnimationBindingSpec rider_flag_frames[6] =
{
  { -0x466C6731, 0, 0, 0 },
  { 0x466C6732, 0, 0, 0 },
  { 0x466C6733, 0, 0, 0 },
  { 0x466C6734, 0, 0, 0 },
  { 0x466C6735, 0, 0, 0 },
  { 0x466C6736, 0, 1, 0 }
};

int rider_direction_vectors[4] =
{
  0, 0x00400000, -0x00800000, -0x00400000
};

MoviePlaybackControl movie_playback_control =
{
  0, { 0, 0, 0 }, 10
};

TextRenderManager *text_manager = 0;

unsigned int text_plut[8] =
{
  0, 0x00018001U, 0, 0x00018001U,
  0, 0x00018001U, 0, 0x00018001U
};

int text_render_workspace[8] = { 0 };

DisplaySystemGlobals display_system_globals =
{
  0, 0, { 0 }, { 0 }, 0, 0, 0, 0, 0,
  -1, { 0, 0, 0 }, 0, { 0, 0, 0 }
};

unsigned int previous_control_pad_state[8] = { 0 };

ParticleResourceGlobals particle_resource_globals = { 0 };

HudGlobals hud_globals =
{
  {
    { 0, 0xE6 }, { 0x42, 0xC8 }, { 0x14, 0xD2 },
    { 0x98, 0xCC }, { 0x88, 0xD2 }, { 0xEC, 0xC8 },
    { 0xF8, 0xD2 }, { 0x14, 0xD2 }, { 0x14, 0xC1 },
    { 0x46, 0xD2 }, { 0xA0, 0xC1 }, { 0x11A, 0xD2 },
    { 0x116, 0xC1 }, { 0x6E, 0xDC }, { 0x6E, 0xD2 },
    { 0xC0, 0xDC }, { 0xC8, 0xD2 }, { 0x64, 0xCD },
    { 0xC3, 0xD2 }, { 0xB6, 0xCD }, { 0xB6, 0xD2 },
    { 0x7E, 0xB4 }, { 0x80, 0xCC }
  },
  { 0 },
  0, 0, 0, 0, 0,
  { 0, 0 },
  0, 0
};

unsigned char pixel_bit_shifts[8] = { 0, 7, 6, 4, 2, 0, 0, 0 };

unsigned char pixel_repeat_counts[4] = { 2, 4, 4, 6 };

RoadSurfaceRenderGlobals road_surface_render_globals =
{
  0, { 0, 0, 0 },
  { 0, 0, 0, 0x100, 0x40, 0x200, 0xC0, 0x200,
    0x100, 0x100, 0x100, 0 },
  0, 0, 8, -1, 0, 0
};

int quad_join_offsets[16][2] =
{
  { 0x3000, -0x6000 }, { 0x6000, -0x6000 },
  { 0x6000, -0x3000 }, { 0x6000, 0 },
  { 0x6000, 0x3000 }, { 0x6000, 0x6000 },
  { 0x3000, 0x6000 }, { 0, 0x6000 },
  { -0x3000, 0x6000 }, { -0x6000, 0x6000 },
  { -0x6000, 0x3000 }, { -0x6000, 0 },
  { -0x6000, -0x3000 }, { -0x6000, -0x6000 },
  { -0x3000, -0x6000 }, { 0, -0x6000 }
};

BigEndianByteFlag road_surface_render_enabled = { 1, { 0, 0, 0 } };

int road_surface_colors[2] = { 0x5294, 0x18C8 };

void *road_surface_dynamic_cels[2] = { 0, 0 };

int road_surface_thresholds[3] = { 32, 16, 8 };

void *road_surface_cel_variants[3][3] = { { 0 } };

void *road_surface_palette_variants[3] = { 0 };

void *road_surface_cel_table_a[6] = { 0 };

void *road_surface_cel_table_b[6] = { 0 };

int stream_audio_reference_count = 0;

AudioPlaybackState audio_playback_state = { 0 };

AudioSourceDescriptor default_audio_source =
{
  { 0 },
  16, 0x5622, 1, 0x53445832U, 2, 0
};

unsigned int stream_input_snapshots[2] = { 0, 0 };

unsigned int stream_input_masks[2] = { 0, 0 };

int async_stream_message_port = 0;

unsigned int stream_frame_cel_terminator_flags = 0;

int stream_runtime_workspace[16] = { 0 };

RwTagArgument stream_open_tags[16] =
{
  { 0x00600000U, 0x0000C000U },
  { 0x00600001U, 1 },
  { 0x00600002U, 0 },
  { 0x00600003U, (unsigned int)-1 },
  { 0x00600004U, (unsigned int)-1 },
  { 0x00600005U, 0 },
  { 0x00600006U, (unsigned int)-1 },
  { 0x00600007U, (unsigned int)-1 },
  { 0x00600008U, (unsigned int)-1 },
  { 0x00600009U, (unsigned int)&stream_frame_cel_terminator_flags },
  { 0x0060000AU, (unsigned int)&stream_frame_cel_terminator_flags },
  { 0x0060000BU, (unsigned int)initialize_stream_audio_playback },
  { 0x0060000CU, (unsigned int)submit_stream_audio_chunk },
  { 0x0060000DU, 0x7FFF },
  { 0x00600010U, 0 },
  { 0, 0 }
};

BackgroundAudioGlobals background_audio_globals =
{
  0,
  1, { 0, 0, 0 },
  0x960,
  {
    background_trip_stream_path,
    background_simple_passing_stream_path,
    background_dinosaur_vacume_stream_path,
    background_the_bridge_stream_path,
    background_jessie_stream_path,
    background_pansy_stream_path,
    background_rusty_cage_stream_path,
    background_outshined_stream_path,
    background_kickstand_stream_path,
    background_superunknown_stream_path,
    background_last_train_stream_path,
    background_duel_stream_path,
    background_teethgrinder_stream_path,
    background_auto_surgery_stream_path
  },
  BACKGROUND_AUDIO_NAME_COUNT,
  0, { 0, 0, 0 },
  0x147,
  10,
  0x7500,
  0
};

RwTagArgument background_stream_tags[5] =
{
  { 0x00600000U, 0x0000C000U },
  { 0x00600001U, 0x108 },
  { 0x00600006U, 0 },
  { 0x00600007U, 0 },
  { 0, 0 }
};

RwTagArgument background_command_tags[2] =
{
  { 0x0060000DU, 0x7FFF },
  { 0x00600010U, 1 }
};

int background_stream_result = 0;

int background_command_result = 0;

MemoryAllocatorGlobals memory_allocator_globals =
{
  0, 0, 0, 0, 0, 0, 0, { 0 }, 0x80000, 0x10000
};

ResourceManagerGlobals resource_manager_globals = { 0, 0, -1 };

MusicLibraryGlobals music_library_globals =
{
  0,
  0, { 0, 0, 0 },
  0, 0,
  { 0, { 0, 0, 0 } },
  0, 0, 0,
  1, { 0, 0, 0 },
  1, { 0, 0, 0 },
  0, { 0, 0, 0 },
  0,
  { 0 },
  0, -1, 0
};

int score_loader_last_error = 0;

const char *mixer_gain_names[24] =
{
  audio_mixer_left_gain_0_name,
  audio_mixer_left_gain_1_name,
  audio_mixer_left_gain_2_name,
  audio_mixer_left_gain_3_name,
  audio_mixer_left_gain_4_name,
  audio_mixer_left_gain_5_name,
  audio_mixer_left_gain_6_name,
  audio_mixer_left_gain_7_name,
  audio_mixer_left_gain_8_name,
  audio_mixer_left_gain_9_name,
  audio_mixer_left_gain_10_name,
  audio_mixer_left_gain_11_name,
  audio_mixer_right_gain_0_name,
  audio_mixer_right_gain_1_name,
  audio_mixer_right_gain_2_name,
  audio_mixer_right_gain_3_name,
  audio_mixer_right_gain_4_name,
  audio_mixer_right_gain_5_name,
  audio_mixer_right_gain_6_name,
  audio_mixer_right_gain_7_name,
  audio_mixer_right_gain_8_name,
  audio_mixer_right_gain_9_name,
  audio_mixer_right_gain_10_name,
  audio_mixer_right_gain_11_name
};

AudioMixerTemplateDefaults audio_mixer_template_defaults =
{
  0x7FFF,
  {
    audio_mixer_2x2_template_name,
    audio_mixer_4x2_template_name,
    audio_mixer_8x2_template_name,
    audio_mixer_12x2_template_name
  }
};


unsigned int operamath_quarter_sine_table[OPERAMATH_QUARTER_SINE_COUNT] =
{
  0x00000000, 0x000c90fe, 0x001921fb, 0x0025b2f8, 0x003243f5, 0x003ed4f2, 0x004b65ee, 0x0057f6e9,
  0x006487e3, 0x007118dc, 0x007da9d4, 0x008a3acb, 0x0096cbc1, 0x00a35cb5, 0x00afeda8, 0x00bc7e99,
  0x00c90f88, 0x00d5a075, 0x00e23160, 0x00eec249, 0x00fb5330, 0x0107e414, 0x011474f6, 0x012105d5,
  0x012d96b1, 0x013a278a, 0x0146b860, 0x01534934, 0x015fda03, 0x016c6ad0, 0x0178fb99, 0x01858c5e,
  0x01921d20, 0x019eaddd, 0x01ab3e97, 0x01b7cf4d, 0x01c45ffe, 0x01d0f0ab, 0x01dd8154, 0x01ea11f7,
  0x01f6a297, 0x02033331, 0x020fc3c6, 0x021c5457, 0x0228e4e2, 0x02357567, 0x024205e8, 0x024e9662,
  0x025b26d7, 0x0267b747, 0x027447b0, 0x0280d813, 0x028d6870, 0x0299f8c7, 0x02a68917, 0x02b31961,
  0x02bfa9a4, 0x02cc39e1, 0x02d8ca16, 0x02e55a44, 0x02f1ea6c, 0x02fe7a8c, 0x030b0aa4, 0x03179ab5,

  0x03242abf, 0x0330bac1, 0x033d4abb, 0x0349daac, 0x03566a96, 0x0362fa78, 0x036f8a51, 0x037c1a22,
  0x0388a9ea, 0x039539a9, 0x03a1c960, 0x03ae590d, 0x03bae8b2, 0x03c7784d, 0x03d407df, 0x03e09767,
  0x03ed26e6, 0x03f9b65b, 0x040645c7, 0x0412d528, 0x041f6480, 0x042bf3cd, 0x04388310, 0x04451249,
  0x0451a177, 0x045e309a, 0x046abfb3, 0x04774ec1, 0x0483ddc3, 0x04906cbb, 0x049cfba7, 0x04a98a88,
  0x04b6195d, 0x04c2a827, 0x04cf36e5, 0x04dbc597, 0x04e8543e, 0x04f4e2d8, 0x05017165, 0x050dffe7,
  0x051a8e5c, 0x05271cc4, 0x0533ab20, 0x0540396f, 0x054cc7b1, 0x055955e6, 0x0565e40d, 0x05727228,
  0x057f0035, 0x058b8e34, 0x05981c26, 0x05a4aa09, 0x05b137df, 0x05bdc5a7, 0x05ca5361, 0x05d6e10c,
  0x05e36ea9, 0x05effc38, 0x05fc89b8, 0x06091729, 0x0615a48b, 0x062231de, 0x062ebf22, 0x063b4c57,

  0x0647d97c, 0x06546692, 0x0660f398, 0x066d808f, 0x067a0d76, 0x06869a4c, 0x06932713, 0x069fb3c9,
  0x06ac406f, 0x06b8cd05, 0x06c5598a, 0x06d1e5fe, 0x06de7262, 0x06eafeb4, 0x06f78af6, 0x07041726,
  0x0710a345, 0x071d2f52, 0x0729bb4e, 0x07364738, 0x0742d311, 0x074f5ed7, 0x075bea8c, 0x0768762e,
  0x077501be, 0x07818d3c, 0x078e18a7, 0x079aa400, 0x07a72f45, 0x07b3ba78, 0x07c04598, 0x07ccd0a5,
  0x07d95b9e, 0x07e5e685, 0x07f27157, 0x07fefc16, 0x080b86c2, 0x08181159, 0x08249bdd, 0x0831264c,
  0x083db0a7, 0x084a3aee, 0x0856c520, 0x08634f3e, 0x086fd947, 0x087c633c, 0x0888ed1b, 0x089576e5,
  0x08a2009a, 0x08ae8a3a, 0x08bb13c5, 0x08c79d3a, 0x08d42699, 0x08e0afe2, 0x08ed3916, 0x08f9c233,
  0x09064b3a, 0x0912d42c, 0x091f5d06, 0x092be5ca, 0x09386e78, 0x0944f70f, 0x09517f8f, 0x095e07f8,

  0x096a9049, 0x09771884, 0x0983a0a7, 0x099028b3, 0x099cb0a7, 0x09a93884, 0x09b5c048, 0x09c247f5,
  0x09cecf89, 0x09db5706, 0x09e7de6a, 0x09f465b5, 0x0a00ece8, 0x0a0d7403, 0x0a19fb04, 0x0a2681ed,
  0x0a3308bd, 0x0a3f8f73, 0x0a4c1610, 0x0a589c94, 0x0a6522fe, 0x0a71a94f, 0x0a7e2f85, 0x0a8ab5a2,
  0x0a973ba5, 0x0aa3c18e, 0x0ab0475c, 0x0abccd11, 0x0ac952aa, 0x0ad5d829, 0x0ae25d8d, 0x0aeee2d7,
  0x0afb6805, 0x0b07ed19, 0x0b147211, 0x0b20f6ee, 0x0b2d7baf, 0x0b3a0055, 0x0b4684df, 0x0b53094d,
  0x0b5f8d9f, 0x0b6c11d5, 0x0b7895f0, 0x0b8519ed, 0x0b919dcf, 0x0b9e2193, 0x0baaa53b, 0x0bb728c7,
  0x0bc3ac35, 0x0bd02f87, 0x0bdcb2bb, 0x0be935d2, 0x0bf5b8cb, 0x0c023ba7, 0x0c0ebe66, 0x0c1b4107,
  0x0c27c389, 0x0c3445ee, 0x0c40c835, 0x0c4d4a5d, 0x0c59cc68, 0x0c664e53, 0x0c72d020, 0x0c7f51cf,

  0x0c8bd35e, 0x0c9854cf, 0x0ca4d620, 0x0cb15752, 0x0cbdd865, 0x0cca5959, 0x0cd6da2d, 0x0ce35ae1,
  0x0cefdb76, 0x0cfc5bea, 0x0d08dc3f, 0x0d155c73, 0x0d21dc87, 0x0d2e5c7b, 0x0d3adc4e, 0x0d475c00,
  0x0d53db92, 0x0d605b03, 0x0d6cda53, 0x0d795982, 0x0d85d88f, 0x0d92577b, 0x0d9ed646, 0x0dab54ef,
  0x0db7d376, 0x0dc451dc, 0x0dd0d01f, 0x0ddd4e40, 0x0de9cc40, 0x0df64a1c, 0x0e02c7d7, 0x0e0f456f,
  0x0e1bc2e4, 0x0e284036, 0x0e34bd66, 0x0e413a72, 0x0e4db75b, 0x0e5a3421, 0x0e66b0c3, 0x0e732d42,
  0x0e7fa99e, 0x0e8c25d5, 0x0e98a1e9, 0x0ea51dd8, 0x0eb199a4, 0x0ebe154b, 0x0eca90ce, 0x0ed70c2c,
  0x0ee38766, 0x0ef0027b, 0x0efc7d6b, 0x0f08f836, 0x0f1572dc, 0x0f21ed5d, 0x0f2e67b8, 0x0f3ae1ee,
  0x0f475bff, 0x0f53d5ea, 0x0f604faf, 0x0f6cc94e, 0x0f7942c7, 0x0f85bc19, 0x0f923546, 0x0f9eae4c,

  0x0fab272b, 0x0fb79fe4, 0x0fc41876, 0x0fd090e1, 0x0fdd0926, 0x0fe98143, 0x0ff5f938, 0x10027107,
  0x100ee8ad, 0x101b602d, 0x1027d784, 0x10344eb4, 0x1040c5bb, 0x104d3c9b, 0x1059b352, 0x106629e1,
  0x1072a048, 0x107f1686, 0x108b8c9b, 0x10980287, 0x10a4784b, 0x10b0ede5, 0x10bd6356, 0x10c9d89e,
  0x10d64dbd, 0x10e2c2b2, 0x10ef377d, 0x10fbac1e, 0x11082096, 0x111494e4, 0x11210907, 0x112d7d00,
  0x1139f0cf, 0x11466473, 0x1152d7ed, 0x115f4b3c, 0x116bbe60, 0x11783159, 0x1184a427, 0x119116c9,
  0x119d8941, 0x11a9fb8d, 0x11b66dad, 0x11c2dfa2, 0x11cf516a, 0x11dbc307, 0x11e83478, 0x11f4a5bd,
  0x120116d5, 0x120d87c1, 0x1219f880, 0x12266913, 0x1232d979, 0x123f49b2, 0x124bb9be, 0x1258299c,
  0x1264994e, 0x127108d2, 0x127d7829, 0x1289e752, 0x1296564d, 0x12a2c51b, 0x12af33ba, 0x12bba22b,

  0x12c8106f, 0x12d47e83, 0x12e0ec6a, 0x12ed5a21, 0x12f9c7aa, 0x13063505, 0x1312a230, 0x131f0f2c,
  0x132b7bf9, 0x1337e897, 0x13445505, 0x1350c144, 0x135d2d53, 0x13699933, 0x137604e2, 0x13827062,
  0x138edbb1, 0x139b46d0, 0x13a7b1bf, 0x13b41c7d, 0x13c0870a, 0x13ccf167, 0x13d95b93, 0x13e5c58e,
  0x13f22f58, 0x13fe98f1, 0x140b0258, 0x14176b8e, 0x1423d492, 0x14303d65, 0x143ca605, 0x14490e74,
  0x145576b1, 0x1461debc, 0x146e4694, 0x147aae3a, 0x148715ae, 0x14937cee, 0x149fe3fc, 0x14ac4ad7,
  0x14b8b17f, 0x14c517f4, 0x14d17e36, 0x14dde445, 0x14ea4a1f, 0x14f6afc7, 0x1503153a, 0x150f7a7a,
  0x151bdf86, 0x1528445d, 0x1534a901, 0x15410d70, 0x154d71aa, 0x1559d5b1, 0x15663982, 0x15729d1f,
  0x157f0086, 0x158b63b9, 0x1597c6b7, 0x15a4297f, 0x15b08c12, 0x15bcee6f, 0x15c95097, 0x15d5b288,

  0x15e21445, 0x15ee75cb, 0x15fad71b, 0x16073834, 0x16139918, 0x161ff9c4, 0x162c5a3b, 0x1638ba7a,
  0x16451a83, 0x16517a55, 0x165dd9f0, 0x166a3953, 0x1676987f, 0x1682f774, 0x168f5632, 0x169bb4b7,
  0x16a81305, 0x16b4711b, 0x16c0cef9, 0x16cd2c9f, 0x16d98a0c, 0x16e5e741, 0x16f2443e, 0x16fea102,
  0x170afd8d, 0x171759df, 0x1723b5f9, 0x173011d9, 0x173c6d80, 0x1748c8ee, 0x17552422, 0x17617f1d,
  0x176dd9de, 0x177a3466, 0x17868eb3, 0x1792e8c6, 0x179f429f, 0x17ab9c3e, 0x17b7f5a3, 0x17c44ecd,
  0x17d0a7bc, 0x17dd0070, 0x17e958ea, 0x17f5b129, 0x1802092c, 0x180e60f4, 0x181ab881, 0x18270fd3,
  0x183366e9, 0x183fbdc3, 0x184c1461, 0x18586ac3, 0x1864c0ea, 0x187116d4, 0x187d6c82, 0x1889c1f3,
  0x18961728, 0x18a26c20, 0x18aec0db, 0x18bb155a, 0x18c7699b, 0x18d3bda0, 0x18e01167, 0x18ec64f0,

  0x18f8b83c, 0x19050b4b, 0x19115e1c, 0x191db0af, 0x192a0304, 0x1936551b, 0x1942a6f3, 0x194ef88e,
  0x195b49ea, 0x19679b07, 0x1973ebe6, 0x19803c86, 0x198c8ce7, 0x1998dd09, 0x19a52ceb, 0x19b17c8f,
  0x19bdcbf3, 0x19ca1b17, 0x19d669fc, 0x19e2b8a2, 0x19ef0707, 0x19fb552c, 0x1a07a311, 0x1a13f0b6,
  0x1a203e1b, 0x1a2c8b3f, 0x1a38d823, 0x1a4524c6, 0x1a517128, 0x1a5dbd49, 0x1a6a0929, 0x1a7654c8,
  0x1a82a026, 0x1a8eeb42, 0x1a9b361d, 0x1aa780b6, 0x1ab3cb0d, 0x1ac01522, 0x1acc5ef6, 0x1ad8a887,
  0x1ae4f1d6, 0x1af13ae3, 0x1afd83ad, 0x1b09cc34, 0x1b161479, 0x1b225c7b, 0x1b2ea43a, 0x1b3aebb6,
  0x1b4732ef, 0x1b5379e5, 0x1b5fc097, 0x1b6c0705, 0x1b784d30, 0x1b849317, 0x1b90d8bb, 0x1b9d1e1a,
  0x1ba96335, 0x1bb5a80c, 0x1bc1ec9e, 0x1bce30ec, 0x1bda74f6, 0x1be6b8ba, 0x1bf2fc3a, 0x1bff3f75,

  0x1c0b826a, 0x1c17c51b, 0x1c240786, 0x1c3049ac, 0x1c3c8b8c, 0x1c48cd27, 0x1c550e7c, 0x1c614f8b,
  0x1c6d9053, 0x1c79d0d6, 0x1c861113, 0x1c925109, 0x1c9e90b8, 0x1caad021, 0x1cb70f43, 0x1cc34e1f,
  0x1ccf8cb3, 0x1cdbcb00, 0x1ce80906, 0x1cf446c5, 0x1d00843d, 0x1d0cc16c, 0x1d18fe54, 0x1d253af5,
  0x1d31774d, 0x1d3db35e, 0x1d49ef26, 0x1d562aa6, 0x1d6265dd, 0x1d6ea0cc, 0x1d7adb73, 0x1d8715d0,
  0x1d934fe5, 0x1d9f89b1, 0x1dabc334, 0x1db7fc6d, 0x1dc4355e, 0x1dd06e04, 0x1ddca662, 0x1de8de75,
  0x1df5163f, 0x1e014dbf, 0x1e0d84f5, 0x1e19bbe0, 0x1e25f282, 0x1e3228d9, 0x1e3e5ee5, 0x1e4a94a7,
  0x1e56ca1e, 0x1e62ff4a, 0x1e6f342c, 0x1e7b68c2, 0x1e879d0d, 0x1e93d10c, 0x1ea004c1, 0x1eac3829,
  0x1eb86b46, 0x1ec49e17, 0x1ed0d09d, 0x1edd02d6, 0x1ee934c3, 0x1ef56664, 0x1f0197b8, 0x1f0dc8c0,

  0x1f19f97b, 0x1f2629ea, 0x1f325a0b, 0x1f3e89e0, 0x1f4ab968, 0x1f56e8a2, 0x1f63178f, 0x1f6f462f,
  0x1f7b7481, 0x1f87a285, 0x1f93d03c, 0x1f9ffda4, 0x1fac2abf, 0x1fb8578b, 0x1fc4840a, 0x1fd0b03a,
  0x1fdcdc1b, 0x1fe907ae, 0x1ff532f2, 0x20015de7, 0x200d888d, 0x2019b2e4, 0x2025dcec, 0x203206a4,
  0x203e300d, 0x204a5927, 0x205681f1, 0x2062aa6b, 0x206ed295, 0x207afa6f, 0x208721f9, 0x20934933,
  0x209f701c, 0x20ab96b5, 0x20b7bcfe, 0x20c3e2f5, 0x20d0089c, 0x20dc2df2, 0x20e852f6, 0x20f477aa,
  0x21009c0c, 0x210cc01d, 0x2118e3dc, 0x21250749, 0x21312a65, 0x213d4d2f, 0x21496fa7, 0x215591cc,
  0x2161b3a0, 0x216dd521, 0x2179f64f, 0x2186172b, 0x219237b5, 0x219e57eb, 0x21aa77cf, 0x21b6975f,
  0x21c2b69c, 0x21ced586, 0x21daf41d, 0x21e71260, 0x21f3304f, 0x21ff4dea, 0x220b6b32, 0x22178826,

  0x2223a4c5, 0x222fc111, 0x223bdd08, 0x2247f8aa, 0x225413f8, 0x22602ef1, 0x226c4996, 0x227863e5,
  0x22847de0, 0x22909785, 0x229cb0d5, 0x22a8c9cf, 0x22b4e274, 0x22c0fac4, 0x22cd12bd, 0x22d92a61,
  0x22e541af, 0x22f158a7, 0x22fd6f48, 0x23098593, 0x23159b88, 0x2321b126, 0x232dc66d, 0x2339db5e,
  0x2345eff8, 0x2352043b, 0x235e1826, 0x236a2bba, 0x23763ef7, 0x238251dd, 0x238e646a, 0x239a76a0,
  0x23a6887f, 0x23b29a05, 0x23beab33, 0x23cabc09, 0x23d6cc87, 0x23e2dcac, 0x23eeec78, 0x23fafbec,
  0x24070b08, 0x241319ca, 0x241f2833, 0x242b3644, 0x243743fa, 0x24435158, 0x244f5e5c, 0x245b6b07,
  0x24677758, 0x2473834f, 0x247f8eec, 0x248b9a2f, 0x2497a517, 0x24a3afa6, 0x24afb9da, 0x24bbc3b4,
  0x24c7cd33, 0x24d3d657, 0x24dfdf20, 0x24ebe78f, 0x24f7efa2, 0x2503f75a, 0x250ffeb7, 0x251c05b8,

  0x25280c5e, 0x253412a8, 0x25401896, 0x254c1e28, 0x2558235f, 0x25642839, 0x25702cb7, 0x257c30d8,
  0x2588349d, 0x25943806, 0x25a03b11, 0x25ac3dc0, 0x25b84012, 0x25c44207, 0x25d0439f, 0x25dc44d9,
  0x25e845b6, 0x25f44635, 0x26004657, 0x260c461b, 0x26184581, 0x26244489, 0x26304333, 0x263c417f,
  0x26483f6c, 0x26543cfb, 0x26603a2c, 0x266c36fe, 0x26783370, 0x26842f84, 0x26902b39, 0x269c268f,
  0x26a82186, 0x26b41c1d, 0x26c01655, 0x26cc102d, 0x26d809a5, 0x26e402bd, 0x26effb76, 0x26fbf3ce,
  0x2707ebc7, 0x2713e35f, 0x271fda96, 0x272bd16d, 0x2737c7e3, 0x2743bdf9, 0x274fb3ae, 0x275ba901,
  0x27679df4, 0x27739285, 0x277f86b5, 0x278b7a84, 0x27976df1, 0x27a360fc, 0x27af53a6, 0x27bb45ed,
  0x27c737d3, 0x27d32956, 0x27df1a77, 0x27eb0b36, 0x27f6fb92, 0x2802eb8c, 0x280edb23, 0x281aca57,

  0x2826b928, 0x2832a796, 0x283e95a1, 0x284a8349, 0x2856708d, 0x28625d6d, 0x286e49ea, 0x287a3604,
  0x288621b9, 0x28920d0a, 0x289df7f8, 0x28a9e281, 0x28b5cca5, 0x28c1b666, 0x28cd9fc1, 0x28d988b8,
  0x28e5714b, 0x28f15978, 0x28fd4140, 0x290928a3, 0x29150fa1, 0x2920f63a, 0x292cdc6d, 0x2938c23a,
  0x2944a7a2, 0x29508ca4, 0x295c7140, 0x29685576, 0x29743946, 0x29801caf, 0x298bffb2, 0x2997e24f,
  0x29a3c485, 0x29afa654, 0x29bb87bc, 0x29c768be, 0x29d34958, 0x29df298b, 0x29eb0957, 0x29f6e8bb,
  0x2a02c7b8, 0x2a0ea64d, 0x2a1a847b, 0x2a266240, 0x2a323f9e, 0x2a3e1c93, 0x2a49f920, 0x2a55d545,
  0x2a61b101, 0x2a6d8c55, 0x2a796740, 0x2a8541c3, 0x2a911bdc, 0x2a9cf58c, 0x2aa8ced3, 0x2ab4a7b1,
  0x2ac08026, 0x2acc5831, 0x2ad82fd2, 0x2ae4070a, 0x2aefddd8, 0x2afbb43c, 0x2b078a36, 0x2b135fc6,

  0x2b1f34eb, 0x2b2b09a6, 0x2b36ddf7, 0x2b42b1dd, 0x2b4e8558, 0x2b5a5868, 0x2b662b0e, 0x2b71fd48,
  0x2b7dcf17, 0x2b89a07b, 0x2b957173, 0x2ba14200, 0x2bad1221, 0x2bb8e1d7, 0x2bc4b120, 0x2bd07ffe,
  0x2bdc4e6f, 0x2be81c74, 0x2bf3ea0d, 0x2bffb73a, 0x2c0b83fa, 0x2c17504d, 0x2c231c33, 0x2c2ee7ad,
  0x2c3ab2b9, 0x2c467d58, 0x2c52478a, 0x2c5e114f, 0x2c69daa6, 0x2c75a390, 0x2c816c0c, 0x2c8d341a,
  0x2c98fbba, 0x2ca4c2ed, 0x2cb089b1, 0x2cbc5006, 0x2cc815ee, 0x2cd3db67, 0x2cdfa071, 0x2ceb650d,
  0x2cf72939, 0x2d02ecf7, 0x2d0eb046, 0x2d1a7325, 0x2d263596, 0x2d31f797, 0x2d3db928, 0x2d497a4a,
  0x2d553afc, 0x2d60fb3e, 0x2d6cbb10, 0x2d787a72, 0x2d843964, 0x2d8ff7e5, 0x2d9bb5f6, 0x2da77397,
  0x2db330c7, 0x2dbeed86, 0x2dcaa9d5, 0x2dd665b2, 0x2de2211e, 0x2deddc19, 0x2df996a3, 0x2e0550bb,

  0x2e110a62, 0x2e1cc397, 0x2e287c5a, 0x2e3434ac, 0x2e3fec8b, 0x2e4ba3f8, 0x2e575af3, 0x2e63117c,
  0x2e6ec792, 0x2e7a7d36, 0x2e863267, 0x2e91e725, 0x2e9d9b70, 0x2ea94f49, 0x2eb502ae, 0x2ec0b5a0,
  0x2ecc681e, 0x2ed81a29, 0x2ee3cbc1, 0x2eef7ce5, 0x2efb2d95, 0x2f06ddd1, 0x2f128d99, 0x2f1e3ced,
  0x2f29ebcc, 0x2f359a37, 0x2f41482e, 0x2f4cf5b0, 0x2f58a2be, 0x2f644f56, 0x2f6ffb7a, 0x2f7ba729,
  0x2f875262, 0x2f92fd26, 0x2f9ea775, 0x2faa514f, 0x2fb5fab2, 0x2fc1a3a0, 0x2fcd4c19, 0x2fd8f41b,
  0x2fe49ba7, 0x2ff042bd, 0x2ffbe95d, 0x30078f86, 0x30133539, 0x301eda75, 0x302a7f3a, 0x30362389,
  0x3041c761, 0x304d6ac1, 0x30590dab, 0x3064b01d, 0x30705217, 0x307bf39b, 0x308794a6, 0x3093353a,
  0x309ed556, 0x30aa74fa, 0x30b61426, 0x30c1b2da, 0x30cd5115, 0x30d8eed8, 0x30e48c22, 0x30f028f4,

  0x30fbc54d, 0x3107612e, 0x3112fc95, 0x311e9783, 0x312a31f8, 0x3135cbf4, 0x31416576, 0x314cfe7f,
  0x3158970e, 0x31642f23, 0x316fc6be, 0x317b5de0, 0x3186f487, 0x31928ab4, 0x319e2067, 0x31a9b5a0,
  0x31b54a5e, 0x31c0dea1, 0x31cc7269, 0x31d805b7, 0x31e39889, 0x31ef2ae1, 0x31fabcbd, 0x32064e1e,
  0x3211df04, 0x321d6f6e, 0x3228ff5c, 0x32348ecf, 0x32401dc6, 0x324bac40, 0x32573a3f, 0x3262c7c1,
  0x326e54c7, 0x3279e151, 0x32856d5e, 0x3290f8ef, 0x329c8402, 0x32a80e99, 0x32b398b3, 0x32bf2250,
  0x32caab6f, 0x32d63412, 0x32e1bc36, 0x32ed43de, 0x32f8cb07, 0x330451b3, 0x330fd7e1, 0x331b5d91,
  0x3326e2c3, 0x33326776, 0x333debab, 0x33496f62, 0x3354f29b, 0x33607554, 0x336bf78f, 0x3377794b,
  0x3382fa88, 0x338e7b46, 0x3399fb85, 0x33a57b44, 0x33b0fa84, 0x33bc7944, 0x33c7f785, 0x33d37546,

  0x33def287, 0x33ea6f48, 0x33f5eb89, 0x3401674a, 0x340ce28b, 0x34185d4b, 0x3423d78a, 0x342f5149,
  0x343aca87, 0x34464345, 0x3451bb81, 0x345d333c, 0x3468aa76, 0x3474212f, 0x347f9766, 0x348b0d1c,
  0x34968250, 0x34a1f702, 0x34ad6b32, 0x34b8dee1, 0x34c4520d, 0x34cfc4b7, 0x34db36df, 0x34e6a885,
  0x34f219a8, 0x34fd8a48, 0x3508fa66, 0x35146a00, 0x351fd918, 0x352b47ad, 0x3536b5be, 0x3542234c,
  0x354d9057, 0x3558fcde, 0x356468e2, 0x356fd461, 0x357b3f5d, 0x3586a9d5, 0x359213c9, 0x359d7d39,
  0x35a8e625, 0x35b44e8c, 0x35bfb66e, 0x35cb1dcc, 0x35d684a6, 0x35e1eafa, 0x35ed50c9, 0x35f8b614,
  0x36041ad9, 0x360f7f19, 0x361ae2d3, 0x36264609, 0x3631a8b8, 0x363d0ae2, 0x36486c86, 0x3653cda3,
  0x365f2e3b, 0x366a8e4d, 0x3675edd9, 0x36814cde, 0x368cab5c, 0x36980954, 0x36a366c6, 0x36aec3b0,

  0x36ba2014, 0x36c57bf0, 0x36d0d746, 0x36dc3214, 0x36e78c5b, 0x36f2e61a, 0x36fe3f52, 0x37099802,
  0x3714f02a, 0x372047ca, 0x372b9ee3, 0x3736f573, 0x37424b7b, 0x374da0fa, 0x3758f5f2, 0x37644a60,
  0x376f9e46, 0x377af1a3, 0x37864477, 0x379196c3, 0x379ce885, 0x37a839be, 0x37b38a6d, 0x37beda93,
  0x37ca2a30, 0x37d57943, 0x37e0c7cc, 0x37ec15cb, 0x37f76341, 0x3802b02c, 0x380dfc8d, 0x38194864,
  0x382493b0, 0x382fde72, 0x383b28a9, 0x38467255, 0x3851bb77, 0x385d040d, 0x38684c19, 0x38739399,
  0x387eda8e, 0x388a20f8, 0x389566d6, 0x38a0ac29, 0x38abf0ef, 0x38b7352a, 0x38c278d9, 0x38cdbbfc,
  0x38d8fe93, 0x38e4409e, 0x38ef821c, 0x38fac30e, 0x39060373, 0x3911434b, 0x391c8297, 0x3927c155,
  0x3932ff87, 0x393e3d2c, 0x39497a43, 0x3954b6cd, 0x395ff2c9, 0x396b2e38, 0x39766919, 0x3981a36d,

  0x398cdd32, 0x3998166a, 0x39a34f13, 0x39ae872f, 0x39b9bebc, 0x39c4f5ba, 0x39d02c2a, 0x39db620b,
  0x39e6975e, 0x39f1cc21, 0x39fd0056, 0x3a0833fc, 0x3a136712, 0x3a1e9999, 0x3a29cb91, 0x3a34fcf9,
  0x3a402dd2, 0x3a4b5e1b, 0x3a568dd4, 0x3a61bcfd, 0x3a6ceb96, 0x3a78199f, 0x3a834717, 0x3a8e7400,
  0x3a99a057, 0x3aa4cc1e, 0x3aaff755, 0x3abb21fb, 0x3ac64c0f, 0x3ad17593, 0x3adc9e86, 0x3ae7c6e7,
  0x3af2eeb7, 0x3afe15f6, 0x3b093ca3, 0x3b1462be, 0x3b1f8848, 0x3b2aad3f, 0x3b35d1a5, 0x3b40f579,
  0x3b4c18ba, 0x3b573b69, 0x3b625d86, 0x3b6d7f10, 0x3b78a007, 0x3b83c06c, 0x3b8ee03e, 0x3b99ff7d,
  0x3ba51e29, 0x3bb03c42, 0x3bbb59c7, 0x3bc676b9, 0x3bd19318, 0x3bdcaee3, 0x3be7ca1a, 0x3bf2e4be,
  0x3bfdfecd, 0x3c091849, 0x3c143130, 0x3c1f4983, 0x3c2a6142, 0x3c35786d, 0x3c408f03, 0x3c4ba504,

  0x3c56ba70, 0x3c61cf48, 0x3c6ce38a, 0x3c77f737, 0x3c830a50, 0x3c8e1cd3, 0x3c992ec0, 0x3ca44018,
  0x3caf50da, 0x3cba6107, 0x3cc5709e, 0x3cd07f9f, 0x3cdb8e09, 0x3ce69bde, 0x3cf1a91c, 0x3cfcb5c4,
  0x3d07c1d6, 0x3d12cd51, 0x3d1dd835, 0x3d28e282, 0x3d33ec39, 0x3d3ef559, 0x3d49fde1, 0x3d5505d2,
  0x3d600d2c, 0x3d6b13ee, 0x3d761a19, 0x3d811fac, 0x3d8c24a8, 0x3d97290b, 0x3da22cd7, 0x3dad300b,
  0x3db832a6, 0x3dc334a9, 0x3dce3614, 0x3dd936e6, 0x3de4371f, 0x3def36c0, 0x3dfa35c8, 0x3e053437,
  0x3e10320d, 0x3e1b2f4a, 0x3e262bee, 0x3e3127f9, 0x3e3c2369, 0x3e471e41, 0x3e52187f, 0x3e5d1222,
  0x3e680b2c, 0x3e73039d, 0x3e7dfb73, 0x3e88f2ae, 0x3e93e950, 0x3e9edf57, 0x3ea9d4c3, 0x3eb4c995,
  0x3ebfbdcd, 0x3ecab169, 0x3ed5a46b, 0x3ee096d1, 0x3eeb889c, 0x3ef679cc, 0x3f016a61, 0x3f0c5a5a,

  0x3f1749b8, 0x3f22387a, 0x3f2d26a0, 0x3f38142a, 0x3f430119, 0x3f4ded6b, 0x3f58d921, 0x3f63c43b,
  0x3f6eaeb8, 0x3f799899, 0x3f8481dd, 0x3f8f6a85, 0x3f9a5290, 0x3fa539fd, 0x3fb020ce, 0x3fbb0702,
  0x3fc5ec98, 0x3fd0d191, 0x3fdbb5ec, 0x3fe699aa, 0x3ff17cca, 0x3ffc5f4d, 0x40074132, 0x40122278,
  0x401d0321, 0x4027e32b, 0x4032c297, 0x403da165, 0x40487f94, 0x40535d24, 0x405e3a16, 0x40691669,
  0x4073f21d, 0x407ecd32, 0x4089a7a8, 0x4094817f, 0x409f5ab6, 0x40aa334e, 0x40b50b46, 0x40bfe29f,
  0x40cab958, 0x40d58f71, 0x40e064ea, 0x40eb39c3, 0x40f60dfb, 0x4100e194, 0x410bb48c, 0x411686e4,
  0x4121589b, 0x412c29b1, 0x4136fa27, 0x4141c9fb, 0x414c992f, 0x415767c1, 0x416235b2, 0x416d0302,
  0x4177cfb1, 0x41829bbe, 0x418d6729, 0x419831f3, 0x41a2fc1a, 0x41adc5a0, 0x41b88e84, 0x41c356c5,

  0x41ce1e65, 0x41d8e561, 0x41e3abbc, 0x41ee7174, 0x41f93689, 0x4203fafb, 0x420ebecb, 0x421981f7,
  0x42244481, 0x422f0667, 0x4239c7aa, 0x42448849, 0x424f4845, 0x425a079e, 0x4264c653, 0x426f8463,
  0x427a41d0, 0x4284fe99, 0x428fbabe, 0x429a763f, 0x42a5311b, 0x42afeb53, 0x42baa4e6, 0x42c55dd4,
  0x42d0161e, 0x42dacdc3, 0x42e584c3, 0x42f03b1e, 0x42faf0d4, 0x4305a5e5, 0x43105a50, 0x431b0e15,
  0x4325c135, 0x433073b0, 0x433b2585, 0x4345d6b3, 0x4350873c, 0x435b371f, 0x4365e65b, 0x437094f1,
  0x437b42e1, 0x4385f02a, 0x43909ccd, 0x439b48c9, 0x43a5f41e, 0x43b09ecc, 0x43bb48d4, 0x43c5f234,
  0x43d09aed, 0x43db42fe, 0x43e5ea68, 0x43f0912b, 0x43fb3746, 0x4405dcb9, 0x44108184, 0x441b25a8,
  0x4425c923, 0x44306bf6, 0x443b0e21, 0x4445afa4, 0x4450507e, 0x445af0b0, 0x44659039, 0x44702f19,

  0x447acd50, 0x44856adf, 0x449007c4, 0x449aa400, 0x44a53f93, 0x44afda7d, 0x44ba74bd, 0x44c50e53,
  0x44cfa740, 0x44da3f83, 0x44e4d71c, 0x44ef6e0b, 0x44fa0450, 0x450499eb, 0x450f2edb, 0x4519c321,
  0x452456bd, 0x452ee9ae, 0x45397bf4, 0x45440d90, 0x454e9e80, 0x45592ec6, 0x4563be60, 0x456e4d4f,
  0x4578db93, 0x4583692c, 0x458df619, 0x4598825a, 0x45a30df0, 0x45ad98da, 0x45b82318, 0x45c2acaa,
  0x45cd358f, 0x45d7bdc9, 0x45e24556, 0x45eccc37, 0x45f7526b, 0x4601d7f3, 0x460c5cce, 0x4616e0fc,
  0x4621647d, 0x462be751, 0x46366978, 0x4640eaf2, 0x464b6bbe, 0x4655ebdd, 0x46606b4e, 0x466aea12,
  0x46756828, 0x467fe590, 0x468a624a, 0x4694de56, 0x469f59b4, 0x46a9d464, 0x46b44e65, 0x46bec7b8,
  0x46c9405c, 0x46d3b852, 0x46de2f99, 0x46e8a631, 0x46f31c1a, 0x46fd9154, 0x470805df, 0x471279ba,

  0x471cece7, 0x47275f63, 0x4731d131, 0x473c424e, 0x4746b2bc, 0x4751227a, 0x475b9188, 0x4765ffe6,
  0x47706d93, 0x477ada91, 0x478546de, 0x478fb27b, 0x479a1d67, 0x47a487a2, 0x47aef12c, 0x47b95a06,
  0x47c3c22f, 0x47ce29a7, 0x47d8906d, 0x47e2f682, 0x47ed5be6, 0x47f7c099, 0x48022499, 0x480c87e8,
  0x4816ea86, 0x48214c71, 0x482badab, 0x48360e32, 0x48406e08, 0x484acd2a, 0x48552b9b, 0x485f8959,
  0x4869e665, 0x487442be, 0x487e9e64, 0x4888f957, 0x48935397, 0x489dad25, 0x48a805ff, 0x48b25e25,
  0x48bcb599, 0x48c70c59, 0x48d16265, 0x48dbb7be, 0x48e60c62, 0x48f06054, 0x48fab391, 0x4905061a,
  0x490f57ee, 0x4919a90f, 0x4923f97b, 0x492e4933, 0x49389836, 0x4942e684, 0x494d341e, 0x49578103,
  0x4961cd33, 0x496c18ae, 0x49766373, 0x4980ad84, 0x498af6df, 0x49953f84, 0x499f8774, 0x49a9ceaf,

  0x49b41533, 0x49be5b02, 0x49c8a01b, 0x49d2e47e, 0x49dd282a, 0x49e76b21, 0x49f1ad61, 0x49fbeeea,
  0x4a062fbd, 0x4a106fda, 0x4a1aaf3f, 0x4a24edee, 0x4a2f2be6, 0x4a396926, 0x4a43a5b0, 0x4a4de182,
  0x4a581c9e, 0x4a625701, 0x4a6c90ad, 0x4a76c9a2, 0x4a8101de, 0x4a8b3963, 0x4a957030, 0x4a9fa645,
  0x4aa9dba2, 0x4ab41046, 0x4abe4433, 0x4ac87767, 0x4ad2a9e2, 0x4adcdba5, 0x4ae70caf, 0x4af13d00,
  0x4afb6c98, 0x4b059b77, 0x4b0fc99d, 0x4b19f70a, 0x4b2423be, 0x4b2e4fb8, 0x4b387af9, 0x4b42a580,
  0x4b4ccf4d, 0x4b56f861, 0x4b6120bb, 0x4b6b485a, 0x4b756f40, 0x4b7f956b, 0x4b89badd, 0x4b93df93,
  0x4b9e0390, 0x4ba826d1, 0x4bb24958, 0x4bbc6b25, 0x4bc68c36, 0x4bd0ac8d, 0x4bdacc28, 0x4be4eb08,
  0x4bef092d, 0x4bf92697, 0x4c034345, 0x4c0d5f37, 0x4c177a6e, 0x4c2194e9, 0x4c2baea9, 0x4c35c7ac,

  0x4c3fdff4, 0x4c49f77f, 0x4c540e4e, 0x4c5e2460, 0x4c6839b7, 0x4c724e50, 0x4c7c622d, 0x4c86754e,
  0x4c9087b1, 0x4c9a9958, 0x4ca4aa41, 0x4caeba6e, 0x4cb8c9dd, 0x4cc2d88f, 0x4ccce684, 0x4cd6f3bb,
  0x4ce10034, 0x4ceb0bf0, 0x4cf516ee, 0x4cff212e, 0x4d092ab0, 0x4d133374, 0x4d1d3b7a, 0x4d2742c2,
  0x4d31494b, 0x4d3b4f16, 0x4d455422, 0x4d4f5870, 0x4d595bfe, 0x4d635ece, 0x4d6d60df, 0x4d776231,
  0x4d8162c4, 0x4d8b6298, 0x4d9561ac, 0x4d9f6001, 0x4da95d96, 0x4db35a6c, 0x4dbd5682, 0x4dc751d8,
  0x4dd14c6e, 0x4ddb4644, 0x4de53f5a, 0x4def37b0, 0x4df92f46, 0x4e03261b, 0x4e0d1c30, 0x4e171184,
  0x4e210617, 0x4e2af9ea, 0x4e34ecfc, 0x4e3edf4d, 0x4e48d0dd, 0x4e52c1ab, 0x4e5cb1b9, 0x4e66a105,
  0x4e708f8f, 0x4e7a7d58, 0x4e846a60, 0x4e8e56a5, 0x4e984229, 0x4ea22ceb, 0x4eac16eb, 0x4eb60029,

  0x4ebfe8a5, 0x4ec9d05e, 0x4ed3b755, 0x4edd9d89, 0x4ee782fb, 0x4ef167aa, 0x4efb4b96, 0x4f052ec0,
  0x4f0f1126, 0x4f18f2c9, 0x4f22d3aa, 0x4f2cb3c7, 0x4f369320, 0x4f4071b6, 0x4f4a4f89, 0x4f542c98,
  0x4f5e08e3, 0x4f67e46a, 0x4f71bf2e, 0x4f7b992d, 0x4f857269, 0x4f8f4ae0, 0x4f992293, 0x4fa2f981,
  0x4faccfab, 0x4fb6a510, 0x4fc079b1, 0x4fca4d8d, 0x4fd420a4, 0x4fddf2f6, 0x4fe7c483, 0x4ff1954b,
  0x4ffb654d, 0x5005348a, 0x500f0302, 0x5018d0b4, 0x50229da1, 0x502c69c8, 0x50363529, 0x503fffc4,
  0x5049c999, 0x505392a8, 0x505d5af1, 0x50672273, 0x5070e92f, 0x507aaf25, 0x50847454, 0x508e38bd,
  0x5097fc5e, 0x50a1bf39, 0x50ab814d, 0x50b5429a, 0x50bf031f, 0x50c8c2de, 0x50d281d5, 0x50dc4005,
  0x50e5fd6d, 0x50efba0d, 0x50f975e6, 0x510330f7, 0x510ceb40, 0x5116a4c1, 0x51205d7b, 0x512a156b,

  0x5133cc94, 0x513d82f4, 0x5147388c, 0x5150ed5c, 0x515aa162, 0x516454a0, 0x516e0715, 0x5177b8c2,
  0x518169a5, 0x518b19bf, 0x5194c910, 0x519e7797, 0x51a82555, 0x51b1d24a, 0x51bb7e75, 0x51c529d7,
  0x51ced46e, 0x51d87e3c, 0x51e22740, 0x51ebcf7a, 0x51f576ea, 0x51ff1d8f, 0x5208c36a, 0x5212687b,
  0x521c0cc2, 0x5225b03d, 0x522f52ee, 0x5238f4d4, 0x524295f0, 0x524c3640, 0x5255d5c5, 0x525f7480,
  0x5269126e, 0x5272af92, 0x527c4bea, 0x5285e777, 0x528f8238, 0x52991c2d, 0x52a2b556, 0x52ac4db4,
  0x52b5e546, 0x52bf7c0b, 0x52c91204, 0x52d2a732, 0x52dc3b92, 0x52e5cf27, 0x52ef61ee, 0x52f8f3e9,
  0x53028518, 0x530c1579, 0x5315a50e, 0x531f33d5, 0x5328c1d0, 0x53324efd, 0x533bdb5d, 0x534566f0,
  0x534ef1b5, 0x53587bad, 0x536204d7, 0x536b8d33, 0x537514c2, 0x537e9b82, 0x53882175, 0x5391a699,

  0x539b2af0, 0x53a4ae77, 0x53ae3131, 0x53b7b31c, 0x53c13439, 0x53cab486, 0x53d43406, 0x53ddb2b6,
  0x53e73097, 0x53f0adaa, 0x53fa29ed, 0x5403a561, 0x540d2005, 0x541699db, 0x542012e1, 0x54298b17,
  0x5433027d, 0x543c7914, 0x5445eedb, 0x544f63d2, 0x5458d7f9, 0x54624b50, 0x546bbdd7, 0x54752f8d,
  0x547ea073, 0x54881089, 0x54917fce, 0x549aee42, 0x54a45be6, 0x54adc8b8, 0x54b734ba, 0x54c09feb,
  0x54ca0a4b, 0x54d373d9, 0x54dcdc96, 0x54e64482, 0x54efab9c, 0x54f911e5, 0x5502775c, 0x550bdc01,
  0x55153fd4, 0x551ea2d6, 0x55280505, 0x55316663, 0x553ac6ee, 0x554426a7, 0x554d858d, 0x5556e3a1,
  0x556040e2, 0x55699d51, 0x5572f8ed, 0x557c53b6, 0x5585adad, 0x558f06d0, 0x55985f20, 0x55a1b69d,
  0x55ab0d46, 0x55b4631d, 0x55bdb81f, 0x55c70c4f, 0x55d05faa, 0x55d9b232, 0x55e303e6, 0x55ec54c6,

  0x55f5a4d2, 0x55fef40a, 0x5608426e, 0x56118ffe, 0x561adcb9, 0x5624289f, 0x562d73b2, 0x5636bdef,
  0x56400758, 0x56494fec, 0x565297ab, 0x565bde95, 0x566524aa, 0x566e69ea, 0x5677ae54, 0x5680f1ea,
  0x568a34a9, 0x56937694, 0x569cb7a8, 0x56a5f7e7, 0x56af3750, 0x56b875e4, 0x56c1b3a1, 0x56caf088,
  0x56d42c99, 0x56dd67d4, 0x56e6a239, 0x56efdbc7, 0x56f9147e, 0x57024c5f, 0x570b8369, 0x5714b99d,
  0x571deefa, 0x5727237f, 0x5730572e, 0x57398a05, 0x5742bc06, 0x574bed2f, 0x57551d80, 0x575e4cfa,
  0x57677b9d, 0x5770a968, 0x5779d65b, 0x57830276, 0x578c2dba, 0x57955825, 0x579e81b8, 0x57a7aa73,
  0x57b0d256, 0x57b9f960, 0x57c31f92, 0x57cc44ec, 0x57d5696d, 0x57de8d15, 0x57e7afe4, 0x57f0d1da,
  0x57f9f2f8, 0x5803133c, 0x580c32a7, 0x58155139, 0x581e6ef1, 0x58278bd1, 0x5830a7d6, 0x5839c302,

  0x5842dd54, 0x584bf6cd, 0x58550f6c, 0x585e2730, 0x58673e1b, 0x5870542c, 0x58796962, 0x58827dbe,
  0x588b9140, 0x5894a3e7, 0x589db5b3, 0x58a6c6a5, 0x58afd6bd, 0x58b8e5f9, 0x58c1f45b, 0x58cb01e1,
  0x58d40e8c, 0x58dd1a5d, 0x58e62552, 0x58ef2f6b, 0x58f838a9, 0x5901410c, 0x590a4893, 0x59134f3e,
  0x591c550e, 0x59255a02, 0x592e5e19, 0x59376155, 0x594063b5, 0x59496538, 0x595265df, 0x595b65aa,
  0x59646498, 0x596d62a9, 0x59765fde, 0x597f5c36, 0x598857b2, 0x59915250, 0x599a4c12, 0x59a344f6,
  0x59ac3cfd, 0x59b53427, 0x59be2a74, 0x59c71fe3, 0x59d01475, 0x59d90829, 0x59e1faff, 0x59eaecf8,
  0x59f3de12, 0x59fcce4f, 0x5a05bdae, 0x5a0eac2e, 0x5a1799d1, 0x5a208695, 0x5a29727b, 0x5a325d82,
  0x5a3b47ab, 0x5a4430f5, 0x5a4d1960, 0x5a5600ec, 0x5a5ee79a, 0x5a67cd69, 0x5a70b258, 0x5a799669,

  0x5a82799a, 0x5a8b5bec, 0x5a943d5e, 0x5a9d1df1, 0x5aa5fda5, 0x5aaedc78, 0x5ab7ba6c, 0x5ac09781,
  0x5ac973b5, 0x5ad24f09, 0x5adb297d, 0x5ae40311, 0x5aecdbc5, 0x5af5b398, 0x5afe8a8b, 0x5b07609d,
  0x5b1035cf, 0x5b190a20, 0x5b21dd90, 0x5b2ab020, 0x5b3381ce, 0x5b3c529c, 0x5b452288, 0x5b4df193,
  0x5b56bfbd, 0x5b5f8d06, 0x5b68596d, 0x5b7124f2, 0x5b79ef96, 0x5b82b958, 0x5b8b8239, 0x5b944a37,
  0x5b9d1154, 0x5ba5d78e, 0x5bae9ce7, 0x5bb7615d, 0x5bc024f0, 0x5bc8e7a2, 0x5bd1a971, 0x5bda6a5d,
  0x5be32a67, 0x5bebe98e, 0x5bf4a7d2, 0x5bfd6534, 0x5c0621b2, 0x5c0edd4e, 0x5c179806, 0x5c2051db,
  0x5c290acc, 0x5c31c2db, 0x5c3a7a05, 0x5c43304d, 0x5c4be5b0, 0x5c549a30, 0x5c5d4dcc, 0x5c660084,
  0x5c6eb258, 0x5c776348, 0x5c801354, 0x5c88c27c, 0x5c9170bf, 0x5c9a1e1e, 0x5ca2ca99, 0x5cab762f,

  0x5cb420e0, 0x5cbccaac, 0x5cc57394, 0x5cce1b97, 0x5cd6c2b5, 0x5cdf68ed, 0x5ce80e41, 0x5cf0b2af,
  0x5cf95638, 0x5d01f8dc, 0x5d0a9a9a, 0x5d133b72, 0x5d1bdb65, 0x5d247a72, 0x5d2d189a, 0x5d35b5db,
  0x5d3e5237, 0x5d46edac, 0x5d4f883b, 0x5d5821e4, 0x5d60baa7, 0x5d695283, 0x5d71e979, 0x5d7a7f88,
  0x5d8314b1, 0x5d8ba8f3, 0x5d943c4e, 0x5d9ccec2, 0x5da5604f, 0x5dadf0f5, 0x5db680b4, 0x5dbf0f8c,
  0x5dc79d7c, 0x5dd02a85, 0x5dd8b6a7, 0x5de141e1, 0x5de9cc33, 0x5df2559e, 0x5dfade20, 0x5e0365bb,
  0x5e0bec6e, 0x5e147239, 0x5e1cf71c, 0x5e257b17, 0x5e2dfe29, 0x5e368053, 0x5e3f0194, 0x5e4781ed,
  0x5e50015d, 0x5e587fe5, 0x5e60fd84, 0x5e697a39, 0x5e71f606, 0x5e7a70ea, 0x5e82eae5, 0x5e8b63f7,
  0x5e93dc1f, 0x5e9c535e, 0x5ea4c9b3, 0x5ead3f1f, 0x5eb5b3a2, 0x5ebe273b, 0x5ec699e9, 0x5ecf0baf,

  0x5ed77c8a, 0x5edfec7b, 0x5ee85b82, 0x5ef0c99f, 0x5ef936d1, 0x5f01a31a, 0x5f0a0e77, 0x5f1278eb,
  0x5f1ae274, 0x5f234b12, 0x5f2bb2c5, 0x5f34198e, 0x5f3c7f6b, 0x5f44e45e, 0x5f4d4865, 0x5f55ab82,
  0x5f5e0db3, 0x5f666ef9, 0x5f6ecf53, 0x5f772ec2, 0x5f7f8d46, 0x5f87eade, 0x5f90478a, 0x5f98a34a,
  0x5fa0fe1f, 0x5fa95807, 0x5fb1b104, 0x5fba0914, 0x5fc26038, 0x5fcab670, 0x5fd30bbc, 0x5fdb601b,
  0x5fe3b38d, 0x5fec0613, 0x5ff457ad, 0x5ffca859, 0x6004f819, 0x600d46ec, 0x601594d1, 0x601de1ca,
  0x60262dd6, 0x602e78f4, 0x6036c325, 0x603f0c69, 0x604754bf, 0x604f9c27, 0x6057e2a2, 0x6060282f,
  0x60686ccf, 0x6070b080, 0x6078f344, 0x60813519, 0x60897601, 0x6091b5fa, 0x6099f505, 0x60a23322,
  0x60aa7050, 0x60b2ac8f, 0x60bae7e1, 0x60c32243, 0x60cb5bb7, 0x60d3943b, 0x60dbcbd1, 0x60e40278,

  0x60ec3830, 0x60f46cf9, 0x60fca0d2, 0x6104d3bc, 0x610d05b7, 0x611536c2, 0x611d66de, 0x6125960a,
  0x612dc447, 0x6135f193, 0x613e1df0, 0x6146495d, 0x614e73da, 0x61569d67, 0x615ec603, 0x6166edb0,
  0x616f146c, 0x61773a37, 0x617f5f12, 0x618782fd, 0x618fa5f7, 0x6197c800, 0x619fe918, 0x61a80940,
  0x61b02876, 0x61b846bc, 0x61c06410, 0x61c88074, 0x61d09be5, 0x61d8b666, 0x61e0cff5, 0x61e8e893,
  0x61f1003f, 0x61f916f9, 0x62012cc2, 0x62094199, 0x6211557e, 0x62196871, 0x62217a72, 0x62298b81,
  0x62319b9d, 0x6239aac7, 0x6241b8ff, 0x6249c645, 0x6251d298, 0x6259ddf8, 0x6261e866, 0x6269f1e1,
  0x6271fa69, 0x627a01fe, 0x628208a1, 0x628a0e50, 0x6292130c, 0x629a16d5, 0x62a219aa, 0x62aa1b8d,
  0x62b21c7b, 0x62ba1c77, 0x62c21b7e, 0x62ca1992, 0x62d216b3, 0x62da12df, 0x62e20e17, 0x62ea085c,

  0x62f201ac, 0x62f9fa09, 0x6301f171, 0x6309e7e4, 0x6311dd64, 0x6319d1ef, 0x6321c585, 0x6329b827,
  0x6331a9d4, 0x63399a8d, 0x63418a50, 0x6349791f, 0x635166f9, 0x635953dd, 0x63613fcd, 0x63692ac7,
  0x637114cc, 0x6378fddc, 0x6380e5f6, 0x6388cd1b, 0x6390b34a, 0x63989884, 0x63a07cc7, 0x63a86015,
  0x63b0426d, 0x63b823cf, 0x63c0043b, 0x63c7e3b1, 0x63cfc231, 0x63d79fba, 0x63df7c4d, 0x63e757ea,
  0x63ef3290, 0x63f70c3f, 0x63fee4f8, 0x6406bcba, 0x640e9386, 0x6416695a, 0x641e3e38, 0x6426121e,
  0x642de50d, 0x6435b706, 0x643d8806, 0x64455810, 0x644d2722, 0x6454f53d, 0x645cc260, 0x64648e8c,
  0x646c59bf, 0x647423fb, 0x647bed3f, 0x6483b58c, 0x648b7ce0, 0x6493433c, 0x649b08a0, 0x64a2cd0c,
  0x64aa907f, 0x64b252fa, 0x64ba147d, 0x64c1d507, 0x64c99498, 0x64d15331, 0x64d910d1, 0x64e0cd78,

  0x64e88926, 0x64f043dc, 0x64f7fd98, 0x64ffb65b, 0x65076e25, 0x650f24f5, 0x6516dacd, 0x651e8faa,
  0x6526438f, 0x652df679, 0x6535a86b, 0x653d5962, 0x6545095f, 0x654cb863, 0x6554666d, 0x655c137d,
  0x6563bf92, 0x656b6aae, 0x657314cf, 0x657abdf6, 0x65826622, 0x658a0d54, 0x6591b38c, 0x659958c9,
  0x65a0fd0b, 0x65a8a052, 0x65b0429f, 0x65b7e3f1, 0x65bf8447, 0x65c723a3, 0x65cec204, 0x65d65f69,
  0x65ddfbd3, 0x65e59742, 0x65ed31b5, 0x65f4cb2d, 0x65fc63a9, 0x6603fb2a, 0x660b91af, 0x66132738,
  0x661abbc5, 0x66224f56, 0x6629e1ec, 0x66317385, 0x66390422, 0x664093c3, 0x66482267, 0x664fb010,
  0x66573cbb, 0x665ec86b, 0x6666531d, 0x666ddcd3, 0x6675658c, 0x667ced49, 0x66847408, 0x668bf9cb,
  0x66937e91, 0x669b0259, 0x66a28524, 0x66aa06f3, 0x66b187c3, 0x66b90797, 0x66c0866d, 0x66c80445,

  0x66cf8120, 0x66d6fcfd, 0x66de77dc, 0x66e5f1be, 0x66ed6aa1, 0x66f4e287, 0x66fc596f, 0x6703cf58,
  0x670b4444, 0x6712b831, 0x671a2b20, 0x67219d10, 0x67290e02, 0x67307df5, 0x6737ecea, 0x673f5ae0,
  0x6746c7d8, 0x674e33d0, 0x67559eca, 0x675d08c4, 0x676471c0, 0x676bd9bd, 0x677340ba, 0x677aa6b8,
  0x67820bb7, 0x67896fb6, 0x6790d2b6, 0x679834b6, 0x679f95b7, 0x67a6f5b8, 0x67ae54ba, 0x67b5b2bb,
  0x67bd0fbd, 0x67c46bbe, 0x67cbc6c0, 0x67d320c1, 0x67da79c3, 0x67e1d1c4, 0x67e928c5, 0x67f07ec5,
  0x67f7d3c5, 0x67ff27c4, 0x68067ac3, 0x680dccc1, 0x68151dbe, 0x681c6dbb, 0x6823bcb7, 0x682b0ab1,
  0x683257ab, 0x6839a3a4, 0x6840ee9b, 0x68483891, 0x684f8186, 0x6856c979, 0x685e106c, 0x6865565c,
  0x686c9b4b, 0x6873df38, 0x687b2224, 0x6882640e, 0x6889a4f6, 0x6890e4dc, 0x689823bf, 0x689f61a1,

  0x68a69e81, 0x68adda5f, 0x68b5153a, 0x68bc4f13, 0x68c387e9, 0x68cabfbd, 0x68d1f68f, 0x68d92c5d,
  0x68e06129, 0x68e794f3, 0x68eec7b9, 0x68f5f97d, 0x68fd2a3d, 0x690459fb, 0x690b88b5, 0x6912b66c,
  0x6919e320, 0x69210ed1, 0x6928397e, 0x692f6328, 0x69368bce, 0x693db371, 0x6944da10, 0x694bffab,
  0x69532442, 0x695a47d6, 0x69616a65, 0x69688bf1, 0x696fac78, 0x6976cbfc, 0x697dea7b, 0x698507f6,
  0x698c246c, 0x69933fde, 0x699a5a4c, 0x69a173b5, 0x69a88c19, 0x69afa378, 0x69b6b9d3, 0x69bdcf29,
  0x69c4e37a, 0x69cbf6c7, 0x69d3090e, 0x69da1a50, 0x69e12a8c, 0x69e839c4, 0x69ef47f6, 0x69f65523,
  0x69fd614a, 0x6a046c6c, 0x6a0b7689, 0x6a127f9f, 0x6a1987b0, 0x6a208ebb, 0x6a2794c1, 0x6a2e99c0,
  0x6a359db9, 0x6a3ca0ad, 0x6a43a29a, 0x6a4aa381, 0x6a51a361, 0x6a58a23c, 0x6a5fa010, 0x6a669cdd,

  0x6a6d98a4, 0x6a749365, 0x6a7b8d1e, 0x6a8285d1, 0x6a897d7d, 0x6a907423, 0x6a9769c1, 0x6a9e5e58,
  0x6aa551e9, 0x6aac4472, 0x6ab335f4, 0x6aba266e, 0x6ac115e2, 0x6ac8044e, 0x6acef1b2, 0x6ad5de0f,
  0x6adcc964, 0x6ae3b3b2, 0x6aea9cf8, 0x6af18536, 0x6af86c6c, 0x6aff529a, 0x6b0637c1, 0x6b0d1bdf,
  0x6b13fef5, 0x6b1ae103, 0x6b21c208, 0x6b28a206, 0x6b2f80fb, 0x6b365ee7, 0x6b3d3bcb, 0x6b4417a6,
  0x6b4af279, 0x6b51cc42, 0x6b58a503, 0x6b5f7cbc, 0x6b66536b, 0x6b6d2911, 0x6b73fdae, 0x6b7ad142,
  0x6b81a3cd, 0x6b88754f, 0x6b8f45c7, 0x6b961536, 0x6b9ce39b, 0x6ba3b0f7, 0x6baa7d49, 0x6bb14892,
  0x6bb812d1, 0x6bbedc06, 0x6bc5a431, 0x6bcc6b53, 0x6bd3316a, 0x6bd9f677, 0x6be0ba7b, 0x6be77d74,
  0x6bee3f62, 0x6bf50047, 0x6bfbc021, 0x6c027ef1, 0x6c093cb6, 0x6c0ff971, 0x6c16b521, 0x6c1d6fc6,

  0x6c242960, 0x6c2ae1f0, 0x6c319975, 0x6c384fef, 0x6c3f055d, 0x6c45b9c1, 0x6c4c6d1a, 0x6c531f67,
  0x6c59d0a9, 0x6c6080e0, 0x6c67300b, 0x6c6dde2b, 0x6c748b3f, 0x6c7b3748, 0x6c81e245, 0x6c888c36,
  0x6c8f351c, 0x6c95dcf6, 0x6c9c83c3, 0x6ca32985, 0x6ca9ce3b, 0x6cb071e4, 0x6cb71482, 0x6cbdb613,
  0x6cc45698, 0x6ccaf610, 0x6cd1947c, 0x6cd831dc, 0x6cdece2f, 0x6ce56975, 0x6cec03af, 0x6cf29cdc,
  0x6cf934fc, 0x6cffcc0f, 0x6d066215, 0x6d0cf70f, 0x6d138afb, 0x6d1a1dda, 0x6d20afac, 0x6d274070,
  0x6d2dd027, 0x6d345ed1, 0x6d3aec6e, 0x6d4178fd, 0x6d48047e, 0x6d4e8ef2, 0x6d551858, 0x6d5ba0b0,
  0x6d6227fa, 0x6d68ae37, 0x6d6f3365, 0x6d75b786, 0x6d7c3a98, 0x6d82bc9d, 0x6d893d93, 0x6d8fbd7a,
  0x6d963c54, 0x6d9cba1f, 0x6da336dc, 0x6da9b28a, 0x6db02d29, 0x6db6a6ba, 0x6dbd1f3c, 0x6dc396b0,

  0x6dca0d14, 0x6dd0826a, 0x6dd6f6b1, 0x6ddd69e9, 0x6de3dc11, 0x6dea4d2b, 0x6df0bd35, 0x6df72c30,
  0x6dfd9a1c, 0x6e0406f8, 0x6e0a72c5, 0x6e10dd82, 0x6e174730, 0x6e1dafce, 0x6e24175c, 0x6e2a7ddb,
  0x6e30e34a, 0x6e3747a9, 0x6e3daaf8, 0x6e440d37, 0x6e4a6e66, 0x6e50ce84, 0x6e572d93, 0x6e5d8b91,
  0x6e63e87f, 0x6e6a445d, 0x6e709f2a, 0x6e76f8e7, 0x6e7d5193, 0x6e83a92f, 0x6e89ffb9, 0x6e905534,
  0x6e96a99d, 0x6e9cfcf5, 0x6ea34f3d, 0x6ea9a073, 0x6eaff099, 0x6eb63fad, 0x6ebc8db0, 0x6ec2daa2,
  0x6ec92683, 0x6ecf7152, 0x6ed5bb10, 0x6edc03bc, 0x6ee24b57, 0x6ee891e1, 0x6eeed758, 0x6ef51bbe,
  0x6efb5f12, 0x6f01a155, 0x6f07e285, 0x6f0e22a3, 0x6f1461b0, 0x6f1a9faa, 0x6f20dc92, 0x6f271868,
  0x6f2d532c, 0x6f338cde, 0x6f39c57d, 0x6f3ffd09, 0x6f463383, 0x6f4c68eb, 0x6f529d40, 0x6f58d082,

  0x6f5f02b2, 0x6f6533ce, 0x6f6b63d8, 0x6f7192cf, 0x6f77c0b3, 0x6f7ded84, 0x6f841942, 0x6f8a43ed,
  0x6f906d84, 0x6f969608, 0x6f9cbd79, 0x6fa2e3d7, 0x6fa90921, 0x6faf2d57, 0x6fb5507a, 0x6fbb728a,
  0x6fc19385, 0x6fc7b36d, 0x6fcdd241, 0x6fd3f001, 0x6fda0cae, 0x6fe02846, 0x6fe642ca, 0x6fec5c3b,
  0x6ff27497, 0x6ff88bde, 0x6ffea212, 0x7004b731, 0x700acb3c, 0x7010de32, 0x7016f014, 0x701d00e1,
  0x7023109a, 0x70291f3e, 0x702f2ccd, 0x70353947, 0x703b44ad, 0x70414efd, 0x70475839, 0x704d6060,
  0x70536771, 0x70596d6d, 0x705f7255, 0x70657626, 0x706b78e3, 0x70717a8a, 0x70777b1c, 0x707d7a98,
  0x708378ff, 0x70897650, 0x708f728b, 0x70956db1, 0x709b67c0, 0x70a160ba, 0x70a7589f, 0x70ad4f6d,
  0x70b34525, 0x70b939c7, 0x70bf2d53, 0x70c51fc8, 0x70cb1128, 0x70d10171, 0x70d6f0a4, 0x70dcdec0,

  0x70e2cbc6, 0x70e8b7b5, 0x70eea28e, 0x70f48c50, 0x70fa74fc, 0x71005c90, 0x7106430e, 0x710c2875,
  0x71120cc5, 0x7117effe, 0x711dd220, 0x7123b32b, 0x7129931f, 0x712f71fb, 0x71354fc0, 0x713b2c6e,
  0x71410805, 0x7146e284, 0x714cbbeb, 0x7152943b, 0x71586b74, 0x715e4194, 0x7164169d, 0x7169ea8f,
  0x716fbd68, 0x71758f29, 0x717b5fd3, 0x71812f65, 0x7186fdde, 0x718ccb3f, 0x71929789, 0x719862b9,
  0x719e2cd2, 0x71a3f5d2, 0x71a9bdba, 0x71af848a, 0x71b54a41, 0x71bb0edf, 0x71c0d265, 0x71c694d2,
  0x71cc5626, 0x71d21662, 0x71d7d585, 0x71dd938f, 0x71e35080, 0x71e90c57, 0x71eec716, 0x71f480bc,
  0x71fa3949, 0x71fff0bc, 0x7205a716, 0x720b5c57, 0x7211107e, 0x7216c38c, 0x721c7580, 0x7222265b,
  0x7227d61c, 0x722d84c4, 0x72333251, 0x7238dec5, 0x723e8a20, 0x72443460, 0x7249dd86, 0x724f8593,

  0x72552c85, 0x725ad25d, 0x7260771b, 0x72661abf, 0x726bbd48, 0x72715eb8, 0x7276ff0d, 0x727c9e47,
  0x72823c67, 0x7287d96c, 0x728d7557, 0x72931027, 0x7298a9dd, 0x729e4277, 0x72a3d9f7, 0x72a9705c,
  0x72af05a7, 0x72b499d6, 0x72ba2cea, 0x72bfbee3, 0x72c54fc1, 0x72cadf83, 0x72d06e2b, 0x72d5fbb7,
  0x72db8828, 0x72e1137d, 0x72e69db7, 0x72ec26d6, 0x72f1aed9, 0x72f735c0, 0x72fcbb8c, 0x7302403c,
  0x7307c3d0, 0x730d4648, 0x7312c7a5, 0x731847e5, 0x731dc70a, 0x73234512, 0x7328c1ff, 0x732e3dcf,
  0x7333b883, 0x7339321b, 0x733eaa96, 0x734421f6, 0x73499838, 0x734f0d5f, 0x73548168, 0x7359f456,
  0x735f6626, 0x7364d6da, 0x736a4671, 0x736fb4ec, 0x73752249, 0x737a8e8a, 0x737ff9ae, 0x738563b5,
  0x738acc9e, 0x7390346b, 0x73959b1b, 0x739b00ad, 0x73a06522, 0x73a5c87a, 0x73ab2ab4, 0x73b08bd1,

  0x73b5ebd1, 0x73bb4ab3, 0x73c0a878, 0x73c6051f, 0x73cb60a8, 0x73d0bb13, 0x73d61461, 0x73db6c91,
  0x73e0c3a3, 0x73e61997, 0x73eb6e6e, 0x73f0c226, 0x73f614c0, 0x73fb663c, 0x7400b69a, 0x740605d9,
  0x740b53fb, 0x7410a0fe, 0x7415ece2, 0x741b37a9, 0x74208150, 0x7425c9da, 0x742b1144, 0x74305790,
  0x74359cbd, 0x743ae0cc, 0x744023bc, 0x7445658d, 0x744aa63f, 0x744fe5d2, 0x74552446, 0x745a619b,
  0x745f9dd1, 0x7464d8e8, 0x746a12df, 0x746f4bb8, 0x74748371, 0x7479ba0b, 0x747eef85, 0x748423e0,
  0x7489571c, 0x748e8938, 0x7493ba34, 0x7498ea11, 0x749e18cd, 0x74a3466b, 0x74a872e8, 0x74ad9e46,
  0x74b2c884, 0x74b7f1a1, 0x74bd199f, 0x74c2407d, 0x74c7663a, 0x74cc8ad8, 0x74d1ae55, 0x74d6d0b2,
  0x74dbf1ef, 0x74e1120c, 0x74e63108, 0x74eb4ee3, 0x74f06b9e, 0x74f58739, 0x74faa1b3, 0x74ffbb0d,

  0x7504d345, 0x7509ea5d, 0x750f0054, 0x7514152b, 0x751928e0, 0x751e3b75, 0x75234ce8, 0x75285d3b,
  0x752d6c6c, 0x75327a7d, 0x7537876c, 0x753c933a, 0x75419de7, 0x7546a772, 0x754bafdc, 0x7550b725,
  0x7555bd4c, 0x755ac251, 0x755fc635, 0x7564c8f8, 0x7569ca99, 0x756ecb18, 0x7573ca75, 0x7578c8b0,
  0x757dc5ca, 0x7582c1c2, 0x7587bc98, 0x758cb64c, 0x7591aedd, 0x7596a64d, 0x759b9c9b, 0x75a091c6,
  0x75a585cf, 0x75aa78b6, 0x75af6a7b, 0x75b45b1d, 0x75b94a9c, 0x75be38fa, 0x75c32634, 0x75c8124d,
  0x75ccfd42, 0x75d1e715, 0x75d6cfc5, 0x75dbb753, 0x75e09dbd, 0x75e58305, 0x75ea672a, 0x75ef4a2c,
  0x75f42c0b, 0x75f90cc7, 0x75fdec60, 0x7602cad5, 0x7607a828, 0x760c8457, 0x76115f63, 0x7616394c,
  0x761b1211, 0x761fe9b3, 0x7624c031, 0x7629958c, 0x762e69c4, 0x76333cd8, 0x76380ec8, 0x763cdf94,

  0x7641af3d, 0x76467dc2, 0x764b4b23, 0x76501760, 0x7654e279, 0x7659ac6f, 0x765e7540, 0x76633ced,
  0x76680376, 0x766cc8db, 0x76718d1c, 0x76765038, 0x767b1231, 0x767fd304, 0x768492b4, 0x7689513f,
  0x768e0ea6, 0x7692cae8, 0x76978605, 0x769c3ffe, 0x76a0f8d2, 0x76a5b082, 0x76aa670d, 0x76af1c72,
  0x76b3d0b4, 0x76b883d0, 0x76bd35c7, 0x76c1e699, 0x76c69647, 0x76cb44cf, 0x76cff232, 0x76d49e70,
  0x76d94989, 0x76ddf37c, 0x76e29c4b, 0x76e743f4, 0x76ebea77, 0x76f08fd5, 0x76f5340e, 0x76f9d721,
  0x76fe790e, 0x770319d6, 0x7707b979, 0x770c57f5, 0x7710f54c, 0x7715917d, 0x771a2c88, 0x771ec66e,
  0x77235f2d, 0x7727f6c6, 0x772c8d3a, 0x77312287, 0x7735b6af, 0x773a49b0, 0x773edb8b, 0x77436c40,
  0x7747fbce, 0x774c8a36, 0x77511778, 0x7755a394, 0x775a2e89, 0x775eb857, 0x776340ff, 0x7767c880,

  0x776c4edb, 0x7770d40f, 0x7775581d, 0x7779db03, 0x777e5cc3, 0x7782dd5c, 0x77875cce, 0x778bdb19,
  0x7790583e, 0x7794d43b, 0x77994f11, 0x779dc8c0, 0x77a24148, 0x77a6b8a9, 0x77ab2ee2, 0x77afa3f5,
  0x77b417df, 0x77b88aa3, 0x77bcfc3f, 0x77c16cb4, 0x77c5dc01, 0x77ca4a27, 0x77ceb725, 0x77d322fc,
  0x77d78daa, 0x77dbf732, 0x77e05f91, 0x77e4c6c9, 0x77e92cd9, 0x77ed91c0, 0x77f1f581, 0x77f65819,
  0x77fab989, 0x77ff19d1, 0x780378f1, 0x7807d6e9, 0x780c33b8, 0x78108f60, 0x7814e9df, 0x78194336,
  0x781d9b65, 0x7821f26b, 0x78264849, 0x782a9cfe, 0x782ef08b, 0x783342ef, 0x7837942b, 0x783be43e,
  0x78403329, 0x784480ea, 0x7848cd83, 0x784d18f4, 0x7851633b, 0x7855ac5a, 0x7859f44f, 0x785e3b1c,
  0x786280bf, 0x7866c53a, 0x786b088c, 0x786f4ab4, 0x78738bb3, 0x7877cb89, 0x787c0a36, 0x788047ba,

  0x78848414, 0x7888bf45, 0x788cf94c, 0x7891322a, 0x789569df, 0x7899a06a, 0x789dd5cb, 0x78a20a03,
  0x78a63d11, 0x78aa6ef5, 0x78ae9fb0, 0x78b2cf41, 0x78b6fda8, 0x78bb2ae5, 0x78bf56f9, 0x78c381e2,
  0x78c7aba2, 0x78cbd437, 0x78cffba3, 0x78d421e4, 0x78d846fb, 0x78dc6ae8, 0x78e08dab, 0x78e4af44,
  0x78e8cfb2, 0x78eceef6, 0x78f10d0f, 0x78f529fe, 0x78f945c3, 0x78fd605d, 0x790179cd, 0x79059212,
  0x7909a92d, 0x790dbf1d, 0x7911d3e2, 0x7915e77c, 0x7919f9ec, 0x791e0b31, 0x79221b4b, 0x79262a3a,
  0x792a37fe, 0x792e4497, 0x79325006, 0x79365a49, 0x793a6361, 0x793e6b4e, 0x79427210, 0x794677a6,
  0x794a7c12, 0x794e7f52, 0x79528167, 0x79568250, 0x795a820e, 0x795e80a1, 0x79627e08, 0x79667a44,
  0x796a7554, 0x796e6f39, 0x797267f2, 0x79765f7f, 0x797a55e0, 0x797e4b16, 0x79823f20, 0x798631ff,

  0x798a23b1, 0x798e1438, 0x79920392, 0x7995f1c1, 0x7999dec4, 0x799dca9a, 0x79a1b545, 0x79a59ec3,
  0x79a98715, 0x79ad6e3c, 0x79b15435, 0x79b53903, 0x79b91ca4, 0x79bcff19, 0x79c0e062, 0x79c4c07e,
  0x79c89f6e, 0x79cc7d31, 0x79d059c8, 0x79d43532, 0x79d80f6f, 0x79dbe880, 0x79dfc064, 0x79e3971c,
  0x79e76ca7, 0x79eb4105, 0x79ef1436, 0x79f2e63a, 0x79f6b711, 0x79fa86bc, 0x79fe5539, 0x7a02228a,
  0x7a05eead, 0x7a09b9a4, 0x7a0d836d, 0x7a114c09, 0x7a151378, 0x7a18d9b9, 0x7a1c9ece, 0x7a2062b5,
  0x7a24256f, 0x7a27e6fb, 0x7a2ba75a, 0x7a2f668c, 0x7a332490, 0x7a36e166, 0x7a3a9d0f, 0x7a3e578b,
  0x7a4210d8, 0x7a45c8f9, 0x7a497feb, 0x7a4d35b0, 0x7a50ea47, 0x7a549db0, 0x7a584feb, 0x7a5c00f9,
  0x7a5fb0d8, 0x7a635f8a, 0x7a670d0d, 0x7a6ab963, 0x7a6e648a, 0x7a720e84, 0x7a75b74f, 0x7a795eec,

  0x7a7d055b, 0x7a80aa9c, 0x7a844eae, 0x7a87f192, 0x7a8b9348, 0x7a8f33d0, 0x7a92d329, 0x7a967153,
  0x7a9a0e50, 0x7a9daa1d, 0x7aa144bc, 0x7aa4de2d, 0x7aa8766f, 0x7aac0d82, 0x7aafa367, 0x7ab3381d,
  0x7ab6cba4, 0x7aba5dfc, 0x7abdef25, 0x7ac17f20, 0x7ac50dec, 0x7ac89b89, 0x7acc27f7, 0x7acfb336,
  0x7ad33d45, 0x7ad6c626, 0x7ada4dd8, 0x7addd45b, 0x7ae159ae, 0x7ae4ddd2, 0x7ae860c7, 0x7aebe28d,
  0x7aef6323, 0x7af2e28b, 0x7af660c2, 0x7af9ddcb, 0x7afd59a4, 0x7b00d44d, 0x7b044dc7, 0x7b07c612,
  0x7b0b3d2c, 0x7b0eb318, 0x7b1227d3, 0x7b159b5f, 0x7b190dbc, 0x7b1c7ee8, 0x7b1feee5, 0x7b235db2,
  0x7b26cb4f, 0x7b2a37bc, 0x7b2da2fa, 0x7b310d07, 0x7b3475e5, 0x7b37dd92, 0x7b3b4410, 0x7b3ea95d,
  0x7b420d7a, 0x7b457068, 0x7b48d225, 0x7b4c32b1, 0x7b4f920e, 0x7b52f03a, 0x7b564d36, 0x7b59a902,

  0x7b5d039e, 0x7b605d09, 0x7b63b543, 0x7b670c4d, 0x7b6a6227, 0x7b6db6d0, 0x7b710a49, 0x7b745c91,
  0x7b77ada8, 0x7b7afd8f, 0x7b7e4c45, 0x7b8199ca, 0x7b84e61f, 0x7b883143, 0x7b8b7b36, 0x7b8ec3f8,
  0x7b920b89, 0x7b9551ea, 0x7b989719, 0x7b9bdb18, 0x7b9f1de6, 0x7ba25f82, 0x7ba59fee, 0x7ba8df28,
  0x7bac1d31, 0x7baf5a09, 0x7bb295b0, 0x7bb5d026, 0x7bb9096b, 0x7bbc417e, 0x7bbf7860, 0x7bc2ae10,
  0x7bc5e290, 0x7bc915dd, 0x7bcc47fa, 0x7bcf78e5, 0x7bd2a89e, 0x7bd5d726, 0x7bd9047c, 0x7bdc30a1,
  0x7bdf5b94, 0x7be28556, 0x7be5ade6, 0x7be8d544, 0x7bebfb70, 0x7bef206b, 0x7bf24434, 0x7bf566cb,
  0x7bf88830, 0x7bfba863, 0x7bfec765, 0x7c01e534, 0x7c0501d2, 0x7c081d3d, 0x7c0b3777, 0x7c0e507e,
  0x7c116853, 0x7c147ef6, 0x7c179467, 0x7c1aa8a6, 0x7c1dbbb3, 0x7c20cd8d, 0x7c23de35, 0x7c26edab,

  0x7c29fbee, 0x7c2d08ff, 0x7c3014de, 0x7c331f8a, 0x7c362904, 0x7c39314b, 0x7c3c3860, 0x7c3f3e42,
  0x7c4242f2, 0x7c45466f, 0x7c4848ba, 0x7c4b49d2, 0x7c4e49b7, 0x7c514869, 0x7c5445e9, 0x7c574236,
  0x7c5a3d50, 0x7c5d3737, 0x7c602fec, 0x7c63276d, 0x7c661dbc, 0x7c6912d7, 0x7c6c06c0, 0x7c6ef976,
  0x7c71eaf9, 0x7c74db48, 0x7c77ca65, 0x7c7ab84e, 0x7c7da505, 0x7c809088, 0x7c837ad8, 0x7c8663f4,
  0x7c894bde, 0x7c8c3294, 0x7c8f1817, 0x7c91fc66, 0x7c94df83, 0x7c97c16b, 0x7c9aa221, 0x7c9d81a3,
  0x7ca05ff1, 0x7ca33d0c, 0x7ca618f3, 0x7ca8f3a7, 0x7cabcd28, 0x7caea574, 0x7cb17c8d, 0x7cb45272,
  0x7cb72724, 0x7cb9faa2, 0x7cbcccec, 0x7cbf9e03, 0x7cc26de5, 0x7cc53c94, 0x7cc80a0f, 0x7ccad656,
  0x7ccda169, 0x7cd06b48, 0x7cd333f3, 0x7cd5fb6a, 0x7cd8c1ae, 0x7cdb86bd, 0x7cde4a98, 0x7ce10d3f,

  0x7ce3ceb2, 0x7ce68ef0, 0x7ce94dfb, 0x7cec0bd1, 0x7ceec873, 0x7cf183e1, 0x7cf43e1a, 0x7cf6f720,
  0x7cf9aef0, 0x7cfc658d, 0x7cff1af5, 0x7d01cf29, 0x7d048228, 0x7d0733f3, 0x7d09e489, 0x7d0c93eb,
  0x7d0f4218, 0x7d11ef11, 0x7d149ad5, 0x7d174564, 0x7d19eebf, 0x7d1c96e5, 0x7d1f3dd6, 0x7d21e393,
  0x7d24881b, 0x7d272b6e, 0x7d29cd8c, 0x7d2c6e76, 0x7d2f0e2b, 0x7d31acaa, 0x7d3449f5, 0x7d36e60b,
  0x7d3980ec, 0x7d3c1a98, 0x7d3eb30f, 0x7d414a51, 0x7d43e05e, 0x7d467536, 0x7d4908d9, 0x7d4b9b46,
  0x7d4e2c7f, 0x7d50bc82, 0x7d534b50, 0x7d55d8e9, 0x7d58654d, 0x7d5af07b, 0x7d5d7a74, 0x7d600338,
  0x7d628ac6, 0x7d65111f, 0x7d679642, 0x7d6a1a31, 0x7d6c9ce9, 0x7d6f1e6c, 0x7d719eba, 0x7d741dd2,
  0x7d769bb5, 0x7d791862, 0x7d7b93da, 0x7d7e0e1c, 0x7d808728, 0x7d82fefe, 0x7d85759f, 0x7d87eb0a,

  0x7d8a5f40, 0x7d8cd240, 0x7d8f4409, 0x7d91b49e, 0x7d9423fc, 0x7d969224, 0x7d98ff17, 0x7d9b6ad3,
  0x7d9dd55a, 0x7da03eab, 0x7da2a6c6, 0x7da50dab, 0x7da77359, 0x7da9d7d2, 0x7dac3b15, 0x7dae9d21,
  0x7db0fdf8, 0x7db35d98, 0x7db5bc02, 0x7db81936, 0x7dba7534, 0x7dbccffc, 0x7dbf298d, 0x7dc181e8,
  0x7dc3d90d, 0x7dc62efc, 0x7dc883b4, 0x7dcad736, 0x7dcd2981, 0x7dcf7a96, 0x7dd1ca75, 0x7dd4191d,
  0x7dd6668f, 0x7dd8b2ca, 0x7ddafdce, 0x7ddd479d, 0x7ddf9034, 0x7de1d795, 0x7de41dc0, 0x7de662b3,
  0x7de8a670, 0x7deae8f7, 0x7ded2a47, 0x7def6a60, 0x7df1a942, 0x7df3e6ee, 0x7df62362, 0x7df85ea0,
  0x7dfa98a8, 0x7dfcd178, 0x7dff0911, 0x7e013f74, 0x7e0374a0, 0x7e05a894, 0x7e07db52, 0x7e0a0cd9,
  0x7e0c3d29, 0x7e0e6c42, 0x7e109a24, 0x7e12c6ce, 0x7e14f242, 0x7e171c7f, 0x7e194584, 0x7e1b6d53,

  0x7e1d93ea, 0x7e1fb94a, 0x7e21dd73, 0x7e240064, 0x7e26221f, 0x7e2842a2, 0x7e2a61ed, 0x7e2c8002,
  0x7e2e9cdf, 0x7e30b885, 0x7e32d2f4, 0x7e34ec2b, 0x7e37042a, 0x7e391af3, 0x7e3b3083, 0x7e3d44dd,
  0x7e3f57ff, 0x7e4169e9, 0x7e437a9c, 0x7e458a17, 0x7e47985b, 0x7e49a567, 0x7e4bb13c, 0x7e4dbbd9,
  0x7e4fc53e, 0x7e51cd6c, 0x7e53d462, 0x7e55da20, 0x7e57dea7, 0x7e59e1f5, 0x7e5be40c, 0x7e5de4ec,
  0x7e5fe493, 0x7e61e303, 0x7e63e03b, 0x7e65dc3b, 0x7e67d703, 0x7e69d093, 0x7e6bc8eb, 0x7e6dc00c,
  0x7e6fb5f4, 0x7e71aaa4, 0x7e739e1d, 0x7e75905d, 0x7e778166, 0x7e797136, 0x7e7b5fce, 0x7e7d4d2f,
  0x7e7f3957, 0x7e812447, 0x7e830dff, 0x7e84f67e, 0x7e86ddc6, 0x7e88c3d5, 0x7e8aa8ac, 0x7e8c8c4b,
  0x7e8e6eb2, 0x7e904fe0, 0x7e922fd6, 0x7e940e94, 0x7e95ec1a, 0x7e97c867, 0x7e99a37c, 0x7e9b7d58,

  0x7e9d55fc, 0x7e9f2d68, 0x7ea1039b, 0x7ea2d896, 0x7ea4ac58, 0x7ea67ee2, 0x7ea85033, 0x7eaa204c,
  0x7eabef2c, 0x7eadbcd4, 0x7eaf8943, 0x7eb1547a, 0x7eb31e78, 0x7eb4e73d, 0x7eb6aeca, 0x7eb8751e,
  0x7eba3a39, 0x7ebbfe1c, 0x7ebdc0c6, 0x7ebf8237, 0x7ec14270, 0x7ec3016f, 0x7ec4bf36, 0x7ec67bc5,
  0x7ec8371a, 0x7ec9f137, 0x7ecbaa1a, 0x7ecd61c5, 0x7ecf1837, 0x7ed0cd70, 0x7ed28171, 0x7ed43438,
  0x7ed5e5c6, 0x7ed7961c, 0x7ed94538, 0x7edaf31c, 0x7edc9fc6, 0x7ede4b38, 0x7edff570, 0x7ee19e6f,
  0x7ee34636, 0x7ee4ecc3, 0x7ee69217, 0x7ee83632, 0x7ee9d914, 0x7eeb7abc, 0x7eed1b2c, 0x7eeeba62,
  0x7ef05860, 0x7ef1f524, 0x7ef390ae, 0x7ef52b00, 0x7ef6c418, 0x7ef85bf7, 0x7ef9f29d, 0x7efb8809,
  0x7efd1c3c, 0x7efeaf36, 0x7f0040f6, 0x7f01d17d, 0x7f0360cb, 0x7f04eedf, 0x7f067bba, 0x7f08075c,

  0x7f0991c4, 0x7f0b1af2, 0x7f0ca2e7, 0x7f0e29a3, 0x7f0faf25, 0x7f11336d, 0x7f12b67c, 0x7f143852,
  0x7f15b8ee, 0x7f173850, 0x7f18b679, 0x7f1a3368, 0x7f1baf1e, 0x7f1d299a, 0x7f1ea2dc, 0x7f201ae5,
  0x7f2191b4, 0x7f230749, 0x7f247ba5, 0x7f25eec7, 0x7f2760af, 0x7f28d15d, 0x7f2a40d2, 0x7f2baf0d,
  0x7f2d1c0e, 0x7f2e87d6, 0x7f2ff263, 0x7f315bb7, 0x7f32c3d1, 0x7f342ab1, 0x7f359057, 0x7f36f4c3,
  0x7f3857f6, 0x7f39b9ee, 0x7f3b1aad, 0x7f3c7a31, 0x7f3dd87c, 0x7f3f358d, 0x7f409164, 0x7f41ec01,
  0x7f434563, 0x7f449d8c, 0x7f45f47b, 0x7f474a30, 0x7f489eaa, 0x7f49f1eb, 0x7f4b43f2, 0x7f4c94be,
  0x7f4de451, 0x7f4f32a9, 0x7f507fc7, 0x7f51cbab, 0x7f531655, 0x7f545fc5, 0x7f55a7fa, 0x7f56eef5,
  0x7f5834b7, 0x7f59793e, 0x7f5abc8a, 0x7f5bfe9d, 0x7f5d3f75, 0x7f5e7f13, 0x7f5fbd77, 0x7f60faa0,

  0x7f62368f, 0x7f637144, 0x7f64aabf, 0x7f65e2ff, 0x7f671a05, 0x7f684fd0, 0x7f698461, 0x7f6ab7b8,
  0x7f6be9d4, 0x7f6d1ab6, 0x7f6e4a5e, 0x7f6f78cb, 0x7f70a5fe, 0x7f71d1f6, 0x7f72fcb4, 0x7f742637,
  0x7f754e80, 0x7f76758e, 0x7f779b62, 0x7f78bffb, 0x7f79e35a, 0x7f7b057e, 0x7f7c2668, 0x7f7d4617,
  0x7f7e648c, 0x7f7f81c6, 0x7f809dc5, 0x7f81b88a, 0x7f82d214, 0x7f83ea64, 0x7f850179, 0x7f861753,
  0x7f872bf3, 0x7f883f58, 0x7f895182, 0x7f8a6272, 0x7f8b7227, 0x7f8c80a1, 0x7f8d8de1, 0x7f8e99e6,
  0x7f8fa4b0, 0x7f90ae3f, 0x7f91b694, 0x7f92bdad, 0x7f93c38c, 0x7f94c831, 0x7f95cb9a, 0x7f96cdc9,
  0x7f97cebd, 0x7f98ce76, 0x7f99ccf4, 0x7f9aca37, 0x7f9bc640, 0x7f9cc10d, 0x7f9dbaa0, 0x7f9eb2f8,
  0x7f9faa15, 0x7fa09ff7, 0x7fa1949e, 0x7fa2880b, 0x7fa37a3c, 0x7fa46b32, 0x7fa55aee, 0x7fa6496e,

  0x7fa736b4, 0x7fa822bf, 0x7fa90d8e, 0x7fa9f723, 0x7faadf7c, 0x7fabc69b, 0x7facac7f, 0x7fad9127,
  0x7fae7495, 0x7faf56c7, 0x7fb037bf, 0x7fb1177b, 0x7fb1f5fc, 0x7fb2d343, 0x7fb3af4e, 0x7fb48a1e,
  0x7fb563b3, 0x7fb63c0d, 0x7fb7132b, 0x7fb7e90f, 0x7fb8bdb8, 0x7fb99125, 0x7fba6357, 0x7fbb344e,
  0x7fbc040a, 0x7fbcd28b, 0x7fbd9fd0, 0x7fbe6bdb, 0x7fbf36aa, 0x7fc0003e, 0x7fc0c896, 0x7fc18fb4,
  0x7fc25596, 0x7fc31a3d, 0x7fc3dda9, 0x7fc49fda, 0x7fc560cf, 0x7fc62089, 0x7fc6df08, 0x7fc79c4b,
  0x7fc85854, 0x7fc91320, 0x7fc9ccb2, 0x7fca8508, 0x7fcb3c23, 0x7fcbf203, 0x7fcca6a7, 0x7fcd5a11,
  0x7fce0c3e, 0x7fcebd31, 0x7fcf6ce8, 0x7fd01b63, 0x7fd0c8a3, 0x7fd174a8, 0x7fd21f72, 0x7fd2c900,
  0x7fd37153, 0x7fd4186a, 0x7fd4be46, 0x7fd562e7, 0x7fd6064c, 0x7fd6a875, 0x7fd74964, 0x7fd7e917,

  0x7fd8878e, 0x7fd924ca, 0x7fd9c0ca, 0x7fda5b8f, 0x7fdaf519, 0x7fdb8d67, 0x7fdc247a, 0x7fdcba51,
  0x7fdd4eec, 0x7fdde24d, 0x7fde7471, 0x7fdf055a, 0x7fdf9508, 0x7fe0237a, 0x7fe0b0b1, 0x7fe13cac,
  0x7fe1c76b, 0x7fe250ef, 0x7fe2d938, 0x7fe36045, 0x7fe3e616, 0x7fe46aac, 0x7fe4ee06, 0x7fe57025,
  0x7fe5f108, 0x7fe670b0, 0x7fe6ef1c, 0x7fe76c4c, 0x7fe7e841, 0x7fe862fa, 0x7fe8dc78, 0x7fe954ba,
  0x7fe9cbc0, 0x7fea418b, 0x7feab61a, 0x7feb296d, 0x7feb9b85, 0x7fec0c62, 0x7fec7c02, 0x7fecea67,
  0x7fed5791, 0x7fedc37e, 0x7fee2e30, 0x7fee97a7, 0x7feeffe1, 0x7fef66e1, 0x7fefcca4, 0x7ff0312c,
  0x7ff09478, 0x7ff0f688, 0x7ff1575d, 0x7ff1b6f6, 0x7ff21553, 0x7ff27275, 0x7ff2ce5b, 0x7ff32905,
  0x7ff38274, 0x7ff3daa6, 0x7ff4319d, 0x7ff48759, 0x7ff4dbd9, 0x7ff52f1d, 0x7ff58125, 0x7ff5d1f1,

  0x7ff62182, 0x7ff66fd7, 0x7ff6bcf0, 0x7ff708ce, 0x7ff75370, 0x7ff79cd6, 0x7ff7e500, 0x7ff82bef,
  0x7ff871a2, 0x7ff8b619, 0x7ff8f954, 0x7ff93b54, 0x7ff97c18, 0x7ff9bba0, 0x7ff9f9ec, 0x7ffa36fc,
  0x7ffa72d1, 0x7ffaad6a, 0x7ffae6c7, 0x7ffb1ee9, 0x7ffb55ce, 0x7ffb8b78, 0x7ffbbfe6, 0x7ffbf319,
  0x7ffc250f, 0x7ffc55ca, 0x7ffc8549, 0x7ffcb38c, 0x7ffce093, 0x7ffd0c5f, 0x7ffd36ee, 0x7ffd6042,
  0x7ffd885a, 0x7ffdaf37, 0x7ffdd4d7, 0x7ffdf93c, 0x7ffe1c65, 0x7ffe3e52, 0x7ffe5f03, 0x7ffe7e79,
  0x7ffe9cb2, 0x7ffeb9b0, 0x7ffed572, 0x7ffeeff8, 0x7fff0943, 0x7fff2151, 0x7fff3824, 0x7fff4dbb,
  0x7fff6216, 0x7fff7536, 0x7fff8719, 0x7fff97c1, 0x7fffa72c, 0x7fffb55c, 0x7fffc251, 0x7fffce09,
  0x7fffd886, 0x7fffe1c6, 0x7fffe9cb, 0x7ffff094, 0x7ffff621, 0x7ffffa73, 0x7ffffd88, 0x7fffff62,

  0x80000000, 0x80000000,
};

unsigned int operamath_atan_ratio_table[OPERAMATH_ATAN_RATIO_COUNT] =
{
  0x00000000, 0x000028be, 0x0000517c, 0x00007a39,
  0x0000a2f6, 0x0000cbb1, 0x0000f46a, 0x00011d22,
  0x000145d7, 0x00016e8a, 0x0001973a, 0x0001bfe7,
  0x0001e890, 0x00021136, 0x000239d7, 0x00026274,
  0x00028b0d, 0x0002b3a0, 0x0002dc2e, 0x000304b6,
  0x00032d38, 0x000355b4, 0x00037e29, 0x0003a698,
  0x0003ceff, 0x0003f75f, 0x00041fb7, 0x00044806,
  0x0004704e, 0x0004988c, 0x0004c0c2, 0x0004e8ef,
  0x00051111, 0x0005392a, 0x00056139, 0x0005893d,
  0x0005b137, 0x0005d925, 0x00060109, 0x000628e0,
  0x000650ac, 0x0006786c, 0x0006a01f, 0x0006c7c6,
  0x0006ef5f, 0x000716ec, 0x00073e6b, 0x000765dc,
  0x00078d3f, 0x0007b494, 0x0007dbdb, 0x00080312,
  0x00082a3b, 0x00085154, 0x0008785e, 0x00089f59,
  0x0008c643, 0x0008ed1d, 0x000913e6, 0x00093a9f,
  0x00096147, 0x000987dd, 0x0009ae62, 0x0009d4d6,
  0x0009fb38, 0x000a2188, 0x000a47c5, 0x000a6df0,
  0x000a9409, 0x000aba0e, 0x000ae000, 0x000b05df,
  0x000b2bab, 0x000b5163, 0x000b7707, 0x000b9c97,
  0x000bc213, 0x000be77a, 0x000c0ccd, 0x000c320b,
  0x000c5734, 0x000c7c47, 0x000ca146, 0x000cc62f,
  0x000ceb02, 0x000d0fc0, 0x000d3468, 0x000d58f9,
  0x000d7d75, 0x000da1d9, 0x000dc628, 0x000dea60,
  0x000e0e80, 0x000e328a, 0x000e567d, 0x000e7a58,
  0x000e9e1d, 0x000ec1c9, 0x000ee55e, 0x000f08dc,
  0x000f2c41, 0x000f4f8f, 0x000f72c4, 0x000f95e1,
  0x000fb8e6, 0x000fdbd3, 0x000ffea7, 0x00102163,
  0x00104406, 0x00106690, 0x00108901, 0x0010ab59,
  0x0010cd98, 0x0010efbe, 0x001111cb, 0x001133bf,
  0x00115599, 0x0011775a, 0x00119902, 0x0011ba8f,
  0x0011dc04, 0x0011fd5e, 0x00121e9f, 0x00123fc6,
  0x001260d3, 0x001281c7, 0x0012a2a0, 0x0012c35f,
  0x0012e405, 0x00130490, 0x00132501, 0x00134558,
  0x00136595, 0x001385b7, 0x0013a5c0, 0x0013c5ae,
  0x0013e582, 0x0014053b, 0x001424da, 0x0014445f,
  0x001463c9, 0x00148319, 0x0014a24e, 0x0014c169,
  0x0014e06a, 0x0014ff50, 0x00151e1c, 0x00153ccd,
  0x00155b64, 0x001579e0, 0x00159842, 0x0015b689,
  0x0015d4b6, 0x0015f2c9, 0x001610c1, 0x00162e9e,
  0x00164c62, 0x00166a0a, 0x00168799, 0x0016a50d,
  0x0016c266, 0x0016dfa6, 0x0016fccb, 0x001719d6,
  0x001736c6, 0x0017539c, 0x00177058, 0x00178cfa,
  0x0017a982, 0x0017c5ef, 0x0017e243, 0x0017fe7c,
  0x00181a9b, 0x001836a1, 0x0018528c, 0x00186e5e,
  0x00188a15, 0x0018a5b3, 0x0018c137, 0x0018dca1,
  0x0018f7f2, 0x00191329, 0x00192e46, 0x0019494a,
  0x00196434, 0x00197f05, 0x001999bc, 0x0019b45a,
  0x0019cedf, 0x0019e94a, 0x001a039c, 0x001a1dd5,
  0x001a37f5, 0x001a51fc, 0x001a6bea, 0x001a85bf,
  0x001a9f7b, 0x001ab91f, 0x001ad2a9, 0x001aec1b,
  0x001b0575, 0x001b1eb6, 0x001b37de, 0x001b50ee,
  0x001b69e5, 0x001b82c5, 0x001b9b8c, 0x001bb43b,
  0x001bccd1, 0x001be550, 0x001bfdb7, 0x001c1606,
  0x001c2e3d, 0x001c465c, 0x001c5e64, 0x001c7654,
  0x001c8e2d, 0x001ca5ee, 0x001cbd98, 0x001cd52a,
  0x001ceca5, 0x001d0409, 0x001d1b56, 0x001d328c,
  0x001d49ab, 0x001d60b3, 0x001d77a4, 0x001d8e7f,
  0x001da542, 0x001dbbf0, 0x001dd287, 0x001de907,
  0x001dff71, 0x001e15c5, 0x001e2c02, 0x001e422a,
  0x001e583b, 0x001e6e37, 0x001e841d, 0x001e99ec,
  0x001eafa7, 0x001ec54b, 0x001edada, 0x001ef054,
  0x001f05b8, 0x001f1b06, 0x001f3040, 0x001f4564,
  0x001f5a73, 0x001f6f6e, 0x001f8453, 0x001f9923,
  0x001faddf, 0x001fc286, 0x001fd718, 0x001feb96,
  0x00200000, 0x00200000
};

LinkedListMemoryApi linked_list_memory_api = { 0, 0, 0 };

int memory_system_last_error = 0;

int resource_system_last_error = 0;

ThreadCreationDefaults audio_manager_thread_defaults =
{
  {
    { 2, 0 },
    { 11, 0x00004000 },
    { 10, 0 },
    { 12, 0 },
    { 15, 0 },
    { 13, 0 },
    { 14, 0 },
    { 1, 0 },
    { 0, 0 }
  },
  audio_manager_thread_name
};

BlockFileApiGlobals block_file_api_globals =
{
  0,
  block_file_allocate_memory,
  block_file_free_memory
};

BlockFileSectorCache block_file_sector_cache =
{
  0,
  0,
  -1,
  0,
  0,
  0
};

RwMemoryVectors byte_file_memory_vectors =
{
  byte_file_allocate_memory,
  byte_file_free_memory
};

int block_file_last_error = 0;

void *stream_acquisition_context_pool = 0;

void *stream_context_pool = 0;

int stream_end_of_file_seen = 0;

CPakSubscriberGlobals cpak_subscriber_globals = { 0, 0, 0 };

void *saudio_subscriber_context_pool = 0;

void *ctrl_subscriber_context_pool = 0;

SAudioTemplateRecord saudio_initial_templates[SAUDIO_TEMPLATE_COUNT] =
{
  { 0x01010202U, 0, saudio_half_mono_8_template_name },
  { 0x01020202U, 0, saudio_half_stereo_8_template_name },
  { 0x01020102U, 0, saudio_fixed_stereo_8_template_name },
  { 0x01010102U, 0, saudio_fixed_mono_8_template_name },
  { 0x01010101U, 0, saudio_fixed_mono_sample_template_name },
  { 0x01020101U, 0, saudio_fixed_stereo_sample_template_name },
  { 0x02010101U, 0, saudio_dcsqxd_mono_template_name },
  { 0x02020101U, 0, saudio_dcsqxd_stereo_template_name },
  { 0x02010201U, 0, saudio_dcsqxd_half_mono_template_name },
  { 0x02020201U, 0, saudio_dcsqxd_half_stereo_template_name }
};

int saudio_template_count = SAUDIO_TEMPLATE_COUNT;

char uppercase_hex_digits[20] = "0123456789ABCDEF";

const char *score_mixer_destination_names[SCORE_MIXER_INPUT_COUNT] =
{
  score_mixer_input_0_name,
  score_mixer_input_1_name,
  score_mixer_input_2_name,
  score_mixer_input_3_name,
  score_mixer_input_4_name,
  score_mixer_input_5_name,
  score_mixer_input_6_name,
  score_mixer_input_7_name,
  score_mixer_input_8_and_9_name,
  score_mixer_input_10_name,
  score_mixer_input_11_name
};

RwTagArgument score_note_start_tags[3] =
{
  { 15, 0 },
  { 14, 0 },
  { 0, 0 }
};

const char *score_mixer_left_gain_names[SCORE_MIXER_GAIN_COUNT] =
{
  score_mixer_left_gain_0_name,
  score_mixer_left_gain_1_name,
  score_mixer_left_gain_2_name,
  score_mixer_left_gain_3_name,
  score_mixer_left_gain_4_name,
  score_mixer_left_gain_5_name,
  score_mixer_left_gain_6_name,
  score_mixer_left_gain_7_name,
  score_mixer_left_gain_8_name,
  score_mixer_left_gain_9_name,
  score_mixer_left_gain_10_name,
  score_mixer_left_gain_11_name
};

const char *score_mixer_right_gain_names[SCORE_MIXER_GAIN_COUNT] =
{
  score_mixer_right_gain_0_name,
  score_mixer_right_gain_1_name,
  score_mixer_right_gain_2_name,
  score_mixer_right_gain_3_name,
  score_mixer_right_gain_4_name,
  score_mixer_right_gain_5_name,
  score_mixer_right_gain_6_name,
  score_mixer_right_gain_7_name,
  score_mixer_right_gain_8_name,
  score_mixer_right_gain_9_name,
  score_mixer_right_gain_10_name,
  score_mixer_right_gain_11_name
};

RwMemoryVectors custom_memory_vectors = { 0, 0 };

void *file_folio_base = 0;
EventBrokerClientState event_broker_client =
{
  0, 0, 0, 0, 0, 0, 0, 0
};

unsigned int ansi_random_seed = 1;

LegacyRandomGeneratorState legacy_random_generator =
{
  {
    0x66d78e85, 0xd5d38c09, 0x0a09d8f5, 0xbf1f87fb,
    0xcb8df767, 0xbdf70769, 0x503d1234, 0x7f4f84c8,
    0x61de02a3, 0xa7408dae, 0x7a24bde8, 0x5115a2ea,
    0xbbe62e57, 0xf6d57fff, 0x632a837a, 0x13861d77,
    0xe19f2e7c, 0x695f5705, 0x87936b2e, 0x50a19a6e,
    0x728b0e94, 0xc5cc55ae, 0xb10a8ab1, 0x856f72d7,
    0xd0225c17, 0x51c4fda3, 0x89ed9861, 0xf1db829f,
    0xbcfbc59d, 0x83eec189, 0x6359b159, 0xcc505c30,
    0x9cbc5ac9, 0x2fe230f9, 0x39f65e42, 0x75157bd2,
    0x40c158fb, 0x27eb9a3e, 0xc582a2d9, 0x0569d6c2,
    0xed8e30b3, 0x1083ddd2, 0x1f1da441, 0x5660e215,
    0x04f32fc5, 0xe18eef99, 0x4a593208, 0x5b7bed4c,
    0x8102fc40, 0x515341d9, 0xacff3dfa, 0x6d096cb5,
    0x2bb3cc1d, 0x253d15ff
  },
  23,
  54
};

unsigned int portable_random_seed = 1;

CamStdioRuntime cam_stdio_runtime =
{
  0,
  0,
  0,
  &gStandardInputStream,
  &gStandardOutputStream,
  &gStandardErrorStream
};

RwTagArgument cam_io_request_tags[2] =
{
  { 11, 0 },
  { 0, 0 }
};

SEMANTIC_SIZE_ASSERT(rider_runtime_slots_object,
                     sizeof(rider_runtime_slots) == 0x400);
SEMANTIC_SIZE_ASSERT(rider_resource_references_object,
                     sizeof(rider_resource_references) == 0x100);
SEMANTIC_SIZE_ASSERT(rider_progression_defaults_object,
                     sizeof(rider_progression_defaults) == 0x1270);
SEMANTIC_SIZE_ASSERT(rider_update_handlers_object,
                     sizeof(rider_update_handlers) == 0x2C);
SEMANTIC_SIZE_ASSERT(opponent_choice_levels_object,
                     sizeof(opponent_choice_levels) == 0x8C0);
SEMANTIC_SIZE_ASSERT(player_profiles_object,
                     sizeof(player_profiles) == 0x78);
SEMANTIC_SIZE_ASSERT(rider_animation_channels_object,
                     sizeof(rider_animation_channels) == 0x518);
SEMANTIC_SIZE_ASSERT(rider_one_animation_bindings_object,
                     sizeof(rider_one_animation_bindings) == 0xB18);
SEMANTIC_SIZE_ASSERT(rider_one_animation_by_state_object,
                     sizeof(rider_one_animation_by_state) == 0xC8);
SEMANTIC_SIZE_ASSERT(rider_two_animation_bindings_object,
                     sizeof(rider_two_animation_bindings) == 0x230);
SEMANTIC_SIZE_ASSERT(rider_two_animation_by_state_object,
                     sizeof(rider_two_animation_by_state) == 0xCC);
SEMANTIC_SIZE_ASSERT(car_animation_selectors_object,
                     sizeof(car_animation_selectors) == 0x18);
SEMANTIC_SIZE_ASSERT(car_animation_resources_object,
                     sizeof(car_animation_resources) == 0x78);
SEMANTIC_SIZE_ASSERT(car_animation_bindings_object,
                     sizeof(car_animation_bindings) == 0x384);
SEMANTIC_SIZE_ASSERT(car_animation_variant_bindings_object,
                     sizeof(car_animation_variant_bindings) == 0xB4);
SEMANTIC_SIZE_ASSERT(resource_catalog_handles_object,
                     sizeof(resource_catalog_handles) == 0x10);
SEMANTIC_SIZE_ASSERT(course_catalog_object,
                     sizeof(course_catalog) == 0x190);
SEMANTIC_SIZE_ASSERT(rash_options_catalog_object,
                     sizeof(rash_options_catalog) == 0x6F4);
SEMANTIC_SIZE_ASSERT(saudio_initial_templates_object,
                     sizeof(saudio_initial_templates) == 0x78);
