/*
https://moddingwiki.shikadi.net/wiki/Crystal_Caves_Sound_format
*/
#include "soundmgr.h"

#include <format>
#include <fstream>

#include "logger.h"
#include "path.h"

#define SND_FILENAME_FMT "CC{}-{}.SND"

// DOS PIT timer 0 in Crystal Caves was reprogrammed with divisor 0x2147 = 8519.
// With the PC PIT base clock of 1.1931816666MHz, INT 1Ch runs at 140.0636 Hz.
constexpr int DOS_TICK_RATE = 140;

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
  const double samples_per_tick = static_cast<double>(spec.freq) / DOS_TICK_RATE;
  // Bytes per sample (single channel)
  const int bps = (spec.format & 0xFF) / 8;

  // Find length of sound (terminated by -1)
  size_t src_len;
  for (src_len = 0; src_len < 300; src_len++)
  {
    if (sound.data[src_len] == -1)
    {
      break;
    }
  }

  const size_t total_samples = static_cast<size_t>(src_len * samples_per_tick);
  std::string s(total_samples * spec.channels * bps, '\0');

  // Amplitude for 16-bit signed audio (symmetrical around 0 to avoid DC bias and pops)
  constexpr int16_t AMPLITUDE_16 = 12000;
  // Amplitude for 8-bit unsigned audio
  constexpr uint8_t AMPLITUDE_8 = 48;

  double phase = 0.0;
  size_t out_idx = 0;

  for (size_t i = 0; i < src_len; i++)
  {
    // Generate square wave at frequency
    const auto freq = sound.data[i];
    const bool is_muted = (freq <= 18) || (sound.vibrate > 0 && ((i + 2) % sound.vibrate == 0));

    const size_t tick_start_sample = static_cast<size_t>(i * samples_per_tick);
    const size_t tick_end_sample = static_cast<size_t>((i + 1) * samples_per_tick);
    const size_t tick_samples = tick_end_sample - tick_start_sample;

    const double phase_increment = (freq > 0) ? (static_cast<double>(freq) / spec.freq) : 0.0;

    for (size_t j = 0; j < tick_samples; j++)
    {
      int16_t sample_16 = 0;
      uint8_t sample_8 = (spec.silence != 0) ? spec.silence : 128;

      if (!is_muted)
      {
        sample_16 = (phase < 0.5) ? AMPLITUDE_16 : -AMPLITUDE_16;
        sample_8 = (phase < 0.5) ? (128 + AMPLITUDE_8) : (128 - AMPLITUDE_8);

        phase += phase_increment;
        if (phase >= 1.0)
        {
          phase -= 1.0;
        }
      }

      if (bps == 2)
      {
        for (int c = 0; c < spec.channels; c++)
        {
          reinterpret_cast<int16_t*>(s.data())[out_idx++] = sample_16;
        }
      }
      else if (bps == 1)
      {
        for (int c = 0; c < spec.channels; c++)
        {
          reinterpret_cast<uint8_t*>(s.data())[out_idx++] = sample_8;
        }
      }
    }
  }

  s.resize(out_idx * bps);
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
  for (const auto& chunk : chunks_)
  {
    Mix_FreeChunk(chunk);
  }
  chunks_.clear();
  raw_chunks_.clear();

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
    int j = 0;
    while (input.read(reinterpret_cast<char*>(&sound), sizeof sound))
    {
      LOG_DEBUG("Read sound %d with priority %d, vibrate %d", j, sound.priority, sound.vibrate);
      LOG_DEBUG("unknown0 %x unknown1 %x unknown2 %x", sound.unknown0, sound.unknown1, sound.unknown2);
      sounds.push_back(sound);
      j++;
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
