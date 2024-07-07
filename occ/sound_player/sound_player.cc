/*
https://moddingwiki.shikadi.net/wiki/Crystal_Caves_Sound_format
*/
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <find_steam_game.h>
#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_mixer.h>

#include "graphics.h"
#include "logger.h"
#include "sdl_wrapper.h"

#define SND_FILENAME_FMT "CC1-{}.SND"
#define GOG_ID "1207665273"
#define GAME_NAME "Crystal Caves"

#define SAMPLE_RATE 44100
#define AUDIO_FMT AUDIO_S16
#define AUDIO_CHANNELS 2

struct Sound
{
  uint16_t priority;
  uint16_t unknown0;
  uint16_t vibrate;
  uint16_t unknown1;
  uint16_t unknown2;
  int16_t data[300];
};

std::filesystem::path get_sound_path(const int idx)
{
  // TODO: use <format> in C++20
  const auto filename =
    std::string(SND_FILENAME_FMT).replace(std::string(SND_FILENAME_FMT).find("{}"), std::string("{}").length(), std::to_string(idx + 1));
  // Try CWD first
  if (std::filesystem::exists(filename))
  {
    return std::filesystem::path(filename);
  }
  // Try GoG
  char buf[4096];
  fsg_get_gog_game_path(buf,
#ifdef _WIN32
                        GOG_ID
#else
                        GAME_NAME
#endif
  );
  if (buf[0])
  {
    const auto gogfilename = std::filesystem::path(buf)
#ifdef __APPLE__
      / "game"
#endif
      / filename;
    if (std::filesystem::exists(gogfilename))
    {
      return std::filesystem::path(gogfilename);
    }
  }
  // TODO: Try steam
  return std::filesystem::path();
}

struct SoundData
{
  SDL_AudioSpec spec;
  std::vector<Sound> sounds;

  std::string to_raw(int sound_index) const
  {
    const int freq_len = 400;
    // Bytes per sample (single channel)
    const int bps = (spec.format & 0xFF) / 8;

    // Find length of sound
    const auto& sound = sounds[sound_index];
    size_t len;
    for (len = 0;; len++)
    {
      const auto freq = sound.data[len];
      if (len >= 300 || freq == -1)
      {
        break;
      }
    }
    std::string s(len * freq_len * spec.channels * bps, '\0');
    Uint8* ptr = (Uint8*)s.data();
    Uint8 dc = 64;
    for (int i = 0; i < len; i++)
    {
      // Generate square wave at frequency
      const auto freq = sound.data[i];
      const int freq_interval = freq > 0 ? (65536 / freq) : 0;
      for (int j = 0, freq_counter = 0; j < freq_len; j++, freq_counter++)
      {
        Uint8 amp = dc;
        // TODO: use vibrate?
        if (freq == 0)
        {
          amp = spec.silence;
        }
        else if (freq_counter == freq_interval)
        {
          dc = 64 - dc;
          freq_counter = 0;
        }
        freq_counter++;
        for (int c = 0; c < spec.channels; c++)
        {
          for (int b = 0; b < bps; b++)
          {
            *ptr++ = dc;
          }
        }
      }
    }
    return s;
  }
};

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

int main()
{
  SoundData sdata;
  for (int i = 0;; i++)
  {
    const auto path = get_sound_path(i);
    if (path.empty())
    {
      break;
    }
    std::cout << "Reading sound file at " << path << "\n";
    std::ifstream input{path, std::ios::binary};
    Sound sound;
    while (input.read(reinterpret_cast<char*>(&sound), sizeof sound))
    {
      sdata.sounds.push_back(sound);
    }
  }
  std::cout << "Read " << sdata.sounds.size() << " sounds\n";

  auto sdl = SDLWrapper::create();
  if (!sdl)
  {
    LOG_CRITICAL("Could not create SDLWrapper");
    return 1;
  }
  auto window = Window::create("OpenCrystalCaves", geometry::Size(10, 10), "");
  if (!window)
  {
    LOG_CRITICAL("Could not create Window");
    return 1;
  }
  window->set_size(geometry::Size{100, 100});

  if (Mix_OpenAudio(SAMPLE_RATE, AUDIO_FMT, AUDIO_CHANNELS, 4096) != 0)
  {
    std::cout << "Could not open audio: " << SDL_GetError() << "\n";
    return 1;
  }
  int channels;
  if (Mix_QuerySpec(&sdata.spec.freq, &sdata.spec.format, &channels) == 0)
  {
    std::cout << "Could not get audio spec: " << SDL_GetError() << "\n";
    return 1;
  }
  sdata.spec.channels = static_cast<Uint8>(channels);

  // Convert sounds to chunks
  std::vector<std::string> raw_chunks;
  std::vector<Mix_Chunk*> chunks;
  for (int i = 0; i < sdata.sounds.size(); i++)
  {
    raw_chunks.push_back(sdata.to_raw(i));
    chunks.push_back(Mix_QuickLoad_RAW((Uint8*)raw_chunks[i].data(), (Uint32)raw_chunks[i].size()));
  }

  std::cout << "Play sound by pressing 0 - " << itoc((int)sdata.sounds.size() - 1) << "\n";

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
        if (index >= 0 && index < (int)chunks.size())
        {
          const auto& sound = sdata.sounds[index];
          std::cout << "Playing sound " << index << " vibrate=" << std::hex << sound.vibrate << " unknown0=" << sound.unknown0
                    << " unknown1=" << sound.unknown1 << " unknown2=" << sound.unknown2 << std::dec << "\n";
          if (Mix_PlayChannel(-1, chunks[index], 0) == -1)
          {
            LOG_CRITICAL("Could not play sound: %s", SDL_GetError());
          }
        }
      }
      else if (event.type == SDL_KEYUP)
      {
        keydown = false;
      }
    }
    sdl->delay(10);
  }

  for (const auto& chunk : chunks)
  {
    Mix_FreeChunk(chunk);
  }
  Mix_CloseAudio();

  return 0;
}
