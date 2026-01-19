#pragma once

#include "geometry.h"
#include "object.h"
#include "sprite.h"

struct Exit
{
  Exit(geometry::Position position) : position(std::move(position)) {}

  geometry::Position position;
  bool open = false;
  int counter = 0;

  void update();
  std::vector<ObjectDef> get_sprites() const;
};
