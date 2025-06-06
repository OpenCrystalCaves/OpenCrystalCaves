#pragma once

#include "geometry.h"
#include "graphics.h"

class Game;
class SpriteManager;
class Surface;
class Window;

class GameRenderer
{
 public:
  GameRenderer(Game* game, SpriteManager* sprite_manager, Surface* game_surface, Window& window);

  void update(unsigned game_tick);
  void render_game(unsigned game_tick) const;

  const geometry::Rectangle& get_game_camera() const { return game_camera_; }

  bool get_debug() const { return debug_; }
  void set_debug(bool debug) { debug_ = debug; }

 private:
  void render_background() const;
  void render_player() const;
  void render_tiles(bool in_front) const;
  void render_objects() const;
  void render_enemies(unsigned game_tick) const;
  void render_complete_border() const;
  void render_statusbar() const;
  void render_tile(const int sprite, const geometry::Position& pos, const Color color = {0xff, 0xff, 0xff}) const;

  Game* game_;
  SpriteManager* sprite_manager_;
  Surface* game_surface_;
  Window& window_;

  geometry::Rectangle game_camera_;

  unsigned game_tick_;
  unsigned game_tick_diff_;
  unsigned game_complete_ticks_ = 20;

  bool debug_;
};
