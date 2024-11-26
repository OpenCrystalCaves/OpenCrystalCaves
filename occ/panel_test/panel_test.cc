/*
Display Crystal Caves window panels
*/
#include <filesystem>
#include <fstream>
#include <iostream>

#include "../occ/src/panel.h"
#include "../occ/src/spritemgr.h"
#include "constants.h"
#include "event.h"
#include "graphics.h"
#include "logger.h"
#include "sdl_wrapper.h"
#include "utils.h"

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
  auto window = Window::create("OpenCrystalCaves", SCREEN_SIZE, "");
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
  std::vector<Panel> panels = {
    {std::vector<std::wstring>({
       L"Welcome to Crystal Caves!",
       L"Press left and right to scroll",
       L" through the text panels... ^",
     }),
     {}},
    {PanelText::PANEL_TEXT_SFX_ON, exe_data},
    {PanelText::PANEL_TEXT_END, exe_data},
    {PanelText::PANEL_TEXT_END_1, exe_data},
    {PanelText::PANEL_TEXT_END_2, exe_data},
    {PanelText::PANEL_TEXT_END_3, exe_data},
    {PanelText::PANEL_TEXT_START_1, exe_data},
    {PanelText::PANEL_TEXT_START_2, exe_data},
    {PanelText::PANEL_TEXT_START_3, exe_data},
    {PanelText::PANEL_TEXT_STORY_1, exe_data},
    {PanelText::PANEL_TEXT_STORY_2, exe_data},
    {PanelText::PANEL_TEXT_PAUSED, exe_data},
    {PanelText::PANEL_TEXT_ORDER_1, exe_data},
    {PanelText::PANEL_TEXT_ORDER_2, exe_data},
    {PanelText::PANEL_TEXT_ORDER_3, exe_data},
    {PanelText::PANEL_TEXT_HIGH_SCORES, exe_data},
    {PanelText::PANEL_TEXT_ENTER_SCORE, exe_data},
    {PanelText::PANEL_TEXT_INSTRUCTIONS_1, exe_data},
    {PanelText::PANEL_TEXT_INSTRUCTIONS_2, exe_data},
    {PanelText::PANEL_TEXT_INSTRUCTIONS_3, exe_data},
    {PanelText::PANEL_TEXT_INSTRUCTIONS_4, exe_data},
    {PanelText::PANEL_TEXT_INSTRUCTIONS_5, exe_data},
    {PanelText::PANEL_TEXT_INSTRUCTIONS_6, exe_data},
    {PanelText::PANEL_TEXT_INSTRUCTIONS_7, exe_data},
    {PanelText::PANEL_TEXT_LEVEL_DONE, exe_data},
    {PanelText::PANEL_TEXT_TIME_STOP, exe_data},
    {PanelText::PANEL_TEXT_MORE_CRYSTALS, exe_data},
    {PanelText::PANEL_TEXT_FILE_ERROR, exe_data},
    {PanelText::PANEL_TEXT_PRESS_ANY_KEY, exe_data},
    {PanelText::PANEL_TEXT_JOYSTICK_ON, exe_data},
    {PanelText::PANEL_TEXT_QUIT_TO_DOS, exe_data},
    {PanelText::PANEL_TEXT_QUIT_TO, exe_data},
    {PanelText::PANEL_TEXT_JOY_ZERO, exe_data},
    {PanelText::PANEL_TEXT_RED_MUSHROOM, exe_data},
    {PanelText::PANEL_TEXT_LEVER, exe_data},
    {PanelText::PANEL_TEXT_SWITCH, exe_data},
    {PanelText::PANEL_TEXT_P, exe_data},
    {PanelText::PANEL_TEXT_GREEN_MUSHROOM, exe_data},
    {PanelText::PANEL_TEXT_CHEAT_MODE, exe_data},
    {PanelText::PANEL_TEXT_SAVED, exe_data},
    {PanelText::PANEL_TEXT_SAVE_FROM_MAIN, exe_data},
    {PanelText::PANEL_TEXT_HIT_AIR, exe_data},
    {PanelText::PANEL_TEXT_RESTORE_FROM_MAIN, exe_data},
    {PanelText::PANEL_TEXT_REDEFINE_KEYS, exe_data},
    {PanelText::PANEL_TEXT_KEY_UNAVAILABLE, exe_data},
    {PanelText::PANEL_TEXT_KEY_LEFT, exe_data},
    {PanelText::PANEL_TEXT_KEY_RIGHT, exe_data},
    {PanelText::PANEL_TEXT_KEY_JUMP, exe_data},
    {PanelText::PANEL_TEXT_KEY_FIRE, exe_data},
    {PanelText::PANEL_TEXT_MAIN_MENU, exe_data},
    {PanelText::PANEL_TEXT_HELP_MENU, exe_data},
    {PanelText::PANEL_TEXT_HIGH_SCORE_NAMES, exe_data},
    {PanelText::PANEL_TEXT_FOREIGN_ORDERS_1, exe_data},
    {PanelText::PANEL_TEXT_FOREIGN_ORDERS_2, exe_data},
    {PanelText::PANEL_TEXT_FOREIGN_ORDERS_3, exe_data},
    {PanelText::PANEL_TEXT_FOREIGN_ORDERS_4, exe_data},
    {PanelText::PANEL_TEXT_BBS, exe_data},
    {PanelText::PANEL_TEXT_ABOUT, exe_data},
    {PanelText::PANEL_TEXT_REVERSE_GRAVITY, exe_data},
    {PanelText::PANEL_TEXT_PERFECT, exe_data},
    {PanelText::PANEL_TEXT_WARP, exe_data},
    {PanelText::PANEL_TEXT_RESTORE, exe_data},
    {PanelText::PANEL_TEXT_SAVE, exe_data},
    {PanelText::PANEL_TEXT_VIDEO_COMPAT, exe_data},
    {PanelText::PANEL_TEXT_START_SEQ_1, exe_data},
    {PanelText::PANEL_TEXT_START_SEQ_2, exe_data},
    {PanelText::PANEL_TEXT_START_SEQ_3, exe_data},
  };
  int index = 0;
  auto event = Event::create();
  if (!event)
  {
    LOG_CRITICAL("Could not create event handler");
    return 1;
  }

  auto sdl_tick = sdl->get_tick();
  const auto ms_per_update = 57;  // 17.5~ ticks per second
  auto tick_last_update = sdl_tick;
  auto lag = 0u;
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
        index = (int)panels.size() - 1;
      }
    }
    else if (input.right.pressed() || input.down.pressed())
    {
      index++;
      if (index == (int)panels.size())
      {
        index = 0;
      }
    }
    sdl_tick = sdl->get_tick();
    auto elapsed_ticks = sdl_tick - tick_last_update;
    tick_last_update = sdl_tick;
    lag += elapsed_ticks;
    while (lag >= ms_per_update)
    {
      panels[index].update(input);

      lag -= ms_per_update;
    }

    window->fill_rect(geometry::Rectangle(0, 0, SCREEN_SIZE), {33u, 33u, 33u});
    panels[index].draw(sprite_manager);
    window->refresh();
    sdl->delay(10);
  }

  return 0;
}
