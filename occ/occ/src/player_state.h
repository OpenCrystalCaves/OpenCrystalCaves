#pragma once

// Player state, to be saved
struct PlayerState
{
  std::string name;
  std::time_t time;
  int episode;  // Player state limited to one episode
  bool levels_completed[16] = {false};
  bool used_lever = false;
  bool used_switch = false;
  bool got_reverse_gravity = false;
  bool got_power = false;
  bool got_stop_time = false;
  bool got_tough = false;

  PlayerState(const std::string& name, const int episode) : name(name), episode(episode) {}
};
