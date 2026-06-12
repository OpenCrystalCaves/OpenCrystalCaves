// Base class of enemies and hazards
#pragma once
#include <utility>

#include "geometry.h"
#include "misc.h"
#include "object.h"
#include "sound.h"
#include "sprite.h"

enum class LeverColor : int
{
  LEVER_COLOR_R = 0,
  LEVER_COLOR_B = 1,
  LEVER_COLOR_G = 2,
};

// What happens if the player touches this actor
enum class TouchType
{
  TOUCH_TYPE_NONE,
  TOUCH_TYPE_AMMO,
  TOUCH_TYPE_POWER,
  TOUCH_TYPE_GRAVITY,
  TOUCH_TYPE_STOP,
  TOUCH_TYPE_KEY,
  TOUCH_TYPE_HURT,
  TOUCH_TYPE_CRUSHING,
  TOUCH_TYPE_DIE,
  TOUCH_TYPE_GREEN_MUSHROOM,
  TOUCH_TYPE_RED_MUSHROOM,
};

struct Level;
struct Player;
class LaserBeam;

class Actor
{
 public:
  Actor(geometry::Position position, geometry::Size size) : position(std::move(position)), size(std::move(size)) {}
  virtual ~Actor() = default;

  virtual bool is_alive() const { return true; }
  virtual int get_points() const { return 0; }
  virtual bool is_solid([[maybe_unused]] const Level& level) const { return false; }
  virtual bool is_solid_top([[maybe_unused]] const Level& level) const { return false; }
  virtual bool is_render_in_front() const { return false; }

  virtual void update([[maybe_unused]] AbstractSoundManager& sound_manager,
                      [[maybe_unused]] const geometry::Rectangle& player_rect,
                      [[maybe_unused]] Level& level)
  {
  }
  virtual bool interact([[maybe_unused]] AbstractSoundManager& sound_manager, [[maybe_unused]] Level& level) { return false; };
  virtual std::vector<ObjectDef> get_sprites(const Level& level) const = 0;
  virtual Vector<double> parallax() const { return {1.0, 1.0}; }
  virtual std::vector<geometry::Rectangle> get_detection_rects([[maybe_unused]] const Level& level) const { return {}; }
  virtual TouchType on_touch([[maybe_unused]] const Player& player,
                             [[maybe_unused]] AbstractSoundManager& sound_manager,
                             [[maybe_unused]] Level& level)
  {
    return TouchType::TOUCH_TYPE_NONE;
  }
  // Returns whether the actor is hittable (missile)
  virtual bool on_hit([[maybe_unused]] const geometry::Rectangle& rect,
                      [[maybe_unused]] AbstractSoundManager& sound_manager,
                      [[maybe_unused]] const geometry::Rectangle& player_rect,
                      Level& level,
                      [[maybe_unused]] const bool power)
  {
    return is_solid(level);
  }
  virtual void on_collide([[maybe_unused]] const Player& player,
                          [[maybe_unused]] AbstractSoundManager& sound_manager,
                          [[maybe_unused]] Level& level) {};
  virtual const std::vector<Sprite>* get_explosion_sprites() const { return nullptr; }

  geometry::Position position;
  geometry::Size size;
  geometry::Rectangle rect() const { return {position, size}; }

 protected:
  std::vector<geometry::Rectangle> create_detection_rects(const int dx,
                                                          const int dy,
                                                          const Level& level,
                                                          const bool include_self = false,
                                                          const int max_len = 0) const;
};

class Projectile;

class ProjectileParent
{
 public:
  virtual void remove_child([[maybe_unused]] Level& level) { child_ = nullptr; }

 protected:
  Projectile* child_ = nullptr;
};

class Lever : public Actor
{
  // ⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛🟥🚨🟥⬛⬛⬛
  // ⬛⬛⬛⬛⬛⬛⬛⬛⬛🟥🚨🟥🟥🟥⬛⬛
  // ⬛⬛⬛⬛⬛⬛⬛⬛⬛🟥🚨🟥🟥🟥⬛⬛
  // ⬛⬛⬛⬛⬛⬛⬛⬛⬛🟥🟥🟥🟥🟥⬛⬛
  // ⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛🟥🟥🟥⬛⬛⬛
  // ⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⚪🟥⬛⬛⬛⬛
  // ⬛⬛⬛⬛⬛⬛⬛⬛⬛⚪🪦⬛⬛⬛⬛⬛
  // ⬛⬛⬛⬛⬛⬛⬛⬛⬛⚪🪦⬛⬛⬛⬛⬛
  // ⬛⬛⬛⬛⬛⬛⬛⬛⚪🪦⬛⬛⬛⬛⬛⬛
  // ⬛⬛⬛⬛⬛⬛⬛⬛⚪🪦⬛⬛⬛⬛⬛⬛
  // ⬛⬛⬛⬛⬛⬛⬛⬛⚪🪦⬛⬛⬛⬛⬛⬛
  // ⬛⬛⬛⬛⬛⬛⬛⚪🪦⬛⬛⬛⬛⬛⬛⬛
  // ⬛⬛⬛⬛⬛⬛⬛⚪🪦⬛⬛⬛⬛⬛⬛⬛
  // ⬛⬛⬛⬛⬛⬛⬛⚪🪦⬛⬛⬛⬛⬛⬛⬛
  // ⬛⬛⬛🪦🪦⚪⚪⬜⬜⚪⚪🪦🪦⬛⬛⬛
  // Opens a door when interacted with
 public:
  Lever(geometry::Position position, LeverColor color) : Actor(position, geometry::Size(16, 16)), color_(color) {}

  virtual bool interact(AbstractSoundManager& sound_manager, Level& level) override;
  virtual std::vector<ObjectDef> get_sprites(const Level& level) const override;

 private:
  LeverColor color_;
};

class Door : public Actor
{
  // ⬛⬛🚨🚨🚨🚨🚨🚨🚨🚨🚨🚨🚨🚨⬛⬛
  // ⬛🚨⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛🪦⬛
  // 🚨⬛🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🚨🪦
  // 🚨⬛🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🚨🪦
  // 🚨⬛🟥🚨🚨🚨🚨🚨🚨🚨🚨🚨🪦🟥🚨🪦
  // 🚨⬛🟥🚨⬛⬛⬛⬛⬛⬛⬛⬛🪦🟥🚨🪦
  // 🚨⬛🟥🚨⬛⬛⬛⬛⬜⬛⬛⬛🪦🟥🚨🪦
  // 🚨⬛🟥🚨⬛⬛⬛⬜⬛⬛⬛⬛🪦🟥🚨🪦
  // 🚨⬛🟥🚨⬛⬛⬜⬛⬛⬛⬛⬛🪦🟥🚨🪦
  // 🚨⬛🟥🪦🪦🪦🪦🪦🪦🪦🪦🪦🪦🟥🚨🪦
  // 🚨⬛🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🚨🪦
  // 🚨⬛🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🚨🪦
  // 🚨⬛🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🚨🪦
  // 🚨⬛🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🚨🪦
  // 🚨⬛🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🚨🪦
  // ⬛🚨🚨🚨🚨🚨🚨🚨🚨🚨🚨🚨🚨🚨🚨⬛
  // ⬛🚨🚨🚨🚨🚨🚨🚨🚨🚨🚨🚨🚨🚨🚨⬛
  // 🚨⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛🚨🪦
  // 🚨⬛🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🚨🪦
  // 🚨⬛🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🚨🪦
  // 🚨⬛🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🚨🪦
  // 🚨⬛🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🚨🪦
  // 🚨⬛🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🚨🪦
  // 🚨⬛🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🚨🪦
  // 🚨⬛🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🚨🪦
  // 🚨⬛🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🚨🪦
  // 🚨⬛🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🚨🪦
  // 🚨⬛🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🚨🪦
  // 🚨⬛🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🚨🪦
  // 🚨⬛🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🟥🚨🪦
  // ⬛🚨🚨🚨🚨🚨🚨🚨🚨🚨🚨🚨🚨🚨🪦⬛
  // ⬛⬛🪦🪦🪦🪦🪦🪦🪦🪦🪦🪦🪦🪦⬛⬛
  // Doors that can be opened by the corresponding coloured lever
 public:
  Door(geometry::Position position, LeverColor color) : Actor(position, geometry::Size(16, 32)), color_(color) {}

  virtual bool is_solid(const Level& level) const override;

  virtual std::vector<ObjectDef> get_sprites(const Level& level) const override;

 private:
  LeverColor color_;
};

class Switch : public Actor
{
  // ⬛⬛⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜⬛⬛
  // ⬛⬜⚪🚨🚨⚪⚪🚨🚨⚪⚪🚨🚨⚪🪦⬛
  // ⬛⬜🚨⚪⚪🚨⚪🚨⚪⚪⚪🚨⚪⚪🪦⬛
  // ⬛⬜🚨⚪⚪🚨⚪🚨🚨⚪⚪🚨🚨⚪🪦⬛
  // ⬛⬜⚪🚨🚨⚪⚪🚨⚪⚪⚪🚨⚪⚪🪦⬛
  // ⬛⬜⚪⚪🇪🇺⚪⚪⚪⚪⚪⚪⚪⚪⚪🪦⬛
  // ⬛⬜⚪🟦🟦🇪🇺⬛⬛⬛⬛⬛⬛⬛⚪🪦⬛
  // ⬛⬜⚪🟦🟦🇪🇺⬛⬛⬛⬛⬛⬛⬛⚪🪦⬛
  // ⬛⬜⚪⚪🇪🇺⚪⚪⚪⚪⚪⚪⚪⚪⚪🪦⬛
  // ⬛⬜⚪⚪⬛⬛⚪⚪⚪⬛⚪⚪⬛⚪🪦⬛
  // ⬛⬜⚪⬛⚪⚪⬛⚪⚪⬛⬛⚪⬛⚪🪦⬛
  // ⬛⬜⚪⬛⚪⚪⬛⚪⚪⬛⚪⬛⬛⚪🪦⬛
  // ⬛⬜⚪⚪⬛⬛⚪⚪⚪⬛⚪⚪⬛⚪🪦⬛
  // ⬛⬛🪦🪦🪦🪦🪦🪦🪦🪦🪦🪦🪦🪦⬛⬛
  // Switches that turn things off/on (e.g. moving platforms)
 public:
  Switch(geometry::Position position, Sprite sprite, int switch_flag)
    : Actor(position, geometry::Size(16, 16)),
      sprite_(sprite),
      switch_flag_(switch_flag)
  {
  }

  virtual bool interact(AbstractSoundManager& sound_manager, Level& level) override;
  virtual std::vector<ObjectDef> get_sprites(const Level& level) const override;

 private:
  Sprite sprite_;
  int switch_flag_;
};

class Chest : public Actor
{
  // ⬛⬛⬛⬛🟠🟠🪦🟠🪦🟠🪦🟠⬛⬛⬛⬛
  // ⬛⬛🟠🟠🟠🪦🟠🟠🟠🪦🟠🪦🟠🟠⬛⬛
  // ⬛🟠🪦🟠🪦🟠🪦🟠🪦🟠🪦🟠🪦🟠🟠⬛
  // ⬛🟠🟠🪦🟠🟠🟠🟠🟠🟠🟠🟠🟠🪦🟠⬛
  // ⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛
  // ⬛🪦🟠🟠🟠🟠🟠🟠🟠🟠🟠🟠🟠🪦🟠⬛
  // ⬛🟠🪦🟠🪦🟠🪦⬛⬛🟠🪦🟠🟠🟠🟠⬛
  // ⬛🪦🟠🪦🟠🟠⬛⬛⬛⬛🟠🪦🟠🪦🟠⬛
  // ⬛🟠🪦🟠🪦🟠⬛⬛⬛⬛🪦🟠🪦🟠🟠⬛
  // ⬛🟠🟠🪦🟠🟠🟠⬛⬛🪦🟠🟠🟠🪦🟠⬛
  // ⬛🟠🪦🟠🪦🟠🪦⬛⬛🟠🪦🟠🪦🟠🟠⬛
  // ⬛🟠🟠🟠🟠🟠🟠⬛⬛⬛🟠🪦🟠🟠🟠⬛
  // ⬛🟠🪦🟠🪦🟠🟠🟠🟠🟠🟠🟠🟠🟠🟠⬛
  // Can collect if player has key - then replaced with an open chest sprite
 public:
  Chest(geometry::Position position) : Actor(position, geometry::Size(16, 16)) {}

  virtual TouchType on_touch(const Player& player, AbstractSoundManager& sound_manager, Level& level) override;
  virtual std::vector<ObjectDef> get_sprites(const Level& level) const override;
  virtual bool is_alive() const override { return !collected_; }
  virtual int get_points() const override
  {
    // Randomly give 1000, 2000 or 5000 points
    return std::array{1000, 2000, 5000}[rand() % 3];
  }

 private:
  bool collected_ = false;
};


class OpenChest : public Actor
{
  // ➖➖➖➖⚫⚫⚫🟨⚫⚫⚫⚫➖➖➖➖
  // ➖⚫➖⚫🟠🟠🟠🟨🟠🟠🟠🟠⚫➖⚫➖
  // ⚫🟨⚫🟠🟠🟠🟠🟨🟠🪦🟠🪦🟠⚫🟨⚫
  // ➖⚫🟨🟠🪦🟠🟠🟨🟠🟠🟠🟠🪦🟨⚫➖
  // ➖⚫🟠🟨🟠🪦🟠🪦🟠⚫🟠🪦🟨🪦⚫➖
  // ➖⚫🪦🟠🪦⚫🟠🟠⚫🟨⚫⚫🪦🟠⚫➖
  // ➖⚫⚫🟠⚫🟨⚫⚫🟨🟨🟨🟨⚫⚫⚫➖
  // ➖⚫🟨⚫🟨🟨⚫🟨🟨🟨🟨🟨⚫🟨⚫➖
  // ⚫🪦🟠🟠🟠🟠🟠🟠🟠🟠🟠🟠🟠🪦🟠⚫
  // ⚫🟠🪦🟠🪦🟠🪦⚫⚫🟠🪦🟠🟠🟠🟠⚫
  // ⚫🪦🟠🪦🟠🟠⚫⚫⚫⚫🟠🪦🟠🪦🟠⚫
  // ⚫🟠🪦🟠🪦🟠⚫⚫⚫⚫🪦🟠🪦🟠🟠⚫
  // ⚫🟠🟠🪦🟠🟠🟠⚫⚫🪦🟠🟠🟠🪦🟠⚫
  // ⚫🟠🪦🟠🪦🟠🪦⚫⚫🟠🪦🟠🪦🟠🟠⚫
  // ⚫🟠🟠🟠🟠🟠🟠⚫⚫⚫🟠🪦🟠🟠🟠⚫
  // ⚫🟠🪦🟠🪦🟠🟠🟠🟠🟠🟠🟠🟠🟠🟠⚫
  // Open chest sprite after collecting
 public:
  OpenChest(geometry::Position position) : Actor(position, geometry::Size(16, 16)) {}

  virtual std::vector<ObjectDef> get_sprites([[maybe_unused]] const Level& level) const override
  {
    return {{position, static_cast<int>(Sprite::SPRITE_CHEST_OPEN), false}};
  }
};


class BumpPlatform : public Actor
{
  // 🟦🟦🟦🟦🟦🟦🟦🟦🟦🟦🟦🟦🟦🟦🟦🟦
  // 📘⬜📘📘📘📘📘📘📘📘⬜⬜📘📘📘📘
  // ⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛
  // 🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺
  // 🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺
  // 🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺
  // 🟦🟦🟦🟦🟦🟦🟦🟦🟦🟦🟦🟦🟦🟦🟦🟦
  // 🟦🟦🟦🟦🟦🟦🟦🟦🟦🟦🟦🟦🟦🟦🟦🟦
  // 🟦🟦🟦🟦🟦🟦🟦🟦🟦🟦🟦🟦🟦🟦🟦🟦
  // 🟦🟦🟦🟦🟦🟦🟦🟦🟦🟦🟦🟦🟦🟦🟦🟦
  // 🟦🟦🟦🟦🟦🟦🟦🟦🟦🟦🟦🟦🟦🟦🟦🟦
  // ⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛
  // 📘📘🪦🪦📘📘📘📘🪦📘📘📘📘📘📘📘
  // 🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺
  // Can be bumped from below and can have hidden crystals
 public:
  BumpPlatform(geometry::Position position, Sprite sprite, bool has_crystal)
    : Actor(position, geometry::Size(16, 16)),
      sprite_(sprite),
      has_crystal_(has_crystal)
  {
  }

  virtual void on_collide(const Player& player, AbstractSoundManager& sound_manager, Level& level) override;
  virtual bool is_solid([[maybe_unused]] const Level& level) const override { return true; }
  virtual std::vector<ObjectDef> get_sprites(const Level& level) const override;
  virtual void update(AbstractSoundManager& sound_manager, const geometry::Rectangle& player_rect, Level& level) override;

 private:
  int frame_ = 0;
  Sprite sprite_;
  bool has_crystal_;
};

class ClearBlock : public Actor
{
  // ⬛⬛⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜⬛⬛
  // ⬛⬜⬜⬜⬜⬜⬛⬛⬛⬛⬛⬛⬛⬛📘⬛
  // ⬛⬜⬜⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛📘⬛
  // ⬛⬜⬜⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛📘⬛
  // ⬛⬜⬜⬛⬛⬛⬜⬛⬛⬛⬛⬛⬛⬛📘⬛
  // ⬛⬜⬛⬛⬛⬜⬛⬛⬛⬛⬛⬛⬛⬛📘⬛
  // ⬛⬜⬛⬛⬜⬛⬛⬛⬛⬛⬛⬛⬛⬛📘⬛
  // ⬛⬜⬛⬛⬛⬛⬛⬛⬛⬛⬛⬜⬛⬛📘⬛
  // ⬛⬜⬛⬛⬛⬛⬛⬛⬛⬛⬜⬛⬛⬛📘⬛
  // ⬛⬜⬛⬛⬛⬛⬛⬛⬛⬜⬛⬛⬛⬛📘⬛
  // ⬛⬜⬛⬛⬛⬛⬛⬛⬜⬛⬛⬛⬛📘📘⬛
  // ⬛⬜⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛📘📘⬛
  // ⬛⬜⬛⬛⬛⬛⬛⬛⬛⬛⬛📘📘📘📘⬛
  // ⬛⬛📘📘📘📘📘📘📘📘📘📘📘📘⬛⬛
  // Destructible block
 public:
  ClearBlock(geometry::Position position) : Actor(position, geometry::Size(16, 16)) {}

  virtual bool is_alive() const override { return is_alive_; }
  virtual bool is_solid([[maybe_unused]] const Level& level) const override { return true; }
  virtual std::vector<ObjectDef> get_sprites([[maybe_unused]] const Level& level) const override
  {
    return {{position, static_cast<int>(Sprite::SPRITE_CLEAR_BLOCK), false}};
  }
  virtual bool on_hit(const geometry::Rectangle& rect,
                      AbstractSoundManager& sound_manager,
                      const geometry::Rectangle& player_rect,
                      Level& level,
                      const bool power) override;

 private:
  bool is_alive_ = true;
};

class HiddenBlock : public Actor
{
  // ⬛⬛🟩🟩🟩🟩🟩🟩🦚🟩🦚🟩🟩🟩⬛⬛
  // ⬛🟩🦚🦚🟩🟩🦚🟩🦚🦚🦚🟩🦚🦚🟩⬛
  // ⬛🟩🦚🟩🦚🦚🦚🟩⬛🟩🦚🟩🦚⬛🦚⬛
  // ⬛🟩⬛🟩🦚⬛🦚⬛⬛🦚⬛⬛🦚⬛🦚⬛
  // ⬛⬛⬛🟩⬛🪦⬛🪦⬛🦚⬛🪦⬛🪦⬛⬛
  // ⬛⚪⚪⬛🪦🪦🪦🪦🪦⬛🪦🪦🪦🪦🪦⬛
  // ⬛⬜⬜🪦🪦⚪🪦⚪🪦🪦🪦⚪⚪🪦🪦⬛
  // ⬛⬜⬜⚪⚪⚪⚪⚪⚪🪦⚪⚪⚪⚪🪦⬛
  // ⬛⬜⚪⚪⚪⚪⚪⚪⚪⚪⚪⚪⚪⚪🪦⬛
  // ⬛⬜⬜⚪⚪⚪⚪⚪⚪⚪⚪⚪⚪🪦⚪⬛
  // ⬛⚪⬜⚪⚪⚪⚪⚪⚪⚪⚪⚪⚪🪦🪦⬛
  // ⬛⬜⚪⚪⚪⚪⚪⚪⚪⚪⚪⚪⚪🪦⚪⬛
  // ⬛⬜⬜⚪⚪⚪⚪⚪⚪⚪⚪⚪⚪🪦🪦⬛
  // ⬛⬛⚪⬜⚪⚪⚪⚪⚪⚪⚪⚪🪦🪦⬛⬛
  // Hidden block, shown when player jumps into it
 public:
  HiddenBlock(geometry::Position position) : Actor(position, geometry::Size(16, 16)) {}

  virtual bool is_solid([[maybe_unused]] const Level& level) const override { return !is_hidden_; }
  virtual std::vector<ObjectDef> get_sprites([[maybe_unused]] const Level& level) const override
  {
    if (is_hidden_)
    {
      return {};
    }
    return {{position, static_cast<int>(Sprite::SPRITE_HIDDEN_BLOCK), false}};
  }
  virtual TouchType on_touch(const Player& player, AbstractSoundManager& sound_manager, Level& level) override;

 private:
  bool is_hidden_ = true;
};

class HiddenCrystal : public Actor
{
  // ➖➖➖⚫⚫⚫⚫⚫⚫⚫⚫⚫⚫➖➖➖
  // ➖➖⚫⚪⬜⬜⚪⬜⬜⚪⬜⬜🪦⚫➖➖
  // ➖⚫⚪➖➖➖➖➖➖➖➖➖➖🪦⚫➖
  // ⚫⚪➖➖➖➖➖➖➖➖➖➖➖➖🪦⚫
  // ⬜➖⬜➖⬜➖⚪⬜⬜⬜⚪➖➖➖➖🪦
  // ⬜➖➖➖➖➖➖➖➖➖➖➖➖➖➖🪦
  // ⚫⚪➖➖➖➖➖➖➖➖➖➖➖➖🪦⚫
  // ➖⚫⚪➖➖➖➖➖➖➖➖➖➖🪦⚫➖
  // ➖➖⚫⚪➖➖➖➖➖➖➖➖🪦⚫➖➖
  // ➖➖➖⚫⚪➖➖➖➖➖➖🪦⚫➖➖➖
  // ➖➖➖➖⚫⚪➖➖➖➖🪦⚫➖➖➖➖
  // ➖➖➖➖➖⚫⚪➖➖🪦⚫➖➖➖➖➖
  // ➖➖➖➖➖➖⚫⚪🪦⚫➖➖➖➖➖➖
  // ➖➖➖➖➖➖➖⚫⚫➖➖➖➖➖➖➖
  // Spawned by BumpPlatform on bump, flies up, lands then dies, giving score
 public:
  HiddenCrystal(geometry::Position position) : Actor(position, geometry::Size(16, 16)) {}

  virtual bool is_alive() const override { return frame_ > 0; }
  virtual std::vector<ObjectDef> get_sprites([[maybe_unused]] const Level& level) const override
  {
    return {{position, static_cast<int>(Sprite::SPRITE_CRYSTAL_HIDDEN), false}};
  }
  virtual void update(AbstractSoundManager& sound_manager, const geometry::Rectangle& player_rect, Level& level) override;
  virtual int get_points() const override { return 5000; }

 private:
  int dy_ = -9;
  int frame_ = 32;
};

class AirTank : public Actor
{
  // ➖➖➖➖➖➖➖⚫⚫➖➖➖➖➖➖➖
  // ➖➖➖➖➖⚫⚫🟨🟨⚫⚫➖➖➖➖➖
  // ➖➖➖➖⚫⬜⚪⚪⚪⚪🪦⚫➖➖➖➖
  // ➖➖➖➖➖⚫⬜⚪⚪🪦⚫➖➖➖➖➖
  // ➖⚫⚫⚫⚫⚫⚫⬜🪦⚫⚫⚫⚫⚫⚫➖
  // ⚫⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜⚫
  // ⬜⚫⚫⚫⚫⚫⚫⚫⚫⚫⚫⚫⚫⚫⚫🪦
  // ⬜⚫⚪⚪⚪⚪⚪⚪⚪⚪⚪⚪⚪⚪⚫🪦
  // ⬜⚫⚪⚫⚫⚫⚫🟥🟥🚨🚨🚨⚫⚪⚫🪦
  // ⬜⚫⚪⚫⚫⚫⚫🟥🟥🚨🚨🚨⚫⚪⚫🪦
  // ⬜⚫⚪⚫⚫⚫⚫🟥🟥⚫⚫⚫⚫⚪⚫🪦
  // ⬜⚫⚪⚫🚨🚨🚨🟥🟥⚫⚫⚫⚫⚪⚫🪦
  // ⬜⚫⚪⚫🚨🚨🚨🟥🟥⚫⚫⚫⚫⚪⚫🪦
  // ⬜⚫⚪⚪⚪⚪⚪⚪⚪⚪⚪⚪⚪⚪⚫🪦
  // ⚫⬜⚫⚫⚫⚫⚫⚫⚫⚫⚫⚫⚫⚫⬜⚫
  // ⬜⚫⚪⚪⚪⚪⚪⚪⚪⚪⚪⚪⚪⚪⚫🪦
  // ⬜⚫⚪🟥🟥⚪⚪🟥🟥⚪🟥🟥🟥⚪⚫🪦
  // ⬜⚫🟥⚪⚪🟥⚪🟥🟥⚪🟥⚪⚪🟥⚫🪦
  // ⬜⚫🟥🟥🟥🟥⚪🟥🟥⚪🟥🟥🟥⚪⚫🪦
  // ⬜⚫🟥⚪⚪🟥⚪🟥🟥⚪🟥⚪🟥⚪⚫🪦
  // ⬜⚫🟥⚪⚪🟥⚪🟥🟥⚪🟥⚪⚪🟥⚫🪦
  // ⬜⚫⚪⚪⚪⚪⚪⚪⚪⚪⚪⚪⚪⚪⚫🪦
  // ⬜⚫⚪⚫⚫⚫⚫⚫⚫⚫⚫⚫⚫⚪⚫🪦
  // ⬜⚫⚫🟥🟥🟥🟨🟨🟨🟨🦚🦚🦚⚫⚫🪦
  // ⬜⚫⚫🟥🟥🟥🟨🟨🟨🟨🦚🦚🦚⚫⚫🪦
  // ⬜⚫⚪⚫⚫⚫⚫⚫⚫⚫⚫⚫⚫⚪⚫🪦
  // ⬜⚫⚪⚪⚪⚪⚪⚪⚪⚫⬜⚫⚪⚪⚫🪦
  // ⬜⚫⚪⚪⚪⚪⚪⚪⚪⚫⬜⚫⚪⚪⚫🪦
  // ⬜⚫⚫⚫⚫⚫⚫⚫⚫⚫⚫⚫⚫⚫⚫🪦
  // ⚫🪦🪦🪦🪦🪦🪦🪦🪦🪦🪦🪦🪦🪦🪦⚫
  // Die if shot
 public:
  AirTank(geometry::Position position, bool top) : Actor(position, geometry::Size(16, 16)), top_(top) {}

  virtual void update(AbstractSoundManager& sound_manager, const geometry::Rectangle& player_rect, Level& level) override;
  virtual std::vector<ObjectDef> get_sprites(const Level& level) const override;
  virtual bool on_hit(const geometry::Rectangle& rect,
                      AbstractSoundManager& sound_manager,
                      const geometry::Rectangle& player_rect,
                      Level& level,
                      const bool power) override;

 private:
  bool top_;
  int frame_ = 0;
  bool is_alive_ = true;
};

class Egg : public Actor
{
  // ➖➖➖➖➖⚫⚫⚫⚫⚫⚫➖➖➖➖➖
  // ➖➖➖⚫⚫⬜⬜⬜🟣🟣🟣⚫⚫➖➖➖
  // ➖➖⚫⬜⬜⬜⬜⬜🟣🟣🟣🟣⬜⚫➖➖
  // ➖⚫⬜⬜⬜⬜⬜⬜⬜🟣🟣⬜⬜⬜⚫➖
  // ➖⚫⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜⚫➖
  // ⚫⬜⬜⬜🟣🟣⬜⬜⬜⬜⬜⬜⬜⬜🪦⚫
  // ⚫⬜⬜🟣🟣🟣🟣⬜⬜⬜⬜⬜⬜⬜🪦⚫
  // ➖⚫⬜🟣🟣🟣🟣⬜⬜⬜⬜⬜⬜🪦⚫➖
  // ➖⚫⬜⬜🟣🟣⬜⬜⬜⬜⬜⬜⬜🪦⚫➖
  // ➖➖⚫⬜⬜⬜⬜⬜⬜⬜⬜🪦🪦⚫➖➖
  // ➖➖➖⚫⚫⬜⬜⬜⬜🪦🪦⚫⚫➖➖➖
  // Gives a small amount of score, leaves BONUS if shot
 public:
  Egg(geometry::Position position) : Actor(position, geometry::Size(16, 16)) {}

  virtual bool is_alive() const override { return is_alive_; }
  virtual std::vector<ObjectDef> get_sprites([[maybe_unused]] const Level& level) const override
  {
    return {{position, static_cast<int>(Sprite::SPRITE_EGG), false}};
  }
  virtual bool on_hit(const geometry::Rectangle& rect,
                      AbstractSoundManager& sound_manager,
                      const geometry::Rectangle& player_rect,
                      Level& level,
                      const bool power) override;
  virtual int get_points() const override
  {
    // Only give points if collected
    return is_collected_ ? 1000 : 0;
  }
  virtual TouchType on_touch([[maybe_unused]] const Player& player,
                             AbstractSoundManager& sound_manager,
                             [[maybe_unused]] Level& level) override
  {
    is_alive_ = false;
    is_collected_ = true;
    sound_manager.play_sound(SoundType::SOUND_CRYSTAL);
    return TouchType::TOUCH_TYPE_NONE;
  }

 private:
  bool is_alive_ = true;
  bool is_collected_ = false;
};

class OneWayPlatform : public Actor
{
  // ➖⚫⚫⚫⚫⚫⚫⚫⚫⚫⚫⚫⚫⚫⚫➖
  // ⚫⬜🟦🟦🟦🟦🟦🟦🟦🟦🟦🟦🟦🟦🇪🇺⚫
  // ⚫🟦🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺⚫
  // ⚫🟦🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺⚫
  // ⚫🟦🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺🇪🇺⚫
  // ➖⚫⚫⚫⚫⚫⚫⚫⚫⚫⚫⚫⚫⚫⚫➖
 public:
  OneWayPlatform(geometry::Position position, Sprite sprite) : Actor(position, geometry::Size(16, 16)), sprite_(sprite) {}
  virtual bool is_solid_top([[maybe_unused]] const Level& level) const override { return true; }

  virtual std::vector<ObjectDef> get_sprites([[maybe_unused]] const Level& level) const override
  {
    return {{position, static_cast<int>(sprite_), false}};
  }

 private:
  Sprite sprite_;
};

class Earth : public Actor
{
  // ➖➖➖➖➖➖⚫⚫⚫⚫➖➖➖➖➖➖
  // ➖➖➖➖⚫⚫🟩🟦🇪🇺🟦⚫⚫➖➖➖➖
  // ➖➖➖⚫🦚🟩🦚🟩🟦🇪🇺🟦🇪🇺⚫➖➖➖
  // ➖➖⚫🟦🟩🦚🟩🦚🟩🟦🇪🇺🟦🇪🇺⚫➖➖
  // ➖⚫🟦🇪🇺🟦🟩🦚🟩🦚🟩🦚🟩🟦🇪🇺⚫➖
  // ➖⚫🇪🇺🟦🟩🦚🇪🇺🟦🇪🇺🦚🟩🦚🇪🇺🟦⚫➖
  // ⚫🇪🇺🟦🇪🇺🟦🇪🇺🟦🇪🇺🇪🇺🇪🇺🦚🟩🟦🇪🇺🟦⚫
  // ⚫🟦🇪🇺🟦🇪🇺🟦🇪🇺🟦🟦🇪🇺🟦🇪🇺🟦🇪🇺🇪🇺⚫
  // ⚫🇪🇺🟦🇪🇺🦚🇪🇺🟦🇪🇺🇪🇺🟦🇪🇺🟦🇪🇺🟦🇪🇺⚫
  // ⚫🟦🇪🇺🦚🟩🦚🇪🇺🟦🇪🇺🟦🟦🦚🟩🇪🇺🟩⚫
  // ➖⚫🦚🟩🦚🟩🟦🇪🇺🟦🇪🇺🇪🇺🟩🦚🟩🦚➖
  // ➖⚫🟩🦚🟩🦚🇪🇺🟦🟩🟦🇪🇺🟦🟩🦚⚫➖
  // ➖➖⚫🟩🦚🟩🦚🟩🦚🟩🟦🟦🦚⚫➖➖
  // ➖➖➖⚫🟩🦚🟩🦚🇪🇺🟦🇪🇺🇪🇺⚫➖➖➖
  // ➖➖➖➖⚫⚫🦚🟩🦚🇪🇺⚫⚫➖➖➖➖
  // ➖➖➖➖➖➖⚫⚫⚫⚫➖➖➖➖➖➖
  // Static or moving actor
 public:
  Earth(geometry::Position position, const bool moving) : Actor(position, geometry::Size(16, 16)), moving_(moving) {}

  virtual bool is_render_in_front() const override { return true; }
  virtual void update(AbstractSoundManager& sound_manager, const geometry::Rectangle& player_rect, Level& level) override;
  virtual std::vector<ObjectDef> get_sprites([[maybe_unused]] const Level& level) const override
  {
    return {{position, static_cast<int>(Sprite::SPRITE_EARTH), false}};
  }
  virtual Vector<double> parallax() const override { return {moving_ ? 0.0 : 1.0, 1.0}; }

 private:
  bool moving_;
  int ticks_ = 0;
};

class Moon : public Actor
{
  // ➖➖➖➖➖➖➖⚫⚫➖➖➖➖➖➖➖
  // ➖➖➖➖➖⚫⚫🪦🪦⚫⚫➖➖➖➖➖
  // ➖➖➖➖⚫🪦🪦📘⚪🪦🪦⚫➖➖➖➖
  // ➖➖➖⚫🪦⚪🩵⚪⬜⚪📘🪦⚫➖➖➖
  // ➖➖⚫🪦📘🩵🩵🩵⚪⚪⬜⚪🪦⚫➖➖
  // ➖➖⚫🪦⬜⚪🩵⚪⬜⚪⚪📘🪦⚫➖➖
  // ➖➖⚫🪦⬜⚪⚪⚪⬜⚪⬜📘🪦⚫➖➖
  // ➖➖➖⚫🪦⚪⬜⚪⚪⚪📘🪦⚫➖➖➖
  // ➖➖➖➖⚫🪦🪦⬜⚪🪦🪦⚫➖➖➖➖
  // ➖➖➖➖➖⚫⚫🪦🪦⚫⚫➖➖➖➖➖
  // ➖➖➖➖➖➖➖⚫⚫➖➖➖➖➖➖➖
  // Orbits earth, can be in front or behind
 public:
  Moon(geometry::Position position, const Earth& earth) : Actor(position, geometry::Size(16, 16)), earth_(earth) {}

  virtual bool is_render_in_front() const override { return in_front_; }
  virtual void update(AbstractSoundManager& sound_manager, const geometry::Rectangle& player_rect, Level& level) override;
  virtual std::vector<ObjectDef> get_sprites([[maybe_unused]] const Level& level) const override
  {
    return {{position, static_cast<int>(in_front_ ? Sprite::SPRITE_MOON : Sprite::SPRITE_MOON_SMALL), false}};
  }
  virtual Vector<double> parallax() const override { return earth_.parallax(); }

 private:
  bool in_front_ = false;
  const Earth& earth_;
  int ticks_ = 0;
};
