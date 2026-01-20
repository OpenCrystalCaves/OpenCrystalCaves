#include "hazard.h"

#include "level.h"
#include "player.h"

void Laser::update(AbstractSoundManager& sound_manager, const geometry::Rectangle& player_rect, Level& level)
{
  const bool can_fire = moving_ || (level.switch_flags & SWITCH_FLAG_LASERS);
  if (can_fire && child_ == nullptr && geometry::is_any_colliding(get_detection_rects(level), player_rect))
  {
    geometry::Position child_pos = position + geometry::Position(left_ ? -6 : 6, -1);
    child_ = new LaserBeam(child_pos, left_, *this);
    level.hazards.emplace_back(child_);
    sound_manager.play_sound(SoundType::SOUND_LASER_FIRE);
  }
  if (moving_)
  {
    int dy = down_ ? 2 : -2;
    if (level.collides_solid(position + geometry::Position{0, dy}, size))
    {
      down_ = !down_;
      dy = -dy;
    }
    position += geometry::Position{0, dy};
  }
}

std::vector<geometry::Rectangle> Laser::get_detection_rects(const Level& level) const
{
  // Create a detection line centered vertically
  std::vector<geometry::Rectangle> rects;
  const int y = position.y() + size.y() / 2;
  if (left_)
  {
    geometry::Rectangle r{position.x(), y, 0, 0};
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
  else  // Right
  {
    geometry::Rectangle r{position.x() + size.x(), y, 0, 0};
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
  return rects;
}

void Projectile::update([[maybe_unused]] AbstractSoundManager& sound_manager,
                        [[maybe_unused]] const geometry::Rectangle& player_rect,
                        Level& level)
{
  frame_++;
  if (frame_ == num_sprites())
  {
    frame_ = 0;
  }
  position += geometry::Position(left_ ? -get_speed() : get_speed(), 0);
  if (level.collides_solid(position, size))
  {
    kill(level);
  }
}

std::vector<ObjectDef> Projectile::get_sprites([[maybe_unused]] const Level& level) const
{
  return {{position - geometry::Size(4, 4), static_cast<int>(get_sprite()) + frame_, false}};
}

void LaserBeam::update(AbstractSoundManager& sound_manager, const geometry::Rectangle& player_rect, Level& level)
{
  if (moving_)
  {
    Projectile::update(sound_manager, player_rect, level);
  }
  else
  {
    // Kill after 2 frames
    frame_++;
    if (frame_ == num_sprites())
    {
      frame_ = 0;
    }
    kill_frame_++;
    if (kill_frame_ == 3)
    {
      alive_ = false;
      parent_.remove_child(level);
    }
  }
}

void Thorn::update(AbstractSoundManager& sound_manager, const geometry::Rectangle& player_rect, Level& level)
{
  if (geometry::is_any_colliding(get_detection_rects(level), player_rect))
  {
    if (frame_ == 0)
    {
      sound_manager.play_sound(SoundType::SOUND_THORN);
    }
    if (frame_ < 4)
    {
      frame_++;
    }
  }
  else
  {
    frame_ = 0;
  }
}

void SpiderWeb::update([[maybe_unused]] AbstractSoundManager& sound_manager,
                       [[maybe_unused]] const geometry::Rectangle& player_rect,
                       Level& level)
{
  position += geometry::Position(0, 4);
  if (level.collides_solid(position + geometry::Position(0, -6), geometry::Size(16, 16)))
  {
    alive_ = false;
    parent_.remove_child();
  }
}

void SpiderWeb::kill()
{
  alive_ = false;
  parent_.remove_child();
}

void Faucet::update([[maybe_unused]] AbstractSoundManager& sound_manager,
                    [[maybe_unused]] const geometry::Rectangle& player_rect,
                    Level& level)
{
  if (child_ == nullptr)
  {
    frame_++;
    if (frame_ == 6)
    {
      frame_ = 0;
      geometry::Position child_pos = position + geometry::Position(0, 8);
      child_ = new Droplet(child_pos, *this);
      level.hazards.emplace_back(child_);
    }
  }
}

void Droplet::update([[maybe_unused]] AbstractSoundManager& sound_manager,
                     [[maybe_unused]] const geometry::Rectangle& player_rect,
                     Level& level)
{
  frame_ = 1 - frame_;
  position += geometry::Position(0, 6);
  if (level.collides_solid(position, size) || level.collides_solid_top(position, size))
  {
    // TODO: make sound
    // TODO: leave alive for one more frame but don't hurt player
    alive_ = false;
    parent_.remove_child();
  }
}


void Hammer::update(AbstractSoundManager& sound_manager,
                    [[maybe_unused]] const geometry::Rectangle& player_rect,
                    [[maybe_unused]] Level& level)
{
  constexpr int BOTTOM_FRAMES = 18;
  if (frame_ > 0)
  {
    // Hammer at bottom
    frame_--;
    if (frame_ == BOTTOM_FRAMES - 1)
    {
      // Slight recoil
      position -= geometry::Position(0, 2);
    }
    else if (frame_ == BOTTOM_FRAMES - 2)
    {
      // Return to bottom
      position += geometry::Position(0, 2);
    }
    else if (frame_ == 0)
    {
      // Rise again
      rising_ = true;
    }
  }
  else if (rising_)
  {
    // Rising
    position -= geometry::Position(0, 2);
    if (level.collides_solid(position, size))
    {
      position += geometry::Position(0, 2);
      rising_ = false;
    }
  }
  else
  {
    // Falling
    position += geometry::Position(0, 8);
    if (level.collides_solid(position, size))
    {
      position -= geometry::Position(0, 8);
      frame_ = BOTTOM_FRAMES;
      sound_manager.play_sound(SoundType::SOUND_HAMMER);
    }
  }
}

constexpr int FLAME_FRAME_TOTAL = 84;
constexpr int FLAME_OFF_FRAMES = 28;

void Flame::update([[maybe_unused]] AbstractSoundManager& sound_manager,
                   [[maybe_unused]] const geometry::Rectangle& player_rect,
                   [[maybe_unused]] Level& level)
{
  frame_++;
  if (frame_ == FLAME_FRAME_TOTAL)
  {
    frame_ = 0;
  }
}

std::vector<ObjectDef> Flame::get_sprites([[maybe_unused]] const Level& level) const
{
  if (!is_on())
    return {{position, static_cast<int>(Sprite::SPRITE_FLAME_0), false}};
  const int d = (frame_ - FLAME_OFF_FRAMES) / 2;
  int sprite = static_cast<int>(Sprite::SPRITE_FLAME_4) + (d % 4);
  if (d <= 2)
  {
    sprite = static_cast<int>(Sprite::SPRITE_FLAME_1) + d;
  }
  else if (d >= (FLAME_FRAME_TOTAL - FLAME_OFF_FRAMES) / 2 - 3)
  {
    sprite = static_cast<int>(Sprite::SPRITE_FLAME_1) + ((FLAME_FRAME_TOTAL - FLAME_OFF_FRAMES) / 2 - d);
  }
  return {{position, sprite, false}};
}

bool Flame::is_on() const
{
  return frame_ >= FLAME_OFF_FRAMES;
}

void Stalactite::update(AbstractSoundManager& sound_manager, const geometry::Rectangle& player_rect, Level& level)
{
  if (asleep_ && geometry::is_any_colliding(get_detection_rects(level), player_rect))
  {
    asleep_ = false;
    sound_manager.play_sound(SoundType::SOUND_STALACTITE_FALL);
  }
  if (!asleep_)
  {
    // TODO: see if we can kill on collision
    position += geometry::Position{0, 8};
  }
}

void AirPipe::update([[maybe_unused]] AbstractSoundManager& sound_manager, const geometry::Rectangle& player_rect, Level& level)
{
  frame_++;
  if (frame_ == 6)
  {
    frame_ = 0;
  }
  if (geometry::is_any_colliding(get_detection_rects(level), player_rect))
  {
    level.dv = geometry::Position{is_left_ ? 1 : -1, 0};
  }
}

std::vector<ObjectDef> AirPipe::get_sprites([[maybe_unused]] const Level& level) const
{
  if (is_left_)
  {
    return {
      {position, static_cast<int>(Sprite::SPRITE_AIR_L_1) + (frame_ % 3), false},
      {position + geometry::Position(16, 0), static_cast<int>(Sprite::SPRITE_AIR_PIPE_L_1) + (frame_ % 2), false},
    };
  }
  else
  {
    return {
      {position, static_cast<int>(Sprite::SPRITE_AIR_PIPE_R_1) + (frame_ % 2), false},
      {position + geometry::Position(16, 0), static_cast<int>(Sprite::SPRITE_AIR_R_1) + (frame_ % 3), false},
    };
  }
}

TouchType AirPipe::on_touch(const Player& player, AbstractSoundManager& sound_manager, Level& level)
{
  geometry::Rectangle soft_attract;
  geometry::Rectangle hard_attract;
  int dx;
  if (is_left_)
  {
    dx = 1;
    soft_attract = geometry::Rectangle{position, geometry::Size{16, 16}};
    hard_attract = geometry::Rectangle{position + geometry::Position{16, 0}, geometry::Size{16, 16}};
  }
  else
  {
    dx = -1;
    soft_attract = geometry::Rectangle{position + geometry::Position{16, 0}, geometry::Size{16, 16}};
    hard_attract = geometry::Rectangle{position, geometry::Size{16, 16}};
  }

  if (geometry::isColliding(player.rect(), hard_attract))
  {
    level.dv = {dx * 6, 0};
    sound_manager.play_sound(SoundType::SOUND_DIE);
    return TouchType::TOUCH_TYPE_DIE;
  }
  else if (geometry::isColliding(player.rect(), soft_attract))
  {
    level.dv = {dx * 3, 0};
  }
  return TouchType::TOUCH_TYPE_NONE;
}

void BirdEgg::update(AbstractSoundManager& sound_manager, [[maybe_unused]] const geometry::Rectangle& player_rect, Level& level)
{
  position += geometry::Position(0, 6);
  if (level.collides_solid(position, geometry::Size(16, 16)))
  {
    alive_ = false;
    sound_manager.play_sound(SoundType::SOUND_HAMMER);
    const auto child_pos = geometry::Position(position.x(), (position.y() / 16) * 16);
    if (misc::random<int>(0, 10) == 0)
    {
      // Spawn birdlet
      auto birdlet = new Birdlet(child_pos, parent_);
      parent_.set_child(birdlet);
      level.enemies.emplace_back(birdlet);
    }
    else
    {
      // Spawn open egg
      auto open_egg = new BirdEggOpen(child_pos, parent_);
      parent_.set_child(open_egg);
      level.hazards.emplace_back(open_egg);
    }
  }
}

void BirdEggOpen::update([[maybe_unused]] AbstractSoundManager& sound_manager,
                         [[maybe_unused]] const geometry::Rectangle& player_rect,
                         [[maybe_unused]] Level& level)
{
  frame_++;
  if (!is_alive())
  {
    sound_manager.play_sound(SoundType::SOUND_HAMMER);
    parent_.remove_child();
  }
}

void FallingSign::update([[maybe_unused]] AbstractSoundManager& sound_manager,
                         [[maybe_unused]] const geometry::Rectangle& player_rect,
                         Level& level)
{
  if (landed_)
    return;
  if (!falling_)
  {
    if (geometry::is_any_colliding(get_detection_rects(level), player_rect))
    {
      falling_ = true;
    }
  }
  if (falling_)
  {
    position += geometry::Position{0, 4};
    if (level.collides_solid_top(position + geometry::Position{0, 4}, size))
    {
      sound_manager.play_sound(SoundType::SOUND_HAMMER);
      landed_ = true;
      falling_ = false;
    }
  }
}

std::vector<ObjectDef> FallingSign::get_sprites([[maybe_unused]] const Level& level) const
{
  std::vector<ObjectDef> result;
  for (size_t i = 0; i < sprites_.size(); i++)
  {
    result.emplace_back(position + geometry::Position(static_cast<int>(i) * 16, 0), static_cast<int>(sprites_[i]), false);
  }
  return result;
}