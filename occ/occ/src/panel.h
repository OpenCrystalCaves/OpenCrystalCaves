#pragma once

#include <array>

#include <event.h>

#include "exe_data.h"
#include "spritemgr.h"

enum class PanelText : int
{
  PANEL_TEXT_QUIT = 0,
  PANEL_TEXT_SFX_ON,
  PANEL_TEXT_END,
  PANEL_TEXT_END_1,
  PANEL_TEXT_END_2,
  PANEL_TEXT_END_3,  // order today and find out
  PANEL_TEXT_START_1,
  PANEL_TEXT_START_2,
  PANEL_TEXT_START_3,
  PANEL_TEXT_STORY_1,
  PANEL_TEXT_STORY_2,
  PANEL_TEXT_PAUSED,
  PANEL_TEXT_ORDER_1,
  PANEL_TEXT_ORDER_2,
  PANEL_TEXT_ORDER_3,
  PANEL_TEXT_HIGH_SCORES,
  PANEL_TEXT_ENTER_SCORE,
  PANEL_TEXT_INSTRUCTIONS_1,
  PANEL_TEXT_INSTRUCTIONS_2,
  PANEL_TEXT_INSTRUCTIONS_3,
  PANEL_TEXT_INSTRUCTIONS_4,
  PANEL_TEXT_INSTRUCTIONS_5,
  PANEL_TEXT_INSTRUCTIONS_6,
  PANEL_TEXT_INSTRUCTIONS_7,
  PANEL_TEXT_LEVEL_DONE,
  PANEL_TEXT_TIME_STOP,
  PANEL_TEXT_MORE_CRYSTALS,
  PANEL_TEXT_FILE_ERROR,
  PANEL_TEXT_PRESS_ANY_KEY,
  PANEL_TEXT_JOYSTICK_ON,
  PANEL_TEXT_QUIT_TO_DOS,
  PANEL_TEXT_QUIT_TO,
  PANEL_TEXT_RED_MUSHROOM,
  PANEL_TEXT_LEVER,
  PANEL_TEXT_SWITCH,
  PANEL_TEXT_P,
  PANEL_TEXT_GREEN_MUSHROOM,
  PANEL_TEXT_CHEAT_MODE,
  PANEL_TEXT_SAVED,
  PANEL_TEXT_SAVE_FROM_MAIN,
  PANEL_TEXT_HIT_AIR,
  PANEL_TEXT_RESTORE_FROM_MAIN,
  PANEL_TEXT_REDEFINE_KEYS,
  PANEL_TEXT_KEY_UNAVAILABLE,
  PANEL_TEXT_KEY_LEFT,
  PANEL_TEXT_KEY_RIGHT,
  PANEL_TEXT_KEY_JUMP,
  PANEL_TEXT_KEY_FIRE,
  PANEL_TEXT_MAIN_MENU,
  PANEL_TEXT_HELP_MENU,
  PANEL_TEXT_HIGH_SCORE_NAMES,
  PANEL_TEXT_ABOUT,
  PANEL_TEXT_REVERSE_GRAVITY,
  PANEL_TEXT_PERFECT,
  PANEL_TEXT_WARP,
  PANEL_TEXT_RESTORE,
  PANEL_TEXT_SAVE,
  PANEL_TEXT_START_SEQ_1,
  PANEL_TEXT_START_SEQ_2,
  PANEL_TEXT_START_SEQ_3,
  NUM_TEXTS,
};

// Panel text locations, by episode
// Texts are pascal strings (first byte is len)
static const std::array<std::array<int, static_cast<size_t>(PanelText::NUM_TEXTS)>, 3> PANEL_TEXT_LOCS{
  {{
     0x1327F,  // QUIT
     0x13322,  // SFX_ON
     0x133D7,  // END
     0x1361A,  // END_1
     0x139A0,  // END_2
     0x13B27,  // END_3
     0x13F29,  // START_1
     0x14063,  // START_2
     0x1411B,  // START_3
     0x14736,  // STORY_1
     0x14890,  // STORY_2
     0x14E07,  // PAUSED
     0x14ED3,  // ORDER_1
     0x15228,  // ORDER_2
     0x1558D,  // ORDER_3
     0x15A63,  // HIGH_SCORES
     0x15CB7,  // ENTER_SCORE
     0x15DF2,  // INSTRUCTIONS_1
     0x15F66,  // INSTRUCTIONS_2
     0x160B7,  // INSTRUCTIONS_3
     0x16220,  // INSTRUCTIONS_4
     0x16386,  // INSTRUCTIONS_5
     0x164B6,  // INSTRUCTIONS_6
     0x165DB,  // INSTRUCTIONS_7
     0x1757C,  // LEVEL_DONE
     0x1769F,  // TIME_STOP
     0x177F4,  // MORE_CRYSTALS
     0x17917,  // FILE_ERROR
     0x179E2,  // PRESS_ANY_KEY
     0x17A62,  // JOYSTICK_ON
     0x17B33,  // QUIT_TO_DOS
     0x17BCB,  // QUIT_TO
     0x180C8,  // RED_MUSHROOM
     0x181AC,  // LEVER
     0x18278,  // SWITCH
     0x18345,  // P
     0x18422,  // GREEN_MUSHROOM
     0x18553,  // CHEAT_MODE
     0x1863E,  // SAVED
     0x187F9,  // SAVE_FROM_MAIN
     0x18913,  // HIT_AIR
     0x18AE4,  // RESTORE_FROM_MAIN
     0x18BFD,  // REDEFINE_KEYS
     0x18DF1,  // KEY_UNAVAILABLE
     0x18EBD,  // KEY_LEFT
     0x18EFD,  // KEY_RIGHT
     0x18F19,  // KEY_JUMP
     0x18F2F,  // KEY_FIRE
     0x1925D,  // MAIN_MENU
     0x19665,  // HELP_MENU
     0x1A15C,  // HIGH_SCORE_NAMES
     0x1BA8F,  // ABOUT
     0x1BE26,  // REVERSE_GRAVITY
     0x1BFB1,  // PERFECT
     0x1C090,  // WARP
     0x1C159,  // RESTORE
     0x1C21C,  // SAVE
     0x1C606,  // START_SEQ_1
     0x1C6C8,  // START_SEQ_2
     0x1C76A,  // START_SEQ_3
   },
   {
     0x131FF,  // QUIT
     0x132A2,  // SFX_ON
     0x13357,  // END
     0x13551,  // END_1
     0x13677,  // END_2
     0x13B51,  // END_3
     0x13EAC,  // START_1
     0x1404A,  // START_2
     0x1404A,  // START_3 (TODO: ep2 has no start 3)
     0x14526,  // STORY_1
     0x14685,  // STORY_2
     0x14BBE,  // PAUSED
     0x14C8A,  // ORDER_1
     0x14FDF,  // ORDER_2
     0x15344,  // ORDER_3
     0x1581A,  // HIGH_SCORES
     0x15A6E,  // ENTER_SCORE
     0x15BA9,  // INSTRUCTIONS_1
     0x15D1D,  // INSTRUCTIONS_2
     0x15E6E,  // INSTRUCTIONS_3
     0x15FD7,  // INSTRUCTIONS_4
     0x1613D,  // INSTRUCTIONS_5
     0x1626D,  // INSTRUCTIONS_6
     0x16392,  // INSTRUCTIONS_7
     0x17333,  // LEVEL_DONE
     0x17456,  // TIME_STOP
     0x175AB,  // MORE_CRYSTALS
     0x176CE,  // FILE_ERROR
     0x17799,  // PRESS_ANY_KEY
     0x17819,  // JOYSTICK_ON
     0x178EA,  // QUIT_TO_DOS
     0x17982,  // QUIT_TO
     0x17E7F,  // RED_MUSHROOM
     0x17F63,  // LEVER
     0x1802F,  // SWITCH
     0x180FC,  // P
     0x181D9,  // GREEN_MUSHROOM
     0x1830A,  // CHEAT_MODE
     0x183F5,  // SAVED
     0x185B0,  // SAVE_FROM_MAIN
     0x186CA,  // HIT_AIR
     0x1889B,  // RESTORE_FROM_MAIN
     0x189B4,  // REDEFINE_KEYS
     0x18BA8,  // KEY_UNAVAILABLE
     0x18C74,  // KEY_LEFT
     0x18CB4,  // KEY_RIGHT
     0x18CD0,  // KEY_JUMP
     0x18CE6,  // KEY_FIRE
     0x19014,  // MAIN_MENU
     0x1941C,  // HELP_MENU
     0x19F13,  // HIGH_SCORE_NAMES
     0x1B82C,  // ABOUT
     0x1BBC3,  // REVERSE_GRAVITY
     0x1BD4E,  // PERFECT
     0x1BE2E,  // WARP
     0x1BEF7,  // RESTORE
     0x1BFBA,  // SAVE
     0x1C3A5,  // START_SEQ_1
     0x1C46F,  // START_SEQ_2
     0x1C50A,  // START_SEQ_3
   },
   {}}};

enum class PanelType
{
  PANEL_TYPE_NONE,
  PANEL_TYPE_DISABLED,
  PANEL_TYPE_NORMAL,
  PANEL_TYPE_PAGES,
  PANEL_TYPE_NEW_GAME,
  PANEL_TYPE_CONTINUE_GAME,
  PANEL_TYPE_RESTART,
  PANEL_TYPE_QUIT_TO_OS,
  PANEL_TYPE_QUIT_TO_TITLE,
  PANEL_TYPE_QUIT_TO_MAIN_LEVEL,
  PANEL_TYPE_WARP_TO_LEVEL,
  PANEL_TYPE_WEBSITE,
  PANEL_TYPE_END_GAME,
  PANEL_TYPE_EPISODE_1,
  PANEL_TYPE_EPISODE_2,
  PANEL_TYPE_EPISODE_3,
};

class Panel
{
 public:
  // Panel that pages through its children
  Panel(const std::vector<Panel> children, bool close_from_page_1 = false);
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
    : Panel(exe_data.data.c_str() + PANEL_TEXT_LOCS[exe_data.episode - 1][static_cast<int>(pt)], sprites, icons, type)
  {
  }
  // Basic panel
  Panel(const PanelType type) : type_(type), children_({}) {}

  Panel* update(const Input& input);

  void set_parent(Panel& parent) { parent_ = &parent; }

  void draw(const SpriteManager& sprite_manager, Window& window) const;

  PanelType get_type() const { return type_; }
  void set_type(PanelType t) { type_ = t; }
  const std::vector<std::wstring>& get_strings() const { return strings_; }

  void add_input(char c);
  const std::string& get_input() const { return input_str_; }
  int index() const { return index_; }
  std::vector<std::pair<int, Panel>>& get_children() { return children_; }
  Panel* get_child_by_string(const std::wstring& s);

 private:
  PanelType type_;
  std::vector<std::wstring> strings_;
  std::vector<std::pair<int, Panel>> children_;
  std::vector<std::pair<int, geometry::Position>> sprites_;
  std::vector<std::pair<Icon, geometry::Position>> icons_;
  int index_ = 0;
  bool close_from_page_1_ = false;
  geometry::Size size_;
  geometry::Position question_pos_ = {0, 0};
  geometry::Position sparkle_pos_ = {0, 0};
  unsigned ticks_ = 0;
  Panel* parent_ = nullptr;
  std::string input_str_ = "";
};
