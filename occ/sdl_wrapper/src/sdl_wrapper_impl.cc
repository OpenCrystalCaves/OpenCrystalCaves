#include "sdl_wrapper_impl.h"

#include <cstdlib>
#include <memory>
#include <utility>

#include <SDL.h>
#include <SDL_mixer.h>

#include "logger.h"

#define SAMPLE_RATE 22050
#define AUDIO_FMT AUDIO_S16
#define AUDIO_CHANNELS 2


std::unique_ptr<SDLWrapper> SDLWrapper::create()
{
  return std::make_unique<SDLWrapperImpl>();
}

void quit()
{
  Mix_CloseAudio();
  SDL_Quit();
}

bool SDLWrapperImpl::init()
{
  // Init SDL
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
  {
    LOG_CRITICAL("Could not initialize SDL: %s", SDL_GetError());
    return false;
  }
  if (Mix_OpenAudio(SAMPLE_RATE, AUDIO_FMT, AUDIO_CHANNELS, 4096) != 0)
  {
    LOG_CRITICAL("Could not open audio: %s", SDL_GetError());
    return false;
  }

  atexit(quit);

  return true;
}

unsigned SDLWrapperImpl::get_tick()
{
  return SDL_GetTicks();
}

void SDLWrapperImpl::delay(const int ms)
{
  SDL_Delay(ms);
}
