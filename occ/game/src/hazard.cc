#include "hazard.h"

#include "level.h"
#include "player.h"

void AirTank::update([[maybe_unused]] AbstractSoundManager& sound_manager,
                     [[maybe_unused]] const geometry::Rectangle& player_rect,
                     [[maybe_unused]] Level& level)
{
  // TODO: check if shot
  frame_++;
  if (frame_ == 2)
  {
    frame_ = 0;
  }
}

std::vector<std::pair<geometry::Position, Sprite>> AirTank::get_sprites([[maybe_unused]] const Level& level) const
{
  return {std::make_pair(
    position, top_ ? static_cast<Sprite>(static_cast<int>(Sprite::SPRITE_AIR_TANK_TOP_1) + frame_) : Sprite::SPRITE_AIR_TANK_BOTTOM)};
}

void Laser::update(AbstractSoundManager& sound_manager, const geometry::Rectangle& player_rect, Level& level)
{
  if ((level.switch_flags & SWITCH_FLAG_LASERS) && child_ == nullptr && geometry::is_any_colliding(get_detection_rects(level), player_rect))
  {
    geometry::Position child_pos = position + geometry::Position((left_ ? -6 : 6) + 4, 3);
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
  else if (left_)
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

void LaserBeam::update([[maybe_unused]] AbstractSoundManager& sound_manager,
                       [[maybe_unused]] const geometry::Rectangle& player_rect,
                       Level& level)
{
  frame_ = 1 - frame_;
  position += geometry::Position(left_ ? -4 : 4, 0);
  // TODO: reduce hitbox
  // TODO: kill outside window
  if (level.collides_solid(position + geometry::Position(0, 1), size))
  {
    alive_ = false;
    parent_.remove_child();
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

std::vector<std::pair<geometry::Position, Sprite>> Flame::get_sprites([[maybe_unused]] const Level& level) const
{
  if (!is_on())
    return {std::make_pair(position, Sprite::SPRITE_FLAME_0)};
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
  return {std::make_pair(position, static_cast<Sprite>(sprite))};
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

std::vector<std::pair<geometry::Position, Sprite>> AirPipe::get_sprites([[maybe_unused]] const Level& level) const
{
  if (is_left_)
  {
    return {
      std::make_pair(position, static_cast<Sprite>(static_cast<int>(Sprite::SPRITE_AIR_L_1) + (frame_ % 3))),
      std::make_pair(position + geometry::Position(16, 0),
                     static_cast<Sprite>(static_cast<int>(Sprite::SPRITE_AIR_PIPE_L_1) + (frame_ % 2))),
    };
  }
  else
  {
    return {
      std::make_pair(position, static_cast<Sprite>(static_cast<int>(Sprite::SPRITE_AIR_PIPE_R_1) + (frame_ % 2))),
      std::make_pair(position + geometry::Position(16, 0), static_cast<Sprite>(static_cast<int>(Sprite::SPRITE_AIR_R_1) + (frame_ % 3))),
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
