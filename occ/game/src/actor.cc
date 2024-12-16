#include "actor.h"

#include "level.h"

std::vector<geometry::Rectangle> Actor::create_detection_rects(const int dx,
                                                               const int dy,
                                                               const Level& level,
                                                               const bool include_self) const
{
  // Create rectangles originating from this actor extending toward a cardinal direction,
  // until there is a solid collision.
  std::vector<geometry::Rectangle> rects;
  if (dx == 1)
  {
    // right
    int y = position.y();
    for (int remain = size.y(); remain > 0; remain -= 16)
    {
      const int h = std::min(remain, 16);
      geometry::Rectangle r{position.x() + (include_self ? 0 : 16), y, 0, h};
      y += h;
      for (;;)
      {
        auto r_new = r;
        r_new.size += geometry::Size{16, 0};
        // TODO: check camera for inside
        if (level.collides_solid(r_new.position, r_new.size) ||
            !geometry::is_inside(r_new, geometry::Rectangle(0, 0, level.width * 16, level.height * 16)))
        {
          rects.push_back(r);
          break;
        }
        r = r_new;
      }
    }
  }
  else if (dx == -1)
  {
    // left
    int y = position.y();
    for (int remain = size.y(); remain > 0; remain -= 16)
    {
      const int h = std::min(remain, 16);
      geometry::Rectangle r{position.x() + (include_self ? 16 : 0), y, 0, h};
      y += h;
      for (;;)
      {
        auto r_new = r;
        r_new.position -= geometry::Position{16, 0};
        r_new.size += geometry::Size{16, 0};
        if (level.collides_solid(r_new.position, r_new.size) ||
            !geometry::is_inside(r_new, geometry::Rectangle(0, 0, level.width * 16, level.height * 16)))
        {
          rects.push_back(r);
          break;
        }
        r = r_new;
      }
    }
  }
  else if (dy == 1)
  {
    // down
    int x = position.x();
    for (int remain = size.x(); remain > 0; remain -= 16)
    {
      const int w = std::min(remain, 16);
      geometry::Rectangle r{x, position.y() + (include_self ? 0 : 16), w, 0};
      x += w;
      for (;;)
      {
        auto r_new = r;
        r_new.size += geometry::Size{0, 16};
        if (level.collides_solid(r_new.position, r_new.size) ||
            !geometry::is_inside(r_new, geometry::Rectangle(0, 0, level.width * 16, level.height * 16)))
        {
          rects.push_back(r);
          break;
        }
        r = r_new;
      }
    }
  }
  else if (dy == -1)
  {
    // up
    int x = position.x();
    for (int remain = size.x(); remain > 0; remain -= 16)
    {
      const int w = std::min(remain, 16);
      geometry::Rectangle r{x, position.y() + (include_self ? 16 : 0), w, 0};
      x += w;
      for (;;)
      {
        auto r_new = r;
        r_new.position -= geometry::Position{0, 16};
        r_new.size += geometry::Size{0, 16};
        if (level.collides_solid(r_new.position, r_new.size) ||
            !geometry::is_inside(r_new, geometry::Rectangle(0, 0, level.width * 16, level.height * 16)))
        {
          rects.push_back(r);
          break;
        }
        r = r_new;
      }
    }
  }
  return rects;
}

bool Lever::interact(Level& level)
{
  if (!level.lever_on.test(static_cast<size_t>(color_)))
  {
    level.lever_on.set(static_cast<size_t>(color_));
    // TODO: play on sound
    return true;
  }
  return false;
}

std::vector<std::pair<geometry::Position, Sprite>> Lever::get_sprites(const Level& level) const
{
  const int sprite =
    static_cast<int>(Sprite::SPRITE_LEVER_R_OFF) + level.lever_on.test(static_cast<size_t>(color_)) + 2 * static_cast<int>(color_);
  return {{position, static_cast<Sprite>(sprite)}};
}

bool Door::is_solid(const Level& level) const
{
  return !level.lever_on.test(static_cast<size_t>(color_));
}

std::vector<std::pair<geometry::Position, Sprite>> Door::get_sprites(const Level& level) const
{
  if (level.lever_on.test(static_cast<size_t>(color_)))
  {
    // Open
    const int sprite = static_cast<int>(Sprite::SPRITE_DOOR_OPEN_R_1) + 2 * static_cast<int>(color_);
    return {
      {position, static_cast<Sprite>(sprite)},
      {position + geometry::Position(0, 16), static_cast<Sprite>(sprite + 1)},
    };
  }
  else
  {
    // Closed
    const int sprite = static_cast<int>(Sprite::SPRITE_DOOR_CLOSED_R_1) + static_cast<int>(color_);
    return {
      {position, static_cast<Sprite>(sprite)},
      {position + geometry::Position(0, 16), static_cast<Sprite>(sprite + 4)},
    };
  }
}

bool Switch::interact(Level& level)
{
  level.switch_flags ^= switch_flag_;
  // TODO: play on sound
  return true;
}

std::vector<std::pair<geometry::Position, Sprite>> Switch::get_sprites(const Level& level) const
{
  return {{position, static_cast<Sprite>(static_cast<int>(sprite_) + static_cast<int>(!!(level.switch_flags & switch_flag_)))}};
}

bool Chest::interact(Level& level)
{
  if (!level.has_key)
  {
    return false;
  }
  // TODO: play on sound
  return true;
}

std::vector<std::pair<geometry::Position, Sprite>> Chest::get_sprites(const Level& level) const
{
  return {{position, collected_ ? Sprite::SPRITE_CHEST_OPEN : Sprite::SPRITE_CHEST_CLOSED}};
}
