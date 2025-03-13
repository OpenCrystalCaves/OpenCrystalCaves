#pragma once

#include <event.h>

#include "exe_data.h"
#include "spritemgr.h"

// Episode 1 panel text offsets
// TODO: other episodes
enum class PanelText : int
{
  PANEL_TEXT_QUIT = 0x1327F,
  PANEL_TEXT_SFX_ON = 0x13322,
  PANEL_TEXT_END = 0x133D7,
  PANEL_TEXT_END_1 = 0x1361A,
  PANEL_TEXT_END_2 = 0x139A0,
  PANEL_TEXT_END_3 = 0x13B27,  // order today and find out
  PANEL_TEXT_START_1 = 0x13F29,
  PANEL_TEXT_START_2 = 0x14063,
  PANEL_TEXT_START_3 = 0x1411B,
  PANEL_TEXT_STORY_1 = 0x14736,
  PANEL_TEXT_STORY_2 = 0x14890,
  PANEL_TEXT_PAUSED = 0x14E07,
  PANEL_TEXT_ORDER_1 = 0x14ED3,
  PANEL_TEXT_ORDER_2 = 0x15228,
  PANEL_TEXT_ORDER_3 = 0x1558D,
  PANEL_TEXT_HIGH_SCORES = 0x15A63,
  PANEL_TEXT_ENTER_SCORE = 0x15CB7,
  PANEL_TEXT_INSTRUCTIONS_1 = 0x15DF2,
  PANEL_TEXT_INSTRUCTIONS_2 = 0x15F66,
  PANEL_TEXT_INSTRUCTIONS_3 = 0x160B7,
  PANEL_TEXT_INSTRUCTIONS_4 = 0x16220,
  PANEL_TEXT_INSTRUCTIONS_5 = 0x16386,
  PANEL_TEXT_INSTRUCTIONS_6 = 0x164B6,
  PANEL_TEXT_INSTRUCTIONS_7 = 0x165DB,
  PANEL_TEXT_LEVEL_DONE = 0x1757C,
  PANEL_TEXT_TIME_STOP = 0x1769F,
  PANEL_TEXT_MORE_CRYSTALS = 0x177F4,
  PANEL_TEXT_FILE_ERROR = 0x17917,
  PANEL_TEXT_PRESS_ANY_KEY = 0x179E2,
  PANEL_TEXT_JOYSTICK_ON = 0x17A62,
  PANEL_TEXT_QUIT_TO_DOS = 0x17B33,
  PANEL_TEXT_QUIT_TO = 0x17BCB,
  PANEL_TEXT_JOY_ZERO = 0x17D65,
  PANEL_TEXT_RED_MUSHROOM = 0x180C8,
  PANEL_TEXT_LEVER = 0x181AC,
  PANEL_TEXT_SWITCH = 0x18278,
  PANEL_TEXT_P = 0x18345,
  PANEL_TEXT_GREEN_MUSHROOM = 0x18422,
  PANEL_TEXT_CHEAT_MODE = 0x18553,
  PANEL_TEXT_SAVED = 0x1863E,
  PANEL_TEXT_SAVE_FROM_MAIN = 0x187F9,
  PANEL_TEXT_HIT_AIR = 0x18913,
  PANEL_TEXT_RESTORE_FROM_MAIN = 0x18AE4,
  PANEL_TEXT_REDEFINE_KEYS = 0x18BFD,
  PANEL_TEXT_KEY_UNAVAILABLE = 0x18DF1,
  PANEL_TEXT_KEY_LEFT = 0x18EBD,
  PANEL_TEXT_KEY_RIGHT = 0x18EFD,
  PANEL_TEXT_KEY_JUMP = 0x18F19,
  PANEL_TEXT_KEY_FIRE = 0x18F2F,
  PANEL_TEXT_MAIN_MENU = 0x1925D,
  PANEL_TEXT_HELP_MENU = 0x19665,
  PANEL_TEXT_HIGH_SCORE_NAMES = 0x1A15C,
  PANEL_TEXT_FOREIGN_ORDERS_1 = 0x1AAB1,
  PANEL_TEXT_FOREIGN_ORDERS_2 = 0x1AC2A,
  PANEL_TEXT_FOREIGN_ORDERS_3 = 0x1AD28,
  PANEL_TEXT_FOREIGN_ORDERS_4 = 0x1AE2D,
  PANEL_TEXT_BBS = 0x1B710,
  PANEL_TEXT_ABOUT = 0x1BA8F,
  PANEL_TEXT_REVERSE_GRAVITY = 0x1BE26,
  PANEL_TEXT_PERFECT = 0x1BFB1,
  PANEL_TEXT_WARP = 0x1C090,
  PANEL_TEXT_RESTORE = 0x1C159,
  PANEL_TEXT_SAVE = 0x1C21C,
  PANEL_TEXT_VIDEO_COMPAT = 0x1C2D8,
  PANEL_TEXT_START_SEQ_1 = 0x1C606,
  PANEL_TEXT_START_SEQ_2 = 0x1C6C8,
  PANEL_TEXT_START_SEQ_3 = 0x1C76A,
};

enum class PanelType
{
  PANEL_TYPE_NONE,
  PANEL_TYPE_DISABLED,
  PANEL_TYPE_NORMAL,
  PANEL_TYPE_PAGES,
  PANEL_TYPE_NEW_GAME,
  PANEL_TYPE_RESTART,
  PANEL_TYPE_QUIT_TO_OS,
  PANEL_TYPE_QUIT_TO_TITLE,
  PANEL_TYPE_QUIT_TO_MAIN_LEVEL,
  PANEL_TYPE_WARP_TO_LEVEL,
  PANEL_TYPE_WEBSITE,
};

class Panel
{
 public:
  // Panel that pages through its children
  Panel(const std::vector<Panel> children);
  // Menu-type panel with selectable children
  Panel(const std::vector<std::wstring> strings,
        const std::vector<std::pair<int, Panel>> children,
        const PanelType type = PanelType::PANEL_TYPE_NORMAL);
  // Panel from decoded text from the EXE
  Panel(const char* ucsd,
        const std::vector<std::pair<int, geometry::Position>> sprites = {},
        const std::vector<std::pair<Icon, geometry::Position>> icons = {},
        const PanelType type = PanelType::PANEL_TYPE_NORMAL);
  // Panel from EXE text
  Panel(const PanelText pt,
        const ExeData& exe_data,
        const std::vector<std::pair<int, geometry::Position>> sprites = {},
        const std::vector<std::pair<Icon, geometry::Position>> icons = {},
        const PanelType type = PanelType::PANEL_TYPE_NORMAL)
    : Panel(exe_data.data.c_str() + static_cast<int>(pt), sprites, icons, type)
  {
  }
  // Basic panel
  Panel(const PanelType type) : type_(type), children_({}) {}

  Panel* update(const Input& input);

  void set_parent(Panel& parent) { parent_ = &parent; }

  void draw(const SpriteManager& sprite_manager) const;

  PanelType get_type() const { return type_; }

  void add_input(char c);
  const std::string& get_input() const { return input_str_; }
  const int index() const { return index_; }

 private:
  PanelType type_;
  std::vector<std::wstring> strings_;
  std::vector<std::pair<int, Panel>> children_;
  std::vector<std::pair<int, geometry::Position>> sprites_;
  std::vector<std::pair<Icon, geometry::Position>> icons_;
  int index_ = 0;
  geometry::Size size_;
  geometry::Position question_pos_ = {0, 0};
  geometry::Position sparkle_pos_ = {0, 0};
  unsigned ticks_ = 0;
  Panel* parent_ = nullptr;
  std::string input_str_ = "";
};
