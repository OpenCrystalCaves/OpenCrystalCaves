/*
https://moddingwiki.shikadi.net/wiki/Crystal_Caves_Sound_format
*/
#include <iostream>
#include <string>
#include <vector>

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_mixer.h>

#include "../occ/src/soundmgr.h"
#include "graphics.h"
#include "logger.h"
#include "sdl_wrapper.h"

char itoc(int i)
{
  if (i < 10)
  {
    return (char)('0' + i);
  }
  i -= 10;
  if (i < 26)
  {
    return (char)('a' + i);
  }
  i -= 26;
  if (i < 26)
  {
    return (char)('A' + i);
  }
  return '?';
}
int keytoi(const SDL_Keycode c, const bool shift)
{
  int d = 0;
  if (c >= SDLK_0 && c <= SDLK_9)
  {
    return c - SDLK_0 + d;
  }
  d += 10;
  if (shift)
  {
    d += 26;
  }
  if (c >= SDLK_a && c <= SDLK_z)
  {
    return c - SDLK_a + d;
  }
  return -1;
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
  auto window = Window::create("Crystal Caves Sound Player", geometry::Size(10, 10), "");
  if (!window)
  {
    LOG_CRITICAL("Could not create Window");
    return 1;
  }
  window->set_size(geometry::Size{200, 100});

  SoundManager soundmgr;
  if (!soundmgr.load_sounds(episode))
  {
    std::cout << "Could not load sounds\n";
    return 1;
  }
  std::cout << "Play sound by pressing 0 - " << itoc((int)soundmgr.size() - 1) << "\n";

  bool quit = false;
  bool keydown = false;
  while (!quit)
  {
    SDL_Event event;
    while (SDL_PollEvent(&event) != 0)
    {
      if (event.type == SDL_QUIT)
      {
        quit = true;
      }
      else if (!keydown && event.type == SDL_KEYDOWN)
      {
        if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
        {
          quit = true;
        }
        keydown = true;
        const bool shift = !!(SDL_GetModState() & KMOD_SHIFT);
        const int index = keytoi(event.key.keysym.sym, shift);
        if (index >= 0 && index < (int)soundmgr.size())
        {
          soundmgr.play_sound(static_cast<SoundType>(index));
        }
      }
      else if (event.type == SDL_KEYUP)
      {
        keydown = false;
      }
    }
    sdl->delay(10);
  }

  return 0;
}
