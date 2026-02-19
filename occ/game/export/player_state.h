#pragma once

#include <chrono>
#include <ctime>
#include <string>

// Player state, to be saved
struct PlayerState
{
  std::string name;
  std::time_t time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
  int episode;  // Player state limited to one episode
  int score = 0;
  int ammo = 5;
  bool levels_completed[16] = {false};
  bool used_lever = false;
  bool used_switch = false;
  bool got_reverse_gravity = false;
  bool got_power = false;
  bool got_stop_time = false;
  bool got_tough = false;

  void set_time() { time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()); }
};
