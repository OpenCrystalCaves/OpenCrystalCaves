#include "enemy.h"

void Hopper::update()
{
  frame_++;
  if (frame_ == 18)
  {
    frame_ = 0;
  }
  // TODO: move, randomly change directions
}

Sprite Hopper::get_sprite() const
{
  return static_cast<Sprite>(static_cast<int>(Sprite::SPRITE_HOPPER_1) + frame_);
}

void Slime::update()
{
  frame_++;
  if (frame_ == 4)
  {
    frame_ = 0;
  }
  // TODO: move, pause
}

Sprite Slime::get_sprite() const
{
  Sprite s = Sprite::SPRITE_SLIME_R_1;
  if (dx_ == 1)
  {
    s = Sprite::SPRITE_SLIME_R_1;
  }
  else if (dx_ == -1)
  {
    s = Sprite::SPRITE_SLIME_L_1;
  }
  else if (dy_ == 1)
  {
    s = Sprite::SPRITE_SLIME_U_1;
  }
  else
  {
    s = Sprite::SPRITE_SLIME_D_1;
  }
  return static_cast<Sprite>(static_cast<int>(s) + frame_);
}

void Snake::update()
{
  frame_++;
  if (frame_ >= (paused_ ? 7 : 9))
  {
    frame_ = 0;
  }
  // TODO: move, pause
}

Sprite Snake::get_sprite() const
{
  const auto s = paused_ ? Sprite::SPRITE_SNAKE_PAUSE_1 : (left_ ? Sprite::SPRITE_SNAKE_WALK_L_1 : Sprite::SPRITE_SNAKE_WALK_R_1);
  return static_cast<Sprite>(static_cast<int>(s) + frame_);
}

void Spider::update()
{
  frame_++;
  if (frame_ == 8)
  {
    frame_ = 0;
  }
  // TODO: move, fire webs
}

Sprite Spider::get_sprite() const
{
  return static_cast<Sprite>(static_cast<int>(up_ ? Sprite::SPRITE_SPIDER_UP_1 : Sprite::SPRITE_SPIDER_DOWN_1) + frame_);
}
