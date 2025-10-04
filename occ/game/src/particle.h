#pragma once

#include "geometry.h"
#include "misc.h"
#include "sprite.h"

class Particle
{
 public:
  Particle(geometry::Position position) : position(position) {}
  virtual ~Particle() = default;

  virtual void update() = 0;
  virtual int get_sprite() const = 0;
  virtual bool is_alive() const = 0;

  geometry::Position position;
};

class Explosion : public Particle
{
  // ➖➖➖➖⚫⚫➖➖➖➖⚫⚫⚫➖➖➖
  // ➖➖➖⚫🟥🟥⚫➖➖⚫🟥🟥🟥⚫➖➖
  // ➖➖➖⚫🟥🟨🟥⚫⚫🟥🟨🟥⚫➖➖➖
  // ➖➖➖⚫🟥🟨🟨🟥⚫🟥🟨🟥⚫➖➖➖
  // ➖➖➖➖⚫🟥🟨🟨🟥🟨🟨🟥⚫➖➖➖
  // ➖➖➖⚫🟥🟨🟨🟨🟨🟨🟨🟥⚫⚫➖➖
  // ➖➖➖⚫🟥🟨🟨🟥🟥🟨🟨🟨🟥🟥⚫➖
  // ➖⚫⚫🟥🟨🟨🟥🟨🟨🟨🟨🟨🟨🟨🟥⚫
  // ⚫🟥🟥🟨🟨🟨🟥🟨🟨🟨🟥🟨🟨🟥🟥⚫
  // ⚫🟥🟥🟥🟨🟨🟨🟨🟨🟥🟨🟨🟥⚫⚫➖
  // ➖⚫⚫⚫🟥🟨🟨🟨🟨🟨🟥🟨🟥⚫➖➖
  // ➖➖➖⚫🟥🟥🟥🟥🟨🟥⚫🟥🟨🟥⚫➖
  // ➖➖⚫🟥🟥⚫⚫⚫🟥🟥⚫⚫🟥🟥⚫➖
  // ➖➖⚫🟥⚫➖➖➖⚫🟥⚫➖⚫⚫➖➖
  // ➖➖➖⚫➖➖➖➖➖⚫➖➖➖➖➖➖
  // Short-lived animated sprite
 public:
  Explosion(geometry::Position position, const std::vector<Sprite>& sprites) : Particle(position), sprites_(sprites) {}

  virtual void update() override;
  virtual int get_sprite() const override;
  virtual bool is_alive() const override;

  static const std::vector<Sprite> sprites_explosion;
  static const std::vector<Sprite> sprites_implosion;

 private:
  unsigned frame_ = 0;
  const std::vector<Sprite>& sprites_;
};

class ScoreParticle : public Particle
{
 public:
  ScoreParticle(geometry::Position position, int score);

  virtual void update() override;
  virtual int get_sprite() const override;
  virtual bool is_alive() const override;

 private:
  unsigned frame_ = 0;
  Sprite sprite_;
};

class HatParticle : public Particle
{
  // ➖➖➖➖➖⚫⚫⚫⚫⚫⚫➖➖➖➖➖
  // ➖➖➖➖⚫🚨🟥🟨🟨🟥🟥⚫➖➖➖➖
  // ➖➖➖⚫🚨🟥🟨🟨🟨🟨🟥🟥⚫➖➖➖
  // ➖➖⚫🚨🟥🟥🟥🟨🟨🟥🟥🟥🟥⚫➖➖
  // ➖➖⚫⚫⚫⚫⚫⚫⚫⚫⚫⚫⚫⚫➖➖
  // Flies off when the player implodes
 public:
  HatParticle(geometry::Position position) : Particle(position) {}

  virtual void update() override;
  virtual int get_sprite() const override { return static_cast<int>(Sprite::SPRITE_PLAYER_HAT); }
  virtual bool is_alive() const override;

 private:
  unsigned frame_ = 0;
};
