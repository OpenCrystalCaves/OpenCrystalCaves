#pragma once

#include "event.h"
#include "game_renderer.h"
#include "graphics.h"
#include "sdl_wrapper.h"
#include "soundmgr.h"
#include "spritemgr.h"

#include "game.h"
#include "panel.h"

/// Represents a game state (e.g. splash, title, game)
/// Contains base logic for fading in/out
class State
{
 public:
  explicit State(const unsigned fade_in_ticks, const unsigned fade_out_ticks, Window& window);
  virtual ~State() {}

  void set_next(State& next) { next_state_ = &next; }

  // Called whenever this state is entered
  virtual void reset()
  {
    ticks_ = 0;
    fade_out_start_ticks_ = 0;
  }

  virtual void update(const Input& input);

  // Called whenever state should start finishing
  virtual void finish()
  {
    if (fade_out_start_ticks_ == 0)
    {
      fade_out_start_ticks_ = ticks_;
    }
  }
  bool has_finished() const { return fade_out_start_ticks_ > 0 && fade_out_start_ticks_ + fade_out_ticks_ < ticks_; }
  virtual State* next_state() { return has_finished() ? next_state_ : this; }

  virtual void draw(Window& window) const = 0;

 protected:
  unsigned ticks_ = 0;
  State* next_state_ = nullptr;
  std::unique_ptr<Surface> overlay_;
  unsigned fade_in_ticks_;
  unsigned fade_out_ticks_;
  unsigned fade_out_start_ticks_ = 0;
};

/// State that can be skipped with a button press
class SkipState : public virtual State
{
 public:
  explicit SkipState() {}
  virtual void update(const Input& input) override;

 protected:
  // Prevent skip for the first ticks
  unsigned grace_ticks_ = 15;
};

class SplashState : public SkipState
{
 public:
  SplashState(SoundManager& sound_manager, std::vector<Surface*>& images, Window& window);

  virtual void reset() override;
  virtual void draw(Window& window) const override;

 private:
  SoundManager& sound_manager_;
  std::vector<Surface*>& images_;
};

class TitleState : public State
{
 public:
  TitleState(SpriteManager& sprite_manager,
             SoundManager& sound_manager,
             Surface& game_surface,
             std::vector<Surface*>& images,
             Window& window,
             ExeData& exe_data);

  virtual void reset() override
  {
    panel_current_ = nullptr;
    State::reset();
  }
  virtual void finish() override;
  virtual void update(const Input& input) override;
  virtual void draw(Window& window) const override;
  virtual State* next_state() override
  {
    if (panel_current_ && panel_current_->get_type() == PanelType::PANEL_TYPE_QUIT_TO_OS)
    {
      return nullptr;
    }
    return State::next_state();
  }

 private:
  SpriteManager& sprite_manager_;
  SoundManager& sound_manager_;
  Surface& game_surface_;
  std::vector<Surface*>& images_;
  unsigned scroll_ticks_ = 0;
  Panel panel_;
  Panel* panel_current_ = nullptr;
};

class GameState : public State
{
 public:
  GameState(Game& game,
            SpriteManager& sprite_manager,
            SoundManager& sound_manager,
            Surface& game_surface,
            Window& window,
            ExeData& exe_data);

  virtual void reset() override;
  virtual void update(const Input& input) override;
  virtual void draw(Window& window) const override;
  virtual State* next_state() override;

 private:
  Game& game_;
  Surface& game_surface_;
  SpriteManager& sprite_manager_;
  SoundManager& sound_manager_;
  GameRenderer game_renderer_;
  ExeData& exe_data_;
  bool debug_info_ = false;
  bool paused_ = false;
  unsigned game_tick_ = 0;
  LevelId level_ = LevelId::MAIN_LEVEL;
  Panel panel_;
  Panel warp_panel_;
  Panel* panel_current_ = nullptr;
  Panel* panel_next_ = nullptr;
};

// TODO: end state
