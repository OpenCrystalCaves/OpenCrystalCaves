#pragma once

#include <memory>
#include <vector>

#include <SDL_mixer.h>

enum class SoundType : int
{
  SOUND_JUMP,
  SOUND_UNKNOWN1,
  SOUND_START_LEVEL,
  SOUND_END_LEVEL,
  SOUND_DIE,
  SOUND_ENEMY_DIE,
  SOUND_ENEMY_HURT,  // TODO: some effect is missing, this is too short
  SOUND_CRYSTAL,
  SOUND_PICKUP_GUN,  // also P pill, blue mushrooms
  SOUND_SECRET_CRYSTAL,
  SOUND_CHEST,
  SOUND_FRUIT,
  SOUND_UNKNOWNC,
  SOUND_FIRE,
  SOUND_POWER_FIRE,
  SOUND_TURRET_FIRE,
  SOUND_UNKNOWNG,
  SOUND_POISONED,
  SOUND_UNKNOWNI,
  SOUND_SWITCH,
  SOUND_LEVER,
  SOUND_BLUE_MUSHROOM,
  SOUND_HAMMER,
  SOUND_UNKNOWNN,
  SOUND_STALACTITE_FALL,
  SOUND_QUIT,
  SOUND_UNKNOWNQ,
  SOUND_UNKNOWNR,
  SOUND_UNKNOWNS,
  SOUND_UNKNOWNT,
  SOUND_DRILL,
  SOUND_APOGEE,
  SOUND_FADE_OUT,
  SOUND_HIGH_SCORE,
  SOUND_PANEL,
  SOUND_MAIN_LEVEL,
};

class SoundManager
{
 public:
  SoundManager();
  virtual ~SoundManager();

  bool load_sounds(const int episode);
  size_t size() const { return chunks_.size(); }
  void play_sound(const SoundType sound) const;

 private:
	std::vector<std::string> raw_chunks_;
  std::vector<Mix_Chunk*> chunks_;
  SDL_AudioSpec spec_;
};
