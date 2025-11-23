#include "moving_platform.h"

#include "level.h"

MovingPlatform::MovingPlatform(geometry::Position position, const bool horizontal, const bool controlled)
  : position(position),
    sprite_id_(static_cast<int>(horizontal ? Sprite::SPRITE_PLATFORM_H_1 : Sprite::SPRITE_PLATFORM_V_1)),
    velocity(horizontal ? Vector<int>(1, 0) * 2 : Vector<int>(0, 1) * 2),
    controlled_(controlled)
{
}

int MovingPlatform::get_sprite() const
{
  const auto d = is_moving ? ticks_ % 4 : 0;
  return (is_reverse() ? 4 - 1 - d : d) + sprite_id_;
}

void MovingPlatform::update(const Level& level)
{
  is_moving = !controlled_ || (level.switch_flags & SWITCH_FLAG_MOVING_PLATFORMS);
  if (is_moving)
  {
    // Move platform
    const auto new_platform_pos = collide_position() + velocity;

    if (level.collides_solid(new_platform_pos, collide_size) ||
        // Don't leave the top of level
        new_platform_pos.y() < 0)
    {
      // Change direction
      velocity = -velocity;
    }
    position += velocity;
  }
  ticks_++;
}


Vector<int> MovingPlatform::get_velocity([[maybe_unused]] const Level& level) const
{
  return is_moving ? velocity : Vector<int>(0, 0);
}
