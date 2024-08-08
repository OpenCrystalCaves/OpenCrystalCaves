/*
Display Crystal Caves levels
*/
#include <filesystem>
#include <fstream>
#include <iostream>

#include "../game/src/level.h"
#include "../game/src/level_loader.h"
#include "../occ/src/constants.h"
#include "../occ/src/spritemgr.h"
#include "../utils/export/exe_data.h"
#include "event.h"
#include "graphics.h"
#include "logger.h"
#include "sdl_wrapper.h"

static constexpr geometry::Size WIN_SIZE = geometry::Size(40 * SPRITE_W, 25 * SPRITE_H);

int main(int argc, char* argv[])
{
  int episode = 1;
  if (argc > 1)
  {
    episode = atoi(argv[1]);
  }
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
  auto window = Window::create("Level Viewer", WIN_SIZE, "");
  if (!window)
  {
    LOG_CRITICAL("Could not create Window");
    return 1;
  }
  SpriteManager sprite_manager;
  if (!sprite_manager.load_tilesets(*window, episode))
  {
    LOG_CRITICAL("Could not load tilesets");
    return 1;
  }
  ExeData exe_data{episode};
  std::vector<std::unique_ptr<Level>> levels;
  for (int level_id = static_cast<int>(LevelId::INTRO); level_id <= static_cast<int>(LevelId::LEVEL_16); level_id++)
  {
    auto l = LevelLoader::load(exe_data, static_cast<LevelId>(level_id));
    levels.emplace_back(std::move(l));
  }
  int index = 0;
  auto event = Event::create();
  if (!event)
  {
    LOG_CRITICAL("Could not create event handler");
    return 1;
  }

  Input input;
  while (true)
  {
    event->poll_event(&input);
    if (input.escape.pressed())
    {
      break;
    }
    if (input.left.pressed() || input.up.pressed())
    {
      index--;
      if (index < 0)
      {
        index = (int)levels.size() - 1;
      }
    }
    else if (input.right.pressed() || input.down.pressed())
    {
      index++;
      if (index == (int)levels.size())
      {
        index = 0;
      }
    }
    const auto& level = levels[index];

    window->fill_rect(geometry::Rectangle(0, 0, WIN_SIZE), {33u, 33u, 33u});
    for (int y = 0; y < level->height; y++)
    {
      for (int x = 0; x < level->width; x++)
      {
        const auto bg_id = level->get_bg(x, y);
        if (bg_id != -1)
        {
          sprite_manager.render_tile(bg_id, {x * SPRITE_W, y * SPRITE_H});
        }

        const auto tile = level->get_tile(x, y);
        const auto sprite_id = tile.get_sprite();
        sprite_manager.render_tile(sprite_id, {x * SPRITE_W, y * SPRITE_H});
        for (const auto& enemy : level->enemies)
        {
          sprite_manager.render_tile(static_cast<int>(enemy->get_sprite()), enemy->position);
        }
        for (const auto& hazard : level->hazards)
        {
          sprite_manager.render_tile(static_cast<int>(hazard->get_sprite()), hazard->position);
        }
        for (const auto& platform : level->moving_platforms)
        {
          sprite_manager.render_tile(platform.sprite_id, platform.position);
        }
        for (const auto& entrance : level->entrances)
        {
          sprite_manager.render_tile(entrance.get_sprite(), entrance.position);
        }
        sprite_manager.render_tile(static_cast<int>(Sprite::SPRITE_STANDING_RIGHT), level->player_spawn);
        const auto& item = level->get_item(x, y);
        if (item.valid())
        {
          sprite_manager.render_tile(static_cast<int>(item.get_sprite()), {x * SPRITE_W, y * SPRITE_H});
        }
      }
    }
    window->refresh();
    sdl->delay(30);
  }

  return 0;
}
