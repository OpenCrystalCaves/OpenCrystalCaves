#include "moving_platform.h"

#include "level.h"

MovingPlatform::MovingPlatform(geometry::Position position, const bool horizontal, const bool controlled)
  : position(position),
    sprite_id(static_cast<int>(horizontal ? Sprite::SPRITE_PLATFORM_H_1 : Sprite::SPRITE_PLATFORM_V_1)),
    num_sprites(4),
    velocity(horizontal ? Vector<int>(1, 0) * 2 : Vector<int>(0, 1) * 2),
    controlled_(controlled)
{
}

void MovingPlatform::update(const Level& level)
{
  if (is_moving(level))
  {
    // Move platform
    const auto new_platform_pos = collide_position() + velocity;

    if (level.collides_solid(new_platform_pos, collide_size))
    {
      // Change direction
      velocity = -velocity;
    }
    position += velocity;
  }
}


Vector<int> MovingPlatform::get_velocity(const Level& level) const
{
  return is_moving(level) ? velocity : Vector<int>(0, 0);
}

bool MovingPlatform::is_moving(const Level& level) const
{
  return !controlled_ || (level.switch_flags & SWITCH_FLAG_MOVING_PLATFORMS);
}
