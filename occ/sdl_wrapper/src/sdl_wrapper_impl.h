#pragma once
#include "sdl_wrapper.h"

class SDLWrapperImpl : public SDLWrapper
{
 public:
  bool init() override;
  unsigned get_tick() override;
  void delay(const int ms) override;
};