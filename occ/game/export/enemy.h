#pragma once
#include <utility>

#include "geometry.h"
#include "misc.h"
#include "sprite.h"

struct Level;

class Enemy
{
 public:
  Enemy(geometry::Position position, geometry::Size size, int health, int points)
    : position(std::move(position)),
      size(std::move(size)),
      health(health),
      points(points)
  {
  }
  virtual ~Enemy() = default;

  virtual void update(const Level& level) = 0;
  virtual std::vector<std::pair<geometry::Position, Sprite>> get_sprites() const = 0;

  geometry::Position position;
  geometry::Size size;
  int health;
  int points;

 protected:
  bool should_reverse(const Level& level) const;
};

class Bigfoot : public Enemy
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
  Bigfoot(geometry::Position position) : Enemy(position - geometry::Position(0, 16), geometry::Size(16, 32), 5, 5000) {}

  virtual void update(const Level& level) override;
  virtual std::vector<std::pair<geometry::Position, Sprite>> get_sprites() const override;

 private:
  bool left_ = false;
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
  Hopper(geometry::Position position) : Enemy(position, geometry::Size(16, 16), 1, 100) {}

  virtual void update(const Level& level) override;
  virtual std::vector<std::pair<geometry::Position, Sprite>> get_sprites() const override;

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
  Slime(geometry::Position position) : Enemy(position, geometry::Size(16, 16), 1, 100) {}

  virtual void update(const Level& level) override;
  virtual std::vector<std::pair<geometry::Position, Sprite>> get_sprites() const override;

 private:
  int dx_ = 1;
  int dy_ = 0;
  int frame_ = 0;
};

class Snake : public Enemy
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
  Snake(geometry::Position position) : Enemy(position, geometry::Size(16, 16), 2, 100) {}

  virtual void update(const Level& level) override;
  virtual std::vector<std::pair<geometry::Position, Sprite>> get_sprites() const override;

 private:
  bool left_ = false;
  bool paused_ = false;
  int frame_ = 0;
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
  // Moves up and down, shoots webs ahead
 public:
  Spider(geometry::Position position) : Enemy(position, geometry::Size(16, 16), 1, 100) {}

  virtual void update(const Level& level) override;
  virtual std::vector<std::pair<geometry::Position, Sprite>> get_sprites() const override;

 private:
  bool up_ = false;
  int frame_ = 0;
};
