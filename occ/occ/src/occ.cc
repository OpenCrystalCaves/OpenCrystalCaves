#include <cassert>
#include <cstdio>
#include <cstdlib>

#include <format>
#include <memory>
#include <utility>

#include "constants.h"
#include "game_renderer.h"
#include "imagemgr.h"
#include "player_state.h"
#include "spritemgr.h"
#include "state.h"
#include "utils.h"
#include <SimpleIni.h>
#include <cfgpath.h>

// From utils
#include "geometry.h"
#include "logger.h"
#include "path.h"

#define ICON_FILENAME_FMT "caves{}.ico"


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
  const auto icon_file = std::format(ICON_FILENAME_FMT, episode);
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

  // Load sound manager
  SoundManager sound_manager;
  if (!sound_manager.load_sounds(episode))
  {
    LOG_CRITICAL("Could not load sounds");
    return 1;
  }
  LOG_INFO("Sounds loaded");

  // Load config file
  // Dummy implementation: just check that it exists
  // TODO: load to PlayerState
  // PlayerState player_state(name, episode);
  char cfgfile[MAX_PATH];
  get_user_config_file(cfgfile, sizeof(cfgfile), "OpenCrystalCaves");
  if (cfgfile[0] == 0)
  {
    LOG_INFO("Could not find config file, using defaults");
  }
  else
  {
    LOG_INFO("Using config file at %s", cfgfile);
    // Write dummy file
    CSimpleIniA ini;
    ini.SetValue("dummy_section", "dummy_key", "dummy_value");
    if (ini.SaveFile(cfgfile) != SI_OK)
    {
      LOG_ERROR("Could not write to config file at %s", cfgfile);
    }
  }

  // Create Game
  std::unique_ptr<Game> game = Game::create();
  if (!game)
  {
    LOG_CRITICAL("Could not create Game");
    return 1;
  }
  ExeData exe_data{episode};
  if (!game->init(sound_manager, exe_data, LevelId::INTRO))
  {
    LOG_CRITICAL("Could not initialize Game");
    return 1;
  }
  LOG_INFO("Game initialized");

  // Create game states
  // TODO: more episodes
  auto splash_images = image_manager.get_images(1, CCImage::IMAGE_APOGEE);
  SplashState splash{sound_manager, splash_images, *window};
  auto title_images = image_manager.get_images(1, CCImage::IMAGE_TITLE);
  auto credits_images = image_manager.get_images(1, CCImage::IMAGE_CREDITS);
  title_images.insert(title_images.end(), credits_images.begin(), credits_images.end());
  TitleState title{sprite_manager, sound_manager, *game_surface, title_images, *window, exe_data};
  splash.set_next(title);
  GameState game_state(*game, sprite_manager, sound_manager, *game_surface, *window, exe_data);
  title.set_next(game_state);
  game_state.set_next(title);
  State* state = &splash;
  state->reset();

  // Game loop
  {
    // Game variables
    Input input;

    // Game loop logic
    auto sdl_tick = sdl->get_tick();
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
      while (true)
      {
        sdl_tick = sdl->get_tick();
        auto elapsed_ticks = sdl_tick - tick_last_update;
        tick_last_update = sdl_tick;
        lag += elapsed_ticks;
        if (lag >= MS_PER_TICK)
        {
          break;
        }
        sdl->delay(MS_PER_TICK - lag);
      }
      while (lag >= MS_PER_TICK)
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

        lag -= MS_PER_TICK;
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
