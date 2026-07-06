#include "audio_ro_data.h"

const char background_trip_stream_path[20] = "HB.Trip_sw22.stream";
const char background_simple_passing_stream_path[32] =
  "HB.SimplePassing_sw22.stream";
const char background_dinosaur_vacume_stream_path[32] =
  "MM.DinosaurVacume_sw22.stream";
const char background_the_bridge_stream_path[28] =
  "Paw.TheBridge_sw22.stream";
const char background_jessie_stream_path[24] =
  "Paw.Jessie_sw22.stream";
const char background_pansy_stream_path[24] =
  "Paw.Pansy_sw22.stream";
const char background_rusty_cage_stream_path[28] =
  "SG.RustyCage_sw22.stream";
const char background_outshined_stream_path[28] =
  "SG.Outshined_sw22.stream";
const char background_kickstand_stream_path[28] =
  "SG.Kickstand_sw22.stream";
const char background_superunknown_stream_path[28] =
  "SG.Superunknown_sw22.stream";
const char background_last_train_stream_path[28] =
  "SD.LastTrain_sw22.stream";
const char background_duel_stream_path[20] = "SD.Duel_sw22.stream";
const char background_teethgrinder_stream_path[28] =
  "Tx.Teethgrinder_sw22.stream";
const char background_auto_surgery_stream_path[28] =
  "Tx.AutoSurgery_sw22.stream";

const char audio_mixer_left_gain_0_name[12] = "LeftGain0";
const char audio_mixer_left_gain_1_name[12] = "LeftGain1";
const char audio_mixer_left_gain_2_name[12] = "LeftGain2";
const char audio_mixer_left_gain_3_name[12] = "LeftGain3";
const char audio_mixer_left_gain_4_name[12] = "LeftGain4";
const char audio_mixer_left_gain_5_name[12] = "LeftGain5";
const char audio_mixer_left_gain_6_name[12] = "LeftGain6";
const char audio_mixer_left_gain_7_name[12] = "LeftGain7";
const char audio_mixer_left_gain_8_name[12] = "LeftGain8";
const char audio_mixer_left_gain_9_name[12] = "LeftGain9";
const char audio_mixer_left_gain_10_name[12] = "LeftGain10";
const char audio_mixer_left_gain_11_name[12] = "LeftGain11";
const char audio_mixer_right_gain_0_name[12] = "RightGain0";
const char audio_mixer_right_gain_1_name[12] = "RightGain1";
const char audio_mixer_right_gain_2_name[12] = "RightGain2";
const char audio_mixer_right_gain_3_name[12] = "RightGain3";
const char audio_mixer_right_gain_4_name[12] = "RightGain4";
const char audio_mixer_right_gain_5_name[12] = "RightGain5";
const char audio_mixer_right_gain_6_name[12] = "RightGain6";
const char audio_mixer_right_gain_7_name[12] = "RightGain7";
const char audio_mixer_right_gain_8_name[12] = "RightGain8";
const char audio_mixer_right_gain_9_name[12] = "RightGain9";
const char audio_mixer_right_gain_10_name[12] = "RightGain10";
const char audio_mixer_right_gain_11_name[12] = "RightGain11";

const char audio_mixer_2x2_template_name[16] = "mixer2x2.dsp";
const char audio_mixer_4x2_template_name[16] = "mixer4x2.dsp";
const char audio_mixer_8x2_template_name[16] = "mixer8x2.dsp";
const char audio_mixer_12x2_template_name[16] = "mixer12x2.dsp";

char audio_manager_thread_name[12] = "AM_Thread00";

const char saudio_half_mono_8_template_name[16] = "halfmono8.dsp";
const char saudio_half_stereo_8_template_name[16] = "halfstereo8.dsp";
const char saudio_fixed_stereo_8_template_name[20] = "fixedstereo8.dsp";
const char saudio_fixed_mono_8_template_name[16] = "fixedmono8.dsp";
const char saudio_fixed_mono_sample_template_name[20] =
  "fixedmonosample.dsp";
const char saudio_fixed_stereo_sample_template_name[24] =
  "fixedstereosample.dsp";
const char saudio_dcsqxd_mono_template_name[16] = "dcsqxdmono.dsp";
const char saudio_dcsqxd_stereo_template_name[20] = "dcsqxdstereo.dsp";
const char saudio_dcsqxd_half_mono_template_name[20] =
  "dcsqxdhalfmono.dsp";
const char saudio_dcsqxd_half_stereo_template_name[24] =
  "dcsqxdhalfstereo.dsp";

const char score_mixer_input_0_name[8] = "Input0";
const char score_mixer_input_1_name[8] = "Input1";
const char score_mixer_input_2_name[8] = "Input2";
const char score_mixer_input_3_name[8] = "Input3";
const char score_mixer_input_4_name[8] = "Input4";
const char score_mixer_input_5_name[8] = "Input5";
const char score_mixer_input_6_name[8] = "Input6";
const char score_mixer_input_7_name[8] = "Input7";
const char score_mixer_input_8_and_9_name[16] = "Input8Input9";
const char score_mixer_input_10_name[8] = "Input10";
const char score_mixer_input_11_name[8] = "Input11";

const char score_mixer_left_gain_0_name[12] = "LeftGain0";
const char score_mixer_left_gain_1_name[12] = "LeftGain1";
const char score_mixer_left_gain_2_name[12] = "LeftGain2";
const char score_mixer_left_gain_3_name[12] = "LeftGain3";
const char score_mixer_left_gain_4_name[12] = "LeftGain4";
const char score_mixer_left_gain_5_name[12] = "LeftGain5";
const char score_mixer_left_gain_6_name[12] = "LeftGain6";
const char score_mixer_left_gain_7_name[12] = "LeftGain7";
const char score_mixer_left_gain_8_name[12] = "LeftGain8";
const char score_mixer_left_gain_9_name[12] = "LeftGain9";
const char score_mixer_left_gain_10_name[12] = "LeftGain10";
const char score_mixer_left_gain_11_name[12] = "LeftGain11";
const char score_mixer_right_gain_0_name[12] = "RightGain0";
const char score_mixer_right_gain_1_name[12] = "RightGain1";
const char score_mixer_right_gain_2_name[12] = "RightGain2";
const char score_mixer_right_gain_3_name[12] = "RightGain3";
const char score_mixer_right_gain_4_name[12] = "RightGain4";
const char score_mixer_right_gain_5_name[12] = "RightGain5";
const char score_mixer_right_gain_6_name[12] = "RightGain6";
const char score_mixer_right_gain_7_name[12] = "RightGain7";
const char score_mixer_right_gain_8_name[12] = "RightGain8";
const char score_mixer_right_gain_9_name[12] = "RightGain9";
const char score_mixer_right_gain_10_name[12] = "RightGain10";
const char score_mixer_right_gain_11_name[12] = "RightGain11";
