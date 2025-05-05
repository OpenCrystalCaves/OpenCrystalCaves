#include "item.h"

#include "level.h"

TouchType Crystal::on_touch(const Player& player, AbstractSoundManager& sound_manager, Level& level)
{
  const auto tt = Item::on_touch(player, sound_manager, level);
  level.crystals--;
  return tt;
}
