#include "enemy.h"

#include "hazard.h"
#include "level.h"


bool Enemy::on_hit(AbstractSoundManager& sound_manager,
                   [[maybe_unused]] const geometry::Rectangle& player_rect,
                   [[maybe_unused]] const Level& level,
                   const bool power)
{
  if (is_tough() && !power)
  {
    return true;
  }
  health -= power ? health : 1;
  // TODO: if hurt, enter flashing state
  if (is_alive())
  {
    sound_manager.play_sound(SoundType::SOUND_ENEMY_HURT);
  }
  return true;
}

void Enemy::on_death(AbstractSoundManager& sound_manager, [[maybe_unused]] Level& level)
{
  sound_manager.play_sound(SoundType::SOUND_ENEMY_DIE);
}

bool Enemy::should_reverse(const Level& level) const
{
  // Reverse direction if colliding left/right or about to fall (non-flying)
  // Note: falling looks at two points near the left- and right- bottom corners
  if (level.collides_solid(position, size))
  {
    return true;
  }
  if (!flying())
  {
    return (!level.collides_solid(position + geometry::Position(1, 1), geometry::Size(1, size.y())) &&
            !level.collides_solid_top(position + geometry::Position(1, 1), geometry::Size(1, size.y()))) ||
      (!level.collides_solid(position + geometry::Position(size.x() - 1, 1), geometry::Size(1, size.y())) &&
       !level.collides_solid_top(position + geometry::Position(size.x() - 1, 1), geometry::Size(1, size.y())));
  }
  return false;
}

void Bigfoot::update([[maybe_unused]] AbstractSoundManager& sound_manager, const geometry::Rectangle& player_rect, Level& level)
{
  frame_++;
  if (frame_ == 8)
  {
    frame_ = 0;
  }
  const int speed = running_ ? 4 : 2;
  const auto d = geometry::Position(left_ ? -speed : speed, 0);
  position += d;
  if (should_reverse(level))
  {
    left_ = !left_;
    running_ = false;
    position -= d;
  }
  // detect player/run
  if (geometry::is_any_colliding(get_detection_rects(level), player_rect))
  {
    running_ = true;
  }
}

bool Bigfoot::on_hit(AbstractSoundManager& sound_manager, const geometry::Rectangle& player_rect, const Level& level, const bool power)
{
  running_ = true;
  // Turn to face player
  if (left_ ^ (player_rect.position.x() < position.x()))
  {
    left_ = !left_;
  }
  return Enemy::on_hit(sound_manager, player_rect, level, power);
}

std::vector<std::pair<geometry::Position, Sprite>> Bigfoot::get_sprites([[maybe_unused]] const Level& level) const
{
  Sprite s = Sprite::SPRITE_BIGFOOT_HEAD_R_1;
  if (left_)
  {
    s = Sprite::SPRITE_BIGFOOT_HEAD_L_1;
  }
  const auto frame = running_ ? frame_ % 4 : frame_ / 2;
  return {
    std::make_pair(position, static_cast<Sprite>(static_cast<int>(s) + frame)),
    std::make_pair(position + geometry::Position(0, 16), static_cast<Sprite>(static_cast<int>(s) + 4 + frame)),
  };
}

void Hopper::update([[maybe_unused]] AbstractSoundManager& sound_manager,
                    [[maybe_unused]] const geometry::Rectangle& player_rect,
                    Level& level)
{
  frame_ += left_ ? -1 : 1;
  if (frame_ == 18)
  {
    frame_ = 0;
  }
  else if (frame_ <= 0)
  {
    frame_ = 17;
  }
  const auto d = geometry::Position(left_ ? -4 : 4, 0);
  position += d;
  if (next_reverse_ == 0 || should_reverse(level))
  {
    left_ = !left_;
    position -= d;
    // Change directions every 1-20 seconds
    next_reverse_ = 17 * (1 + static_cast<int>(rand() % 19));
  }
  next_reverse_--;
}

std::vector<std::pair<geometry::Position, Sprite>> Hopper::get_sprites([[maybe_unused]] const Level& level) const
{
  return {std::make_pair(position, static_cast<Sprite>(static_cast<int>(Sprite::SPRITE_HOPPER_1) + frame_))};
}

void Slime::update([[maybe_unused]] AbstractSoundManager& sound_manager,
                   [[maybe_unused]] const geometry::Rectangle& player_rect,
                   Level& level)
{
  frame_++;
  if (frame_ == 4)
  {
    frame_ = 0;
  }
  const auto d = geometry::Position(dx_, dy_) * 4;
  if (level.collides_solid(position + d, size, true))
  {
    // Randomly change direction
    switch (misc::random<int>(0, 4))
    {
      case 0:
        dx_ = 1;
        dy_ = 0;
        break;
      case 1:
        dx_ = -1;
        dy_ = 0;
        break;
      case 2:
        dx_ = 0;
        dy_ = 1;
        break;
      case 3:
        dx_ = 0;
        dy_ = -1;
        break;
      default:
        break;
    }
  }
  else
  {
    position += d;
  }
}

std::vector<std::pair<geometry::Position, Sprite>> Slime::get_sprites([[maybe_unused]] const Level& level) const
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
    s = Sprite::SPRITE_SLIME_D_1;
  }
  else
  {
    s = Sprite::SPRITE_SLIME_U_1;
  }
  return {std::make_pair(position, static_cast<Sprite>(static_cast<int>(s) + frame_))};
}

void Snake::update([[maybe_unused]] AbstractSoundManager& sound_manager,
                   [[maybe_unused]] const geometry::Rectangle& player_rect,
                   Level& level)
{
  // State changes / pause
  frame_++;
  if (paused_)
  {
    if (frame_ == 14)
    {
      paused_ = false;
      frame_ = 0;
    }
  }
  else if (frame_ == 100)
  {
    paused_ = true;
    frame_ = 0;
  }

  if (!paused_)
  {
    const auto d = geometry::Position(left_ ? -2 : 2, 0);
    position += d;
    if (should_reverse(level))
    {
      left_ = !left_;
      position -= d;
    }
  }
}

std::vector<std::pair<geometry::Position, Sprite>> Snake::get_sprites([[maybe_unused]] const Level& level) const
{
  const auto s = paused_ ? Sprite::SPRITE_SNAKE_PAUSE_1 : (left_ ? Sprite::SPRITE_SNAKE_WALK_L_1 : Sprite::SPRITE_SNAKE_WALK_R_1);
  const int frame = frame_ % (paused_ ? 7 : 9);
  return {std::make_pair(position, static_cast<Sprite>(static_cast<int>(s) + frame))};
}

void Snake::on_death(AbstractSoundManager& sound_manager, Level& level)
{
  Enemy::on_death(sound_manager, level);
  // Create a corpse
  level.hazards.emplace_back(new CorpseSlime(position, Sprite::SPRITE_SNAKE_SLIME));
  // TODO: authentic mode, align corpse to tile coord
}

void Spider::update([[maybe_unused]] AbstractSoundManager& sound_manager,
                    [[maybe_unused]] const geometry::Rectangle& player_rect,
                    [[maybe_unused]] Level& level)
{
  frame_++;
  if (frame_ == 8)
  {
    frame_ = 0;
  }
  const auto d = geometry::Position(0, up_ ? -2 : 2);
  position += d;
  if (level.collides_solid(position, size))
  {
    up_ = !up_;
    position -= d;
  }
  // fire webs
  if (child_ == nullptr && geometry::is_any_colliding(get_detection_rects(level), player_rect))
  {
    child_ = new SpiderWeb(position, *this);
    level.hazards.emplace_back(child_);
  }
}

std::vector<std::pair<geometry::Position, Sprite>> Spider::get_sprites([[maybe_unused]] const Level& level) const
{
  return {std::make_pair(position,
                         static_cast<Sprite>(static_cast<int>(up_ ? Sprite::SPRITE_SPIDER_UP_1 : Sprite::SPRITE_SPIDER_DOWN_1) + frame_))};
}

void Rockman::update([[maybe_unused]] AbstractSoundManager& sound_manager, const geometry::Rectangle& player_rect, Level& level)
{
  // Wake on detection
  if (asleep_)
  {
    const auto detection_rects = get_detection_rects(level);
    if (geometry::isColliding(detection_rects[0], player_rect))
    {
      asleep_ = false;
      frame_ = 0;
      left_ = true;
    }
    else if (geometry::isColliding(detection_rects[1], player_rect))
    {
      asleep_ = false;
      frame_ = 0;
      left_ = false;
    }
  }
  // TODO: sleep on going off camera
  if (!asleep_)
  {
    frame_++;
    // Frame 4 is initially standing up; don't walk yet
    // unless we're looping back to this frame
    if (frame_ > 4)
    {
      const auto d = geometry::Position(left_ ? -2 : 2, 0);
      position += d;
      if (should_reverse(level))
      {
        left_ = !left_;
        position -= d;
      }
    }
    if (frame_ == 12)
    {
      frame_ = 4;
    }
  }
}

std::vector<std::pair<geometry::Position, Sprite>> Rockman::get_sprites([[maybe_unused]] const Level& level) const
{
  const int frame = static_cast<int>(Sprite::SPRITE_ROCKMAN_L_1) + frame_ + (left_ ? 0 : 12);
  return {std::make_pair(position, static_cast<Sprite>(frame))};
}

std::vector<geometry::Rectangle> Rockman::get_detection_rects(const Level& level) const
{
  auto lr = create_detection_rects(-1, 0, level);
  auto rr = create_detection_rects(1, 0, level);
  lr.insert(lr.end(), rr.begin(), rr.end());
  return lr;
}

void MineCart::update([[maybe_unused]] AbstractSoundManager& sound_manager,
                      [[maybe_unused]] const geometry::Rectangle& player_rect,
                      Level& level)
{
  if (pause_frame_ > 0)
  {
    pause_frame_--;
  }
  else
  {
    frame_++;
    if (frame_ == 4)
    {
      frame_ = 0;
    }
    const auto d = geometry::Position(left_ ? -4 : 4, 0);
    position += d;
    if (should_reverse(level))
    {
      position -= d;
      pause_frame_ = 56;
      left_ = !left_;
    }
  }
}

std::vector<std::pair<geometry::Position, Sprite>> MineCart::get_sprites([[maybe_unused]] const Level& level) const
{
  return {std::make_pair(position,
                         pause_frame_ > 0 ? Sprite::SPRITE_MINE_CART_1
                                          : static_cast<Sprite>((static_cast<int>(Sprite::SPRITE_MINE_CART_1) + frame_)))};
}

Caterpillar::Caterpillar(geometry::Position position) : Enemy(position, geometry::Size(16, 16), 1) {}

void Caterpillar::update([[maybe_unused]] AbstractSoundManager& sound_manager,
                         [[maybe_unused]] const geometry::Rectangle& player_rect,
                         Level& level)
{
  frame_++;
  const auto d = geometry::Position(left_ ? -2 : 2, 0);
  position += d;
  if (should_reverse(level))
  {
    left_ = !left_;
    position -= d;
  }
  if (frame_ == 8)
  {
    frame_ = 0;
  }
}

std::vector<std::pair<geometry::Position, Sprite>> Caterpillar::get_sprites([[maybe_unused]] const Level& level) const
{
  Sprite base_sprite = Sprite::SPRITE_CATERPILLAR_L_HEAD_1;
  int flip_d = 10;
  if (rank_ == 3 || (rank_ > 0 && child_ == nullptr))
  {
    base_sprite = Sprite::SPRITE_CATERPILLAR_L_TAIL_1;
    flip_d = 2;
  }
  else if (rank_ > 0)
  {
    base_sprite = Sprite::SPRITE_CATERPILLAR_L_BODY_1;
    flip_d = 6;
  }
  const int frame = static_cast<int>(base_sprite) + (frame_ / 2 & 0x1) + (left_ ? 0 : flip_d);
  return {std::make_pair(position, static_cast<Sprite>(frame))};
}


void Caterpillar::on_death(AbstractSoundManager& sound_manager, Level& level)
{
  Enemy::on_death(sound_manager, level);

  // Decrease rank on all children
  for (auto child = child_; child != nullptr; child = child->child_)
  {
    child->rank_--;
  }
}

void Caterpillar::set_child(Caterpillar& child)
{
  child_ = &child;
  child.rank_ = rank_ + 1;
  // Initialise frame based on rank so we get a ^v^v pattern
  child.frame_ = child.rank_ * 2;
}

void Snoozer::update([[maybe_unused]] AbstractSoundManager& sound_manager,
                     [[maybe_unused]] const geometry::Rectangle& player_rect,
                     Level& level)
{
  if (pause_frame_ > 0)
  {
    pause_frame_--;
    if (pause_frame_ == 0)
    {
      // Roll for a random time
      frame_ = misc::random<int>(50, 100);
    }
  }
  else if (frame_ > 0)
  {
    frame_--;
    if (frame_ == 0)
    {
      pause_frame_ = 35;
    }
    const auto d = geometry::Position(left_ ? -4 : 4, 0);
    position += d;
    if (should_reverse(level))
    {
      position -= d;
      left_ = !left_;
    }
  }

  // TODO: shoot at player
}

std::vector<std::pair<geometry::Position, Sprite>> Snoozer::get_sprites([[maybe_unused]] const Level& level) const
{
  int frame = static_cast<int>(Sprite::SPRITE_SNOOZER_SLEEP);
  int dy = 0;
  if (frame_ > 0)
  {
    const int df = 3 - (frame_ / 2) % 4;
    if (df == 2)
    {
      // Correction for off by one sprite dy
      dy = 1;
    }
    frame = static_cast<int>(left_ ? Sprite::SPRITE_SNOOZER_L_1 : Sprite::SPRITE_SNOOZER_R_1) + df;
  }
  std::vector<std::pair<geometry::Position, Sprite>> sprites = {
    std::make_pair(position + geometry::Position{0, dy}, static_cast<Sprite>(frame))};
  if (pause_frame_ > 0)
  {
    const int z_frame = static_cast<int>(Sprite::SPRITE_SNOOZER_Z_1) + ((pause_frame_ / 3) % 4);
    sprites.push_back(std::make_pair(position - geometry::Position{0, 16}, static_cast<Sprite>(z_frame)));
  }
  return sprites;
}

void Triceratops::update([[maybe_unused]] AbstractSoundManager& sound_manager,
                         [[maybe_unused]] const geometry::Rectangle& player_rect,
                         Level& level)
{
  // TODO: shoot at the player
  // TODO: face player once hit
  frame_++;
  const auto d = geometry::Position(left_ ? -2 : 2, 0);
  position += d;
  if (should_reverse(level))
  {
    position -= d;
    left_ = !left_;
  }
}

std::vector<std::pair<geometry::Position, Sprite>> Triceratops::get_sprites([[maybe_unused]] const Level& level) const
{
  int frame = static_cast<int>(left_ ? Sprite::SPRITE_TRICERATOPS_HEAD_L_1 : Sprite::SPRITE_TRICERATOPS_TAIL_R_1) + ((frame_ / 2) % 4);
  int df = left_ ? 4 : -4;
  return {std::make_pair(position, static_cast<Sprite>(frame)),
          std::make_pair(position + geometry::Position{16, 0}, static_cast<Sprite>(frame + df)),
          std::make_pair(position + geometry::Position{32, 0}, static_cast<Sprite>(frame + df * 2))};
}

void Bat::update([[maybe_unused]] AbstractSoundManager& sound_manager,
                 [[maybe_unused]] const geometry::Rectangle& player_rect,
                 Level& level)
{
  frame_++;
  if (frame_ == 10)
  {
    frame_ = 0;
  }
  const auto d = geometry::Position(left_ ? -2 : 2, 0);
  position += d;
  if (next_reverse_ == 0 || should_reverse(level))
  {
    left_ = !left_;
    position -= d;
    // Change directions every 1-10 seconds
    next_reverse_ = 17 * (1 + static_cast<int>(rand() % 10));
  }
  next_reverse_--;
}

std::vector<std::pair<geometry::Position, Sprite>> Bat::get_sprites([[maybe_unused]] const Level& level) const
{
  return {std::make_pair(position, static_cast<Sprite>(static_cast<int>(Sprite::SPRITE_BAT_1) + frame_))};
}

void WallMonster::update([[maybe_unused]] AbstractSoundManager& sound_manager, const geometry::Rectangle& player_rect, Level& level)
{
  const auto detection_rects = get_detection_rects(level);
  if (geometry::isColliding(detection_rects[0], player_rect))
  {
    awake_ = true;
  }
  if (awake_ && frame_ == 16)
  {
    awake_ = false;
  }
  if (awake_ && frame_ < 16)
  {
    frame_++;
  }
  if (!awake_ && frame_ > 0)
  {
    frame_--;
  }
}

std::vector<std::pair<geometry::Position, Sprite>> WallMonster::get_sprites([[maybe_unused]] const Level& level) const
{
  return {std::make_pair(position,
                         static_cast<Sprite>(static_cast<int>(Sprite::SPRITE_WALL_MONSTER_R_1) + (left_ ? 9 : 0) + std::min(frame_, 8)))};
}

void Bird::update([[maybe_unused]] AbstractSoundManager& sound_manager,
                  [[maybe_unused]] const geometry::Rectangle& player_rect,
                  Level& level)
{
  frame_++;
  if (frame_ == 10)
  {
    frame_ = 0;
  }
  const auto d = geometry::Position(left_ ? -2 : 2, 0);
  position += d;
  if (should_reverse(level))
  {
    left_ = !left_;
    position -= d;
  }

  // TODO: lay eggs
}

std::vector<std::pair<geometry::Position, Sprite>> Bird::get_sprites([[maybe_unused]] const Level& level) const
{
  return {std::make_pair(position, static_cast<Sprite>(static_cast<int>(Sprite::SPRITE_BIRD_1) + frame_))};
}
