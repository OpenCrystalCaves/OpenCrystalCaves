#include "level.h"

#include "constants.h"

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

bool Level::collides_solid(const geometry::Position& position,
                           const geometry::Size& size,
                           const bool is_slime,
                           Actor** collides_actor) const
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
  // Check colliding solid actors
  for (auto& a : actors)
  {
    if (a->is_solid(*this) && geometry::isColliding(geometry::Rectangle(a->position, a->size), {position, size}))
    {
      if (collides_actor)
        *collides_actor = a.get();
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

/**
 * Checks if given position and size collides with any actor.
 *
 * Returns the first colliding actor, or null if none found.
 * TODO: combine this with collides enemy/hazard
 */
Actor* Level::collides_actor(const geometry::Position& position, const geometry::Size& size) const
{
  const auto rect = geometry::Rectangle(position, size);
  for (auto&& actor : actors)
  {
    if (geometry::isColliding(rect, geometry::Rectangle(actor->position, actor->size)))
    {
      return actor.get();
    }
  }
  return nullptr;
}

Hazard* Level::collides_hazard(const geometry::Position& position, const geometry::Size& size) const
{
  const auto rect = geometry::Rectangle(position, size);
  for (auto&& hazard : hazards)
  {
    if (geometry::isColliding(rect, geometry::Rectangle(hazard->position, hazard->size)))
    {
      return hazard.get();
    }
  }
  return nullptr;
}

/**
 * Checks if given position and size collides with any enemy.
 *
 * Returns the first colliding enemy, or null if none found.
 */
Enemy* Level::collides_enemy(const geometry::Position& position, const geometry::Size& size) const
{
  const auto rect = geometry::Rectangle(position, size);
  for (auto&& enemy : enemies)
  {
    if (geometry::isColliding(rect, geometry::Rectangle(enemy->position, enemy->size)))
    {
      return enemy.get();
    }
  }
  return nullptr;
}

bool Level::player_on_platform(const geometry::Position& position, const geometry::Size& size) const
{
  // Need to check both static platforms (e.g. foreground items with SOLID_TOP)
  // and moving platforms and hazards that are solid on top.

  // Standing on a static platform requires the player to stand on the edge of a tile
  if ((position.y() + size.y() - 1) % SPRITE_H == 0)
  {
    // Player can be on either 1 or 2 tiles, check both (or same...)
    if (get_tile(position.x() / SPRITE_W, (position.y() + size.y() - 1) / SPRITE_H).is_solid_top() ||
        get_tile((position.x() + size.x()) / SPRITE_W, (position.y() + size.y() - 1) / SPRITE_H).is_solid_top())
    {
      return true;
    }
  }

  // Check moving platforms
  for (const auto& platform : moving_platforms)
  {
    // Player only collides if standing exactly on top of the platform, just like with static platforms
    if (platform.is_moving && (position.y() + size.y() - 1 == platform.position.y()) && (position.x() < platform.position.x() + SPRITE_W) &&
        (position.x() + size.x() > platform.position.x()))
    {
      return true;
    }
  }

  // Check hazards that are solid on top
  for (const auto& hazard : hazards)
  {
    if (hazard->is_solid_top(*this) && (position.y() + size.y() - 1 == hazard->position.y()) &&
        (position.x() < hazard->position.x() + hazard->size.x()) && (position.x() + size.x() > hazard->position.x()))
    {
      return true;
    }
  }

  return false;
}
