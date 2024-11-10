#pragma once
#include <utility>

#include "actor.h"
#include "geometry.h"
#include "misc.h"
#include "sprite.h"

struct Level;
class Spider;

class Hazard : public Actor
{
 public:
  Hazard(geometry::Position position) : Actor(position, geometry::Size(16, 16)) {}
  virtual ~Hazard() = default;
};

class AirTank : public Hazard
{
  // ⬛⬜⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬜⬛
  // ⬜⬛⚪⚪⚪⚪⚪⚪⚪⚪⚪⚪⚪⚪⬛🪦
  // ⬜⬛⚪🟥🟥⚪⚪🟥🟥⚪🟥🟥🟥⚪⬛🪦
  // ⬜⬛🟥⚪⚪🟥⚪🟥🟥⚪🟥⚪⚪🟥⬛🪦
  // ⬜⬛🟥🟥🟥🟥⚪🟥🟥⚪🟥🟥🟥⚪⬛🪦
  // ⬜⬛🟥⚪⚪🟥⚪🟥🟥⚪🟥⚪🟥⚪⬛🪦
  // ⬜⬛🟥⚪⚪🟥⚪🟥🟥⚪🟥⚪⚪🟥⬛🪦
  // ⬜⬛⚪⚪⚪⚪⚪⚪⚪⚪⚪⚪⚪⚪⬛🪦
  // ⬜⬛⚪⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⚪⬛🪦
  // ⬜⬛⬛🟥🟥🟥🟨🟨🟨🟨🦚🦚🦚⬛⬛🪦
  // ⬜⬛⬛🟥🟥🟥🟨🟨🟨🟨🦚🦚🦚⬛⬛🪦
  // ⬜⬛⚪⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⚪⬛🪦
  // ⬜⬛⚪⚪⚪⚪⚪⚪⚪⬛⬜⬛⚪⚪⬛🪦
  // ⬜⬛⚪⚪⚪⚪⚪⚪⚪⬛⬜⬛⚪⚪⬛🪦
  // ⬜⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛🪦
  // ⬛🪦🪦🪦🪦🪦🪦🪦🪦🪦🪦🪦🪦🪦🪦⬛
  // Die if shot
 public:
  AirTank(geometry::Position position, bool top) : Hazard(position), top_(top) {}

  virtual void update(const geometry::Rectangle& player_rect, Level& level) override;
  virtual std::vector<std::pair<geometry::Position, Sprite>> get_sprites(const Level& level) const override;

 private:
  bool top_;
  int frame_ = 0;
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
 public:
  Laser(geometry::Position position, bool left) : Hazard(position), left_(left) {}

  virtual void update(const geometry::Rectangle& player_rect, Level& level) override;
  virtual std::vector<std::pair<geometry::Position, Sprite>> get_sprites([[maybe_unused]] const Level& level) const override
  {
    return {std::make_pair(position, left_ ? Sprite::SPRITE_LASER_L : Sprite::SPRITE_LASER_R)};
  }
  virtual std::vector<geometry::Rectangle> get_detection_rects(const Level& level) const override
  {
    return create_detection_rects(left_ ? -1 : 1, 0, level);
  }
  void remove_child() { child_ = nullptr; }

 private:
  bool left_;
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
  LaserBeam(geometry::Position position, bool left, Laser& parent) : Hazard(position), left_(left), parent_(parent) {}

  virtual void update(const geometry::Rectangle& player_rect, Level& level) override;
  virtual std::vector<std::pair<geometry::Position, Sprite>> get_sprites([[maybe_unused]] const Level& level) const override
  {
    return {std::make_pair(position, frame_ == 0 ? Sprite::SPRITE_LASER_BEAM_1 : Sprite::SPRITE_LASER_BEAM_2)};
  }
  virtual bool is_alive() const override { return alive_; }

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

  virtual void update(const geometry::Rectangle& player_rect, Level& level) override;
  virtual std::vector<std::pair<geometry::Position, Sprite>> get_sprites([[maybe_unused]] const Level& level) const override
  {
    return {std::make_pair(position, static_cast<Sprite>(static_cast<int>(Sprite::SPRITE_THORN_1) + frame_))};
  }
  virtual std::vector<geometry::Rectangle> get_detection_rects(const Level& level) const override
  {
    return create_detection_rects(0, -1, level, true);
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

  virtual void update(const geometry::Rectangle& player_rect, Level& level) override;
  virtual std::vector<std::pair<geometry::Position, Sprite>> get_sprites([[maybe_unused]] const Level& level) const override
  {
    return {std::make_pair(position, Sprite::SPRITE_SPIDER_WEB)};
  }
  virtual bool is_alive() const override { return alive_; }

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

  virtual void update(const geometry::Rectangle& player_rect, Level& level) override;
  virtual std::vector<std::pair<geometry::Position, Sprite>> get_sprites([[maybe_unused]] const Level& level) const override
  {
    return {std::make_pair(position, sprite_)};
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

  virtual void update(const geometry::Rectangle& player_rect, Level& level) override;
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

  virtual void update(const geometry::Rectangle& player_rect, Level& level) override;
  virtual std::vector<std::pair<geometry::Position, Sprite>> get_sprites([[maybe_unused]] const Level& level) const override
  {
    return {std::make_pair(position, frame_ == 0 ? Sprite::SPRITE_DROPLET_1 : Sprite::SPRITE_DROPLET_2)};
  }
  virtual bool is_alive() const override { return alive_; }

 private:
  bool left_;
  int frame_ = 0;
  Faucet& parent_;
  bool alive_ = true;
};