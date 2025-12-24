#pragma once
#include <utility>

#include "actor.h"
#include "geometry.h"
#include "misc.h"
#include "particle.h"
#include "sprite.h"

struct Level;
class Bird;
class Spider;

class Hazard : public Actor
{
 public:
  Hazard(geometry::Position position, geometry::Size size) : Actor(position, size) {}
  Hazard(geometry::Position position) : Actor(position, geometry::Size(16, 16)) {}
  virtual ~Hazard() = default;
};

class Laser
  : public Hazard
  , public ProjectileParent
{
  // ⚫🩵🩵⚫⚫⚫⚫⚫⚫⚫⚫⚫⚫⚫⚫⚫
  // ⚫🔴🟥🩵🩵⚫⚫⚫⚫⚫⚫⚫⚫⚫⚫⚫
  // ⚫⚫🔴🟥⚪🩵⚫⚫⚫⚫⚫⚫⚫⚫⚫⚫
  // ⚫⚫🔴🟥⚪⚪🩵🩵⚫⚫⚫⚫⚫⚫⬜⚫
  // ⚫🔴🟥⚪⚪⚪⚪⚪⬜⬜⬜⬜⬜⚫📘⚫
  // ⚫🩵🩵⚪🩵⚪🩵⚪⚫⚪⚪⚪📘⚫📘⚫
  // ⚫🩵🩵🩵⚪🩵⚪🩵⚫⚪⚪⚪📘⚫📘⚫
  // ⚫🔴🟥⚪⚪⚪⚪⚪⚫🪦🪦🪦🪦⚫📘⚫
  // ⚫⚫🔴🟥⚪⚪🪦🪦⚫⚫⚫⚫⚫⚫🪦⚫
  // ⚫⚫🔴🟥⚪🪦⚫⚫⚫⚫⚫⚫⚫⚫⚫⚫
  // ⚫🔴🟥🪦🪦⚫⚫⚫⚫⚫⚫⚫⚫⚫⚫⚫
  // ⚫🪦🪦⚫⚫⚫⚫⚫⚫⚫⚫⚫⚫⚫⚫⚫
  // Faces left/right, fires slow laser at player when they enter line
  // Optionally moves up/down
 public:
  Laser(geometry::Position position, bool left, bool moving = false) : Hazard(position), left_(left), moving_(moving) {}

  virtual void update(AbstractSoundManager& sound_manager, const geometry::Rectangle& player_rect, Level& level) override;
  virtual std::vector<std::pair<geometry::Position, Sprite>> get_sprites([[maybe_unused]] const Level& level) const override
  {
    return {std::make_pair(position, left_ ? Sprite::SPRITE_LASER_L : Sprite::SPRITE_LASER_R)};
  }
  virtual std::vector<geometry::Rectangle> get_detection_rects(const Level& level) const override;

 private:
  bool left_;
  bool moving_;
  bool down_ = false;
};

class Projectile : public Hazard
{
  // ABC, Fired by enemy, moves in a straight line, hurts player on touch, has parent (only one per parent)
 public:
  Projectile(geometry::Position position, const bool left, ProjectileParent& parent)
    : Hazard(position + geometry::Position(4, 4), geometry::Size(8, 8)),
      left_(left),
      parent_(parent)
  {
  }

  virtual void update(AbstractSoundManager& sound_manager, const geometry::Rectangle& player_rect, Level& level) override;
  virtual std::vector<std::pair<geometry::Position, Sprite>> get_sprites(const Level& level) const override;
  virtual TouchType on_touch([[maybe_unused]] const Player& player,
                             [[maybe_unused]] AbstractSoundManager& sound_manager,
                             [[maybe_unused]] Level& level) override
  {
    return TouchType::TOUCH_TYPE_HURT;
  }
  virtual bool is_alive() const override { return alive_; }
  void kill(Level& level)
  {
    alive_ = false;
    parent_.remove_child(level);
  }

 protected:
  virtual int get_speed() const = 0;
  virtual Sprite get_sprite() const = 0;
  virtual int num_sprites() const = 0;
  bool left_;
  bool alive_ = true;
  ProjectileParent& parent_;
  int frame_ = 0;
};

class HittableProjectile : public Projectile
{
  // ABC for projectile that can be destroyed by player
 public:
  using Projectile::Projectile;

  virtual bool on_hit([[maybe_unused]] const geometry::Rectangle& rect,
                      AbstractSoundManager& sound_manager,
                      [[maybe_unused]] const geometry::Rectangle& player_rect,
                      Level& level,
                      [[maybe_unused]] const bool power) override
  {
    kill(level);
    sound_manager.play_sound(SoundType::SOUND_ENEMY_DIE);
    return true;
  }
  virtual const std::vector<Sprite>* get_explosion_sprites() const override { return &Explosion::sprites_implosion; }
};

class Eyeball : public HittableProjectile
{
  // ➖➖➖➖⚫⚫⚫⚫➖➖➖⚫⚫⚫➖➖
  // ➖➖➖⚫⬜⬜🦚🦚⚫⚫⚫🦚🦚🦚⚫➖
  // ➖➖⚫🟦🟦📘⬜🦚🦚🦚🦚⚫⚫⚫➖➖
  // ➖⚫🟦⚫⚫🟦📘⬜🦚🦚⚫⚫⚫⚫➖➖
  // ➖⚫🟦⚫⚫🟦📘⬜🦚🦚🦚🦚🦚🦚⚫➖
  // ➖➖⚫🟦🟦📘⬜🦚🦚⚫⚫⚫⚫⚫➖➖
  // ➖➖➖⚫⬜⬜🦚🦚⚫➖➖➖➖➖➖➖
  // ➖➖➖➖⚫⚫⚫⚫➖➖➖➖➖➖➖➖
 public:
  using HittableProjectile::HittableProjectile;

 protected:
  virtual int get_speed() const override { return 4; }
  virtual Sprite get_sprite() const override { return left_ ? Sprite::SPRITE_EYEBALL_L_1 : Sprite::SPRITE_EYEBALL_R_1; }
  virtual int num_sprites() const override { return 4; }
};

class TriceratopsShot : public HittableProjectile
{
  // ➖➖➖➖➖⚫➖➖➖➖⚫➖➖➖➖➖
  // ➖➖➖➖⚫🟥⚫➖➖⚫🟥⚫➖➖➖➖
  // ➖➖➖➖➖⚫🟥⚫⚫🟥⚫➖➖➖➖➖
  // ➖➖➖➖➖➖⚫🟨🟨⚫➖➖➖➖➖➖
  // ➖➖➖➖➖➖⚫🟨🟨⚫➖➖➖➖➖➖
  // ➖➖➖➖➖⚫🟥⚫⚫🟥⚫➖➖➖➖➖
  // ➖➖➖➖⚫🟥⚫➖➖⚫🟥⚫➖➖➖➖
  // ➖➖➖➖➖⚫➖➖➖➖⚫➖➖➖➖➖
 public:
  using HittableProjectile::HittableProjectile;

 protected:
  virtual int get_speed() const override { return 8; }
  virtual Sprite get_sprite() const override { return Sprite::SPRITE_TRICERATOPS_SHOT_1; }
  virtual int num_sprites() const override { return 2; }
};

class LaserBeam : public Projectile
{
  // 🟥⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛🟥
  // ⬛🟥🚨🚨⬛⬛🟥⬛🚨⬛🟥🟥🟥⬛🟥⬛
  // ⬛🚨🟥⬛🚨🟥⬛🚨⬛🚨⬛⬛⬛🟥⬛🚨
  // 🚨⬛⬛🟥🟥⬛🚨⬛🟥⬛🚨🚨⬛⬛🚨⬛
  // ⬛⬛⬛⬛⬛🚨⬛⬛⬛⬛⬛⬛🚨🚨⬛⬛
  // Moves left/right, disappear on collide or out of frame
 public:
  LaserBeam(geometry::Position position, bool left, ProjectileParent& parent, bool moving = true)
    : Projectile(position, left, parent),
      moving_(moving)
  {
  }

  virtual void update(AbstractSoundManager& sound_manager, const geometry::Rectangle& player_rect, Level& level) override;

 protected:
  virtual int get_speed() const override { return moving_ ? 4 : 0; }
  virtual Sprite get_sprite() const override { return Sprite::SPRITE_LASER_BEAM_1; }
  virtual int num_sprites() const override { return 2; }

 private:
  bool moving_;
  int kill_frame_ = 0;
};

class Thorn : public Hazard
{
  // ⬛⬛⬛⬛⬛⬛⬛🦚⬛⬛⬛⬛⬛⬛⬛⬛
  // ⬛⬛⬛⬛⬛⬛⬛🦚⬛⬛⬛⬛⬛⬛⬛⬛
  // ⬛⬛⬛⬛⬛⬛⬛🦚🦚⬛⬛⬛⬛⬛⬛⬛
  // ⬛⬛⬛⬛⬛⬛⬛🟩🦚⬛⬛⬛⬛⬛⬛⬛
  // ⬛⬛⬛⬛⬛⬛⬛🟩⬛⬛⬛⬛⬛⬛⬛⬛
  // ⬛⬛⬛⬛⬛⬛🟩🦚🦚⬛⬛⬛⬛⬛⬛⬛
  // ⬛⬛⬛⬛⬛⬛🟩🦚🦚⬛⬛⬛⬛⬛⬛⬛
  // ⬛⬛⬛⬛⬛⬛🦚🦚🦚🦚⬛⬛⬛⬛⬛⬛
  // ⬛⬛⬛⬛⬛⬛🦚🟩🦚🦚⬛⬛⬛⬛⬛⬛
  // ⬛⬛⬛⬛⬛🦚🟩🦚🦚⬛⬛⬛⬛⬛⬛⬛
  // Thrusts up when player is above
 public:
  Thorn(geometry::Position position) : Hazard(position) {}

  virtual void update(AbstractSoundManager& sound_manager, const geometry::Rectangle& player_rect, Level& level) override;
  virtual std::vector<std::pair<geometry::Position, Sprite>> get_sprites([[maybe_unused]] const Level& level) const override
  {
    return {std::make_pair(position, static_cast<Sprite>(static_cast<int>(Sprite::SPRITE_THORN_1) + frame_))};
  }
  virtual std::vector<geometry::Rectangle> get_detection_rects(const Level& level) const override
  {
    return create_detection_rects(0, -1, level, true);
  }
  virtual TouchType on_touch([[maybe_unused]] const Player& player,
                             [[maybe_unused]] AbstractSoundManager& sound_manager,
                             [[maybe_unused]] Level& level) override
  {
    // TODO: sound
    return TouchType::TOUCH_TYPE_HURT;
  }

 private:
  int frame_ = 0;
};

class SpiderWeb : public Hazard
{
  // ⬛⚪⬛⬛⬛⬛⬛⚪⬛⬛⬛⬛⬛⬛⚪⬛
  // ⬛⚪⬜⬜⬛⬛⬛⚪⬛⬛⬛⬛⬛⬛⚪⬛
  // ⬛⚪⬛⬛⬜⬜⬜⚪⬛⬛⬛⬜⬜⬜⚪⬛
  // ⬛⚪⬛⬛⬛⬛⬛⚪⬜⬜⬜⬛⬛⬛⚪⬛
  // ⬛⚪⬛⬛⬛⬛⬛⚪⬛⬛⬛⬛⬛⚪⬛⬛
  // ⬛⬛⚪⬜⬜⬛⬛⬛⚪⬛⬛⬜⬜⚪⬛⬛
  // ⬛⬛⚪⬛⬛⬜⬜⬜⚪⬜⬜⬛⬛⬛⚪⬛
  // ⬛⬛⚪⬛⬛⬛⬛⬛⚪⬛⬛⬛⬛⬛⚪⬛
  // ⬛⬛⚪⬜⬜⬛⬛⬛⬛⚪⬛⬛⬛⬛⚪⬛
  // ⬛⚪⬛⬛⬛⬜⬜⬜⬜⚪⬛⬛⬜⬜⚪⬛
  // ⬛⚪⬛⬛⬛⬛⬛⬛⬛⚪⬜⬜⬛⬛⚪⬛
  // ⬛⚪⬛⬛⬛⬛⬛⬛⚪⬛⬛⬛⬛⬛⚪⬛
  // ⬛⚪⬛⬛⬜⬜⬜⬜⚪⬜⬜⬛⬛⚪⬛⬛
  // ⬛⚪⬜⬜⬛⬛⬛⬛⚪⬛⬛⬜⬜⚪⬛⬛
  // Moves down, disappear on collide or out of frame
 public:
  SpiderWeb(geometry::Position position, Spider& parent) : Hazard(position), parent_(parent) {}

  virtual void update(AbstractSoundManager& sound_manager, const geometry::Rectangle& player_rect, Level& level) override;
  virtual std::vector<std::pair<geometry::Position, Sprite>> get_sprites([[maybe_unused]] const Level& level) const override
  {
    return {std::make_pair(position, Sprite::SPRITE_SPIDER_WEB)};
  }
  virtual bool is_alive() const override { return alive_; }
  virtual TouchType on_touch([[maybe_unused]] const Player& player,
                             [[maybe_unused]] AbstractSoundManager& sound_manager,
                             [[maybe_unused]] Level& level) override
  {
    // TODO: sound
    return TouchType::TOUCH_TYPE_HURT;
  }
  void kill();

 private:
  Spider& parent_;
  bool alive_ = true;
};


class CorpseSlime : public Hazard
{
  // ⬛⬛⬛⬛⬛⬛⬛⬛⬛🟪🟪🟪🟪⬛⬛⬛
  // ⬛⬛🟪🟪🟪🟪🟪🟪🟪🟣🟣🟣🟣🟪⬛⬛
  // ⬛🟪🟣🟣🟣🟣🟣🟣🟣🟣🟣🟣🟣🟣🟣⬛
  //
  // ⬛⬛⬛🟩🟩🟩🟩🟩⬛⬛⬛⬛⬛⬛⬛⬛
  // ⬛⬛🟩🦚🦚🦚🦚🦚🦚🟩🟩🟩🟩⬛⬛⬛
  // ⬛🟩🦚🦚🦚🦚🦚🦚🦚🦚🦚🦚🦚🦚⬛⬛
  // Hurts player if they step on it; created by dead snake/tentacle
 public:
  CorpseSlime(geometry::Position position, Sprite sprite) : Hazard(position), sprite_(sprite) {}

  virtual std::vector<std::pair<geometry::Position, Sprite>> get_sprites([[maybe_unused]] const Level& level) const override
  {
    return {std::make_pair(position, sprite_)};
  }
  virtual TouchType on_touch([[maybe_unused]] const Player& player,
                             [[maybe_unused]] AbstractSoundManager& sound_manager,
                             [[maybe_unused]] Level& level) override
  {
    // TODO: sound
    return TouchType::TOUCH_TYPE_HURT;
  }

 private:
  Sprite sprite_;
};


class Droplet;

class Faucet : public Hazard
{
  // ⬛⬛⬛⬛⬛🩵🩵📘🟦🟦🇪🇺🇪🇺⬛⬛⬛⬛
  // ⬛⬛⬛⬛⬛🩵🩵📘🟦🟦🇪🇺🇪🇺⬛⬛⬛⬛
  // ⬛⬛⬛⬛⬛🩵🩵📘🟦🟦🇪🇺🇪🇺⬛⬛⬛⬛
  // ⬛⬛⬛⬛⬛🩵🩵📘🟦🟦🇪🇺🇪🇺⬛⬛⬛⬛
  // ⬛⬛⬛⬛⬛🩵🩵📘🟦🟦🇪🇺🇪🇺⬛⬛⬛⬛
  // ⬛⬛⬛⬛⬛🩵🩵📘🟦🟦🇪🇺🇪🇺⬛⬛⬛⬛
  // ⬛⬛⬛⬛⬛🩵🩵📘🟦🟦🇪🇺🇪🇺⬛⬛⬛⬛
  // ⬛⬛⬛⬛⬛🩵🩵📘🟦🟦🇪🇺🇪🇺⬛⬛⬛⬛
  // ⬛⬛⬛⬛⬛🩵🩵📘🟦🟦🇪🇺🇪🇺⬛⬛⬛⬛
  // ⬛⬛⬛⬛⬛🩵🩵📘🟦🟦🇪🇺🇪🇺⬛⬛⬛⬛
  // ⬛⬛⬛⬛⬛🩵🩵📘🟦🟦🇪🇺🇪🇺⬛⬛⬛⬛
  // ⬛⬛⬛⬛⬛⬜⬜⬜⚪⚪⚪🪦⬛⬛⬛⬛
  // ⬛⬛⬛⬛🩵🩵🩵📘🟦🟦🇪🇺🇪🇺🇪🇺⬛⬛⬛
  // ⬛⬛⬛🩵🩵🩵📘🟦🟦🟦🟦🇪🇺🇪🇺🇪🇺⬛⬛
  // ⬛⬛🩵🩵🩵📘🟦🟦🟦🟦🟦🟦🇪🇺🇪🇺🇪🇺⬛
  // Drips droplet below
 public:
  Faucet(geometry::Position position) : Hazard(position) {}

  virtual void update(AbstractSoundManager& sound_manager, const geometry::Rectangle& player_rect, Level& level) override;
  virtual std::vector<std::pair<geometry::Position, Sprite>> get_sprites([[maybe_unused]] const Level& level) const override
  {
    return {std::make_pair(position, static_cast<Sprite>(static_cast<int>(Sprite::SPRITE_FAUCET_1) + frame_))};
  }
  void remove_child() { child_ = nullptr; }

 private:
  int frame_ = 0;
  Droplet* child_ = nullptr;
};

class Droplet : public Hazard
{
  // ⬛⬛⬛⬛⬛⬛⬛⬛🟦🟦⬛⬛⬛⬛⬛⬛
  // ⬛⬛⬛⬛⬛⬛⬛🟦🟦🩵⬛⬛⬛⬛⬛⬛
  // ⬛⬛⬛⬛⬛⬛🟦🩵🟦🟦⬛⬛⬛⬛⬛⬛
  // ⬛⬛⬛⬛⬛🟦🩵🩵🩵🟦⬛⬛⬛⬛⬛⬛
  // ⬛⬛⬛⬛⬛🟦🩵🩵🩵🟦🟦⬛⬛⬛⬛⬛
  // ⬛⬛⬛⬛🟦🩵🩵🩵🟦🟦🟦⬛⬛⬛⬛⬛
  // ⬛⬛⬛🟦🩵🩵🩵🩵🩵🟦🟦🟦⬛⬛⬛⬛
  // ⬛⬛⬛🟦🩵🩵🩵🩵🩵🩵🟦🟦⬛⬛⬛⬛
  // ⬛⬛⬛🟦🩵🩵🩵🩵🩵🩵🟦🟦🟦⬛⬛⬛
  // ⬛⬛⬛🟦🩵🟦🩵🩵🩵🩵🩵🟦🟦⬛⬛⬛
  // ⬛⬛⬛🟦🩵🩵🩵🩵🩵🩵🩵🟦🟦⬛⬛⬛
  // ⬛⬛⬛🟦🩵🩵🩵🟦🩵🩵🩵🟦🟦⬛⬛⬛
  // ⬛⬛⬛⬛🟦🩵🩵🟦🩵🩵🟦🟦🟦⬛⬛⬛
  // ⬛⬛⬛⬛⬛🟦🟦🟦🟦🟦🟦🟦⬛⬛⬛⬛
  // Drops down, disappear on collide or out of frame
 public:
  Droplet(geometry::Position position, Faucet& parent) : Hazard(position), parent_(parent) {}

  virtual void update(AbstractSoundManager& sound_manager, const geometry::Rectangle& player_rect, Level& level) override;
  virtual std::vector<std::pair<geometry::Position, Sprite>> get_sprites([[maybe_unused]] const Level& level) const override
  {
    return {std::make_pair(position, frame_ == 0 ? Sprite::SPRITE_DROPLET_1 : Sprite::SPRITE_DROPLET_2)};
  }
  virtual bool is_alive() const override { return alive_; }
  virtual TouchType on_touch([[maybe_unused]] const Player& player,
                             [[maybe_unused]] AbstractSoundManager& sound_manager,
                             [[maybe_unused]] Level& level) override
  {
    // TODO: sound
    return TouchType::TOUCH_TYPE_HURT;
  }

 private:
  int frame_ = 0;
  Faucet& parent_;
  bool alive_ = true;
};

class Hammer : public Hazard
{
  // ⬛⬛⬛⬛🪦🪦🪦🪦🪦🪦🪦🪦🪦⬛⬛⬛⬛⬛⬛🪦🪦🪦🪦🪦🪦🪦🪦🪦⬛⬛⬛⬛
  // ⬛⬛⬛⬛🪦🪦🪦🪦🪦🪦🪦🪦🪦⬛⬛⬛⬛⬛⬛🪦🪦🪦🪦🪦🪦🪦🪦🪦⬛⬛⬛⬛
  // ⬛⬛⬛⬛🪦🪦🪦🪦🪦🪦🪦🪦🪦⬛⬛⬛⬛⬛⬛🪦🪦🪦🪦🪦🪦🪦🪦🪦⬛⬛⬛⬛
  // ⬛⬛⬛⬛⬛⚪🪦🪦🪦🪦🪦🪦🪦🪦⬛⬛⬛⬛⚪🪦🪦🪦🪦🪦🪦🪦🪦⬛⬛⬛⬛⬛
  // ⬛⬛⬛⬛⬛⚪🪦🪦🪦🪦🪦🪦🪦🪦⬛⬛⬛⬛⚪🪦🪦🪦🪦🪦🪦🪦🪦⬛⬛⬛⬛⬛
  // ⬛⬛⬛⬛⬛⚪🪦🪦🪦🪦🪦🪦🪦🪦⬛⬛⬛⬛⚪🪦🪦🪦🪦🪦🪦🪦🪦⬛⬛⬛⬛⬛
  // ⬛⬛⬛⬛⬛⚪🪦🪦🪦🪦🪦🪦🪦🪦⬛⬛⬛⬛⚪🪦🪦🪦🪦🪦🪦🪦🪦⬛⬛⬛⬛⬛
  // ⬛⬛⬛⬛⬛⬛⚪🪦🪦🪦⚪⚪⚪⚪⚪⬛⬛⚪⚪⚪⚪🪦🪦🪦🪦🪦⬛⬛⬛⬛⬛⬛
  // ⬛⬛⬛⬛⬛⬛⚪⚪⚪⚪⚪⚪⚪⚪⚪⬛⬛⚪⚪⚪⚪⚪⚪⚪⚪⚪⬛⬛⬛⬛⬛⬛
  // ⬛⬛⬛⬛⬛⬛⚪⚪⚪⚪⚪⚪⚪🪦🪦⬛⬛⚪⚪⚪⚪⚪⚪⚪⚪⚪⬛⬛⬛⬛⬛⬛
  // ⬛⬛⬛⬛⬛⬛⚪⚪⚪⚪🪦🪦🪦🪦🪦🪦⚪🪦🪦🪦🪦🪦🪦⚪⚪⚪⬛⬛⬛⬛⬛⬛
  // ⬛⬛⬛⬛⬛⬛⬛⚪🪦🪦🪦🪦🪦⬛⬛⬛⬛⬛⬛🪦🪦🪦🪦🪦🪦⬛⬛⬛⬛⬛⬛⬛
  // ⬛⬛⬛⬛⬛⬛⬛⚪🪦🪦🪦🪦⬛🟠🟠🟠🟠🟠🟠⬛🪦🪦🪦🪦⬛⬛⬛⬛⬛⬛⬛⬛
  // ⬛⬛⬛⬛⬛⬛⬛⬛⚪🪦🪦⬛🟠🟠🟠🟠🟠🟠🟠🟠⬛🪦🪦⬛⬛⬛⬛⬛⬛⬛⬛⬛
  // ⬛⬛⬛⬛⬛⬛⬛⬛⚪🪦🪦⬛🟠🟠🟠🟠🟠🟠🟠🟠⬛🪦🪦⬛⬛⬛⬛⬛⬛⬛⬛⬛
  // ⬛⬛⬛⬛⬛⬛⬛⬛⬛🪦🪦⬛🟠🟠🟠🟠🟠🟠🟠🟠⬛🪦⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛
  // ⬛⬛⬛⬛⬛⬛⬛⬛⬛🪦🪦⬛🟠🟠🟠🟠🟠🟠🟠🟠⬛🪦⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛
  // ⬛⬛⬛⬛⬛⬛⬛⬛⬛🪦🪦⬛🟠🟠🟠🟠🟠🟠🟠🟠⬛🪦⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛
  // ⬛⬛⬛⬛⬛⬛⬛⬛⬛🪦🪦⚪⬛🟠🟠🟠🟠🟠🟠⬛🪦🪦⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛
  // ⬛⬛⬛⬛⬛⬛⬛⬛⬛🪦🪦⚪⚪⬛⬛⬛⬛⬛⬛⬛🪦🪦⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛
  // ⬛⬛⬛⬛⬛⬛⬛⬛⬛🪦🪦⚪⚪🪦🪦🪦🪦🪦⬛⬛🪦🪦⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛
  // ⬛⬛⬛⬛⬛⬛⬛⬛🪦🪦⚪⚪⚪🪦🪦🪦🪦🪦⬛⬛🪦🪦🪦⬛⬛⬛⬛⬛⬛⬛⬛⬛
  // ⬛⬛⬛⬛⬛⬛⬛🪦🪦⚪⚪⚪🪦🪦🪦🪦🪦🪦🪦⬛⬛🪦🪦🪦⬛⬛⬛⬛⬛⬛⬛⬛
  // ⬛⬛⬛⬛⬛⬛🪦🪦⚪⚪⚪🪦🪦🪦🪦🪦🪦🪦🪦🪦⬛⬛🪦🪦🪦⬛⬛⬛⬛⬛⬛⬛
  // ⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛
  // ⬛⬛⬛⬛🪦🪦⬜⚪⬜⚪⚪⚪⚪⚪⚪⚪⚪⚪🪦⚪🪦⚪🪦🪦🪦🪦⬛⬛⬛⬛⬛⬛
  // ⬛⬛⬛🪦🪦⬜⚪⬜⚪⬜⚪⚪⚪⚪⚪⚪⚪⚪⚪🪦⚪🪦⚪🪦🪦🪦🪦⬛⬛⬛⬛⬛
  // ⬛⬛⬛🪦🪦⚪⬜⚪⬜⚪⚪⚪⚪⚪⚪⚪⚪⚪⚪⚪🪦⚪🪦⚪🪦🪦🪦⬛⬛⬛⬛⬛
  // ⬛⬛⬛🪦🪦⬜⚪⬜⚪⬜⚪⚪⚪⚪⚪⚪⚪⚪⚪🪦⚪🪦⚪🪦🪦🪦🪦⬛⬛⬛⬛⬛
  // ⬛⬛⬛🪦🪦🪦⬜⚪⬜⚪⚪⚪⚪⚪⚪⚪⚪⚪🪦⚪🪦⚪🪦🪦🪦🪦⬛⬛⬛⬛⬛⬛
  // Rises slowly and drops rapidly
 public:
  Hammer(geometry::Position position) : Hazard(position, geometry::Size(32, 32)) {}

  virtual void update(AbstractSoundManager& sound_manager, const geometry::Rectangle& player_rect, Level& level) override;
  virtual std::vector<std::pair<geometry::Position, Sprite>> get_sprites([[maybe_unused]] const Level& level) const override
  {
    return {
      std::make_pair(position, Sprite::SPRITE_HAMMER_1),
      std::make_pair(position + geometry::Position(16, 0), Sprite::SPRITE_HAMMER_2),
      std::make_pair(position + geometry::Position(0, 16), Sprite::SPRITE_HAMMER_3),
      std::make_pair(position + +geometry::Position(16, 16), Sprite::SPRITE_HAMMER_4),
    };
  }
  virtual TouchType on_touch([[maybe_unused]] const Player& player,
                             [[maybe_unused]] AbstractSoundManager& sound_manager,
                             [[maybe_unused]] Level& level) override
  {
    // TODO: sound
    return (rising_ || frame_ > 0) ? TouchType::TOUCH_TYPE_HURT : TouchType::TOUCH_TYPE_CRUSHING;
  }

 private:
  bool rising_ = true;
  int frame_ = 0;
};

class Flame : public Hazard
{
  // ⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛🟥⬛⬛⬛⬛⬛
  // ⬛⬛⬛⬛⬛⬛⬛⬛🟥⬛⬛⬛⬛⬛⬛⬛
  // ⬛⬛⬛🟥⬛⬛⬛🟥🟥⬛⬛⬛⬛⬛⬛⬛
  // ⬛⬛⬛⬛⬛⬛🟥🟥🟥🟥⬛⬛🟥⬛⬛⬛
  // ⬛⬛⬛⬛⬛⬛🟥🟥🟥🟥🟥⬛⬛⬛⬛⬛
  // ⬛⬛⬛⬛⬛⬛🟥🟥🟥🟥🟥⬛⬛⬛⬛⬛
  // ⬛⬛⬛🟨⬛🟥🟥🟥🚨🟥🟥⬛🟨⬛⬛⬛
  // ⬛⬛⬛⬛⬛🟥🟥🟥🚨🟥🟥🟥⬛⬛⬛⬛
  // ⬛⬛⬛⬛🟥🟥🚨🚨🚨🚨🟥🟥🟥⬛⬛⬛
  // ⬛⬛⬛🟥🟥🚨🚨🚨🚨🚨🚨🟥🟥⬛⬛⬛
  // 🟥⬛⬛🟥🟥🚨🚨🟨🟨🚨🚨🟥🟥🟥⬛🟥
  // ⬛⬛🟥🟥🟥🚨🟨🟨🟨🟨🚨🟥🟥🟥⬛⬛
  // ⬛⬛🟥🟥🚨🚨🟨⬜🟨🟨🚨🚨🟥🟥⬛🟥
  // ⬛⬛🟥🚨🚨🟨🟨⬜⬜🟨🟨🚨🟥🟥⬛⬛
  // ⬛🟥🟥🚨🚨🟨🟨⬜⬜🟨🟨🚨🟥🟥⬛⬛
  // ⬛🟥🚨🚨🟨🟨⬜⬜⬜⬜🟨🟨🚨🟥⬛⬛
  // Hurts player when turned on
 public:
  Flame(geometry::Position position) : Hazard(position) {}

  virtual void update(AbstractSoundManager& sound_manager, const geometry::Rectangle& player_rect, Level& level) override;
  virtual std::vector<std::pair<geometry::Position, Sprite>> get_sprites(const Level& level) const override;
  virtual TouchType on_touch([[maybe_unused]] const Player& player,
                             [[maybe_unused]] AbstractSoundManager& sound_manager,
                             [[maybe_unused]] Level& level) override
  {
    // TODO: sound
    return is_on() ? TouchType::TOUCH_TYPE_HURT : TouchType::TOUCH_TYPE_NONE;
  }

 private:
  bool is_on() const;
  int frame_ = 48;
};

class Stalactite : public Hazard
{
  // ⚫🟨🟠🟠🟠🟠🟠⚫⬛🟨🟠🟠🟠🟠🟠⬛
  // ⚫🟠🟠🟠🟠🟠🟥⬛⚫🟠🟨🟠🟠🟠🟥⚫
  // ⚫🟨🟠🟠🟠🟠🟠⚫⬛🟠🟠🟠🟠🟠🟠⬛
  // ⬛⚫🟠🟠🟠🟥⚫⬛⚫🟠🟠🟠🟠🟠🟠⚫
  // ⬛⚫🟠🟨🟠🟠⚫⬛⬛⚫🟠🟨🟠🟥⚫⬛
  // ⬛⚫🟨🟠🟠🟥⚫⬛⬛⚫🟨🟠🟠🟠⚫⬛
  // ⬛⚫🟠🟠🟠🟠⚫⬛⬛⚫🟠🟨🟠🟥⚫⬛
  // ⬛⬛⚫🟠🟨🟠⚫⬛⬛⚫🟠🟠🟠🟠⚫⬛
  // ⬛⬛⚫🟨🟠🟥⚫⬛⬛⚫🟠🟠🟠🟠⚫⬛
  // ⬛⬛⚫🟠🟠🟠⚫⬛⬛⚫🟨🟠🟥⚫⬛⬛
  // ⬛⬛⚫🟠🟨🟠⚫⬛⬛⚫🟠🟠🟠⚫⬛⬛
  // ⬛⬛⬛⚫🟠⚫⬛⬛⬛⚫🟨🟠🟥⚫⬛⬛
  // ⬛⬛⬛⚫🟨⚫⬛⬛⬛⚫🟠🟨🟠⚫⬛⬛
  // ⬛⬛⬛⬛⚫⬛⬛⬛⬛⚫🟠🟠🟠⚫⬛⬛
  // ⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⚫🟠⚫⬛⬛⬛
  // Falls if player gets under
 public:
  Stalactite(geometry::Position position) : Hazard(position) {}

  virtual bool is_alive() const override { return position.y() < 1000; }
  virtual void update(AbstractSoundManager& sound_manager, const geometry::Rectangle& player_rect, Level& level) override;
  virtual std::vector<std::pair<geometry::Position, Sprite>> get_sprites([[maybe_unused]] const Level& level) const override
  {
    return {std::make_pair(position, Sprite::SPRITE_STALACTITE_1)};
  }
  virtual TouchType on_touch([[maybe_unused]] const Player& player,
                             [[maybe_unused]] AbstractSoundManager& sound_manager,
                             [[maybe_unused]] Level& level) override
  {
    // TODO: sound
    return TouchType::TOUCH_TYPE_HURT;
  }
  virtual std::vector<geometry::Rectangle> get_detection_rects(const Level& level) const override
  {
    return create_detection_rects(0, 1, level);
  }

 private:
  bool asleep_ = true;
};

class AirPipe : public Hazard
{
  // ⚫🟨🟨⬜🟨🟨🟨🟠🟨⚫➖➖⚫⚫⚫➖
  // ⚫🟨🟨⬜🟨🟨🟨🟠🟨⚫➖⚫🟨🟨🟨⚫
  // ⚫🟨🟨🟨🟨🟨🟨🟨🟨⚫⚫🟨🟠⚫🟠🟨
  // ⚫🟨🟨⬜🟨🟨🟨🟠🟨🟨🟨🟨🟠⚫🟠🟨
  // ⚫🟨🟨⬜🟨🟨🟨🟨🟨🟨🟠🟠🟠⚫🟠🟨➖➖⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜➖➖➖➖
  // ⚫🟨🟨🟨🟨🟨🟨🟨🟠🟠🟨🟨🟠⚫🟠🟨
  // ⚫🟨🟨⬜🟨🟨🟨🟨🟨🟨🟨🟨🟠⚫🟠🟨
  // ➖⚫🟨🟨🟨🟨🟨🟨🟨🟨🟨🟨🟠⚫🟠🟨➖➖⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜➖➖
  // ➖⚫🟨🟨🟨🟨🟨🟨🟨🟨🟨🟨🟠⚫🟠🟨
  // ➖➖⚫🟨🟨🟨🟨🟨🟨🟨🟨🟨🟠⚫🟠🟨
  // ➖➖➖⚫🟨🟨⬜⬜🟨🟨🟨🟨🟠⚫🟠🟨➖➖⬜⬜⬜⬜⬜⬜⬜⬜➖➖➖➖➖➖
  // ➖➖➖➖⚫🟨🟨🟨⬜⬜🟨🟨🟠⚫🟠🟨
  // ➖➖➖➖➖⚫⚫🟨🟨🟨⬜🟨🟠⚫🟠🟨
  // ➖➖➖➖➖➖➖⚫⚫🟨🟨🟨🟠⚫🟠🟨
  // ➖➖➖➖➖➖➖➖➖⚫⚫⚫🟨🟨🟨⚫
  // ➖➖➖➖➖➖➖➖➖➖➖➖⚫⚫⚫➖
  // Sucks in player and kills them on touch
 public:
  AirPipe(geometry::Position position, bool is_left) : Hazard(position), is_left_(is_left) {}

  virtual void update(AbstractSoundManager& sound_manager, const geometry::Rectangle& player_rect, Level& level) override;
  virtual std::vector<std::pair<geometry::Position, Sprite>> get_sprites(const Level& level) const override;
  virtual TouchType on_touch(const Player& player, AbstractSoundManager& sound_manager, Level& level) override;
  virtual std::vector<geometry::Rectangle> get_detection_rects(const Level& level) const override
  {
    return create_detection_rects(is_left_ ? -1 : 1, 0, level);
  }

 private:
  bool is_left_;
  int frame_ = 0;
};

class Speleothem : public Hazard
{
  // ➖➖➖➖➖➖➖➖➖➖➖⚫➖➖➖➖
  // ➖➖➖➖➖➖➖➖➖➖⚫🟠⚫➖➖➖
  // ➖➖➖➖⚫➖➖➖➖⚫🟠🟠🟠⚫➖➖
  // ➖➖➖⚫🟨⚫➖➖➖⚫🟠🟨🟠⚫➖➖
  // ➖➖➖⚫🟠⚫➖➖➖⚫🟨🟠🟥⚫➖➖
  // ➖➖⚫🟠🟨🟠⚫➖➖⚫🟠🟠🟠⚫➖➖
  // ➖➖⚫🟠🟠🟠⚫➖➖⚫🟠🟠🟥⚫➖➖
  // ➖➖⚫🟨🟠🟥⚫➖➖⚫🟠🟨🟠🟠⚫➖
  // ➖➖⚫🟠🟠🟠⚫➖➖⚫🟨🟠🟠🟠⚫➖
  // ➖⚫🟠🟨🟠🟠⚫➖➖⚫🟠🟠🟠🟥⚫➖
  // ➖⚫🟨🟠🟠🟥⚫➖➖⚫🟨🟠🟠🟠⚫➖
  // ➖⚫🟠🟠🟠🟠⚫➖➖⚫🟠🟨🟠🟥⚫➖
  // ➖⚫🟨🟠🟠🟥⚫➖⚫🟠🟠🟠🟠🟠🟠⚫
  // ⚫🟠🟠🟠🟠🟠🟠⚫⚫🟨🟠🟠🟠🟠🟠⚫
  // ⚫🟠🟨🟠🟠🟠🟥⚫⚫🟠🟨🟠🟠🟠🟥⚫
  // ⚫🟨🟠🟠🟠🟠🟠⚫⚫🟠🟠🟠🟠🟠🟠⚫
  // Hurts player on touch
 public:
  Speleothem(geometry::Position position, const Sprite sprite) : Hazard(position), sprite_(sprite) {}

  virtual std::vector<std::pair<geometry::Position, Sprite>> get_sprites([[maybe_unused]] const Level& level) const override
  {
    return {{position, sprite_}};
  }
  virtual TouchType on_touch([[maybe_unused]] const Player& player,
                             [[maybe_unused]] AbstractSoundManager& sound_manager,
                             [[maybe_unused]] Level& level) override
  {
    // TODO: sound
    return TouchType::TOUCH_TYPE_HURT;
  }

 private:
  Sprite sprite_;
};

class FallingRock : public Hazard
{
  // ➖➖➖➖➖⚫⚫⚫⚫⚫⚫➖➖➖➖➖
  // ➖➖➖⚫⚫🪦⚪🪦⚪🪦⚫⚫⚫➖➖➖
  // ➖➖⚫⚪🪦⚪🪦⚪🪦🟠🪦⚫🪦⚫➖➖
  // ➖⚫⚪🪦⚪🪦⚪🪦⚪🪦🟠🪦⚫🪦⚫➖
  // ⚫⚪🪦🟠🪦⚪🪦🟠🪦🟠🪦🟠🪦⚫🪦⚫
  // ⚫🪦⚪🪦⚪🪦🟠🪦🟠🪦⚫🪦⚫🪦⚫⚫
  // ⚫⚪🪦⚪🪦🟠🪦🟠🪦🟠🪦🟠🪦⚫🪦⚫
  // ⚫🪦⚪🪦🟠🪦🟠🪦🟠🪦⚫🪦⚫🪦⚫⚫
  // ⚫⚪🪦⚪🪦🟠🪦🟠🪦🟠🪦⚫🪦⚫🪦⚫
  // ⚫🪦⚪🪦⚪🪦🟠🪦🟠🪦🟠🪦⚫🪦⚫➖
  // ➖⚫🪦⚪🪦🟠🪦🟠🪦🟠🪦⚫🪦⚫➖➖
  // ➖➖⚫🪦⚪🪦🟠🪦🟠🪦⚫🪦⚫➖➖➖
  // ➖➖➖⚫⚫🟠🪦🟠🪦⚫🪦⚫➖➖➖➖
  // ➖➖➖➖➖⚫⚫⚫⚫➖➖➖➖➖➖➖
  // Falls from above, hurts player on touch
 public:
  FallingRock(geometry::Position position) : Hazard(position) {}

  virtual bool is_alive() const override { return position.y() < 1000; }
  virtual void update([[maybe_unused]] AbstractSoundManager& sound_manager,
                      [[maybe_unused]] const geometry::Rectangle& player_rect,
                      [[maybe_unused]] Level& level) override
  {
    position += geometry::Position{0, 6};
  }
  virtual std::vector<std::pair<geometry::Position, Sprite>> get_sprites([[maybe_unused]] const Level& level) const override
  {
    return {{position, Sprite::SPRITE_FALLING_ROCK}};
  }
  virtual TouchType on_touch([[maybe_unused]] const Player& player,
                             [[maybe_unused]] AbstractSoundManager& sound_manager,
                             [[maybe_unused]] Level& level) override
  {
    // TODO: sound
    return TouchType::TOUCH_TYPE_HURT;
  }
};

class BirdEgg : public Hazard
{
  // ➖➖➖➖➖➖➖⚫⚫⚫➖➖➖➖➖➖
  // ➖➖➖➖➖➖⚫⬜⬜⬜⚫➖➖➖➖➖
  // ➖➖➖➖➖⚫⬜⬜⚪⚪⬜⚫➖➖➖➖
  // ➖➖➖➖⚫⬜⬜⬜⬜⬜⚪⬜⚫➖➖➖
  // ➖➖➖➖⚫⬜⬜⬜⬜⬜⬜⬜⚫➖➖➖
  // ➖➖➖➖➖⚫⬜⬜⬜⬜⬜⚫➖➖➖➖
  // ➖➖➖➖➖➖⚫⬜⬜⬜⚫➖➖➖➖➖
  // ➖➖➖➖➖➖➖⚫⚫⚫➖➖➖➖➖➖
  // Moves down, breaks on ground, sometimes hatches into a small bird, 10% chance
 public:
  BirdEgg(geometry::Position position, Bird& parent) : Hazard(position), parent_(parent) {}

  virtual void update(AbstractSoundManager& sound_manager, const geometry::Rectangle& player_rect, Level& level) override;
  virtual std::vector<std::pair<geometry::Position, Sprite>> get_sprites([[maybe_unused]] const Level& level) const override
  {
    return {std::make_pair(position, Sprite::SPRITE_BIRD_EGG)};
  }
  virtual bool is_alive() const override { return alive_; }
  virtual TouchType on_touch([[maybe_unused]] const Player& player,
                             [[maybe_unused]] AbstractSoundManager& sound_manager,
                             [[maybe_unused]] Level& level) override
  {
    // TODO: sound
    return TouchType::TOUCH_TYPE_HURT;
  }

 private:
  Bird& parent_;
  bool alive_ = true;
};

class BirdEggOpen : public CorpseSlime
{
  // ➖➖➖➖➖➖➖⚫⚫⚫➖➖➖➖➖➖
  // ➖➖⚫⚫⚫⚫⚫🟨🟨🟨⚫⚫⚫⚫⚫➖
  // ➖⚫⬜⬜⬜⬜🟨🟨🟨🟨🟨⬜⬜⬜⬜⚫
  // ➖➖⚫⬜⬜⬜⬜🟨🟨🟨⬜⬜⬜⬜⚫➖
  // Disappears shortly
 public:
  BirdEggOpen(geometry::Position position, Bird& parent) : CorpseSlime(position, Sprite::SPRITE_BIRD_EGG_OPEN), parent_(parent) {}
  virtual void update(AbstractSoundManager& sound_manager, const geometry::Rectangle& player_rect, Level& level) override;
  virtual bool is_alive() const override { return frame_ < 24; }

 private:
  Bird& parent_;
  int frame_ = 0;
};

class FallingSign : public Hazard
{
  // 🚨🚨🚨🚨🚨🚨🚨🚨🚨🚨🚨🚨🚨🚨🚨🚨🚨🚨🚨🚨🚨🚨🚨🚨🚨🚨🚨🚨🚨🚨🚨⚫
  // 🚨🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥⚫
  // 🚨🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥⚫
  // 🚨🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟨🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥⚫
  // 🚨🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟨🟥🟥🟨🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥⚫
  // 🚨🟥🟥🟥🟨🟨🟥🟥🟥🟨🟥🟥🟨🟨🟥🟨🟥🟨🟨🟨🟥🟥🟥🟥🟨🟨🟨🟥🟥🟥🟥⚫
  // 🚨🟥🟥🟥🟨🟥🟨🟥🟨🟥🟨🟥🟨🟥🟨🟨🟥🟨🟥🟨🟥🟨🟨🟥🟨🟥🟥🟨🟥🟥🟥⚫
  // 🚨🟥🟥🟥🟨🟥🟨🟥🟨🟥🟨🟥🟨🟥🟨🟥🟥🟨🟨🟨🟥🟨🟥🟥🟨🟥🟨🟥🟥🟥🟥⚫
  // 🚨🟥🟥🟨🟥🟥🟨🟥🟨🟨🟨🟥🟥🟥🟥🟥🟥🟥🟥🟨🟥🟨🟨🟥🟨🟨🟥🟥🟥🟥🟥⚫
  // 🚨🟥🟥🟨🟨🟨🟥🟥🟨🟥🟥🟨🟥🟥🟥🟥🟥🟥🟨🟨🟥🟨🟥🟥🟥🟨🟨🟥🟥🟥🟥⚫
  // 🚨🟥🟥🟥🟥🟥🟥🟨🟥🟥🟥🟥🟥🟥🟥🟥🟥🟨🟥🟥🟥🟨🟨🟥🟥🟨🟥🟨🟥🟥🟥⚫
  // 🚨🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟨🟥🟥🟥🟥🟥🟥⚫
  // 🚨🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥⚫
  // 🚨🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥⚫
  // 🚨🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥⚫
  // Falls when player is underneath
 public:
  FallingSign(geometry::Position position, const std::vector<Sprite>& sprites)
    : Hazard(position, geometry::Size(16 * static_cast<int>(sprites.size()), 16)),
      sprites_(sprites)
  {
  }
  virtual void update(AbstractSoundManager& sound_manager, const geometry::Rectangle& player_rect, Level& level) override;
  virtual std::vector<std::pair<geometry::Position, Sprite>> get_sprites(const Level& level) const override;
  virtual std::vector<geometry::Rectangle> get_detection_rects(const Level& level) const override
  {
    return create_detection_rects(0, 1, level, true);
  }
  virtual TouchType on_touch([[maybe_unused]] const Player& player,
                             [[maybe_unused]] AbstractSoundManager& sound_manager,
                             [[maybe_unused]] Level& level) override
  {
    return falling_ ? TouchType::TOUCH_TYPE_CRUSHING : TouchType::TOUCH_TYPE_NONE;
  }
  virtual bool is_solid_top([[maybe_unused]] const Level& level) const override { return landed_; }

 private:
  bool falling_ = false;
  bool landed_ = false;
  std::vector<Sprite> sprites_;
};
