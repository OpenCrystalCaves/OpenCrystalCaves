#pragma once

#include <memory>
#include <vector>

#include "enemy.h"
#include "exe_data.h"
#include "hazard.h"
#include "item.h"
#include "level_id.h"
#include "object.h"
#include "player.h"
#include "player_input.h"
#include "player_state.h"
#include "tile.h"

class AbstractSoundManager;
struct Level;

class Game
{
 public:
  static std::unique_ptr<Game> create();

  virtual ~Game() = default;

  virtual bool init(AbstractSoundManager& sound_manager, const ExeData& exe_data, const LevelId level, const PlayerState& player_state) = 0;
  virtual void update(unsigned game_tick, const PlayerInput& player_input) = 0;

  virtual const Player& get_player() const = 0;

  virtual const Level& get_level() const = 0;

  virtual int get_tile_width() const = 0;
  virtual int get_tile_height() const = 0;

  virtual int get_bg_sprite(const int x, const int y) const = 0;

  virtual const std::vector<Object>& get_objects() const = 0;

  virtual unsigned get_score() const = 0;
  virtual unsigned get_num_ammo() const = 0;

  virtual std::wstring get_debug_info() const = 0;

  LevelId entering_level = LevelId::INTRO;
};
