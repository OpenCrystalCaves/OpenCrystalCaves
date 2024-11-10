#include "hazard.h"

#include "level.h"

void AirTank::update([[maybe_unused]] const geometry::Rectangle& player_rect, [[maybe_unused]] Level& level)
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

void Laser::update(const geometry::Rectangle& player_rect, Level& level)
{
  if (child_ == nullptr && geometry::is_any_colliding(get_detection_rects(level), player_rect))
  {
    geometry::Position child_pos = position + geometry::Position(left_ ? -12 : 12, -1);
    child_ = new LaserBeam(child_pos, left_, *this);
    level.hazards.emplace_back(child_);
  }
}

void LaserBeam::update([[maybe_unused]] const geometry::Rectangle& player_rect, Level& level)
{
  frame_ = 1 - frame_;
  position += geometry::Position(left_ ? -4 : 4, 0);
  if (level.collides_solid(position + geometry::Position(0, 1), geometry::Size(16, 16)))
  {
    alive_ = false;
    parent_.remove_child();
  }
}

void Thorn::update(const geometry::Rectangle& player_rect, Level& level)
{
  if (geometry::is_any_colliding(get_detection_rects(level), player_rect))
  {
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

void SpiderWeb::update([[maybe_unused]] const geometry::Rectangle& player_rect, Level& level)
{
  position += geometry::Position(0, 4);
  if (level.collides_solid(position + geometry::Position(0, -6), geometry::Size(16, 16)))
  {
    alive_ = false;
    parent_.remove_child();
  }

  // TODO: hurt player
}

void CorpseSlime::update([[maybe_unused]] const geometry::Rectangle& player_rect, [[maybe_unused]] Level& level)
{
  // TODO: hurt player
}

void Faucet::update([[maybe_unused]] const geometry::Rectangle& player_rect, Level& level)
{
  if (child_ == nullptr)
  {
    frame_++;
    if (frame_ == 6)
    {
      frame_ = 0;
      // TODO: make sound
      geometry::Position child_pos = position + geometry::Position(0, 8);
      child_ = new Droplet(child_pos, *this);
      level.hazards.emplace_back(child_);
    }
  }
}

void Droplet::update([[maybe_unused]] const geometry::Rectangle& player_rect, Level& level)
{
  frame_ = 1 - frame_;
  position += geometry::Position(0, 6);
  if (level.collides_solid(position, geometry::Size(16, 16)) || level.collides_solid_top(position, geometry::Size(16, 16)))
  {
    // TODO: leave alive for one more frame but don't hurt player
    alive_ = false;
    parent_.remove_child();
  }
}