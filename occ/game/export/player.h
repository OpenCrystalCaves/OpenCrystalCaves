#pragma once

#include <array>

#include "geometry.h"
#include "level_id.h"
#include "sound.h"
#include "vector.h"

enum class TouchType;
struct Level;

struct Player
{
  // Note: this is the player's actual size and is used for collision detection
  //       and other things. The player sprite is still size 16x16.
  static constexpr geometry::Size size = geometry::Size(12, 16);

  geometry::Position position = geometry::Position(0, 0);
  geometry::Position position_last = geometry::Position(0, 0);
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

  bool crushed = false;

  unsigned power_tick = 0u;
  unsigned gravity_tick = 0u;
  unsigned stop_tick = 0u;
  unsigned tough_tick = 0u;
  unsigned hurt_tick = 0u;
  unsigned health_ = 3u;
  unsigned dying_tick = 0u;

  bool noclip = false;
  bool godmode = false;

  void update(AbstractSoundManager& sound_manager, Level& level);
  void hurt(const TouchType& touch_type);
  bool is_flashing() const;
  geometry::Rectangle rect() const { return {position, size}; }
  // Get the effective reverse gravity w.r.t. powerups
  bool is_reverse_gravity() const { return gravity_tick > 0; }
};
