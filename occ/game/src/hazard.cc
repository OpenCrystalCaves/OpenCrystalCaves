#include "hazard.h"

#include "level.h"

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
    geometry::Position child_pos = position + geometry::Position(left_ ? -12 : 12, -1);
    child_ = new LaserBeam(child_pos, left_, *this);
    level.hazards.emplace_back(child_);
    sound_manager.play_sound(SoundType::SOUND_LASER_FIRE);
  }
}

void LaserBeam::update([[maybe_unused]] AbstractSoundManager& sound_manager,
                       [[maybe_unused]] const geometry::Rectangle& player_rect,
                       Level& level)
{
  frame_ = 1 - frame_;
  position += geometry::Position(left_ ? -4 : 4, 0);
  if (level.collides_solid(position + geometry::Position(0, 1), geometry::Size(16, 16)))
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
