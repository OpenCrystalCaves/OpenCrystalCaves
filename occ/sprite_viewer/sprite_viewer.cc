/*
https://moddingwiki.shikadi.net/wiki/ProGraphx_Toolbox_tileset_format
*/
#include <filesystem>
#include <fstream>
#include <iostream>

#include <SDL.h>

#include "../occ/src/spritemgr.h"
#include "constants.h"
#include "event.h"
#include "graphics.h"
#include "logger.h"
#include "sdl_wrapper.h"
#include "sprite.h"

void draw(Window& window, const Surface& surface, const int page_index, int x, int y, const Input& input, SpriteManager& sprite_manager)
{
  window.fill_rect(geometry::Rectangle(0, 0, geometry::Size{surface.width(), SPRITE_ROWS * SPRITE_H}), {33u, 33u, 33u});
  surface.blit_surface({0, page_index * SPRITE_ROWS * SPRITE_H, surface.width(), SPRITE_ROWS * SPRITE_H},
                       {0, 0, surface.width(), SPRITE_ROWS * SPRITE_H});
  // Show hovered sprite and draw its index
  const geometry::Rectangle rect{{x * SPRITE_W, y * SPRITE_H}, {SPRITE_W, SPRITE_H}};
  window.render_rectangle(rect, {255, 255, 255, 255});
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
  window.refresh();
}

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
  auto window = Window::create("OpenCrystalCaves", geometry::Size(SPRITE_STRIDE * SPRITE_W, SPRITE_ROWS * SPRITE_H), "");
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
  const int pages = surface->height() / (SPRITE_H * SPRITE_ROWS);
  auto event = Event::create();
  if (!event)
  {
    LOG_CRITICAL("Could not create event handler");
    return 1;
  }

  Input input;
  int index = 0;
  int last_index = -1;
  int last_mx = -1;
  int last_my = -1;
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
        index = pages - 1;
      }
    }
    else if (input.right.pressed() || input.down.pressed())
    {
      index++;
      if (index == pages)
      {
        index = 0;
      }
    }

    const int mx = input.mouse.x() / SPRITE_W;
    const int my = input.mouse.y() / SPRITE_H;
    if (mx != last_mx || my != last_my || index != last_index)
    {
      draw(*window, *surface, index, mx, my, input, sprite_manager);
      last_mx = mx;
      last_my = my;
      last_index = index;
    }
    sdl->delay(10);
  }

  return 0;
}
