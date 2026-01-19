#include "exit.h"

void Exit::update()
{
  if (open && counter < 3)
  {
    counter++;
  }
}

std::vector<ObjectDef> Exit::get_sprites() const
{
  return {
    {position, static_cast<int>(Sprite::SPRITE_EXIT_TOP_LEFT_1) + counter, false},
    {position + geometry::Position(0, 16), static_cast<int>(Sprite::SPRITE_EXIT_BOTTOM_LEFT_1) + counter, false},
  };
}
