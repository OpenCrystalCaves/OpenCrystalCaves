/*
https://moddingwiki.shikadi.net/wiki/ProGraphx_Toolbox_tileset_format
*/
#include <filesystem>
#include <fstream>
#include <iostream>

#include <SDL_Clipboard.h>
#include <SDL_error.h>

#include "../occ/src/spritemgr.h"
#include "constants.h"
#include "event.h"
#include "graphics.h"
#include "logger.h"
#include "sdl_wrapper.h"

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
  auto window = Window::create("OpenCrystalCaves", geometry::Size(10, 10), "");
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
  const auto surface = sprite_manager.get_surface();
  window->set_size(surface->size());
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
    window->fill_rect(geometry::Rectangle(0, 0, surface->size()), {33u, 33u, 33u});
    surface->blit_surface();
    // Show hovered sprite and draw its index
    const int x = input.mouse.x() / SPRITE_W;
    const int y = input.mouse.y() / SPRITE_H;
    const geometry::Rectangle rect{{x * SPRITE_W, y * SPRITE_H}, {SPRITE_W, SPRITE_H}};
    window->render_rectangle(rect, {255, 255, 255, 255});
    const int index = x + y * SPRITE_STRIDE;
    if (input.mouse_left.pressed())
    {
      const auto s = std::to_string(index);
      if (SDL_SetClipboardText(s.c_str()) != 0)
      {
        std::cerr << "Failed to set clipboard text: " << SDL_GetError() << "\n";
      }
      else
      {
        std::cout << "Copied to clipboard: " << s << "\n";
      }
    }
    const auto tooltip = L"x: " + std::to_wstring(x) + L"\ny: " + std::to_wstring(y) + L"\nidx: " + std::to_wstring(index);
    const int tooltip_x = std::min(rect.position.x() + rect.size.x(), SPRITE_STRIDE * SPRITE_W - 80);
    const int tooltip_y = std::min(rect.position.y() + rect.size.y(), 22 * SPRITE_H - 16);
    sprite_manager.render_text(tooltip, geometry::Position{tooltip_x, tooltip_y});
    window->refresh();
    sdl->delay(10);
  }

  return 0;
}
