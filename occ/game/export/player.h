#pragma once

#include <array>

#include "geometry.h"
#include "level_id.h"
#include "vector.h"

class Level;

struct Player
{
  // Note: this is the player's actual size and is used for collision detection
  //       and other things. The player sprite is still size 16x16.
  static constexpr geometry::Size size = geometry::Size(12, 16);

  geometry::Position position = geometry::Position(0, 0);
  Vector<int> velocity = Vector<int>(0, 0);
  enum class Direction
  {
    right,
    left
  } direction = Direction::right;

  bool collide_x = false;
  bool collide_y = false;

  bool walking = false;
  unsigned walk_tick = 0u;

  bool jumping = false;
  unsigned jump_tick = 0u;

  bool falling = false;

  bool shooting = false;

  unsigned power_tick = 0u;
  unsigned hurt_tick = 0u;
  unsigned health_ = 3u;

  bool noclip = false;
  bool godmode = false;
  bool reverse_gravity = false;

  void update(const Level& level);
  void hurt();
  bool is_flashing() const;
};
