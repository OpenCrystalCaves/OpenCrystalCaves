#pragma once

#include <array>
#include <chrono>
#include <ctime>
#include <string>

// Player state, to be saved
struct PlayerState
{
  std::string name;
  std::time_t time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
  int episode;  // Player state limited to one episode
  int score;
  int ammo;
  std::array<bool, 20> levels_completed;
  bool used_lever;
  bool used_switch;
  bool got_reverse_gravity;
  bool got_power;
  bool got_stop_time;
  bool got_tough;

  PlayerState(const int episode);
  void reset();
  void load();
  void save();

  void set_time() { time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()); }
};
