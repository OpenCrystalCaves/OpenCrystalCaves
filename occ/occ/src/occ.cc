#include <cassert>
#include <cstdio>
#include <cstdlib>

#include <memory>

#include "constants.h"
#include "game_renderer.h"
#include "spritemgr.h"

// From game
#include "game.h"
#include "player_input.h"
#include "player.h"

// From sdl_wrapper
#include "sdl_wrapper.h"
#include "event.h"
#include "graphics.h"

// From utils
#include "geometry.h"
#include "logger.h"

PlayerInput input_to_player_input(const Input& input)
{
  PlayerInput pi;
  pi.left = input.left.pressed;
  pi.right = input.right.pressed;
  pi.jump = input.z.pressed;
  pi.shoot = input.x.pressed;
  return pi;
}

void render_statusbar(Surface* surface, unsigned score, unsigned num_ammo, unsigned num_lives)
{
  constexpr auto statusbar_height = WINDOW_SIZE.y() / 12 / 2;
  constexpr auto statusbar_rect = geometry::Rectangle(0, WINDOW_SIZE.y() - statusbar_height,
                                                      WINDOW_SIZE.x(), statusbar_height);

  char statusbar_text[64];
  snprintf(statusbar_text, 64, "Score: %8u Ammo: %2u Life: %u", score, num_ammo, num_lives);

  surface->fill_rect(statusbar_rect, { 0u, 0u, 0u });
  surface->render_text(statusbar_rect.position + geometry::Position(32, 0),
                       statusbar_text,
                       24,
                       { 255u, 255u, 255u });
}

int main()
{
  LOG_INFO("Starting!");

  // Init SDL wrapper
  auto sdl = SDLWrapper::create();
  if (!sdl)
  {
    LOG_CRITICAL("Could not create SDLWrapper");
    return 1;
  }
  if (!sdl->init())
  {
    LOG_CRITICAL("Could not initialize SDLWrapper");
    return 1;
  }
  LOG_INFO("SDLWrapper initialized");

  // Create Window
  auto window = Window::create("OpenCrystalCaves", WINDOW_SIZE);
  if (!window)
  {
    LOG_CRITICAL("Could not create Window");
    return 1;
  }
  auto window_surface = window->get_surface();
  LOG_INFO("Window created");

  // Create game surface
  std::unique_ptr<Surface> game_surface = window->create_surface(CAMERA_SIZE);
  if (!game_surface)
  {
    LOG_CRITICAL("Could not create game surface");
    return 1;
  }
  LOG_INFO("Game surface created");

  // Create event handler
  auto event = Event::create();
  if (!event)
  {
    LOG_CRITICAL("Could not create event handler");
    return 1;
  }

  // Load tileset
  SpriteManager sprite_manager;
  if (!sprite_manager.load_tileset("media/sprites.bmp"))
  {
    LOG_CRITICAL("Could not load tileset");
    return 1;
  }
  LOG_INFO("Tileset loaded");

  // Create Game
  std::unique_ptr<Game> game = Game::create();
  if (!game)
  {
    LOG_CRITICAL("Could not create Game");
    return 1;
  }
  if (!game->init())
  {
    LOG_CRITICAL("Could not initialize Game");
    return 1;
  }
  LOG_INFO("Game initialized");

  // Create GameRenderer
  GameRenderer game_renderer(game.get(), &sprite_manager, game_surface.get());

  // Game loop
  {
    // Game variables
    bool paused = false;
    unsigned game_tick = 0u;
    Input input;

    // Debug information
    bool debug = false;

    // Game loop logic
    auto sdl_tick = sdl->get_tick();
    const auto ms_per_update = 57;  // 17.5~ ticks per second
    auto tick_last_update = sdl_tick;
    auto lag = 0u;

    // FPS logic
    auto fps_num_renders = 0u;
    auto fps_last_calc = sdl_tick;
    auto fps_start_time = sdl_tick;
    auto fps = 0u;

    while (true)
    {
      /////////////////////////////////////////////////////////////////////////
      ///
      ///  Logic
      ///
      /////////////////////////////////////////////////////////////////////////
      sdl_tick = sdl->get_tick();
      auto elapsed_ticks = sdl_tick - tick_last_update;
      tick_last_update = sdl_tick;
      lag += elapsed_ticks;
      while (lag >= ms_per_update)
      {
        // Read input
        event->poll_event(&input);

        // Handle input
        if (input.quit)
        {
          return 0;  // Quit ASAP
        }
        if (input.num_1.pressed && !input.num_1.repeated)
        {
          debug = !debug;
        }
        if (input.enter.pressed && !input.enter.repeated)
        {
          paused = !paused;
        }

        if (!paused || (paused && input.space.pressed && !input.space.repeated))
        {
          // Call game loop
          game->update(game_tick, input_to_player_input(input));
          game_tick += 1;
        }

        lag -= ms_per_update;
      }

      /////////////////////////////////////////////////////////////////////////
      ///
      ///  Render
      ///
      /////////////////////////////////////////////////////////////////////////

      // Clear window surface
      window_surface->fill_rect(geometry::Rectangle(0, 0, WINDOW_SIZE), { 33u, 33u, 33u });

      // Render game
      game_renderer.render_game(game_tick);

      // Render game surface to window surface, centered and scaled
      window_surface->blit_surface(game_surface.get(),
                                   geometry::Rectangle(0, 0, CAMERA_SIZE),
                                   geometry::Rectangle((WINDOW_SIZE - CAMERA_SIZE_SCALED) / 2, CAMERA_SIZE_SCALED),
                                   BlitType::SCALE);

      // Render statusbar
      render_statusbar(window_surface.get(), game->get_score(), game->get_num_ammo(), game->get_num_lives());

      // Render FPS
      auto fps_str = "fps: " + std::to_string(fps);
      window_surface->render_text(geometry::Position(5, 5), fps_str, 12, { 255u, 255u, 255u });

      // Debug
      if (debug)
      {
        // Put a black box where we're going to the draw the debug text
        window_surface->fill_rect({ 0, 24, 200, 165 }, { 0u, 0u, 0u });

        const auto& game_camera = game_renderer.get_game_camera();

        // Debug text
        const auto camera_position_str = "camera position: (" + std::to_string(game_camera.position.x()) + ", " + std::to_string(game_camera.position.y()) + ")";
        const auto player_position_str = "player position: (" + std::to_string(game->get_player().position.x()) + ", " + std::to_string(game->get_player().position.y()) + ")";
        const auto player_velocity_str = "player velocity: (" + std::to_string(game->get_player().velocity.x()) + ", " + std::to_string(game->get_player().velocity.y()) + ")";
        const auto player_walking_str  = std::string("player walking: ") + (game->get_player().walking ? "true" : "false");
        const auto player_jumping_str  = std::string("player jumping: ") + (game->get_player().jumping ? "true" : "false");
        const auto player_falling_str  = std::string("player falling: ") + (game->get_player().falling ? "true" : "false");
        const auto player_shooting_str = std::string("player shooting: ") + (game->get_player().shooting ? "true" : "false");
        const auto collide_str         = std::string("player collide: ")  + (game->get_player().collide_x ? "x " : "_ ") + (game->get_player().collide_y ? "y" : "_");

        window_surface->render_text(geometry::Position(5,  25), camera_position_str, 12, { 255u, 0u, 0u });
        window_surface->render_text(geometry::Position(5,  45), player_position_str, 12, { 255u, 0u, 0u });
        window_surface->render_text(geometry::Position(5,  65), player_velocity_str, 12, { 255u, 0u, 0u });
        window_surface->render_text(geometry::Position(5,  85), player_walking_str , 12, { 255u, 0u, 0u });
        window_surface->render_text(geometry::Position(5, 105), player_jumping_str , 12, { 255u, 0u, 0u });
        window_surface->render_text(geometry::Position(5, 125), player_falling_str , 12, { 255u, 0u, 0u });
        window_surface->render_text(geometry::Position(5, 145), player_shooting_str, 12, { 255u, 0u, 0u });
        window_surface->render_text(geometry::Position(5, 165), collide_str        , 12, { 255u, 0u, 0u });

      }

      // Update screen
      window->refresh();

      // Calculate FPS each second
      fps_num_renders++;
      if (sdl_tick >= (fps_last_calc + 1000))
      {
        const auto total_time = sdl_tick - fps_start_time;
        fps = fps_num_renders / (total_time / 1000);
        fps_last_calc = sdl_tick;

        // Reset
        fps_num_renders = 0;
        fps_start_time = sdl_tick;
      }
    }
  }

  return 0;
}
