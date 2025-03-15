#pragma once

#include "actor.h"

class Item : public Actor
{
 public:
  Item(geometry::Position position, Sprite sprite, TouchType touch_type)
    : Actor(position, geometry::Size(16, 16)),
      sprite_(sprite),
      touch_type_(touch_type)
  {
  }

  virtual bool is_alive() const override { return is_alive_; }
  virtual TouchType on_touch() override
  {
    is_alive_ = false;
    return touch_type_;
  }
  virtual std::vector<std::pair<geometry::Position, Sprite>> get_sprites(const Level& level) const override
  {
    return {{position, sprite_}};
  }

 private:
  Sprite sprite_;
  bool is_alive_ = true;
  TouchType touch_type_;
};

class Crystal : public Item
{
  // ⬛⬛⬛⬜⬜⬜⬜⬜⬜⬜⬜⬜🟥⬛⬛⬛
  // ⬛⬛⬜🚨🟥🚨🚨🟥🟥🚨🟥🚨🚨🟥⬛⬛
  // ⬛⬜🚨🟥🚨🚨🟥🟥🚨🚨🚨🟥🚨🟥🟥⬛
  // ⬜🚨⬜🚨⬜🟥⬜⬜⬜⬜⬜🟥🟥🟥🟥🟥
  // ⬜🚨🚨🚨🚨🟥🚨🚨🚨🚨🚨🟥🟥🚨🟥🟥
  // ⬛⬜🚨🚨🚨🚨🟥🚨🚨🚨🟥🟥🟥🚨🟥⬛
  // ⬛⬛⬜🚨🚨🚨🟥🚨🚨🚨🟥🟥🚨🟥⬛⬛
  // ⬛⬛⬛⬜🚨🚨🚨🟥🚨🚨🟥🚨🟥⬛⬛⬛
  // ⬛⬛⬛⬛⬜🚨🚨🚨🟥🟥🚨🟥⬛⬛⬛⬛
  // ⬛⬛⬛⬛⬛⬜🚨🚨🟥🚨🟥⬛⬛⬛⬛⬛
  // ⬛⬛⬛⬛⬛⬛⬜🚨🟥🟥⬛⬛⬛⬛⬛⬛
  // ⬛⬛⬛⬛⬛⬛⬛⬜🟥⬛⬛⬛⬛⬛⬛⬛
  // Gives score, need to collect all to finish level
  // TODO: collect all crystals
 public:
  Crystal(geometry::Position position, Sprite sprite) : Item(position, sprite, TouchType::TOUCH_TYPE_SCORE) {}

  virtual int get_points() const override { return 50; }
};


class Ammo : public Item
{
  // ⬛⬛⬛🪦🪦🪦🪦🪦🪦🪦🪦🪦🪦⬛⬛⬛
  // ⬛⬛⬛⬛⬛⬛🪦⬛⬛🪦⬛⬛⬛⬛⬛⬛
  // ⬛🩵⚪⚪⚪⚪⚪⚪⚪⚪⚪⚪⚪⚪⚪⬛
  // ⬛⬛🩵⚪⚪⚪⚪⚪⚪⚪⚪🩵🩵🩵🩵⬛
  // ⬛⬛⬛🩵⚪⚪⚪⚪⚪⚪⚪⚪⚪⚪⚪⬛
  // ⬛⬛⬛🩵⚪⚪🚨🟥⬛🪦🪦🪦🪦🪦🪦⬛
  // ⬛⬛🩵⚪⚪🪦🚨🟥⬛🪦⬛⬛⬛⬛⬛⬛
  // ⬛🩵⚪⚪🪦⬛🪦🪦🪦⬛⬛⬛⬛⬛⬛⬛
  // ⬛🩵⚪🪦⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛
  // ⬛⬛🪦⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛
  // Gives ammo
 public:
  Ammo(geometry::Position position) : Item(position, Sprite::SPRITE_PISTOL, TouchType::TOUCH_TYPE_AMMO) {}
};


class ScoreItem : public Item
{
  // ⬛⬛⬛⬛⬛🟥⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛
  // ⬛⬛⬛⬛🟥🚨🟥⬛⬛⬛⬛⬛⬛⬛⬛⬛
  // ⬛⬛⬛⬛🟥🚨🟥⬛⬛⬛⬛⬛⬛⬛⬛⬛
  // ⬛⬛⬛⬛⬛🟥⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛
  // ⬛⬛⬛⬛⬛⚪⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛
  // ⬛⬛⬛⬛⬜⬜⬜⬛⬛⬛⬛⬛⬛⬛⬛⬛
  // ⬛⬛⬛⬛⚪⬜⬜⬛⬛⬛⬛🟨🟨🟨⬛⬛
  // ⬛⬛⬛⬛⚪⬜⬜⬛⬛⬛🟨⬛⬛⬛🟨⬛
  // ⬛⬛⬛⬛⚪⬜⚪⬛⬛⬛🟨⬛⬛⬛🟠⬛
  // ⬛⬛⬛⬛⚪⬜⚪⬛⬛⬛⬛⬛🟠🟠⬛⬛
  // ⬛⬛⬛⚪⬜⬜⚪⚪⬛⬛⬛🟠⬛⬛⬛⬛
  // ⬛⬜🟨🟨🟨🟨🟨🟨🟨🟨🟠⬛⬛⬛⬛⬛
  // ⬛⬛⬜🟨🟨🟨🟨🟨🟨🟠⬛⬛⬛⬛⬛⬛
  // ⬛⬛⬛🟠🟠🟠🟠🟠🟠⬛⬛⬛⬛⬛⬛⬛
  // Generic item that gives score
  // TODO: different sounds
 public:
  ScoreItem(geometry::Position position, Sprite sprite, int score) : Item(position, sprite, TouchType::TOUCH_TYPE_SCORE), score_(score) {}

  virtual int get_points() const override { return score_; }

 private:
  int score_;
};

class Key : public Item
{
  // ⬛⬛⬛⬛⬛⬛🟨🟨🟨🟨⬛⬛⬛⬛⬛⬛
  // ⬛⬛⬛⬛⬛🟨⬛⬛⬛⬛🟨⬛⬛⬛⬛⬛
  // ⬛⬛⬛⬛⬛🟨⬛⬛⬛⬛🟨⬛⬛⬛⬛⬛
  // ⬛⬛⬛⬛⬛🟨⬛⬛⬛⬛🟨⬛⬛⬛⬛⬛
  // ⬛⬛⬛⬛⬛⬛🟨🟨🟨🟨⬛⬛⬛⬛⬛⬛
  // ⬛⬛⬛⬛⬛⬛⬛🟨⬛⬛⬛⬛⬛⬛⬛⬛
  // ⬛⬛⬛⬛⬛⬛⬛🟨⬛⬛⬛⬛⬛⬛⬛⬛
  // ⬛⬛⬛⬛⬛⬛⬛🟨⬛⬛⬛⬛⬛⬛⬛⬛
  // ⬛⬛⬛⬛⬛⬛⬛🟨⬛⬛⬛⬛⬛⬛⬛⬛
  // ⬛⬛⬛⬛⬛⬛⬛🟨⬛⬛⬛⬛⬛⬛⬛⬛
  // ⬛⬛⬛⬛⬛⬛⬛🟨🟨🟨⬛⬛⬛⬛⬛⬛
  // ⬛⬛⬛⬛⬛⬛⬛🟨⬛⬛⬛⬛⬛⬛⬛⬛
  // ⬛⬛⬛⬛⬛⬛⬛🟨⬛⬛⬛⬛⬛⬛⬛⬛
  // ⬛⬛⬛⬛⬛⬛⬛🟨🟨🟨🟨⬛⬛⬛⬛⬛
  // Gives player ability to open chests
 public:
  Key(geometry::Position position) : Item(position, Sprite::SPRITE_KEY, TouchType::TOUCH_TYPE_KEY) {}
};


class Power : public Item
{
  // ⬛⬛⬛⬛⬛⬜⚪⚪⚪⚪🪦⬛⬛⬛⬛⬛
  // ⬛⬛⬛⬜⬜⚪⚪⚪⚪⚪⚪🪦🪦⬛⬛⬛
  // ⬛⬛⬜⚪⚪⚪⚪⚪⚪⚪⚪⚪⚪🪦⬛⬛
  // ⬛⬛⬜⚪⚪🟥🟥🟥🟥🟥⚪⚪⚪🪦⬛⬛
  // ⬛⬜⚪⚪⚪🟥🟥🟥🟥🟥🟥⚪⚪⚪🪦⬛
  // ⬛⬜⚪⚪⚪🟥🟥⚪⚪🟥🟥⚪⚪⚪🪦⬛
  // ⬛⬜⚪⚪⚪🟥🟥🟥🟥🟥🟥⚪⚪⚪🪦⬛
  // ⬛⬜⚪⚪⚪🟥🟥🟥🟥🟥⚪⚪⚪⚪🪦⬛
  // ⬛⬜⚪⚪⚪🟥🟥⚪⚪⚪⚪⚪⚪⚪🪦⬛
  // ⬛⬜⚪⚪⚪🟥🟥⚪⚪⚪⚪⚪⚪⚪🪦⬛
  // ⬛⬛⬜⚪⚪🟥🟥⚪⚪⚪⚪⚪⚪🪦⬛⬛
  // ⬛⬛⬜⚪⚪⚪⚪⚪⚪⚪⚪⚪⚪🪦⬛⬛
  // ⬛⬛⬛⬜⬜⚪⚪⚪⚪⚪⚪🪦🪦⬛⬛⬛
  // ⬛⬛⬛⬛⬛⬜⚪⚪⚪⚪🪦⬛⬛⬛⬛⬛
  // Gives player timed power shots that can kill tough enemies
 public:
  Power(geometry::Position position) : Item(position, Sprite::SPRITE_POWER, TouchType::TOUCH_TYPE_POWER) {}
};

// TODO: egg
// TODO: gravity