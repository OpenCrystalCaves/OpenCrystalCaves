#pragma once

#include "geometry.h"

struct ObjectDef
{
  geometry::Position position;
  int sprite_id;
  bool bright;
};

struct Object
{
  Object(geometry::Position position, int sprite_id, bool bright, int num_sprites, const bool reverse)
    : position(position),
      sprite_id(sprite_id),
      bright(bright),
      num_sprites(num_sprites),
      reverse(reverse)
  {
  }

  int get_sprite(const int ticks) const
  {
    const auto d = ticks % num_sprites;
    return (reverse ? num_sprites - 1 - d : d) + sprite_id;
  }

  geometry::Position position;
  int sprite_id;
  bool bright;
  int num_sprites;
  bool reverse;
};
