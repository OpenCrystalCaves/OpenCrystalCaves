#pragma once

#include <bitset>
#include <vector>

#include "enemy.h"
#include "entrance.h"
#include "exit.h"
#include "geometry.h"
#include "hazard.h"
#include "item.h"
#include "level_id.h"
#include "moving_platform.h"
#include "sprite.h"
#include "tile.h"

struct Level
{
  LevelId level_id;

  int width;
  int height;

  geometry::Position player_spawn;

  const Tile& get_tile(const int x, const int y) const;
  int get_bg(const int x, const int y) const;
  const Item& get_item(const int x, const int y) const;
  void remove_item(const int x, const int y);
  bool collides_solid(const geometry::Position& position, const geometry::Size& size, const bool is_slime = false) const;
  bool collides_solid_top(const geometry::Position& position, const geometry::Size& size) const;

  std::vector<int> bgs;
  std::vector<Tile> tiles;
  std::vector<Item> items;

  std::vector<std::unique_ptr<Enemy>> enemies;
  std::vector<std::unique_ptr<Hazard>> hazards;
  std::vector<std::unique_ptr<Actor>> actors;
  std::vector<MovingPlatform> moving_platforms;
  std::vector<Entrance> entrances;
  std::unique_ptr<Exit> exit;
  bool has_earth = false;
  bool has_moon = false;
  bool switch_on = false;
  std::bitset<3> lever_on = {0};
};
