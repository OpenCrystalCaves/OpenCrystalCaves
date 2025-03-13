/*
https://moddingwiki.shikadi.net/wiki/Crystal_Caves_Sound_format
*/
#include "soundmgr.h"

#include <format>
#include <fstream>

#include "logger.h"
#include "path.h"

#define SND_FILENAME_FMT "CC{}-{}.SND"

struct Sound
{
  int16_t data[300];
  uint16_t priority;
  uint16_t unknown0;
  uint16_t vibrate;
  uint16_t unknown1;
  uint16_t unknown2;
};

std::string to_raw(const Sound& sound, const SDL_AudioSpec spec)
{
  // Magic number to get the right sound duration
  const int freq_len = 320 * spec.freq / 44100;
  // Bytes per sample (single channel)
  const int bps = (spec.format & 0xFF) / 8;

  // Find length of sound
  size_t src_len;
  for (src_len = 0; src_len < 300; src_len++)
  {
    const auto freq = sound.data[src_len];
    if (freq == -1)
    {
      break;
    }
  }
  std::string s(src_len * freq_len * spec.channels * bps, '\0');
  Uint8* ptr = (Uint8*)s.data();
  // Magic number for how loud the sound is
  const Uint8 DC = 32;
  Uint8 dc = DC;
  for (size_t i = 0; i < src_len; i++)
  {
    // Generate square wave at frequency
    const auto freq = sound.data[i];
    // Controls pitch given the sound spec frequency
    const int freq_interval = freq > 0 ? (spec.freq / 2 / freq) : 0;
    for (int j = 0, freq_counter = 0; j < freq_len; j++, freq_counter++)
    {
      Uint8 amp = dc;
      if (freq == 0 || (i % sound.vibrate) != 0)
      {
        amp = spec.silence;
      }
      else if (freq_counter == freq_interval)
      {
        dc = DC - dc;
        amp = dc;
        freq_counter = 0;
      }
      for (int c = 0; c < spec.channels; c++)
      {
        for (int b = 0; b < bps; b++)
        {
          *ptr++ = amp;
        }
      }
    }
  }
  return s;
}

SoundManager::SoundManager() : chunks_()
{
  int channels;
  if (Mix_QuerySpec(&spec_.freq, &spec_.format, &channels) == 0)
  {
    LOG_CRITICAL("Could not get audio spec: %s", SDL_GetError());
    return;
  }
  spec_.channels = static_cast<Uint8>(channels);
}

SoundManager::~SoundManager()
{
  for (const auto& chunk : chunks_)
  {
    Mix_FreeChunk(chunk);
  }
}

bool SoundManager::load_sounds(const int episode)
{
  std::vector<Sound> sounds;
  // Read raw audio into memory
  for (int i = 0;; i++)
  {
    const auto path = get_data_path(std::format(SND_FILENAME_FMT, episode, i + 1));
    if (path.empty())
    {
      break;
    }
    LOG_DEBUG("Reading sound file at %s", path.c_str());
    std::ifstream input{path, std::ios::binary};
    Sound sound;
    while (input.read(reinterpret_cast<char*>(&sound), sizeof sound))
    {
      sounds.push_back(sound);
    }
  }
  LOG_DEBUG("Read %d sounds", static_cast<int>(sounds.size()));

  // Convert sounds to chunks
  for (const auto& sound : sounds)
  {
    auto& raw_chunk = raw_chunks_.emplace_back(to_raw(sound, spec_));
    chunks_.push_back(Mix_QuickLoad_RAW((Uint8*)raw_chunk.data(), (Uint32)raw_chunk.size()));
  }
  return !chunks_.empty();
}

void SoundManager::play_sound(const SoundType sound) const
{
  if (Mix_PlayChannel(-1, chunks_[static_cast<int>(sound)], 0) == -1)
  {
    LOG_CRITICAL("Could not play sound: %s", SDL_GetError());
  }
}
