#include "missile.h"

#include "level.h"
#include "player.h"

constexpr decltype(Missile::size) Missile::size;
constexpr decltype(Missile::speed) Missile::speed;

void Missile::init(const Player& player)
{
  alive = true;
  frame = 0;
  is_power = player.power_tick > 0;
  if (player.direction == Player::Direction::right)
  {
    right = true;
    position = player.position + geometry::Position(player.size.x() - 2, 0);
  }
  else
  {
    right = false;
    position = player.position - geometry::Position(player.size.x() - 2, 0);
  }
  // TODO: shot sound
}

bool Missile::is_in_cooldown() const
{
  return alive || cooldown > 0;
}

void Missile::set_cooldown()
{
  cooldown = 7;
}

bool Missile::update(const Level& level)
{
  bool explode = false;
  if (alive)
  {
    const auto s = is_power ? 8 : (frame < speed.size() ? speed[frame] : speed.back());
    const int dx = right ? 1 : -1;
    // Adjust position due to collision size being smaller than sprite size
    const auto collision_position = position + geometry::Position(0, 3);
    for (int i = 0; i < s; i++)
    {
      position += geometry::Position(dx, 0);

      if (level.collides_solid(collision_position, size))
      {
        alive = false;
        set_cooldown();
        explode = true;
        // TODO: sound
        break;
      }

      auto enemy = level.collides_enemy(collision_position, size);
      if (enemy)
      {
        alive = false;

        enemy->on_hit(is_power);

        break;
      }
    }
  }

  if (cooldown > 0)
  {
    cooldown--;
  }
  if (alive)
  {
    frame++;
    // TODO: Missile doesn't disappear after a certain number of frames, but rather
    //       when it's outside of the screen. So we might need to make GameImpl aware
    //       of the game camera...
    if (frame > 27)
    {
      alive = false;
    }
  }

  return explode;
}

int Missile::get_sprite() const
{
  if (is_power)
  {
    return static_cast<int>(right ? Sprite::SPRITE_MISSILE_POWER_R_1 : Sprite::SPRITE_MISSILE_POWER_L_1);
  }
  return static_cast<int>(right ? Sprite::SPRITE_MISSILE_R_1 : Sprite::SPRITE_MISSILE_L_1);
}

int Missile::get_num_sprites() const
{
  return is_power ? 4 : 6;
}
