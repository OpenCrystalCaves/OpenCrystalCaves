#pragma once

#include <sprite.h>

enum class ItemType : int
{
  ITEM_TYPE_CRYSTAL = 0,
  ITEM_TYPE_AMMO = 1,
  ITEM_TYPE_SCORE = 2,
  ITEM_TYPE_POWER = 3,
  ITEM_TYPE_EGG = 4,  // TODO: shootable, spawn BONUS
  ITEM_TYPE_KEY = 5,
};

#define CRYSTAL_SCORE 50
#define MAX_AMMO 99
#define AMMO_AMOUNT 5

class Item
{
 public:
  Item() : valid_(false), sprite_(Sprite::SPRITE_NONE), type_(ItemType::ITEM_TYPE_CRYSTAL), amount_(0) {}

  Item(Sprite sprite, ItemType type, int amount) : valid_(true), sprite_(sprite), type_(type), amount_(amount) {}

  bool valid() const { return valid_; }
  void invalidate() { valid_ = false; }

  Sprite get_sprite() const { return sprite_; }
  ItemType get_type() const { return type_; }
  int get_amount() const { return amount_; }

  static const Item INVALID;

 private:
  bool valid_;
  Sprite sprite_;
  ItemType type_;
  int amount_;
};
