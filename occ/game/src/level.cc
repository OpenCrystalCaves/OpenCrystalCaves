#include "level.h"

const Tile& Level::get_tile(const int x, const int y) const
{
  if (x < 0 || x >= width || y < 0 || y >= height)
  {
    return Tile::INVALID;
  }
  return tiles[(y * width) + x];
}

int Level::get_bg(const int x, const int y) const
{
  if (x < 0 || x >= width || y < 0 || y >= height)
  {
    return -1;
  }
  return bgs[(y * width) + x];
}

const Item& Level::get_item(const int x, const int y) const
{
  if (x < 0 || x >= width || y < 0 || y >= height)
  {
    return Item::INVALID;
  }
  return items[(y * width) + x];
}

void Level::remove_item(const int x, const int y)
{
  items[(y * width) + x].invalidate();
}

bool Level::collides_solid(const geometry::Position& position, const geometry::Size& size, const bool is_slime) const
{
  // Note: this function only works with size x and y <= 16
  // With size 16x16 the object can cover at maximum 4 tiles
  // Check all 4 tiles, even though we might check the same tile multiple times
  const std::array<geometry::Position, 4> positions = {
    geometry::Position((position.x() + 0) / 16, (position.y() + 0) / 16),
    geometry::Position((position.x() + size.x() - 1) / 16, (position.y() + 0) / 16),
    geometry::Position((position.x() + 0) / 16, (position.y() + size.y() - 1) / 16),
    geometry::Position((position.x() + size.x() - 1) / 16, (position.y() + size.y() - 1) / 16)};
  for (const auto& p : positions)
  {
    const auto& tile = get_tile(p.x(), p.y());
    if (is_slime ? tile.is_solid_for_slime() : tile.is_solid())
    {
      return true;
    }
  }
  // Check colliding solid actors (closed doors)
  for (const auto& a : actors)
  {
    if (a->is_solid(*this) && geometry::isColliding(geometry::Rectangle(a->position, a->size), {position, size}))
    {
      return true;
    }
  }
  return false;
}

bool Level::collides_solid_top(const geometry::Position& position, const geometry::Size& size) const
{
  // Note: this function only works with size x and y <= 16
  // With size 16x16 the object can cover at maximum 4 tiles
  // Check all 4 tiles, even though we might check the same tile multiple times
  const std::array<geometry::Position, 4> positions = {
    geometry::Position((position.x() + 0) / 16, (position.y() + 0) / 16),
    geometry::Position((position.x() + size.x() - 1) / 16, (position.y() + 0) / 16),
    geometry::Position((position.x() + 0) / 16, (position.y() + size.y() - 1) / 16),
    geometry::Position((position.x() + size.x() - 1) / 16, (position.y() + size.y() - 1) / 16)};
  for (const auto& p : positions)
  {
    const auto& tile = get_tile(p.x(), p.y());
    if (tile.is_solid_top())
    {
      return true;
    }
  }
  return false;
}
