#include "game_renderer.h"

#include "constants.h"
#include "game.h"
#include "graphics.h"
#include "level.h"
#include "misc.h"
#include "occ_math.h"
#include "player.h"
#include "spritemgr.h"
#include "utils.h"

GameRenderer::GameRenderer(Game* game, SpriteManager* sprite_manager, Surface* game_surface, Window& window)
  : game_(game),
    sprite_manager_(sprite_manager),
    game_surface_(game_surface),
    window_(window),
    game_camera_(math::clamp(game_->get_player().position.x() + (game_->get_player().size.x() / 2) - (CAMERA_SIZE.x() / 2),
                             0,
                             (game_->get_tile_width() * 16) - CAMERA_SIZE.x()),
                 math::clamp(game_->get_player().position.y() + (game_->get_player().size.y() / 2) - (CAMERA_SIZE.y() / 2),
                             0,
                             (game_->get_tile_height() * 16) - CAMERA_SIZE.y()),
                 CAMERA_SIZE.x(),
                 CAMERA_SIZE.y()),
    game_tick_(0u),
    game_tick_diff_(0u),
    debug_(false)
{
}

void GameRenderer::update(unsigned game_tick)
{
  game_tick_diff_ = game_tick - game_tick_;
  game_tick_ = game_tick;

  if (!game_->get_level().has_crystals || game_->get_level().crystals > 0)
  {
    game_complete_ticks_ = 20;
  }
  else if (game_->get_level().has_crystals && game_complete_ticks_ > 0)
  {
    game_complete_ticks_--;
  }

  // Update game camera
  // Note: this isn't exactly how the Crystal Caves camera work, but it's good enough
  const geometry::Position player_camera_relative_position{(game_->get_player().position + (game_->get_player().size / 2)) -
                                                           (game_camera_.position + (game_camera_.size / 2))};
  if (player_camera_relative_position.x() < -4)
  {
    game_camera_.position = geometry::Position(
      math::clamp(game_camera_.position.x() + player_camera_relative_position.x() + 4, 0, (game_->get_tile_width() * 16) - CAMERA_SIZE.x()),
      game_camera_.position.y());
  }
  else if (player_camera_relative_position.x() > 20)
  {
    game_camera_.position = geometry::Position(math::clamp(game_camera_.position.x() + player_camera_relative_position.x() - 20,
                                                           0,
                                                           (game_->get_tile_width() * 16) - CAMERA_SIZE.x()),
                                               game_camera_.position.y());
  }

  if (player_camera_relative_position.y() < -10)
  {
    game_camera_.position = geometry::Position(game_camera_.position.x(),
                                               math::clamp(game_camera_.position.y() + player_camera_relative_position.y() + 10,
                                                           0,
                                                           (game_->get_tile_height() * 16) - CAMERA_SIZE.y()));
  }
  else if (player_camera_relative_position.y() > 32)
  {
    game_camera_.position = geometry::Position(game_camera_.position.x(),
                                               math::clamp(game_camera_.position.y() + player_camera_relative_position.y() - 32,
                                                           0,
                                                           (game_->get_tile_height() * 16) - CAMERA_SIZE.y()));
  }
}

void GameRenderer::render_game(unsigned game_tick) const
{
  window_.set_render_target(game_surface_);
  // Clear game surface (background now)
  window_.fill_rect(geometry::Rectangle(0, 0, CAMERA_SIZE), {33u, 33u, 33u});
  render_background();
  render_tiles(false);
  render_objects();
  render_enemies(game_tick);
  render_player();
  render_tiles(true);
  render_complete_border();
  render_statusbar();
  window_.set_render_target(nullptr);
}

void GameRenderer::render_background() const
{
  // TODO: Create a surface of size CAMERA + (background.size() * 16) and render the background
  //       to it _once_, then just keep render that surface (with game_camera offset) until the
  //       level changes.

  const auto start_tile_x = game_camera_.position.x() > 0 ? game_camera_.position.x() / 16 : 0;
  const auto start_tile_y = game_camera_.position.y() > 0 ? game_camera_.position.y() / 16 : 0;
  const auto end_tile_x = (game_camera_.position.x() + game_camera_.size.x()) / 16;
  const auto end_tile_y = (game_camera_.position.y() + game_camera_.size.y()) / 16;

  for (int tile_y = start_tile_y; tile_y <= end_tile_y; tile_y++)
  {
    for (int tile_x = start_tile_x; tile_x <= end_tile_x; tile_x++)
    {
      const auto sprite_id = game_->get_bg_sprite(tile_x, tile_y);
      if (sprite_id != -1)
      {
        render_tile(sprite_id, {tile_x * SPRITE_W, tile_y * SPRITE_H});
      }
    }
  }

  if (game_->get_level().has_earth)
  {
    const int earth_orbit_radius = (game_->get_tile_width() - 4) * 16 / 2;
    const int earth_pos_x =
      (game_->get_tile_width() - 2) * 16 / 2 + 16 + static_cast<int>(sin(game_tick_ / 500.0 - M_PI_2) * earth_orbit_radius);
    // Assume there's a moon
    constexpr int moon_orbit_radius = 2 * 16;
    constexpr double moon_orbit_period = 30.0;
    const int moon_pos_x = earth_pos_x + static_cast<int>(sin(game_tick_ / moon_orbit_period) * moon_orbit_radius);
    const bool moon_right = cos(game_tick_ / moon_orbit_period) > 0;
    const int moon_sprite = static_cast<int>(moon_right ? Sprite::SPRITE_MOON_SMALL : Sprite::SPRITE_MOON);

    const auto earth_rect = geometry::Rectangle(geometry::Position(earth_pos_x, 0), geometry::Size(16, 16));
    const auto moon_rect = geometry::Rectangle(geometry::Position(moon_pos_x, 0), geometry::Size(16, 16));
    if (moon_right)
    {
      // Moon is behind earth, render moon first
      if (geometry::isColliding(game_camera_, moon_rect))
      {
        render_tile(moon_sprite, moon_rect.position);
      }
      if (geometry::isColliding(game_camera_, earth_rect))
      {
        render_tile(static_cast<int>(Sprite::SPRITE_EARTH), earth_rect.position);
      }
    }
    else
    {
      // Earth is behind moon, render earth first
      if (geometry::isColliding(game_camera_, earth_rect))
      {
        render_tile(static_cast<int>(Sprite::SPRITE_EARTH), earth_rect.position);
      }
      if (geometry::isColliding(game_camera_, moon_rect))
      {
        render_tile(moon_sprite, moon_rect.position);
      }
    }
  }

  // MAIN_LEVEL has some special things that needs to be rendered
  if (game_->get_level().level_id == LevelId::MAIN_LEVEL)
  {
    // Might be cleaner to have this in a dedicated struct for MainLevel stuff

    static bool volcano_active = false;
    static unsigned volcano_tick_start = 0u;

    // Update volcano
    if (volcano_active && game_tick_ - volcano_tick_start >= 81u)
    {
      volcano_active = false;
      volcano_tick_start = game_tick_;
    }
    else if (!volcano_active && game_tick_ - volcano_tick_start >= 220u)
    {
      volcano_active = true;
      volcano_tick_start = game_tick_;
    }

    // Render volcano fire if active and visible
    if (volcano_active)
    {
      if (start_tile_x <= 29 && end_tile_x >= 29)
      {
        const auto sprite_id = 752 + ((game_tick_ - volcano_tick_start) / 3) % 4;
        render_tile(sprite_id, {29 * SPRITE_W, 2 * SPRITE_H});
      }
      if (start_tile_x <= 30 && end_tile_x >= 30)
      {
        const auto sprite_id = 748 + ((game_tick_ - volcano_tick_start) / 3) % 4;
        render_tile(sprite_id, {30 * SPRITE_W, 2 * SPRITE_H});
      }
    }
  }
}

// TODO: refactor to player add object
void GameRenderer::render_player() const
{
  // Player sprite ids
  static constexpr std::array<Sprite, 7> sprite_explode = {Sprite::SPRITE_PLAYER_EXPLODE_1,
                                                           Sprite::SPRITE_PLAYER_EXPLODE_1,
                                                           Sprite::SPRITE_PLAYER_EXPLODE_1,
                                                           Sprite::SPRITE_PLAYER_EXPLODE_2,
                                                           Sprite::SPRITE_PLAYER_EXPLODE_3,
                                                           Sprite::SPRITE_PLAYER_EXPLODE_3,
                                                           Sprite::SPRITE_PLAYER_EXPLODE_3};
  static constexpr std::array<int, 12> sprite_walking_right = {260, 261, 262, 263, 264, 265, 266, 267, 268, 269, 270, 271};
  static constexpr int sprite_jumping_right = 284;
  static constexpr int sprite_shooting_right = 286;

  static constexpr int sprite_standing_left = 272;
  static constexpr std::array<int, 12> sprite_walking_left = {272, 273, 274, 275, 276, 277, 278, 279, 280, 281, 282, 283};
  static constexpr int sprite_jumping_left = 285;
  static constexpr int sprite_shooting_left = 287;

  static constexpr std::array<int, 12> sprite_walking_dy = {0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1};

  const int sprite = [this]()
  {
    const auto& player = game_->get_player();
    int sprite = 0;

    if (player.dying_tick > 0)
    {
      const int ds = 12 - player.dying_tick / 4;
      if (ds > 6)
      {
        // TODO: spawn particle and flying hat
        return 0;
      }
      return static_cast<int>(sprite_explode[ds]);
    }

    // Sprite selection priority: (currently 'shooting' means pressing shoot button without ammo)
    // If walking:
    //   1. Jumping or falling
    //   2. Walking
    // Else:
    //   1. Shooting
    //   2. Jumping or falling
    //   3. Standing still

    if (player.direction == Player::Direction::right)
    {
      if (player.walking)
      {
        if (player.jumping || player.falling)
        {
          sprite = sprite_jumping_right;
        }
        else
        {
          sprite = sprite_walking_right[player.walk_tick % sprite_walking_right.size()];
        }
      }
      else
      {
        if (player.shooting)
        {
          sprite = sprite_shooting_right;
        }
        else if (player.jumping || player.falling)
        {
          sprite = sprite_jumping_right;
        }
        else
        {
          sprite = static_cast<int>(Sprite::SPRITE_STANDING_RIGHT);
        }
      }
    }
    else  // player_.direction == Player::Direction::left
    {
      if (player.walking)
      {
        if (player.jumping || player.falling)
        {
          sprite = sprite_jumping_left;
        }
        else
        {
          sprite = sprite_walking_left[player.walk_tick % sprite_walking_left.size()];
        }
      }
      else
      {
        if (player.shooting)
        {
          sprite = sprite_shooting_left;
        }
        else if (player.jumping || player.falling)
        {
          sprite = sprite_jumping_left;
        }
        else
        {
          sprite = sprite_standing_left;
        }
      }
    }
    if (player.is_reverse_gravity() ^ game_->get_level().reverse_gravity)
    {
      sprite += 104;
    }
    return sprite;
  }();
  // Note: player size is 12x16 but the sprite is 16x16 so we need to adjust where
  // the player is rendered
  const auto player_render_pos = game_->get_player().position - geometry::Position{2, 0};

  const geometry::Rectangle dest_rect{player_render_pos - game_camera_.position, 16, 16};

  if (sprite > 0)
  {
    if (game_->get_player().crushed)
    {
      // only render the hat and the feet
      constexpr int feet_h = 2;
      constexpr int hat_h = 3;
      const auto src_rect = sprite_manager_->get_rect_for_tile(sprite);
      const geometry::Rectangle feet_src_rect{src_rect.position.x(), src_rect.position.y() + 16 - feet_h, src_rect.size.x(), feet_h};
      const geometry::Rectangle feet_dest_rect{dest_rect.position.x(), dest_rect.position.y() + 16 - feet_h, dest_rect.size.x(), feet_h};
      sprite_manager_->get_surface()->blit_surface(feet_src_rect, feet_dest_rect);
      const int hat_dy = sprite_walking_dy[game_->get_player().walk_tick % sprite_walking_dy.size()];
      const geometry::Rectangle hat_src_rect{src_rect.position, src_rect.size.x(), hat_h};
      const geometry::Rectangle hat_dest_rect{
        dest_rect.position.x(), dest_rect.position.y() + 16 - hat_h - feet_h, dest_rect.size.x(), hat_h + hat_dy};
      sprite_manager_->get_surface()->blit_surface(hat_src_rect, hat_dest_rect);
    }
    else if (game_->get_player().is_flashing())
    {
      // TODO: draw white sprite
      window_.fill_rect(dest_rect, {255u, 255u, 255u});
    }
    else
    {
      // Render tough player as red tinted
      const int period = FPS / 8;
      float d = sprite_manager_->remaster ? (float)(game_->get_player().tough_tick % period) / period : 0;
      if (d > 0.5f)
      {
        d = 1 - d;
      }
      const std::uint8_t gb = 0xff - static_cast<std::uint8_t>(d * 2 * 0xff);
      render_tile(sprite, player_render_pos, {0xff, gb, gb});
    }
  }

  if (debug_)
  {
    window_.render_rectangle(dest_rect, {255, 0, 0});
  }
}

void GameRenderer::render_tiles(bool in_front) const
{
  const auto start_tile_x = game_camera_.position.x() > 0 ? game_camera_.position.x() / 16 : 0;
  const auto start_tile_y = game_camera_.position.y() > 0 ? game_camera_.position.y() / 16 : 0;
  const auto end_tile_x = (game_camera_.position.x() + game_camera_.size.x()) / 16;
  const auto end_tile_y = (game_camera_.position.y() + game_camera_.size.y()) / 16;

  for (int tile_y = start_tile_y; tile_y <= end_tile_y; tile_y++)
  {
    for (int tile_x = start_tile_x; tile_x <= end_tile_x; tile_x++)
    {
      const auto& tile = game_->get_level().get_tile(tile_x, tile_y);

      if (debug_ && !tile.is_solid() && tile.is_solid_for_slime())
      {
        const geometry::Rectangle dest_rect{geometry::Position(tile_x * SPRITE_W, tile_y * SPRITE_H) - game_camera_.position,
                                            geometry::Size(SPRITE_W, SPRITE_H)};
        window_.render_rectangle(dest_rect, {0, 128, 0});
      }

      if (!tile.valid())
      {
        continue;
      }

      if ((in_front && !tile.is_render_in_front()) || (!in_front && tile.is_render_in_front()))
      {
        continue;
      }

      const auto sprite_id = [&tile](unsigned game_tick)
      {
        if (tile.is_animated())
        {
          return tile.get_sprite() + static_cast<int>((game_tick / 2) % tile.get_sprite_count());
        }
        else
        {
          return tile.get_sprite();
        }
      }(game_tick_);
      render_tile(sprite_id, {tile_x * SPRITE_W, tile_y * SPRITE_H});
    }
  }
}

void GameRenderer::render_objects() const
{
  for (const auto& object : game_->get_objects())
  {
    static constexpr geometry::Size object_size = geometry::Size(16, 16);
    if (geometry::isColliding(geometry::Rectangle(object.position, object_size), game_camera_))
    {
      const auto sprite_id = object.get_sprite(game_tick_);
      render_tile(sprite_id, object.position);

      if (debug_)
      {
        const geometry::Rectangle dest_rect{object.position - game_camera_.position, object_size};
        window_.render_rectangle(dest_rect, {255, 0, 0});
      }
    }
  }
  if (debug_)
  {
    for (const auto& hazard : game_->get_level().hazards)
    {
      window_.render_rectangle({hazard->position - game_camera_.position, hazard->size}, {255, 128, 0});
      for (const auto r : hazard->get_detection_rects(game_->get_level()))
      {
        const geometry::Rectangle dest_rect{r.position - game_camera_.position, r.size};
        window_.render_rectangle(dest_rect, {255, 255, 0});
      }
    }
  }
}

void GameRenderer::render_enemies(unsigned game_tick) const
{
  for (const auto& enemy : game_->get_level().enemies)
  {
    if (geometry::isColliding(enemy->rect(), game_camera_))
    {
      for (const auto& sprite_pos : enemy->get_sprites(game_->get_level()))
      {
        // Blink if time is stopped
        // Blink faster when the effect is about to wear off
        if (game_->get_player().stop_tick == 0 ||
            (game_->get_player().stop_tick < 2 * FPS / FRAMES_PER_TICK ? (game_tick % 4) < 3 : (game_tick % 10) < 7))
        {
          render_tile(static_cast<int>(sprite_pos.second), sprite_pos.first);
        }

        if (debug_)
        {
          const geometry::Rectangle dest_rect{enemy->position - game_camera_.position, enemy->size};
          window_.render_rectangle(dest_rect, {255, 0, 0});
        }
      }
    }
    if (debug_)
    {
      for (const auto r : enemy->get_detection_rects(game_->get_level()))
      {
        const geometry::Rectangle dest_rect{r.position - game_camera_.position, r.size};
        window_.render_rectangle(dest_rect, {255, 255, 0});
      }
    }
  }

  // Falling rocks detection rects
  if (debug_)
  {
    for (const auto& r : game_->get_level().falling_rocks_areas)
    {
      const geometry::Rectangle dest_rect{r.position - game_camera_.position, r.size};
      window_.render_rectangle(dest_rect, {255, 0, 255});
    }
  }
}

void GameRenderer::render_complete_border() const
{
  if ((game_complete_ticks_ % 8) < 4)
  {
    const auto statusbar_rect = geometry::Rectangle(0, 0, game_camera_.size.x() - 1, game_camera_.size.y() - 1);
    window_.render_rectangle(statusbar_rect, {0u, 255u, 0u});
  }
}

void GameRenderer::render_statusbar() const
{
  constexpr auto statusbar_height = CHAR_H;
  const auto statusbar_rect = geometry::Rectangle(0, game_camera_.size.y() - CHAR_H, game_camera_.size.x(), statusbar_height);

  window_.fill_rect(statusbar_rect, {0u, 0u, 0u});

  constexpr int dy = 1;
  // $
  sprite_manager_->render_text(L"$", statusbar_rect.position + geometry::Position(0, dy));
  // score
  sprite_manager_->render_number(game_->get_score(), statusbar_rect.position + geometry::Position(8 * CHAR_W, dy));
  // Gun
  sprite_manager_->render_icon(Icon::ICON_GUN, statusbar_rect.position + geometry::Position(11 * CHAR_W, dy));
  // ammo
  sprite_manager_->render_number(game_->get_num_ammo(), statusbar_rect.position + geometry::Position(15 * CHAR_W, dy));
  // Hearts
  for (unsigned i = 0; i < game_->get_player().health_; i++)
  {
    sprite_manager_->render_icon(Icon::ICON_HEART, statusbar_rect.position + geometry::Position((i + 19) * CHAR_W, dy));
  }
  // Key
  if (game_->has_key())
  {
    sprite_manager_->render_icon(Icon::ICON_KEY, statusbar_rect.position + geometry::Position(23 * CHAR_W, dy));
  }
  // Timers
  // TODO: how to handle/allow multiple timed powerups
  if (game_->get_player().power_tick > 0)
  {
    sprite_manager_->render_text(L"*", statusbar_rect.position + geometry::Position(27 * CHAR_W, dy));
    sprite_manager_->render_number(game_->get_player().power_tick * FRAMES_PER_TICK / FPS,
                                   statusbar_rect.position + geometry::Position(30 * CHAR_W, dy));
    sprite_manager_->render_text(L"*", statusbar_rect.position + geometry::Position(30 * CHAR_W, dy));
  }
  else if (game_->get_player().gravity_tick > 0)
  {
    sprite_manager_->render_text(L"*", statusbar_rect.position + geometry::Position(27 * CHAR_W, dy));
    sprite_manager_->render_number(game_->get_player().gravity_tick * FRAMES_PER_TICK / FPS,
                                   statusbar_rect.position + geometry::Position(30 * CHAR_W, dy));
    sprite_manager_->render_text(L"*", statusbar_rect.position + geometry::Position(30 * CHAR_W, dy));
  }
  else if (game_->get_player().stop_tick > 0)
  {
    sprite_manager_->render_text(L"*", statusbar_rect.position + geometry::Position(27 * CHAR_W, dy));
    sprite_manager_->render_number(game_->get_player().stop_tick * FRAMES_PER_TICK / FPS,
                                   statusbar_rect.position + geometry::Position(30 * CHAR_W, dy));
    sprite_manager_->render_text(L"*", statusbar_rect.position + geometry::Position(30 * CHAR_W, dy));
  }
  else if (game_->get_player().tough_tick > 0)
  {
    sprite_manager_->render_text(L"*", statusbar_rect.position + geometry::Position(27 * CHAR_W, dy));
    sprite_manager_->render_number(game_->get_player().tough_tick * FRAMES_PER_TICK / FPS,
                                   statusbar_rect.position + geometry::Position(30 * CHAR_W, dy));
    sprite_manager_->render_text(L"*", statusbar_rect.position + geometry::Position(30 * CHAR_W, dy));
  }
}

void GameRenderer::render_tile(const int sprite, const geometry::Position& pos, const Color color) const
{
  if (!(game_->get_level().switch_flags & SWITCH_FLAG_LIGHTS))
  {
    sprite_manager_->render_tile(sprite + SPRITE_TOTAL, pos, game_camera_.position, color);
  }
  else
  {
    sprite_manager_->render_tile(sprite, pos, game_camera_.position, color);
  }
}
