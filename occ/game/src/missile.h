#pragma once

#include "geometry.h"
#include "misc.h"

class AbstractSoundManager;
struct Level;
struct Player;

struct Missile
{
  Missile() : position() {}

  bool alive = false;
  bool killed_enemy = false;
  unsigned frame = 0;
  bool is_power = false;
  geometry::Position position;
  bool right = false;     // Direction...
  unsigned cooldown = 0;  // cooldown from previous missile explosion

  void init(AbstractSoundManager& sound_manager, const Player& player);
  bool is_in_cooldown() const;
  void set_cooldown();
  // Returns whether it exploded
  bool update(AbstractSoundManager& sound_manager, const geometry::Rectangle& player_rect, Level& level);
  int get_sprite() const;
  int get_num_sprites() const;

  // Note: this is the actual size of the missile and is used for collision detection.
  //       The missile sprites are size 16x16.
  static constexpr auto size = geometry::Size(16, 11);
  static constexpr auto speed = misc::make_array(4, 4, 4, 4, 4, 4, 6, 8, 10, 12, 14);
};
