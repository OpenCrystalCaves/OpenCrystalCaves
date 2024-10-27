#include "panel.h"

#include <codecvt>
#include <string_view>

#include "constants.h"
#include "event.h"
#include "misc.h"
#include "utils.h"

constexpr Icon S_ICONS[]{Icon::ICON_SPARKLE_1, Icon::ICON_SPARKLE_2, Icon::ICON_SPARKLE_3, Icon::ICON_SPARKLE_4};

// https://stackoverflow.com/a/42844629/2038264

static bool ends_with(std::string_view str, std::string_view suffix)
{
  return str.size() >= suffix.size() && str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

/*
static bool starts_with(std::string_view str, std::string_view prefix)
{
  return str.size() >= prefix.size() && str.compare(0, prefix.size(), prefix) == 0;
}
*/

// https://stackoverflow.com/a/25829233/2038264
inline std::string& ltrim(std::string& s, const char* t = " \t\n\r\f\v")
{
  s.erase(0, s.find_first_not_of(t));
  return s;
}

constexpr size_t len_limit = 33;

Panel::Panel(const std::vector<Panel> children) : type_(PanelType::PANEL_TYPE_PAGES)
{
  int i = 0;
  for (auto child : children)
  {
    if (this->parent_)
    {
      child.set_parent(*this->parent_);
    }
    children_.push_back({i, std::move(child)});
    i++;
  }
}


Panel::Panel(const std::vector<std::wstring> strings, const std::vector<std::pair<int, Panel>> children, const PanelType type)
  : type_(type),
    strings_(std::move(strings)),
    children_(std::move(children))
{
  for (auto& child : children_)
  {
    child.second.set_parent(*this);
  }
  size_t max_len = 0;
  for (const auto& s : strings_)
  {
    max_len = std::max(s.length(), max_len);
    // Find the spinning question mark
    const auto question = s.find_first_of(L"^");
    if (question != std::string::npos)
    {
      question_pos_ = geometry::Position((static_cast<int>(question) + 2) * CHAR_W, (static_cast<int>(strings_.size()) + 1) * CHAR_H);
    }
  }
  max_len = std::min(max_len, len_limit);
  size_ = geometry::Position(static_cast<int>(max_len) + 1, static_cast<int>(strings_.size()) + 1);
}

Panel::Panel(const char* ucsd,
             const std::vector<std::pair<int, geometry::Position>> sprites,
             const std::vector<std::pair<Icon, geometry::Position>> icons,
             const PanelType type)
  : type_(type),
    sprites_(std::move(sprites)),
    icons_(std::move(icons))
{
  // Convert input text into a vector of strings, ignoring the
  // END OF WINDOW text
  // Also find the max string size to determine window size
  std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
  size_t max_len = 0;
  const char* p_ucsd = ucsd;
  while (true)
  {
    const size_t len = *p_ucsd;
    max_len = std::max(len, max_len);
    p_ucsd++;
    std::string s(p_ucsd, len);
    if (len == 85 || s == "END OF WINDOW")
    {
      break;
    }
    // Add a paragraph break if the line is longer than the limit
    if (len > len_limit)
    {
      strings_.push_back(L"");
    }
    // If too long, break the string
    while (s.size() > len_limit)
    {
      strings_.push_back(converter.from_bytes(s.substr(0, len_limit)));
      s = s.substr(len_limit);
    }
    // Find the spinning question mark
    const auto question = s.find_first_of("^");
    if (question != std::string::npos)
    {
      question_pos_ = geometry::Position((static_cast<int>(question) + 2) * CHAR_W, (static_cast<int>(strings_.size()) + 2) * CHAR_H);
    }
    // Replace some text
    if (ends_with(s, "for only $30, plus $3 shipping."))
    {
      strings_.push_back(L" from Steam or GOG.");
      strings_.push_back(L"");
    }
    else if (ends_with(s, "ll encounter many dangerous"))
    {
      strings_.push_back(L"         INSTRUCTIONS");
      strings_.push_back(L"");
      strings_.push_back(converter.from_bytes(s));
    }
    else if (ends_with(s, "changed by pressing F1 during"))
    {
      // TODO: allow remapping controls
      strings_.push_back(L"customized in a future release.");
    }
    else if (ends_with(s, "the game.  The defaults are:"))
    {
      // TODO: allow remapping controls
      strings_.push_back(L"They are:");
      strings_.push_back(L"");
    }
    else if (ends_with(s, "Jump        - Ctrl"))
    {
      // TODO: allow remapping controls
      strings_.push_back(L"Jump        - Z");
    }
    else if (ends_with(s, "Fire Pistol - Alt"))
    {
      // TODO: allow remapping controls
      strings_.push_back(L"Fire Pistol - X");
    }
    else if (ends_with(s, "Action Key  - Alt"))
    {
      // TODO: allow remapping controls
      strings_.push_back(L"Action Key  - X");
      strings_.push_back(L"");
    }
    // Trim some space to allow room for sprites
    else if (ends_with(s, "collect all the crystals in") || ends_with(s, "each cave you encounter. Once") ||
             ends_with(s, "Clear colored blocks can be") || ends_with(s, "shot away. Look for these, as"))
    {
      // TODO: allow remapping controls
      strings_.push_back(converter.from_bytes(ltrim(s)));
    }
    // Add extra spaces to allow room for sprites
    else if (ends_with(s, "you may exit the"))
    {
      strings_.push_back(L"accomplished, you may exit");
    }
    else if (ends_with(s, "cave through the main airlock."))
    {
      strings_.push_back(L"the cave through the main");
      strings_.push_back(L"airlock.");
      strings_.push_back(L"");
    }
    else
    {
      strings_.push_back(converter.from_bytes(s));
    }
    // Add a paragraph break after some special lines - there doesn't seem to be a consistent pattern here
    if (  // About
      ends_with(s, "About Apogee Software") || ends_with(s, "think our goal can be achieved.") ||
      ends_with(s, "Shareware is not free software.") || ends_with(s, "Your honesty pays...") ||
      // Ordering info
      ends_with(s, "HOW TO ORDER CRYSTAL CAVES!") || ends_with(s, "original and incredible episodes:") ||
      ends_with(s, "3) Mylo vs. the Supernova") || ends_with(s, "levels, and all-new challenges.") ||
      // Instructions
      ends_with(s, "lead to the inner caves.") || ends_with(s, "in any order you choose.") || ends_with(s, "ll find.") ||
      ends_with(s, "S HEALTH") || ends_with(s, "the level from the beginning.") || ends_with(s, "TIMED EVENTS") ||
      ends_with(s, "bottom of the screen.") || ends_with(s, "PLAYER CONTROLS") || ends_with(s, "HINTS AND SECRETS") ||
      ends_with(s, "platforms to find hidden gems.") || ends_with(s, "that allows you to defeat them.") ||
      ends_with(s, "have infinite men in this game.") || ends_with(s, "HINTS AND SECRETS ") ||
      ends_with(s, "sometimes they block your path.") || ends_with(s, "caves have reversed gravity!") ||
      ends_with(s, "by jumping over creatures.") ||
      // Story
      ends_with(s, "-----") || ends_with(s, "981,231,783,813,651") || ends_with(s, "OCCUPATION:  Space Trader") ||
      ends_with(s, "Yorp herding business.") || ends_with(s, "continued...") || ends_with(s, "ahead of his creditors.") ||
      ends_with(s, "of Ghoulbone IV.") || ends_with(s, "- END OF ENTRY -"))
    {
      strings_.push_back(L"");
    }
    else if (ends_with(s, "with the matching color.") || ends_with(s, "status line.") || ends_with(s, "asterisk characters, i.e. *12*.") ||
             ends_with(s, "you are in front of them.") || ends_with(s, "what they do.") || ends_with(s, "have different effects on Mylo."))
    {
      strings_.push_back(L"");
      question_pos_ = geometry::Position((22 + 2) * CHAR_W, (static_cast<int>(strings_.size()) + 2) * CHAR_H);
      strings_.push_back(L"        Press Any Key ^");
      break;
    }
    // End immediately on some special lines
    if (ends_with(s, "^"))
    {
      break;
    }
    p_ucsd += len;
  }
  max_len = std::min(max_len, len_limit);
  size_ = geometry::Position(static_cast<int>(max_len) + 1, static_cast<int>(strings_.size()) + 1);
}

Panel* Panel::update(const Input& input)
{
  auto next = this;
  const auto pinput = input_to_player_input(input);
  if (type_ == PanelType::PANEL_TYPE_PAGES)
  {
    // Update child but ignore the input result
    children_[index_].second.update(input);
    if (pinput.up_pressed || input.up.pressed() || pinput.left_pressed || input.left.pressed())
    {
      index_--;
      if (index_ < 0)
      {
        next = parent_;
      }
    }
    else if (pinput.down_pressed || input.down.pressed() || pinput.right_pressed || input.right.pressed() || pinput.jump_pressed ||
             pinput.shoot_pressed || input.enter.pressed())
    {
      index_++;
      if (index_ == (int)children_.size())
      {
        next = parent_;
      }
    }
    else if (input.escape.pressed())
    {
      // hide panel / go back using escape
      next = parent_;
    }
    if (next == parent_)
    {
      index_ = 0;
    }
    return next;
  }
  ticks_ += 1;

  if (size_ != geometry::Size())
  {
    // Randomly switch the sparkle position around
    const auto sparkle_frame = (ticks_ / 3) % (std::size(S_ICONS) + 1);
    if (sparkle_frame == 0)
    {
      if (misc::random<int>(0, 1) == 0)
      {
        // Put the sparkle on the top edge
        sparkle_pos_ = geometry::Position(misc::random<int>(1, size_.x()) * CHAR_W, 0);
      }
      else
      {
        // Put the sparkle on the left edge
        sparkle_pos_ = geometry::Position(0, misc::random<int>(1, size_.y()) * CHAR_H);
      }
    }
  }

  if (!children_.empty())
  {
    const auto index_start = index_;
    if (pinput.up_pressed || input.up.pressed())
    {
      do
      {
        index_--;
        if (index_ < 0)
        {
          index_ = (int)children_.size() - 1;
        }
      } while (index_ != index_start &&
               (children_[index_].second.get_type() == PanelType::PANEL_TYPE_NONE ||
                children_[index_].second.get_type() == PanelType::PANEL_TYPE_DISABLED));
    }
    else if (pinput.down_pressed || input.down.pressed())
    {
      do
      {
        index_++;
        if (index_ == (int)children_.size())
        {
          index_ = 0;
        }
      } while (index_ != index_start &&
               (children_[index_].second.get_type() == PanelType::PANEL_TYPE_NONE ||
                children_[index_].second.get_type() == PanelType::PANEL_TYPE_DISABLED));
    }
    else if (pinput.jump_pressed || pinput.shoot_pressed || input.enter.pressed())
    {
      next = &children_[index_].second;
    }
  }
  else
  {
    if (pinput.jump_pressed || pinput.shoot_pressed || input.enter.pressed())
    {
      if (type_ == PanelType::PANEL_TYPE_WARP_TO_LEVEL)
      {
        // Ignore
      }
      else
      {
        next = parent_;
      }
    }
  }
  if (input.escape.pressed())
  {
    // hide panel / go back using escape
    next = parent_;
  }
  return next;
}

void Panel::draw(const SpriteManager& sprite_manager) const
{
  if (type_ == PanelType::PANEL_TYPE_PAGES)
  {
    // Draw the current child instead
    children_[index_].second.draw(sprite_manager);
    return;
  }
  else if (type_ == PanelType::PANEL_TYPE_NEW_GAME)
  {
    return;
  }
  const geometry::Position frame_pos(((SCREEN_SIZE.x() / CHAR_W - size_.x() - 1) / 2 - 1) * CHAR_W,
                                     ((SCREEN_SIZE.y() / CHAR_H - size_.y() - 1) / 2 - 2) * CHAR_H);
  const geometry::Size frame_size = size_ + geometry::Size(2, 2);
  // Draw frame
  // Top-left corner
  sprite_manager.render_icon(Icon::ICON_FRAME_NW, frame_pos);
  // Top edge
  for (int x = 1; x < frame_size.x(); x++)
  {
    sprite_manager.render_icon(Icon::ICON_FRAME_N, frame_pos + geometry::Position(x * CHAR_W, 0));
  }
  // Top-right corner
  sprite_manager.render_icon(Icon::ICON_FRAME_NE, frame_pos + geometry::Position(frame_size.x() * CHAR_W, 0));
  // Left edge
  for (int y = 1; y < frame_size.y(); y++)
  {
    sprite_manager.render_icon(Icon::ICON_FRAME_W, frame_pos + geometry::Position(0, y * CHAR_H));
  }
  // Right edge
  for (int y = 1; y < frame_size.y(); y++)
  {
    sprite_manager.render_icon(Icon::ICON_FRAME_E, frame_pos + geometry::Position(frame_size.x() * CHAR_W, y * CHAR_W));
  }
  // Bottom-left corner
  sprite_manager.render_icon(Icon::ICON_FRAME_SW, frame_pos + geometry::Position(0, frame_size.y() * CHAR_H));
  // Bottom edge
  for (int x = 1; x < frame_size.x(); x++)
  {
    sprite_manager.render_icon(Icon::ICON_FRAME_S, frame_pos + geometry::Position(x * CHAR_W, frame_size.y() * CHAR_H));
  }
  // Bottom-right corner
  sprite_manager.render_icon(Icon::ICON_FRAME_SE, frame_pos + geometry::Position(frame_size.x() * CHAR_W, frame_size.y() * CHAR_H));

  // Frame interior
  for (int y = 1; y < frame_size.y(); y++)
  {
    for (int x = 1; x < frame_size.x(); x++)
    {
      sprite_manager.render_icon(Icon::ICON_FRAME_INNER, frame_pos + geometry::Position(x * CHAR_W, y * CHAR_H));
    }
  }

  // Shadow right edge (incl bottom-right corner)
  for (int y = 1; y < frame_size.y() + 2; y++)
  {
    sprite_manager.render_icon(Icon::ICON_FRAME_SHADOW, frame_pos + geometry::Position((frame_size.x() + 1) * CHAR_W, y * CHAR_H));
  }
  // Shadow bottom edge
  for (int x = 1; x < frame_size.x() + 1; x++)
  {
    sprite_manager.render_icon(Icon::ICON_FRAME_SHADOW, frame_pos + geometry::Position(x * CHAR_W, (frame_size.y() + 1) * CHAR_H));
  }

  // Spinning question mark frame
  constexpr Icon q_icons[]{Icon::ICON_QUESTION_1,
                           Icon::ICON_QUESTION_2,
                           Icon::ICON_QUESTION_4,
                           Icon::ICON_QUESTION_3,
                           Icon::ICON_QUESTION_1,
                           Icon::ICON_QUESTION_3,
                           Icon::ICON_QUESTION_4,
                           Icon::ICON_QUESTION_2};
  constexpr bool q_flip[]{false, false, false, false, true, false, false, false};
  const auto q_frame = (ticks_ / 2) % std::size(q_icons);

  // Draw text
  int y = frame_pos.y() + 2 * CHAR_H;
  int x = frame_pos.x() + 2 * CHAR_W;
  int row = 0;
  for (const auto& string : strings_)
  {
    Color tint{0xff, 0xff, 0xff};
    if (!children_.empty())
    {
      if (children_[index_].first == row)
      {
        tint = {0xff, 0xff, 0x00};
        // Show spinning question mark if this is the selected menu item
        const auto first_char_idx = string.find_first_not_of(L" ");
        if (first_char_idx != std::string::npos)
        {
          sprite_manager.render_icon(q_icons[q_frame], geometry::Position(x + (static_cast<int>(first_char_idx) - 2) * CHAR_W, y), q_flip[q_frame]);
        }
      }
      else
      {
        // See if this row is disabled
        for (const auto& child : children_)
        {
          if (child.first == row)
          {
            if (child.second.get_type() == PanelType::PANEL_TYPE_DISABLED)
            {
              tint = {0x66, 0x66, 0x66};
            }
            break;
          }
        }
      }
    }
    sprite_manager.render_text(string, geometry::Position(x, y), tint);
    y += CHAR_H;
    row++;
  }

  // Spinning question mark
  if (question_pos_ != geometry::Position(0, 0))
  {
    auto pos = frame_pos + question_pos_;
    // If there's input text, render it just before the question mark
    if (!input_str_.empty())
    {
      std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
      std::wstring panel_input = converter.from_bytes(input_str_);
      Color tint{0xff, 0xff, 0xff};
      pos = sprite_manager.render_text(panel_input, pos, tint);
    }
    sprite_manager.render_icon(q_icons[q_frame], pos, q_flip[q_frame]);
  }

  // Sparkle
  const auto sparkle_frame = (ticks_ / 3) % (std::size(S_ICONS) + 1);
  if (sparkle_frame > 0)
  {
    sprite_manager.render_icon(S_ICONS[sparkle_frame - 1], frame_pos + sparkle_pos_);
  }

  // Draw any sprites/icons in addition
  for (const auto& sprite : sprites_)
  {
    sprite_manager.render_tile(sprite.first, sprite.second);
  }
  for (const auto& icon : icons_)
  {
    sprite_manager.render_icon(icon.first, icon.second);
  }
}

void Panel::add_input(char c)
{
  if (c == '\x8')
  {
    // backspace
    if (!input_str_.empty())
    {
      input_str_ = input_str_.substr(0, input_str_.size() - 1);
    }
  }
  else
  {
    input_str_ += c;
  }
  if (type_ == PanelType::PANEL_TYPE_WARP_TO_LEVEL)
  {
    if (input_str_.size() > 0)
    {
      // Only allow inputs between 1-16; otherwise delete the last character
      try
      {
        const int i = std::stoi(input_str_);
        if (i > 16 || i < 1)
        {
          input_str_ = input_str_.substr(0, input_str_.size() - 1);
        }
        else
        {
          // Sanitise the input string
          input_str_ = std::to_string(i);
        }
      }
      catch (std::invalid_argument const&)
      {
        input_str_ = "";
      }
    }
  }
}
