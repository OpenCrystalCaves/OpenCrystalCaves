#pragma once

enum class SoundType : int
{
  SOUND_JUMP,
  SOUND_UNKNOWN1,
  SOUND_ENTER_LEVEL,
  SOUND_EXIT_LEVEL,
  SOUND_DIE,  // TODO: use
  SOUND_ENEMY_DIE,
  SOUND_ENEMY_HURT,      // TODO: some effect is missing, this is too short
  SOUND_CRYSTAL,         // TODO: use
  SOUND_PICKUP_GUN,      // also P pill; TODO: use
  SOUND_SECRET_CRYSTAL,  // TODO: use
  SOUND_CHEST,
  SOUND_FRUIT,  // TODO: use
  SOUND_UNKNOWNC,
  SOUND_FIRE,
  SOUND_POWER_FIRE,
  SOUND_LASER_FIRE,
  SOUND_UNKNOWNG,
  SOUND_POISONED,  // TODO: use
  SOUND_UNKNOWNI,
  SOUND_SWITCH,
  SOUND_LEVER,
  SOUND_BLUE_MUSHROOM,  // TODO: use
  SOUND_HAMMER,
  SOUND_UNKNOWNN,
  SOUND_STALACTITE_FALL,  // TODO: use
  SOUND_QUIT,             // TODO: use
  SOUND_UNKNOWNQ,
  SOUND_CLEAR_BLOCK,
  SOUND_UNKNOWNS,
  SOUND_UNKNOWNT,
  SOUND_THORN,
  SOUND_APOGEE,
  SOUND_START_GAME,
  SOUND_HIGH_SCORE,  // TODO: use
  SOUND_PANEL,
  SOUND_MAIN_LEVEL,  // TODO: use
};

class AbstractSoundManager
{
 public:
  virtual void play_sound(const SoundType sound) const = 0;
};
