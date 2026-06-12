#pragma once

#include "geometry.h"

struct ObjectDef
{
  geometry::Position position;
  int sprite_id;
  bool bright;
};

enum class ObjectFlags : int
{
  NONE = 0,
  RENDER_IN_FRONT = 1 << 0,
  BRIGHT = 1 << 1,
};

struct Object
{
  Object(geometry::Position position,
         int sprite_id,
         int num_sprites = 1,
         bool reverse = false,
         int flags = 0,
         Vector<double> parallax = {0.0, 0.0})
    : position(position),
      sprite_id(sprite_id),
      num_sprites(num_sprites),
      reverse(reverse),
      parallax(parallax),
      flags(flags)
  {
  }

  int get_sprite(const int ticks) const
  {
    const auto d = ticks % num_sprites;
    return (reverse ? num_sprites - 1 - d : d) + sprite_id;
  }

  geometry::Position position;
  int sprite_id;
  int num_sprites;
  bool reverse;
  int flags;
  Vector<double> parallax;
};
