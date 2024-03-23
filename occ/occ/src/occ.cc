#include <cassert>
#include <cstdio>
#include <cstdlib>

#include <memory>
#include <utility>

#include "constants.h"
#include "game_renderer.h"
#include "imagemgr.h"
#include "spritemgr.h"
#include "state.h"

// From utils
#include "geometry.h"
#include "logger.h"
#include "path.h"

#define ICON_FILENAME_FMT "caves%d.ico"


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

  // TODO: select episode
  const int episode = 1;

  // Create Window
  const auto icon_file = misc::string_format(ICON_FILENAME_FMT, episode);
  const auto icon_path = get_data_path(icon_file);
  if (icon_path.empty())
  {
    LOG_ERROR("could not find icon file %s", icon_file.c_str());
  }
  auto window = Window::create("OpenCrystalCaves", WINDOW_SIZE, icon_path);
  if (!window)
  {
    LOG_CRITICAL("Could not create Window");
    return 1;
  }
  LOG_INFO("Window created");

  // Create game surface
  auto game_surface = window->create_target_surface(CAMERA_SIZE);
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
  if (!sprite_manager.load_tilesets(*window, episode))
  {
    LOG_CRITICAL("Could not load tilesets");
    return 1;
  }
  LOG_INFO("Tileset loaded");

  // Load image manager
  ImageManager image_manager;
  if (!image_manager.load_images(*window))
  {
    LOG_CRITICAL("Could not load images");
    return 1;
  }
  LOG_INFO("Images loaded");

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

  // Create game states
  // TODO: more episodes
  auto splash_images = image_manager.get_images(1, CCImage::IMAGE_APOGEE);
  SplashState splash{splash_images, *window};
  ExeData exe_data{episode};
  auto title_images = image_manager.get_images(1, CCImage::IMAGE_TITLE);
  auto credits_images = image_manager.get_images(1, CCImage::IMAGE_CREDITS);
  title_images.insert(title_images.end(), credits_images.begin(), credits_images.end());
  TitleState title{sprite_manager, *game_surface, title_images, *window, exe_data};
  splash.set_next(title);
  GameState game_state(*game, sprite_manager, *game_surface, *window);
  title.set_next(game_state);
  State* state = &splash;

  // Game loop
  {
    // Game variables
    Input input;

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
        state->update(input);
        auto new_state = state->next_state();
        if (new_state != state)
        {
          if (new_state == nullptr)
          {
            return 0;
          }
          new_state->reset();
          state = new_state;
        }

        lag -= ms_per_update;
      }

      /////////////////////////////////////////////////////////////////////////
      ///
      ///  Render
      ///
      /////////////////////////////////////////////////////////////////////////

      state->draw(*window);

      // Render FPS
      auto fps_str = L"fps: " + std::to_wstring(fps);
      sprite_manager.render_text(fps_str, geometry::Position(5, 5));

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
