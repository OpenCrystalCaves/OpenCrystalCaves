#pragma once
#include <utility>

#include "actor.h"
#include "geometry.h"
#include "misc.h"
#include "sprite.h"

struct Level;
class SpiderWeb;

class Enemy : public Actor
{
 public:
  Enemy(geometry::Position position, geometry::Size size, int health, int points) : Actor(position, size), health(health), points(points) {}
  virtual ~Enemy() = default;

  virtual bool is_alive() const override { return health > 0; }
  virtual int get_points() const override { return points; }
  virtual void on_hit() { health--; }
  virtual void on_death([[maybe_unused]] Level& level) {}

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

  virtual void update(const geometry::Rectangle& player_rect, Level& level) override;
  virtual std::vector<std::pair<geometry::Position, Sprite>> get_sprites(const Level& level) const override;
  virtual std::vector<geometry::Rectangle> get_detection_rects(const Level& level) const override
  {
    return create_detection_rects(left_ ? -1 : 1, 0, level);
  }
  virtual void on_hit() override
  {
    health--;
    running_ = true;
  }

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

  virtual void update(const geometry::Rectangle& player_rect, Level& level) override;
  virtual std::vector<std::pair<geometry::Position, Sprite>> get_sprites(const Level& level) const override;

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

  virtual void update(const geometry::Rectangle& player_rect, Level& level) override;
  virtual std::vector<std::pair<geometry::Position, Sprite>> get_sprites(const Level& level) const override;

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

  virtual void update(const geometry::Rectangle& player_rect, Level& level) override;
  virtual std::vector<std::pair<geometry::Position, Sprite>> get_sprites(const Level& level) const override;
  virtual void on_death(Level& level) override;

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
  // Moves up and down, shoots webs below
 public:
  Spider(geometry::Position position) : Enemy(position, geometry::Size(16, 16), 1, 100) {}

  virtual void update(const geometry::Rectangle& player_rect, Level& level) override;
  virtual std::vector<std::pair<geometry::Position, Sprite>> get_sprites(const Level& level) const override;
  virtual std::vector<geometry::Rectangle> get_detection_rects(const Level& level) const override
  {
    return create_detection_rects(0, 1, level);
  }
  void remove_child() { child_ = nullptr; }

 private:
  bool up_ = false;
  int frame_ = 0;
  SpiderWeb* child_ = nullptr;
};
