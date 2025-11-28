#pragma once
#include <utility>

#include "actor.h"
#include "geometry.h"
#include "misc.h"
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

class LaserBeam;

class Laser : public Hazard
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
  void remove_child() { child_ = nullptr; }

 private:
  bool left_;
  bool moving_;
  bool down_ = false;
  LaserBeam* child_ = nullptr;
};

class LaserBeam : public Hazard
{
  // 🟥⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛🟥
  // ⬛🟥🚨🚨⬛⬛🟥⬛🚨⬛🟥🟥🟥⬛🟥⬛
  // ⬛🚨🟥⬛🚨🟥⬛🚨⬛🚨⬛⬛⬛🟥⬛🚨
  // 🚨⬛⬛🟥🟥⬛🚨⬛🟥⬛🚨🚨⬛⬛🚨⬛
  // ⬛⬛⬛⬛⬛🚨⬛⬛⬛⬛⬛⬛🚨🚨⬛⬛
  // Moves left/right, disappear on collide or out of frame
 public:
  LaserBeam(geometry::Position position, bool left, Laser& parent) : Hazard(position, {8, 8}), left_(left), parent_(parent) {}

  virtual void update(AbstractSoundManager& sound_manager, const geometry::Rectangle& player_rect, Level& level) override;
  virtual std::vector<std::pair<geometry::Position, Sprite>> get_sprites([[maybe_unused]] const Level& level) const override
  {
    return {std::make_pair(position - geometry::Size(4, 4), frame_ == 0 ? Sprite::SPRITE_LASER_BEAM_1 : Sprite::SPRITE_LASER_BEAM_2)};
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
  bool left_;
  int frame_ = 0;
  Laser& parent_;
  bool alive_ = true;
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
  // ➖➖➖➖➖➖➖⚫⚫⚫➖➖➖➖➖➖
  // ➖➖⚫⚫⚫⚫⚫🟨🟨🟨⚫⚫⚫⚫⚫➖
  // ➖⚫⬜⬜⬜⬜🟨🟨🟨🟨🟨⬜⬜⬜⬜⚫
  // ➖➖⚫⬜⬜⬜⬜🟨🟨🟨⬜⬜⬜⬜⚫➖
  // Moves down
  // TODO: breaks on ground
  // TODO: sometimes hatches into a small bird
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
