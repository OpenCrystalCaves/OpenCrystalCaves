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
  Enemy(geometry::Position position, geometry::Size size, int health) : Actor(position, size), health(health) {}
  virtual ~Enemy() = default;

  virtual bool is_alive() const override { return health > 0; }
  virtual void on_hit(const bool power);
  virtual void on_death([[maybe_unused]] Level& level)
  {
    // TODO: create corpse animation
  }
  // Whether this enemy requires the power powerup to hit/kill
  virtual bool is_tough() const { return false; }

  int health;

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
  Bigfoot(geometry::Position position) : Enemy(position - geometry::Position(0, 16), geometry::Size(16, 32), 5) {}

  virtual void update(const geometry::Rectangle& player_rect, Level& level) override;
  virtual std::vector<std::pair<geometry::Position, Sprite>> get_sprites(const Level& level) const override;
  virtual std::vector<geometry::Rectangle> get_detection_rects(const Level& level) const override
  {
    return create_detection_rects(left_ ? -1 : 1, 0, level);
  }
  virtual void on_hit(const bool power) override
  {
    Enemy::on_hit(power);
    running_ = true;
  }
  virtual int get_points() const override { return 5000; }

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
  Hopper(geometry::Position position) : Enemy(position, geometry::Size(16, 16), 1) {}

  virtual void update(const geometry::Rectangle& player_rect, Level& level) override;
  virtual std::vector<std::pair<geometry::Position, Sprite>> get_sprites(const Level& level) const override;
  virtual int get_points() const override { return 100; }

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

  virtual void update(const geometry::Rectangle& player_rect, Level& level) override;
  virtual std::vector<std::pair<geometry::Position, Sprite>> get_sprites(const Level& level) const override;
  virtual int get_points() const override { return 100; }

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
  Snake(geometry::Position position) : Enemy(position, geometry::Size(16, 16), 2) {}

  virtual void update(const geometry::Rectangle& player_rect, Level& level) override;
  virtual std::vector<std::pair<geometry::Position, Sprite>> get_sprites(const Level& level) const override;
  virtual void on_death(Level& level) override;
  virtual int get_points() const override { return 100; }

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
  Spider(geometry::Position position) : Enemy(position, geometry::Size(16, 16), 1) {}

  virtual void update(const geometry::Rectangle& player_rect, Level& level) override;
  virtual std::vector<std::pair<geometry::Position, Sprite>> get_sprites(const Level& level) const override;
  virtual std::vector<geometry::Rectangle> get_detection_rects(const Level& level) const override
  {
    return create_detection_rects(0, 1, level);
  }
  void remove_child() { child_ = nullptr; }
  virtual int get_points() const override { return 100; }

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
  // TODO: require P to kill
  Rockman(geometry::Position position) : Enemy(position, geometry::Size(16, 16), 1) {}

  virtual void update(const geometry::Rectangle& player_rect, Level& level) override;
  virtual std::vector<std::pair<geometry::Position, Sprite>> get_sprites(const Level& level) const override;
  virtual std::vector<geometry::Rectangle> get_detection_rects(const Level& level) const override;
  virtual int get_points() const override { return 100; }
  virtual bool is_tough() const { return true; }

 private:
  bool left_ = false;
  int frame_ = 0;
  bool asleep_ = true;
};
