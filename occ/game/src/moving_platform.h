#pragma once

#include "geometry.h"
#include "vector.h"

struct Level;

struct MovingPlatform
{
  MovingPlatform(geometry::Position position, const bool horizontal, const bool controlled);

  int get_sprite() const;
  void update(const Level& level);

  geometry::Position position;
  geometry::Size collide_size = geometry::Size(16, 32);

  Vector<int> get_velocity(const Level& level) const;
  bool is_moving = false;

 private:
  bool is_reverse() const { return velocity.x() > 0 || velocity.y() > 0; }
  geometry::Position collide_position() const { return position + geometry::Position(0, -16); }
  Vector<int> velocity;
  bool controlled_;
  const int sprite_id_;
  int ticks_ = 0;
};
