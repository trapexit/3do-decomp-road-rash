#ifndef ROADRASH_AUDIO_MANAGER_MIXER_INTERNAL_H
#define ROADRASH_AUDIO_MANAGER_MIXER_INTERNAL_H

enum AudioManagerMixerConstant
{
  AUDIO_MANAGER_MIXER_CONNECTION_ERROR = -35,
  AUDIO_MANAGER_MIXER_LEFT_INITIAL_PAN = 0x20,
  AUDIO_MANAGER_MIXER_CENTER_PAN = 0x40,
  AUDIO_MANAGER_MIXER_RIGHT_INITIAL_PAN = 0x60,
  AUDIO_MANAGER_MIXER_MAXIMUM_PAN = 0x7F,
  AUDIO_MANAGER_SIGN_BIT_SHIFT = 31
};

int
connect_instrument_output_to_global_mixer(int   instrument_item,
                                          char *output_name);
int
disconnect_global_mixer_connection(int connection_key);
int
set_global_mixer_connection_pan(int connection_key,
                                int pan);

#endif
