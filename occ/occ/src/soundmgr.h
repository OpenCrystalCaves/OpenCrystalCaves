#pragma once

#include <memory>
#include <vector>

#include <SDL_mixer.h>

#include "sound.h"

class SoundManager : public AbstractSoundManager
{
 public:
  SoundManager();
  virtual ~SoundManager();

  bool load_sounds(const int episode);
  size_t size() const { return chunks_.size(); }
  virtual void play_sound(const SoundType sound) const override;

 private:
  std::vector<std::string> raw_chunks_;
  std::vector<Mix_Chunk*> chunks_;
  SDL_AudioSpec spec_;
};
