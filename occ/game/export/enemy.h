#pragma once
#include <utility>

#include "actor.h"
#include "geometry.h"
#include "misc.h"
#include "particle.h"
#include "sprite.h"

struct Level;
class Hazard;
class SpiderWeb;

class Enemy : public Actor
{
 public:
  Enemy(geometry::Position position, geometry::Size size, int health) : Actor(position, size), health(health) {}
  virtual ~Enemy() = default;

  virtual bool is_alive() const override { return health > 0; }
  virtual bool on_hit(const geometry::Rectangle& rect,
                      AbstractSoundManager& sound_manager,
                      const geometry::Rectangle& player_rect,
                      Level& level,
                      const bool power) override;
  virtual void on_death(AbstractSoundManager& sound_manager, Level& level);
  // Whether this enemy requires the power powerup to hit/kill
  virtual bool is_tough() const { return false; }
  virtual TouchType on_touch([[maybe_unused]] const Player& player,
                             [[maybe_unused]] AbstractSoundManager& sound_manager,
                             [[maybe_unused]] Level& level) override
  {
    // TODO: play sound
    return TouchType::TOUCH_TYPE_HURT;
  }
  virtual bool flying() const { return false; }

  int health;

 protected:
  virtual bool should_reverse(const Level& level) const;
};

class FacePlayerOnHit : public Enemy
{
  // ABC for enemies that face the player when hit
 public:
  using Enemy::Enemy;
  virtual bool on_hit(const geometry::Rectangle& rect,
                      AbstractSoundManager& sound_manager,
                      const geometry::Rectangle& player_rect,
                      Level& level,
                      const bool power) override;

 protected:
  bool left_ = false;
};

class Bigfoot : public FacePlayerOnHit
{
  // ⚫⚫⚫⚫⚫⚫🟩🟩🟩⚫⚫⚫⚫⚫⚫⚫
  // ⚫⚫⚫⚫🟩🟩🟢🟢🟢🟢🟢⚫⚫⚫⚫⚫
  // ⚫⚫⚫🟩🟢⚫⚫⚫🟢🟢🟢⚫⚫⚫⚫⚫
  // ⚫⚫⚫🟩🟦⚫📘⬜⚫🟢🟢🟢⚫⚫⚫⚫
  // ⚫⚫🟩🟢🟦⚫🟦⬜⚫🟢🟢🟢⚫⚫⚫⚫
  // ⚫🟩⚫🟢🟢🟦⬜⚫🟢🟢🟢🟢🟢⚫⚫⚫
  // ⚫🟩🟢🟢🟢🟢🟢🟢🟢🟢🟢🟢🟢⚫⚫⚫
  // ⚫🟩🟢🟢🟢🟢🟢🟢🟢🟢🟢🟢🟢⚫⚫⚫
  // ⚫⬜⚫⬜⚫🟢🟢🟢🟢🟢🟢🟢🟢⚫⚫⚫
  // ⚫⚫⚫⚫⚫⚫🟢🟢🟢🟩🟢🟢🟢🟢⚫⚫
  // ⚫⚫⬜⚫⬜⚫🟢🟢🟩⚫🟢🟢🟢🟢⚫⚫
  // ⚫⚫🟩🟢🟢🟢🟢🟩⚫🟢🟢🟢🟢🟢⚫⚫
  // ⚫⚫⚫⚫⚫⚫⚫⚫🟩🟢🟢🟢🟢🟢⚫⚫
  // ⚫⚫⚫⚫⚫⚫⚫⚫🟩🟢🟢🟢🟢🟢⚫⚫
  // ⚫⚫⚫⚫⚫⚫⚫⚫🟩🟢🟢🟢🟢🟢⚫⚫
  // ⚫⚫⚫⬜⚫⚫⚫⚫🟩🟢🟢🟢🟢🟢⚫⚫
  // ⚫⬜🟢🟢🟢⚫⚫🟩⚫⚫🟢🟢🟢🟢⚫⚫
  // ⚫⚫🟢🟢🟢🟩⚫⚫🟩🟩🟢🟢🟢🟢🟢⚫
  // ⚫⬜⚫⚫🟢🟢🟩🟩🟢🟢🟢🟢🟢🟢🟢⚫
  // ⚫⚫⚫⚫⚫🟢🟢🟢🟢🟢⚫⚫🟢🟢🟢⚫
  // ⚫⚫⚫⚫⚫⚫🟢🟢🟢⚫⚫🟢🟢🟢🟢⚫
  // ⚫⚫⚫⚫⚫⚫⚫⚫⚫⚫🟢🟢🟢🟢⚫⚫
  // ⚫⚫⚫⚫⚫⚫⚫🟩🟢🟢🟢🟢🟢🟢⚫⚫
  // ⚫⚫⚫⚫⚫⚫⚫⚫🟩🟢🟢🟢🟢⚫⚫⚫
  // ⚫⚫⚫⚫⚫⚫⚫⚫🟩🟢🟢🟢⚫⬛⚫⚫
  // ⚫🟩🟩⚫⚫⚫⚫🟩🟢🟢🟢⚫⬛🟢⚫⚫
  // ⚫🟢🟢🟩🟩⚫🟩🟢🟢🟢⚫⚫🟢🟢⬛⚫
  // ⚫⚫🟢🟢🟢🟩🟢🟢🟢⚫⚫⚫⚫⬛🟢⚫
  // ⚫⚫⚫⚫🟢🟢🟢🟢⚫⚫⬛🟢⬛🟢⬛⚫
  // ⚫⚫⚫⚫⚫⚫🟢⚫⚫⬛🟢⬛🟢⬛🟢⚫
  // 2-tile tall enemy, runs if they see player
 public:
  Bigfoot(geometry::Position position) : FacePlayerOnHit(position - geometry::Position(0, 16), geometry::Size(16, 32), 5) {}

  virtual void update(AbstractSoundManager& sound_manager, const geometry::Rectangle& player_rect, Level& level) override;
  virtual std::vector<ObjectDef> get_sprites(const Level& level) const override;
  virtual std::vector<geometry::Rectangle> get_detection_rects(const Level& level) const override
  {
    return create_detection_rects(left_ ? -1 : 1, 0, level);
  }
  virtual bool on_hit(const geometry::Rectangle& rect,
                      AbstractSoundManager& sound_manager,
                      const geometry::Rectangle& player_rect,
                      Level& level,
                      const bool power) override;
  virtual int get_points() const override { return 5000; }
  virtual const std::vector<Sprite>* get_explosion_sprites() const override { return &Explosion::sprites_bones; }

 private:
  bool running_ = false;
  int frame_ = 0;
};

class Hopper : public Enemy
{
  // ⚫⚫⚫⚫⚫⚫⚫⚫⚫⚫⬜🟦⚫⚫⚫⚫
  // ⚫⚫⚫⚫⚫⚫⚫⚫🟢⬜⬜🟦🟦⚫⚫⚫
  // ⚫⚫⚫⚫⚫⚫⚫🟢🟢⬜⬜⬜🟦⚫⚫⚫
  // ⚫⚫⚫⚫⚫⚫🟢🟢🟢⚫⬜⬜⚫⚫⚫⚫
  // ⚫⚫⚫⚫⚫🟢🟢🟢⚫⚫⚫⚫⚫⚫⚫⚫
  // ⚫⚫⬜⬜🟢🟢🟢⚫⚫⚫⚫⚫⚫⚫⚫⚫
  // ⚫🟦🟦⬜⬜🟢⚫⚫⚫⚫⚫⚫⚫⚫⚫⚫
  // ⚫🟦🟦⬜⬜⚫⚫⚫⚫⚫⚫⚫⚫⚫⚫⚫
  // ⚫⚫⬜⬜⚫⚫⚫⚫⚫⚫⚫⚫⚫⚫⚫⚫
  // Moves left and right erratically
 public:
  Hopper(geometry::Position position) : Enemy(position, geometry::Size(16, 16), 1) {}

  virtual void update(AbstractSoundManager& sound_manager, const geometry::Rectangle& player_rect, Level& level) override;
  virtual std::vector<ObjectDef> get_sprites(const Level& level) const override;
  virtual int get_points() const override { return 100; }
  virtual const std::vector<Sprite>* get_explosion_sprites() const override { return &Explosion::sprites_implosion; }

 private:
  bool left_ = false;
  int frame_ = 0;
  int next_reverse_ = 0;
};

class Slime : public Enemy
{
  // ⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛🟩🟩🟩🟩⬛⬛
  // ⬛⬛⬛⬛⬛⬛⬛🟩🟩🟩🟩🦚🦚🦚⬛⬛
  // ⬛⬛⬛⬛⬛🟩🟩🟩🦚🦚🦚🦚⬜⬛🦚⬛
  // ⬛⬛⬛⬛🟩🦚🦚🦚🦚🦚🦚🦚⬜⬛🦚⬛
  // ⬛⬛⬛🟩🦚🦚🦚🦚🦚🦚🦚🦚🦚🦚🦚⬛
  // ⬛🟩🟩🦚🦚🦚🦚🦚🦚🦚🦚🦚🦚🦚🦚⬛
  // ⬛⬛⬛🦚🦚🦚🦚🦚🦚🦚🦚🦚⬜⬛🦚⬛
  // ⬛⬛⬛⬛⬛🦚🦚🦚🦚🦚🦚🦚⬜⬛🦚⬛
  // ⬛⬛⬛⬛⬛⬛⬛⬛⬛🦚🦚🦚🦚🦚⬛⬛
  // Flies around, pauses and changes directions erratically
 public:
  Slime(geometry::Position position) : Enemy(position, geometry::Size(16, 16), 1) {}

  virtual void update(AbstractSoundManager& sound_manager, const geometry::Rectangle& player_rect, Level& level) override;
  virtual std::vector<ObjectDef> get_sprites(const Level& level) const override;
  virtual int get_points() const override { return 100; }
  virtual const std::vector<Sprite>* get_explosion_sprites() const override { return &Explosion::sprites_implosion; }

 private:
  int dx_ = 1;
  int dy_ = 0;
  int frame_ = 0;
};

class SlimeLeaver : public Enemy
{
  // Moves left/right, pauses, leaves slime
 public:
  SlimeLeaver(geometry::Position position) : Enemy(position, geometry::Size(16, 16), 2) {}

  virtual void update(AbstractSoundManager& sound_manager, const geometry::Rectangle& player_rect, Level& level) override;
  virtual std::vector<ObjectDef> get_sprites(const Level& level) const override;
  virtual void on_death(AbstractSoundManager& sound_manager, Level& level) override;
  virtual int get_points() const override { return 100; }

 protected:
  virtual Sprite get_slime_sprite() const = 0;
  virtual Sprite get_pause_sprite() const = 0;
  virtual Sprite get_walk_left_sprite() const = 0;
  virtual Sprite get_walk_right_sprite() const = 0;
  virtual int num_pause_frames() const = 0;
  virtual int num_walk_frames() const = 0;
  bool left_ = false;
  bool paused_ = false;
  int frame_ = 0;
};

class Snake : public SlimeLeaver
{
  // ⚫⚫⚫⚫🟪🟨🟪🟪⚫⚫⚫⚫⚫⚫⚫⚫
  // ⚫⚫⚫🟪🟪🟪🟪🟪🟪🟪⚫⚫⚫⚫⚫⚫
  // ⚫⚫🟥⚫⚫⚫🟣🟣🟪🟪🟪⚫⚫⚫⚫⚫
  // ⚫🟥⚫⚫⚫⚫⚫🟣🟣🟪🟪🟪⚫⚫⚫⚫
  // ⚫⚫⚫⚫⚫⚫⚫🟣🟣🟪🟪🟪⚫⚫⚫⚫
  // ⚫⚫⚫⚫⚫⚫⚫🟣🟣🟪🟪🟪⚫⚫⚫⚫
  // ⚫⚫⚫⚫⚫⚫⚫🟣🟪🟪🟪🟪⚫⚫⚫⚫
  // ⚫⚫⚫⚫⚫⚫🟣🟪🟪🟪🟪⚫⚫⚫⚫⚫
  // ⚫⚫⚫⚫⚫⚫🟪🟪🟪⚫⚫⚫⚫⚫⚫⚫
  // ⚫⚫⚫⚫⚫🟪🟪🟪⚫⚫⚫⚫⚫⚫⚫⚫
  // ⚫⚫⚫⚫🟪🟪🟪⚫⚫🟪🟪⚫⚫⚫🟪⚫
  // ⚫⚫⚫🟪🟪🟪🟪🟪🟪🟪🟪🟣⚫⚫⚫🟪
  // ⚫⚫⚫🟣🟪🟪🟪🟪⚫⚫🟪🟪🟪🟪🟪⚫
  // Moves left/right, pauses, leaves slime
 public:
  Snake(geometry::Position position) : SlimeLeaver(position) {}

  virtual const std::vector<Sprite>* get_explosion_sprites() const override { return &Explosion::sprites_bones; }

 protected:
  virtual Sprite get_slime_sprite() const override { return Sprite::SPRITE_SNAKE_SLIME; }
  virtual Sprite get_pause_sprite() const override { return Sprite::SPRITE_SNAKE_PAUSE_1; }
  virtual Sprite get_walk_left_sprite() const override { return Sprite::SPRITE_SNAKE_WALK_L_1; }
  virtual Sprite get_walk_right_sprite() const override { return Sprite::SPRITE_SNAKE_WALK_R_1; }
  virtual int num_pause_frames() const override { return 7; }
  virtual int num_walk_frames() const override { return 9; }
};

class Tentacle : public SlimeLeaver
{
  // ➖➖⚫⚫➖⚫⚫⚫⚫⚫⚫➖➖➖➖➖
  // ➖⚫➖➖⚫🚨🟩🟩🟩🟩🦚⚫➖➖➖➖
  // ➖➖➖⚫⬜⬜🦚🦚🦚🦚🟩🦚⚫➖➖➖
  // ➖➖⚫🟦⬜⬜🟩🟩🟩🟩🦚🟩🦚⚫➖➖
  // ➖➖⚫🟦⬜⬜🦚🦚🦚🟩🟩🦚🟩🦚⚫➖
  // ➖➖➖⚫⬜⬜🟩🟩🟩🦚🟩🟩🦚🟩⚫➖
  // ➖➖➖➖⚫🚨🦚🦚🦚🟩🦚🟩🦚🟩⚫➖
  // ➖➖➖➖⚫⚫⚫⚫🦚🟩🦚🟩🦚🟩⚫➖
  // ➖➖➖⚫➖➖➖⚫🦚🦚🦚🟩🦚🦚🦚⚫
  // ➖➖➖➖⚫➖➖➖⚫🟩🦚🟩🦚🟩⚫➖
  // ➖➖➖➖➖➖➖⚫🦚🦚🦚🟩🦚🦚🦚⚫
  // ➖➖➖➖➖➖➖➖⚫🟩🦚🟩🦚🟩⚫➖
  // ➖➖➖➖➖➖➖⚫🦚🦚🦚🟩🦚🦚🦚⚫
  // ➖➖➖➖➖➖➖➖⚫⚫⚫⚫⚫⚫⚫➖
  // Moves left/right, pauses, leaves slime
 public:
  Tentacle(geometry::Position position) : SlimeLeaver(position) {}

  virtual const std::vector<Sprite>* get_explosion_sprites() const override { return &Explosion::sprites_implosion; }

 protected:
  virtual Sprite get_slime_sprite() const override { return Sprite::SPRITE_TENTACLE_SLIME; }
  virtual Sprite get_pause_sprite() const override { return Sprite::SPRITE_TENTACLE_PAUSE_1; }
  virtual Sprite get_walk_left_sprite() const override { return Sprite::SPRITE_TENTACLE_WALK_L_1; }
  virtual Sprite get_walk_right_sprite() const override { return Sprite::SPRITE_TENTACLE_WALK_R_1; }
  virtual int num_pause_frames() const override { return 7; }
  virtual int num_walk_frames() const override { return 16; }
};

class Spider : public Enemy
{
  // ⚫🟢🟢⚫⚫⚫🟥⚫⚫🟥⚫⚫⚫🟢🟢⚫
  // ⚫⚫⚫⬜⚫🟥⚫⚫⚫⚫🟥⚫⬜⚫⚫⚫
  // ⚫⚫⚫🟩⚫⚫🟣🔵🔵🟣⚫⚫🟩⚫⚫⚫
  // ⚫⚫⚫⚫🟢🔵🟦🟦🟦🟦🔵🟢⚫⚫⚫⚫
  // ⚫⚫⚫⚫⚫🟢🔵📘📘🔵🟢⚫⚫⚫⚫⚫
  // 🟢🟩⬜🟢🟢⚫🔵🔵🔵🔵⚫🟢🟢⬜🟩🟢
  // ⚫⚫⚫⚫⚫🔵🔵🟦🟦🔵🔵⚫⚫⚫⚫⚫
  // ⚫⚫🟢🟢🔵🟦🟦📘📘🟦🟦🔵🟢🟢⚫⚫
  // ⚫🟢⚫🔵🔵🟦📘🩵🩵📘🟦🔵🔵⚫🟢⚫
  // ⚫⬜⚫🔵🟦📘🩵🩵🩵🩵📘🟦🔵⚫⬜⚫
  // ⚫🟩⚫🔵🟦🟦📘📘📘📘🟦🟦🔵⚫🟩⚫
  // ⚫🟢⚫🔵🔵🟦🟦🟦🟦🟦🟦🔵🔵⚫🟢⚫
  // ⚫🟢⚫⚫🔵🔵🔵🔵🔵🔵🔵🔵⚫⚫🟢⚫
  // 🟢⚫⚫⚫⚫⚫🔵🔵🔵🔵⚫⚫⚫⚫⚫🟢
  // Moves up and down, shoots webs below
 public:
  Spider(geometry::Position position) : Enemy(position, geometry::Size(16, 16), 1) {}

  virtual void update(AbstractSoundManager& sound_manager, const geometry::Rectangle& player_rect, Level& level) override;
  virtual std::vector<ObjectDef> get_sprites(const Level& level) const override;
  virtual std::vector<geometry::Rectangle> get_detection_rects(const Level& level) const override
  {
    return create_detection_rects(0, 1, level);
  }
  void remove_child() { child_ = nullptr; }
  virtual int get_points() const override { return 100; }
  virtual const std::vector<Sprite>* get_explosion_sprites() const override { return &Explosion::sprites_bones; }
  virtual void on_death(AbstractSoundManager& sound_manager, Level& level) override;

 private:
  bool up_ = false;
  int frame_ = 0;
  SpiderWeb* child_ = nullptr;
};


class Rockman : public Enemy
{
  // ⬛⬛⬛⬛⬛⚪🪦⚪🪦⚪⬛⬛⬛⬛⬛⬛
  // ⬛⬛⬛🪦⚪🪦⚪🪦⚪🪦🟠🪦⬛⬛⬛⬛
  // ⬛⬛⬛⚪🪦⚪🪦🟠🪦🟠🪦🟠🪦⬛⬛⬛
  // ⬛⬛⚪🪦🟠🪦🟠🪦🟠🪦🟠🪦⬛🪦⬛⬛
  // ⬛⚪🪦⚪🪦🟠🪦🟠🪦🟠🪦🟠🪦⬛🪦⬛
  // ⬛🪦⚪🪦⚪🪦🟠🪦🟠🪦🟠🪦🟠🪦⬛⬛
  // ⬛⚪🪦⚪🪦🟠🪦🟠🪦🟠🪦⬛🪦⬛🪦⬛
  // ⬛🪦⚪🪦⚪🪦🟠🪦⬛🪦⬛🪦⬛🪦⬛⬛
  // ⬛⬛🪦⚪🪦⚪🪦🟠🪦⬛🪦⬛🪦⬛⬛⬛
  // ⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛
  // ⬛⬛⬛⬛⬛🟪🟪🟣🟣⬛🟣🟣⬛⬛⬛⬛
  // ⬛🟪🟪⬛🟪🟪🟣🟣⬛⬛🟣🟣🟣⬛⬛⬛
  // ⬛⬛🟪🟪🟪🟣🟣⬛⬛🟣🟣🟣🟣🟣⬛⬛
  // Initially stopped, wakes on vision of player, moves left/right, needs P to kill
 public:
  Rockman(geometry::Position position) : Enemy(position, geometry::Size(16, 16), 1) {}

  virtual void update(AbstractSoundManager& sound_manager, const geometry::Rectangle& player_rect, Level& level) override;
  virtual std::vector<ObjectDef> get_sprites(const Level& level) const override;
  virtual std::vector<geometry::Rectangle> get_detection_rects(const Level& level) const override;
  virtual int get_points() const override { return 100; }
  virtual bool is_tough() const override { return true; }
  virtual const std::vector<Sprite>* get_explosion_sprites() const override { return &Explosion::sprites_implosion; }

 private:
  bool left_ = false;
  int frame_ = 0;
  bool asleep_ = true;
};

class MineCart : public Enemy
{
  // ⬛⬛⬛⬛⬛⬛⬛⬛🟨⬛⬛⬛⬛⬛⬛⬛
  // ⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛
  // ⬛⬛⬛🟨⬛🟨🟨🟨🟨🟨⬛⬛🟨🟨⬛⬛
  // ⬛⚪⚪⚪⚪⚪⚪⚪⚪⚪⚪⚪⚪⚪⚪⬛
  // ⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛
  // ⬛⬛🪦🪦🪦🪦🪦🪦🪦🪦🪦🪦🪦🪦⬛⬛
  // ⬛⬛🪦🪦🪦🪦🪦🪦🪦🪦🪦🪦🪦🪦⬛⬛
  // ⬛⬛⬛🪦🪦🪦🪦🪦🪦🪦🪦🪦🪦⬛⬛⬛
  // ⬛⬛⬛⬜⬜⬛🪦🪦🪦🪦⬛⬜⬜⬛⬛⬛
  // ⬛⬛⬜⬛⬛⬜⬛⬛⬛⬛⬜⬛⬜⬜⬛⬛
  // ⬛⬛⬜⬜⬛⬜⬛⬛⬛⬛⬜⬛⬛⬜⬛⬛
  // ⬛⬛⬛⬜⬜⬛⬛⬛⬛⬛⬛⬜⬜⬛⬛⬛
  // Moves left/right and pauses at edges, needs P to kill
 public:
  MineCart(geometry::Position position) : Enemy(position, geometry::Size(16, 16), 1) {}

  virtual void update(AbstractSoundManager& sound_manager, const geometry::Rectangle& player_rect, Level& level) override;
  virtual std::vector<ObjectDef> get_sprites(const Level& level) const override;
  // TODO: confirm points
  virtual int get_points() const override { return 100; }
  virtual bool is_tough() const override { return true; }
  virtual const std::vector<Sprite>* get_explosion_sprites() const override { return &Explosion::sprites_implosion; }

 private:
  bool left_ = false;
  int frame_ = 0;
  int pause_frame_ = 0;
};


class Caterpillar : public Enemy
{
  // ⬛⬛⬛⬜⬛⬛⬛⬛⬛⬛⬜⬜⬛⬛⬛⬛
  // ⬛⬛⬛⬛⬜⬜⬜⬛⬛⬜⬛⬛⬛⬛⬛⬛
  // ⬛⬛⬛⬛⬛⬛⬛⬜⬛🦚🪦⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬜⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬜⬛⬛⬛⬛⬛⬛⬛
  // ⬛⬛⬛⬛🟩🟩🟩⬛⬜⬛🦚🪦🦚⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬜⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬜⬛⬛⬛⬛⬛⬛⬛
  // ⬛⬛⬛🟩🟩⬛⬛🦚⬛🦚🪦🦚🪦🦚⬛⬛⬛⬛⬛⬛⬛⬛⬛🪦⬜🪦⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛🪦⬜🪦⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬜⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛
  // ⬛⬛⬛🟩⬛📘⬜🟩🦚🟩🦚🪦🦚🪦⬛⬛⬛⬛⬛⬛⬛⬛⬛⚪⬜⚪⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⚪⬜⚪⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬜⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛
  // ⬛⬛🟩🟦⬛🟦⬜🦚🟩🦚🟩🦚🪦🦚🪦⬛⬛⬛⬜⬛⬛⬛🪦⚪⬜⚪🪦⬛⬛⬛⬜⬛⬛⬛⬜⬛⬛⬛🪦⚪⬜⚪🪦⬛⬛⬛⬜⬛⬛⬛⬛⬛⬛⬜⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛
  // ⬛⬛🟩🟦⬛⬜🦚🟩🦚🟩🦚🟩🦚🪦🦚⬛⬛⬛⬛⬜⚪🪦🪦⚪⬜⚪🪦🪦⚪⬜⬛⬛⬛⬛⬛⬜⚪🪦🪦⚪⬜⚪🪦🪦⚪⬜⬛⬛⬛⬛⬛⬛⚪⬜⚪⬛⬛⬛⬛⬛⬛⬜⬛⬛
  // ⬛⬛🟩🟩🟩🦚🟩🦚🟩🦚🟩🦚🪦🦚🪦⬛⬛⬛⬛⚪⬜⚪🟩🦚🟩🪦🦚⚪⬜⚪⬛⬛⬛⬛⬛⚪⬜⚪🟩🦚🟩🪦🦚⚪⬜⚪⬛⬛⬛⬛⬛🪦⚪⬜⚪⬛⬛⬛⬛⚪⬜⬛⬛⬛
  // ⬛⬛🟩🟩🟩🟩🦚🟩🦚🟩🦚🟩🦚🪦🦚⬛⬛⬛🪦🟩🦚🦚🟩🦚🟩🦚🟩🦚🪦🦚🪦⬛⬛⬛🪦🟩🦚🦚🟩🦚🟩🦚🪦🦚🪦🦚🪦⬛⬛⬛🪦🦚🟩🦚⚪🪦⬛🪦⚪⬜⚪⬛⬛⬛
  // ⬛⬛⬛⬛⬛⬛⬛🦚🟩🦚🟩🦚🪦🦚⬛⬛⬛⬛🟩🦚🟩🟩🦚🟩🦚🟩🦚🟩🦚🪦🦚⬛⬛⬛🟩🟩🦚🟩🦚🟩🦚🟩🦚🟩🦚🪦🦚⬛⬛⬛🦚🟩🦚🦚🟩🦚🪦🦚⬜⚪🪦⬛⬛⬛
  // ⬛⬛⬛🟩🟩🟩🦚⬛🦚🟩🦚🪦🦚🪦⬛⬛⬛⬛🟩🟩🦚🟩🟩🦚🟩🦚🟩🦚🪦🦚🪦⬛⬛⬛🟩🦚🟩🦚🟩🦚🟩🦚🟩🦚🪦🦚🪦⬛⬛⬛🟩🦚🟩🦚🦚🪦🦚🪦🦚🪦🦚⬛⬛⬛
  // ⬛⬛⬛⬛🟩🟩🟩🦚🟩🦚🪦🦚🪦⬛⬛⬛⬛⬛🟩🦚🟩🦚🟩🟩🦚🟩🦚🪦🦚🪦🦚⬛⬛⬛🟩🦚🟩🟩🦚🟩🦚🟩🦚🟩🦚🪦🦚⬛⬛⬛🦚🟩🦚🟩🦚🟩🪦🦚🪦🦚🪦🦚🪦⬛
  // ⬛⬛⬛⬛⬛⬛🟩🟩🟩🪦🦚⬛⬛⬛⬛⬛⬛⬛⬛🟩🦚🟩🦚🟩🟩🦚🪦🦚🪦🦚⬛⬛⬛⬛⬛🟩🦚🟩🦚🦚🟩🦚🟩🦚🪦🦚⬛⬛⬛⬛⬛🦚🟩🦚🟩🦚🟩🪦🦚🪦🦚🪦⬛⬛
  // Moves left/right; only head is vulnerable; following segments become heads once previous head is destroyed
 public:
  Caterpillar(geometry::Position position);

  virtual void update(AbstractSoundManager& sound_manager, const geometry::Rectangle& player_rect, Level& level) override;
  virtual std::vector<ObjectDef> get_sprites(const Level& level) const override;
  virtual int get_points() const override { return 1000; }
  virtual bool is_tough() const override
  {  // only head of caterpillar vulnerable
    return rank_ > 0;
  }
  virtual void on_death(AbstractSoundManager& sound_manager, Level& level) override;
  void set_child(Caterpillar& child);
  virtual const std::vector<Sprite>* get_explosion_sprites() const override { return &Explosion::sprites_bones; }

 private:
  // Rank 0 = head
  int rank_ = 0;
  bool left_ = true;
  int frame_ = 0;
  Caterpillar* child_ = nullptr;
};

class Snoozer
  : public ProjectileParent
  , public FacePlayerOnHit
{
  // ⬛⬛⬛⬛⬛🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺⬛⬛⬛⬛⬛
  // ⬛⬛⬛🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺⬛⬛⬛
  // ⬛⬛🇪🇺🇪🇺🇪🇺🟦🟦🇪🇺🇪🇺🇪🇺🇪🇺⬛⬛🇪🇺⬛⬛
  // ⬛⬛🇪🇺🇪🇺🟦🟦🇪🇺🇪🇺🇪🇺🇪🇺⬛⬜⬜⬛⬛⬛
  // ⬛🇪🇺🇪🇺🟦🟦🇪🇺🇪🇺🇪🇺🇪🇺⬛⬜📘🚨📘🇪🇺⬛
  // ⬛🇪🇺🇪🇺🟦🟦🇪🇺🇪🇺🇪🇺🇪🇺⬛⬜🚨🚨⬛🇪🇺⬛
  // ⬛🇪🇺🟦🟦🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺⬛📘⬛⬛🇪🇺⬛
  // ⬛🇪🇺🟦🟦🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺⬛⬛🇪🇺🇪🇺⬛
  // ⬛🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺⬛
  // ⬛🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺⬛⬛⬛🇪🇺🇪🇺🇪🇺⬛
  // ⬛⬛🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺⬛⬛⬛🇪🇺🇪🇺⬛⬛⬛⬛
  // ⬛⬛🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺⬛⬛
  // ⬛⬛⬛🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺⬛⬛⬛
  // ⬛⬛⬛⬛⬛🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺⬛⬛⬛⬛⬛
  // Moves left/right and sleeps occasionally, can only be harmed when sleeping
 public:
  Snoozer(geometry::Position position) : FacePlayerOnHit(position, geometry::Size(16, 16), 3) {}

  virtual void update(AbstractSoundManager& sound_manager, const geometry::Rectangle& player_rect, Level& level) override;
  virtual std::vector<ObjectDef> get_sprites(const Level& level) const override;
  virtual int get_points() const override { return 5000; }
  virtual bool is_tough() const override { return pause_frame_ == 0; }
  virtual const std::vector<Sprite>* get_explosion_sprites() const override { return &Explosion::sprites_implosion; }
  virtual std::vector<geometry::Rectangle> get_detection_rects(const Level& level) const override
  {
    return create_detection_rects(left_ ? -1 : 1, 0, level);
  }
  virtual void on_death(AbstractSoundManager& sound_manager, Level& level) override;

 private:
  int frame_ = 0;
  int pause_frame_ = 1;
  int next_shoot_ = 0;
};

class Triceratops
  : public ProjectileParent
  , public FacePlayerOnHit
{
  // ⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛🚨🚨⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛🚨🟥🟥🟥🟥⬛⬛⬛
  // ⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛🟥🚨⬛⬛⬛⬛⬛🚨🚨🚨🚨🟥🟥🟥🟥🟥🟥⬛⬛
  // ⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛🟥🟥🟥⬛⬛⬛🚨🚨🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥⬛
  // ⬛⬛⬛⬛⬛⬛⬛⬛⬛🚨🟥🟥🟥🟥🚨⬛🚨🚨🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥
  // ⬛⬜⬛⬛⬛⬛⬛🚨🚨🟥⬛🟥🟥🚨🟥⬛🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥
  // ⬛⚪⚪⬛⬛⬛🚨⬛⬛⬛🟥🟥🟥🟥🟥⬛🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🚨🟥🟥🟥🟥
  // ⬛🟥🟥🚨🚨🟥🟥🟦⬜⬜🟥🟥🟥🟥🟥⬛🟥🟥🟥🟥🟥🟥🟥🟥🟥🚨🚨🟥🟥🟥🟥🟥
  // ⬛🟥⬛🟥🟥🟥🟥⬛🟦🟥🟥🟥🟥🚨🚨⬛🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛
  // ⬛🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥⬛🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛
  // ⬛🟥🟥🟥🟥🟥🟥🟥🟥🟥⬛🟥🟥🟥🟥⬛⬛🟥⬛🚨🟥🟥🟥🟥🟥🟥🟥⬛🚨🟥🟥⬛🟥🟥🟥🟥⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛
  // ⬛⬛⬛⬛⬛🟥🟥🟥🟥🟥⬛🟥🟥🚨🟥⬛⬛⬛⬛🚨🟥🟥⬛🟥🟥🟥🟥⬛🚨🟥🟥⬛🚨🟥🟥🟥🟥⬛⬛⬛⬛⬜⬛⬛⬛⬛⬛⬛
  // ⬛⬛🟥🟥🟥🟥🟥🟥🟥⬛🟥🟥🟥🟥🚨⬛⬛⬛⬛🚨🟥🟥⬛⬛⬛⬛⬛⬛🚨🟥🟥⬛⬛🚨🟥🟥🟥🟥🟥⬛⬜⬛⬛⬜⬛⬛⬛⬛
  // ⬛⬛⬛⬛🟥🟥🟥🟥⬛⬛⬛🟥🟥🟥⬛⬛⬛⬛🚨🟥🟥🟥⬛⬛⬛⬛⬛🚨🟥🟥🟥⬛⬛⬛⬛🚨🚨🟥🟥🟥🟥🟥⬜⬛⬛⬛⬛⬛
  // ⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛🚨🟥🟥🟥⬛⬛⬛⬛⬛🚨🟥🟥🟥⬛⬛⬛⬛⬛⬛⬛⬛🚨🟥🟥🟥🟥🟥🟥⬛⬛⬛
  // Moves left/right, fires projectiles
 public:
  Triceratops(geometry::Position position) : FacePlayerOnHit(position, geometry::Size(48, 16), 5) {}

  virtual void update(AbstractSoundManager& sound_manager, const geometry::Rectangle& player_rect, Level& level) override;
  virtual std::vector<ObjectDef> get_sprites(const Level& level) const override;
  virtual int get_points() const override { return 5000; }
  virtual const std::vector<Sprite>* get_explosion_sprites() const override { return &Explosion::sprites_implosion; }
  virtual std::vector<geometry::Rectangle> get_detection_rects(const Level& level) const override
  {
    return create_detection_rects(left_ ? -1 : 1, 0, level);
  }
  virtual void on_death(AbstractSoundManager& sound_manager, Level& level) override;

 private:
  int frame_ = 0;
};

class Flier : public Enemy
{
  // ABC for enemy that flies left and right
 public:
  Flier(geometry::Position position) : Enemy(position, geometry::Size(16, 16), 1) {}

  virtual void update(AbstractSoundManager& sound_manager, const geometry::Rectangle& player_rect, Level& level) override;
  virtual std::vector<ObjectDef> get_sprites([[maybe_unused]] const Level& level) const override
  {
    return {{position, static_cast<int>(get_sprite()) + frame_, false}};
  }
  virtual int get_points() const override { return 100; }
  virtual bool flying() const { return true; }

 protected:
  virtual Sprite get_sprite() const = 0;
  virtual int num_frames() const = 0;
  bool left_ = false;

 private:
  int frame_ = 0;
  int next_reverse_ = 0;
};

class Bat : public Flier
{
  // ➖⚫⚫⚫⚫➖🟨⚫⚫🟨➖⚫⚫⚫⚫➖
  // ⚫⚫⚫➖⚫⚫⚫⚫⚫⚫⚫⚫➖⚫⚫⚫
  // ⚫⚫➖➖➖➖⚫⚫⚫⚫➖➖➖➖⚫⚫
  // ⚫➖➖➖➖➖➖⚫⚫➖➖➖➖➖➖⚫
  // Moves left and right erratically, flies
 public:
  using Flier::Flier;
  virtual void update(AbstractSoundManager& sound_manager, const geometry::Rectangle& player_rect, Level& level) override;

 protected:
  virtual Sprite get_sprite() const override { return Sprite::SPRITE_BAT_1; }
  virtual int num_frames() const override { return 10; }
  virtual bool should_reverse(const Level& level) const override { return next_reverse_ == 0 || Enemy::should_reverse(level); }
  virtual const std::vector<Sprite>* get_explosion_sprites() const override { return &Explosion::sprites_bones; }

 private:
  int next_reverse_ = 0;
};

class WallMonster : public Enemy
{
  // ⚫⚫⚫⚫⚫⚫⚫⚫➖➖➖➖➖➖➖➖
  // 🟩🦚🦚🟩🦚🦚🟩🦚⚫⚫⚫⚫➖➖➖➖
  // 🦚🟩🦚🦚🟩🦚🦚🟩🦚🟪🟪🟪⚫➖➖➖
  // 🟪🟪🟪🟪🟪🟪🟪🟪🟪🟣🟣🟨🟪⚫➖➖
  // 🟣🟣🟣🟣🟣🟣🟣🟣🟣🟣🟣🟨⚫🟪⚫➖
  // 🟣🟣🟣🟣🟣🟣🟣🟣🟣🟣🟣🟣🟣🟣🟪⚫
  // 🦚🟩🦚🟩🦚🟩🟣🟣🟣🟣🟣🟣🟣🟣🟣🟪
  // 🟣🟣🟣🟣🟣🟣🟣🟣🟣🟣⚫⬜🟣🟣🟣🟪
  // 🟣🟣🟣🟣🟣🟣🟣🟣🟣🟣⬜⚫⬜🟣🟣⚫
  // 🦚🟩🦚🦚🟩🦚🦚⚫🟣🟣🟣⬜⚫⚫⚫➖
  // 🟩🦚🦚🟩🦚🦚⚫➖⚫🟣🟣🟣🟣⚫➖➖
  // ⚫⚫⚫⚫⚫⚫➖➖➖⚫🟣🟣🟣⚫➖➖
  // ➖➖➖➖➖➖➖➖➖➖⚫⚫⚫➖➖➖
  // Pops out of the wall when close, tough
 public:
  WallMonster(geometry::Position position, bool left) : Enemy(position, geometry::Size(16, 16), 1), left_(left) {}

  virtual void update(AbstractSoundManager& sound_manager, const geometry::Rectangle& player_rect, Level& level) override;
  virtual std::vector<ObjectDef> get_sprites(const Level& level) const override;
  virtual int get_points() const override { return 100; }
  virtual bool is_tough() const override { return true; }
  virtual std::vector<geometry::Rectangle> get_detection_rects([[maybe_unused]] const Level& level) const override
  {
    std::vector<geometry::Rectangle> rects;
    rects.push_back(left_ ? geometry::Rectangle{position.x() - 16, position.y(), 16 * 2, size.y()}
                          : geometry::Rectangle{position.x(), position.y(), 16 * 2, size.y()});
    return rects;
  }
  virtual const std::vector<Sprite>* get_explosion_sprites() const override { return &Explosion::sprites_implosion; }

 private:
  int frame_ = 0;
  bool awake_ = false;
  bool left_;
};

// TODO: make bird children orphanable
class Bird : public Flier
{
  // ➖➖➖➖➖➖⚫⚫⚫⚫➖➖➖➖➖➖
  // ➖➖⚫⚫⚫⚫📘📘📘📘⚫⚫⚫⚫➖➖
  // ➖⚫🟦🟦🟦📘🟨📘📘🟨📘🟦🟦🟦⚫➖
  // ⚫🟦🟦⚫⚫⚫📘📘📘📘⚫⚫⚫🟦🟦⚫
  // ⚫🟦🟦⚫➖➖🚨➖➖🚨➖➖⚫🟦🟦⚫
  // 🟦⚫⚫➖⚫🚨⚫➖➖➖🚨⚫➖➖⚫🟦
  // ⚫➖➖⚫🚨➖🚨⚫➖🚨➖🚨⚫➖➖⚫
  // ➖➖➖➖⚫➖⚫➖➖⚫➖⚫➖➖➖➖
  // Moves left and right, lays eggs
 public:
  using Flier::Flier;
  virtual void update(AbstractSoundManager& sound_manager, const geometry::Rectangle& player_rect, Level& level) override;
  virtual std::vector<geometry::Rectangle> get_detection_rects(const Level& level) const override
  {
    return create_detection_rects(0, 1, level);
  }
  void remove_child() { child_ = nullptr; }
  void set_child(Actor* child) { child_ = child; }
  virtual const std::vector<Sprite>* get_explosion_sprites() const override { return &Explosion::sprites_bones; }

 protected:
  virtual Sprite get_sprite() const override { return Sprite::SPRITE_BIRD_1; }
  virtual int num_frames() const override { return 10; }

 private:
  Actor* child_ = nullptr;
};

class Birdlet : public Flier
{
  // ➖➖➖➖➖➖➖⚫⚫➖➖➖➖➖➖➖
  // ➖➖⚫⚫⚫⚫⚫📘📘⚫⚫⚫⚫⚫➖➖
  // ➖⚫🟦🟦🟦📘🟨📘📘🟨📘🟦🟦🟦⚫➖
  // ⚫🟦⚫⚫⚫⚫⚫📘📘⚫⚫⚫⚫⚫🟦⚫
  // ➖⚫➖➖➖⚫🚨⚫⚫🚨⚫➖➖➖⚫➖
  // ➖➖➖➖⚫🚨➖🚨🚨➖🚨⚫➖➖➖➖
  // ➖➖➖➖➖⚫➖⚫⚫➖⚫➖➖➖➖➖
  // Simple flier spawned by egg
 public:
  Birdlet(geometry::Position position, Bird& parent) : Flier(position), parent_(parent) {}
  virtual void on_death(AbstractSoundManager& sound_manager, Level& level) override;
  virtual const std::vector<Sprite>* get_explosion_sprites() const override { return &Explosion::sprites_bones; }

 protected:
  virtual Sprite get_sprite() const override { return Sprite::SPRITE_BIRDLET_1; }
  virtual int num_frames() const override { return 5; }

 private:
  Bird& parent_;
};

class Robot
  : public FacePlayerOnHit
  , public ProjectileParent
{
  // ➖⚫⚫⚫⚫⚫⚫⚫⚫⚫⚫⚫⚫➖➖➖
  // ⚫⬜⬜⬜⬜🩵🩵🩵🩵🩵🩵🩵🩵⚫➖➖
  // ⚫⬜⬜🩵🩵🩵🩵🩵🩵🟥🟥🟥🟥⚫➖➖
  // ⚫⬜⬜⬜⬜🩵🩵🩵🩵🟥🟥🟥🟥⚫➖➖
  // ⚫⬜⬜🩵🩵🩵🩵🩵🩵📘📘📘📘⚫➖➖
  // ➖➖➖➖⚫⬜🩵📘⚫➖➖➖➖➖➖➖
  // ⚫⬜⬜⬜🩵🩵🩵🩵📘📘📘📘📘⚫⚫⚫
  // ⚫⬜⬜⬜⬜🩵🩵🩵🩵📘📘📘📘⚫🇪🇺🇪🇺
  // ⚫⬜⬜⬜🩵🩵🩵🩵📘📘📘🇪🇺🇪🇺🇪🇺⚫⚫
  // ⚫⬜⬜⬜⬜🩵🩵🩵🩵📘📘📘📘⚫🇪🇺🇪🇺
  // ⚫⬜⬜⬜🩵🩵🩵🩵📘📘📘📘📘⚫⚫⚫
  // ⚫⬜⬜⬜⬜🩵🩵🩵🩵📘📘📘📘⚫➖➖
  // ➖⚫⚫🪦⚪⚪⚪⚪⚪⚪🪦⚫⚫➖➖➖
  // ➖⚫⬜⬜⬜🩵🩵🩵📘📘🟦🟦⚫➖➖➖
  // ➖➖⚫⚫⚫⚫⚫⚫⚫⚫⚫⚫➖➖➖➖
  // Moves left and right erratically, zaps player when they are close
 public:
  Robot(geometry::Position position) : FacePlayerOnHit(position, geometry::Size(16, 16), 3) {}

  virtual void update(AbstractSoundManager& sound_manager, const geometry::Rectangle& player_rect, Level& level) override;
  virtual std::vector<ObjectDef> get_sprites(const Level& level) const override;
  virtual bool on_hit(const geometry::Rectangle& rect,
                      AbstractSoundManager& sound_manager,
                      const geometry::Rectangle& player_rect,
                      Level& level,
                      const bool power) override;
  virtual int get_points() const override { return 100; }
  virtual std::vector<geometry::Rectangle> get_detection_rects(const Level& level) const override
  {
    return create_detection_rects(left_ ? -1 : 1, 0, level, false, 2);
  }
  virtual void remove_child(Level& level) override;
  virtual const std::vector<Sprite>* get_explosion_sprites() const override { return &Explosion::sprites_implosion; }
  virtual void on_death(AbstractSoundManager& sound_manager, Level& level) override;

 private:
  int frame_ = 0;
  int next_reverse_ = 0;
  bool zapping_ = false;
};

class EyeMonster
  : public Enemy
  , public ProjectileParent
{
  // ➖➖➖➖➖⚫⚫⚫⚫➖➖➖➖➖➖➖➖➖➖➖➖➖⚫⚫⚫⚫➖➖➖➖➖➖➖➖➖➖➖➖➖⚫⚫⚫⚫➖➖➖➖➖
  // ➖➖➖⚫⚫🟩🦚🦚🦚⚫⚫➖➖➖➖➖➖➖➖➖⚫⚫🟩🦚🦚🦚⚫⚫➖➖➖➖➖➖➖➖➖⚫⚫🟩🦚🦚🦚⚫⚫➖➖➖
  // ➖➖⚫🟩🟩🦚⚫⚫🦚🦚🦚⚫➖➖➖➖➖➖➖⚫🟩🟩🦚🦚🦚🦚🦚🦚⚫➖➖➖➖➖➖➖⚫🟩🟩🦚⚫⚫🦚🦚🦚⚫➖➖
  // ➖⚫🟩🦚⚫⚫🦚🦚⚫⚫🦚🦚⚫➖➖➖➖➖⚫🟩🦚🦚⚫⬜⚫⬜🦚🦚🦚⚫➖➖➖➖➖⚫🟩🦚⚫⚫🦚🦚⚫⚫🦚🦚⚫➖
  // ➖⚫🟩⚫🦚🦚🟦🟦🦚🦚⚫🦚⚫➖➖⚫⚫⚫⚫🟩🦚⬜⚫⚫⚫⚫⚫🦚🦚⚫⚫⚫⚫➖➖⚫🟩⚫🦚🦚🟦🟦🦚🦚⚫🦚⚫➖
  // ⚫🟩⚫⬜📘🟦⚫⚫🟦📘⬜⚫🦚⚫⚫🟩🟩🟩⚫🟩🦚⚫⚫⚫⚫⚫⚫⬜🦚🟩🟩🟩🟩⚫⚫🟩⚫⬜📘🟦⚫⚫🟦📘⬜⚫🦚⚫
  // ⚫🟩⚫⬜🟦⚫⚫⚫⚫🟦⬜⚫🦚🟩🟩🦚🦚🦚🟩🦚⚫⚫⚫⚫⚫⚫⚫⚫🦚🦚🦚🦚🦚🟩🟩🟩⚫⬜🟦⚫⚫⚫⚫🟦⬜⚫🦚⚫
  // ⚫🟩⚫⬜🟦⚫⚫⚫⚫🟦⬜⚫🦚🦚🦚⚫⚫🦚🦚🦚⚫⚫⚫⚫⚫⚫⚫⚫🦚🦚🦚⚫⚫🦚🦚🟩⚫⬜🟦⚫⚫⚫⚫🟦⬜⚫🦚⚫
  // ⚫🟩⚫⬜📘🟦⚫⚫🟦📘⬜⚫🦚⚫⚫➖➖⚫⚫🟩⚫⚫⬜⚫⬜⚫⚫⚫🦚⚫⚫➖➖⚫⚫🟩⚫⬜📘🟦⚫⚫🟦📘⬜⚫🦚⚫
  // ➖⚫🟩⚫🦚🦚🟦🟦🦚🦚⚫🦚⚫➖➖➖➖➖⚫🟩⬜🦚🦚🦚🦚⚫⬜⚫🦚⚫➖➖➖➖➖⚫🟩⚫🦚🦚🟦🟦🦚🦚⚫🦚⚫➖
  // ➖⚫🟩🦚⚫⚫🦚🦚⚫⚫🦚🦚⚫➖➖➖➖➖⚫🟩🦚🦚🦚🦚🦚🦚🦚🦚🦚⚫➖➖➖➖➖⚫🟩🦚⚫⚫🦚🦚⚫⚫🦚🦚⚫➖
  // ➖➖⚫🟩🟩🦚⚫⚫🦚🦚🦚⚫➖➖➖➖➖➖⚫🟩🦚🦚🦚⚫⚫🟩🦚🦚🦚⚫➖➖➖➖➖➖⚫🟩🟩🦚⚫⚫🦚🦚🦚⚫➖➖
  // ➖➖➖⚫⚫🟩🦚🦚🦚⚫⚫➖➖➖➖➖➖➖⚫🟩🦚🦚🦚⚫⚫🟩🦚🦚🦚⚫➖➖➖➖➖➖➖⚫⚫🟩🦚🦚🦚⚫⚫➖➖➖
  // ➖➖➖➖➖⚫⚫⚫⚫➖➖➖➖➖➖➖➖⚫🟩🦚🦚🦚⚫➖➖⚫🟩🦚🦚🦚⚫➖➖➖➖➖➖➖➖⚫⚫⚫⚫➖➖➖➖➖
  // ➖➖➖➖➖➖➖➖➖➖➖➖➖➖➖➖⚫🟩🦚🦚🦚🦚⚫➖➖⚫🟩🦚🦚🦚🦚⚫
  // Walks left/right, fires eyeballs, need to kill eyes first (tough when closed)
 public:
  EyeMonster(geometry::Position position) : Enemy(position, geometry::Size(48, 16), 2) {}

  virtual void update(AbstractSoundManager& sound_manager, const geometry::Rectangle& player_rect, Level& level) override;
  virtual std::vector<ObjectDef> get_sprites(const Level& level) const override;
  virtual int get_points() const override { return 5000; }
  virtual bool on_hit(const geometry::Rectangle& rect,
                      AbstractSoundManager& sound_manager,
                      const geometry::Rectangle& player_rect,
                      Level& level,
                      const bool power) override;
  virtual const std::vector<Sprite>* get_explosion_sprites() const override { return &Explosion::sprites_bones; }
  virtual std::vector<geometry::Rectangle> get_detection_rects(const Level& level) const override
  {
    return create_detection_rects(left_ ? -1 : 1, 0, level);
  }
  virtual void on_death(AbstractSoundManager& sound_manager, Level& level) override;

 private:
  bool left_ = false;
  int frame_ = 0;
  bool left_closed_ = false;
  bool right_closed_ = false;
  int left_health_ = 2;
  int right_health_ = 2;
  int left_close_counter_ = 0;
  int right_close_counter_ = 0;
  int left_frame_ = 0;
  int right_frame_ = 0;
  int next_shoot_ = 0;
};

class Ostrich
  : public Enemy
  , public ProjectileParent
{
  // ➖➖⚫⚫⚫⚫⚫⚫⚫⚫⚫⚫⚫➖➖➖
  // ➖⚫🟦🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺⚫➖➖
  // ⚫🟦🇪🇺⚫⚫🇪🇺🇪🇺🇪🇺🇪🇺⚫🇪🇺⚫🇪🇺🇪🇺⚫➖
  // ➖⚫🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺⚫🇪🇺⚫🇪🇺⚫➖
  // ➖⚫🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺⚫➖
  // ⚫🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺⚫📘⚫🇪🇺🇪🇺🇪🇺🇪🇺⚫➖
  // ➖⚫🇪🇺🇪🇺🇪🇺🇪🇺⚫⬜📘📘⚫🇪🇺🇪🇺⚫➖➖
  // ➖➖⚫⚫⚫⚫⚫⬜📘📘⚫📘🪦⚫➖➖
  // ➖➖➖➖➖➖➖⚫⬜📘📘⚫📘🪦⚫➖
  // ➖➖➖➖➖➖➖➖⚫⬜📘📘⚫📘🪦⚫
  // ➖➖➖➖➖➖➖⚫⬜📘📘⚫📘🪦⚫➖
  // ➖➖➖➖➖➖⚫⬜📘📘⚫📘🪦⚫➖➖
  // ➖➖➖⚫⚫⚫⬜📘📘⚫📘🪦⚫➖➖➖
  // ➖➖⚫⬜⬜⬜📘📘⚫📘🪦⚫➖➖➖➖
  // Walks left/right, shoots fast projectiles when player is in front (doesn't turn on hit)
 public:
  Ostrich(geometry::Position position) : Enemy(position, geometry::Size(16, 16), 2) {}
  virtual void update(AbstractSoundManager& sound_manager, const geometry::Rectangle& player_rect, Level& level) override;
  virtual std::vector<ObjectDef> get_sprites([[maybe_unused]] const Level& level) const override
  {
    return {{position, static_cast<int>(left_ ? Sprite::SPRITE_OSTRICH_L_1 : Sprite::SPRITE_OSTRICH_R_1) + frame_, false}};
  }
  virtual int get_points() const override { return 100; }
  virtual std::vector<geometry::Rectangle> get_detection_rects(const Level& level) const override
  {
    return create_detection_rects(left_ ? -1 : 1, 0, level);
  }
  virtual void on_death(AbstractSoundManager& sound_manager, Level& level) override;

 private:
  int frame_ = 0;
  bool left_ = false;
  int next_shoot_ = 0;
};
