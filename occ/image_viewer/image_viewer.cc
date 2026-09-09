/*
https://moddingwiki.shikadi.net/wiki/Crystal_Caves
*/
#include <filesystem>
#include <fstream>
#include <iostream>

#include "../occ/src/imagemgr.h"
#include "event.h"
#include "graphics.h"
#include "logger.h"
#include "sdl_wrapper.h"

int main(int argc, char* argv[])
{
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
  int episode = 1;
  if (argc > 1)
  {
    episode = atoi(argv[1]);
  }
  auto window = Window::create("Crystal Caves Image Viewer", geometry::Size(10, 10), "");
  if (!window)
  {
    LOG_CRITICAL("Could not create Window");
    return 1;
  }
  ImageManager image_manager;
  if (!image_manager.load_images(*window))
  {
    LOG_CRITICAL("Could not load images");
    return 1;
  }
  int image = (int)IMAGE_APOGEE;
  auto surfaces = image_manager.get_images(episode, (CCImage)image);
  int index = 0;
  window->set_size(surfaces[index]->size());
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
        image--;
        if (image < 0)
        {
          image = (int)IMAGE_END;
          episode--;
          if (episode < 1)
          {
            episode = (int)image_manager.number_of_episodes();
          }
        }
        surfaces = image_manager.get_images(episode, (CCImage)image);
        index = (int)surfaces.size() - 1;
      }
    }
    else if (input.right.pressed() || input.down.pressed())
    {
      index++;
      if (index == (int)surfaces.size())
      {
        image++;
        if (image > (int)IMAGE_END)
        {
          image = (int)IMAGE_APOGEE;
          episode++;
          if (episode > (int)image_manager.number_of_episodes())
          {
            episode = 1;
          }
        }
        surfaces = image_manager.get_images(episode, (CCImage)image);
        index = 0;
      }
    }
    const auto surface = surfaces[index];
    window->fill_rect(geometry::Rectangle(0, 0, surface->size()), {33u, 33u, 33u});
    surface->blit_surface();
    window->refresh();
  }

  return 0;
}
