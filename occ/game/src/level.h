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
#include "particle.h"
#include "sprite.h"
#include "tile.h"

struct Player;

enum SwitchFlag
{
  SWITCH_FLAG_MOVING_PLATFORMS = 1,
  SWITCH_FLAG_LASERS = 2,
  SWITCH_FLAG_LIGHTS = 4,
};

struct Level
{
  LevelId level_id;

  int width;
  int height;

  geometry::Position player_spawn;

  const Tile& get_tile(const int x, const int y) const;
  int get_bg(const int x, const int y) const;
  bool collides_solid(const geometry::Position& position,
                      const geometry::Size& size,
                      const bool is_slime = false,
                      Actor** collides_actor = nullptr) const;
  bool collides_solid_top(const geometry::Position& position, const geometry::Size& size) const;
  Actor* collides_actor(const geometry::Position& position, const geometry::Size& size) const;
  Hazard* collides_hazard(const geometry::Position& position, const geometry::Size& size) const;
  Enemy* collides_enemy(const geometry::Position& position, const geometry::Size& size) const;
  bool player_on_platform(const geometry::Position& position, const geometry::Size& size) const;

  // Helper fields for the level viewer
  std::vector<int> tile_ids;
  std::vector<bool> tile_unknown;

  std::vector<int> bgs;
  std::vector<Tile> tiles;

  std::vector<std::unique_ptr<Enemy>> enemies;
  std::vector<std::unique_ptr<Hazard>> hazards;
  std::vector<std::unique_ptr<Actor>> actors;
  std::vector<std::unique_ptr<Particle>> particles;
  std::vector<MovingPlatform> moving_platforms;
  std::vector<Entrance> entrances;
  std::unique_ptr<Exit> exit;
  bool has_earth = false;
  bool has_moon = false;
  int switch_flags = SWITCH_FLAG_LASERS | SWITCH_FLAG_LIGHTS;
  bool has_key = false;
  int crystals = 0;
  bool has_crystals = false;
  std::bitset<3> lever_on = {0};
  geometry::Position dv;
  std::vector<geometry::Rectangle> falling_rocks_areas = {};
  int falling_rock_ticks = 0;
  bool reverse_gravity = false;
  bool no_air = false;
};
