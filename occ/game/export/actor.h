// Base class of enemies and hazards
#pragma once
#include <utility>

#include "geometry.h"
#include "misc.h"
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
  TOUCH_TYPE_SCORE,
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

class Actor
{
 public:
  Actor(geometry::Position position, geometry::Size size) : position(std::move(position)), size(std::move(size)) {}
  virtual ~Actor() = default;

  virtual bool is_alive() const { return true; }
  virtual int get_points() const { return 0; }
  virtual bool is_solid([[maybe_unused]] const Level& level) const { return false; }

  virtual void update([[maybe_unused]] AbstractSoundManager& sound_manager,
                      [[maybe_unused]] const geometry::Rectangle& player_rect,
                      [[maybe_unused]] Level& level)
  {
  }
  virtual bool interact([[maybe_unused]] AbstractSoundManager& sound_manager, [[maybe_unused]] Level& level) { return false; };
  virtual std::vector<std::pair<geometry::Position, Sprite>> get_sprites(const Level& level) const = 0;
  virtual std::vector<geometry::Rectangle> get_detection_rects([[maybe_unused]] const Level& level) const { return {}; }
  virtual TouchType on_touch([[maybe_unused]] const Player& player,
                             [[maybe_unused]] AbstractSoundManager& sound_manager,
                             [[maybe_unused]] Level& level)
  {
    return TouchType::TOUCH_TYPE_NONE;
  }
  // Returns whether the actor is hittable
  virtual bool on_hit([[maybe_unused]] AbstractSoundManager& sound_manager, const Level& level, [[maybe_unused]] const bool power)
  {
    return is_solid(level);
  }

  geometry::Position position;
  geometry::Size size;
  geometry::Rectangle rect() const { return {position, size}; }

 protected:
  std::vector<geometry::Rectangle> create_detection_rects(const int dx,
                                                          const int dy,
                                                          const Level& level,
                                                          const bool include_self = false) const;
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
  virtual std::vector<std::pair<geometry::Position, Sprite>> get_sprites(const Level& level) const override;

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

  virtual std::vector<std::pair<geometry::Position, Sprite>> get_sprites(const Level& level) const override;

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
  virtual std::vector<std::pair<geometry::Position, Sprite>> get_sprites(const Level& level) const override;

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

  virtual bool interact(AbstractSoundManager& sound_manager, Level& level) override;
  virtual std::vector<std::pair<geometry::Position, Sprite>> get_sprites(const Level& level) const override;

 private:
  bool collected_ = false;
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

  virtual bool interact(AbstractSoundManager& sound_manager, Level& level) override;
  virtual std::vector<std::pair<geometry::Position, Sprite>> get_sprites(const Level& level) const override;
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
  virtual bool is_solid([[maybe_unused]] const Level& level) const { return true; }
  virtual std::vector<std::pair<geometry::Position, Sprite>> get_sprites([[maybe_unused]] const Level& level) const override
  {
    return {std::make_pair(position, Sprite::SPRITE_CLEAR_BLOCK)};
  }
  virtual bool on_hit(AbstractSoundManager& sound_manager, const Level& level, const bool power) override;

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
  virtual std::vector<std::pair<geometry::Position, Sprite>> get_sprites([[maybe_unused]] const Level& level) const override
  {
    if (is_hidden_)
    {
      return {};
    }
    return {std::make_pair(position, Sprite::SPRITE_HIDDEN_BLOCK)};
  }
  virtual TouchType on_touch(const Player& player, AbstractSoundManager& sound_manager, Level& level) override;

 private:
  bool is_hidden_ = true;
};
