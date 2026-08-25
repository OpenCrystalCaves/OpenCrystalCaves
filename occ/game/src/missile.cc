#include "missile.h"

#include "level.h"
#include "player.h"

constexpr decltype(Missile::size) Missile::size;
constexpr decltype(Missile::speed) Missile::speed;

void Missile::init(AbstractSoundManager& sound_manager, const Player& player)
{
  alive = true;
  killed_enemy = false;
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
  sound_manager.play_sound(is_power ? SoundType::SOUND_POWER_FIRE : SoundType::SOUND_FIRE);
}

bool Missile::is_in_cooldown() const
{
  return alive || cooldown > 0;
}

void Missile::set_cooldown()
{
  cooldown = 7;
}

bool Missile::update(AbstractSoundManager& sound_manager, const geometry::Rectangle& player_rect, Level& level)
{
  bool explode = false;
  if (alive)
  {
    const auto s = is_power ? 8 : (frame < speed.size() ? speed[frame] : speed.back());
    const int dx = right ? 1 : -1;
    for (int i = 0; i < s; i++)
    {
      position += geometry::Position(dx, 0);
      // Adjust position due to collision size being smaller than sprite size
      const auto collision_position = position + geometry::Position(0, 3);
      const auto crect = geometry::Rectangle{collision_position, size};

      // Check colliding solid actors (closed doors)
      const auto rect = geometry::Rectangle(collision_position, size);
      Actor* actorp = nullptr;
      for (auto&& actor : level.actors)
      {
        if (geometry::isColliding(rect, geometry::Rectangle(actor->position, actor->size)) &&
            actor->on_hit(crect, sound_manager, player_rect, level, is_power))
        {
          actorp = actor.get();
          break;
        }
      }
      if (actorp)
      {
        alive = false;
        set_cooldown();
        explode = true;
        sound_manager.play_sound(is_power ? SoundType::SOUND_POWER_FIRE : SoundType::SOUND_FIRE);
        break;
      }

      Enemy* enemyp = nullptr;
      for (auto&& enemy : level.enemies)
      {
        if (geometry::isColliding(rect, geometry::Rectangle(enemy->position, enemy->size)) &&
            enemy->on_hit(crect, sound_manager, player_rect, level, is_power))
        {
          enemyp = enemy.get();
          break;
        }
      }
      if (enemyp)
      {
        alive = false;
        // If enemy killed, spawn explosion
        auto explosion_sprites = enemyp->get_explosion_sprites();
        if (explosion_sprites && !enemyp->is_alive())
        {
          level.particles.emplace_back(new Explosion(position, *explosion_sprites, right ? 2 : -2));
          killed_enemy = true;
        }
        break;
      }

      Hazard* hazardp = nullptr;
      for (auto&& hazard : level.hazards)
      {
        if (geometry::isColliding(rect, geometry::Rectangle(hazard->position, hazard->size)) &&
            hazard->on_hit(crect, sound_manager, player_rect, level, is_power))
        {
          hazardp = hazard.get();
          break;
        }
      }
      if (hazardp)
      {
        alive = false;
        // If hazard killed, spawn explosion
        auto explosion_sprites = hazardp->get_explosion_sprites();
        if (explosion_sprites && !hazardp->is_alive())
        {
          level.particles.emplace_back(new Explosion(position, *explosion_sprites, right ? 2 : -2));
          killed_enemy = true;
        }
        break;
      }

      if (level.collides_solid(collision_position, size))
      {
        alive = false;
        set_cooldown();
        explode = true;
        // TODO: sound
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
