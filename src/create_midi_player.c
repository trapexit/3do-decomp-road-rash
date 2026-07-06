#include "midi_player_runtime.h"
#include "runtime_data_types.h"
#include "score_context_api.h"

MidiPlayer *
create_midi_player(int program_count)
{
  RwMemoryAllocator allocate;
  MidiPlayer *player;
  ScoreContext *score_context;
  int creation_error;
  int empty;

  allocate = music_library_globals.allocate;
  player = (MidiPlayer *)allocate(MIDI_PLAYER_ALLOCATION_SIZE, 0);
  creation_error = -1;
  if(player == 0)
    {
      score_loader_last_error = creation_error;
      delete_midi_player(player);
      return 0;
    }

  empty = 0;
  player->runtime = 0;
  player->score_context = 0;
  player->stream = 0;
  player->pending_stream = 0;
  player->callback = empty;
  player->callback_context = empty;
  player->reserved_18 = empty;
  player->reserved_1c = empty;
  player->audio_clock_item = creation_error;
  player->saved_audio_rate = empty;
  player->reserved_28 = empty;
  player->score_loaded = 0;
  player->repeat_count = empty;
  player->score_resource_list = 0;
  player->mixer_amplitude = creation_error;
  player->mixer_voice_count = empty;

  score_context = CreateScoreContext(
    program_count & MIDI_SCORE_PROGRAM_COUNT_MASK);
  player->score_context = score_context;
  if(score_context == 0)
    {
      score_loader_last_error = creation_error;
      delete_midi_player(player);
      return 0;
    }
  return player;
}
