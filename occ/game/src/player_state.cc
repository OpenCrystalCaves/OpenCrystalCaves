#include "player_state.h"

#include "logger.h"

#include <SimpleIni.h>
#include <cfgpath.h>

#define CONFIG_NAME "OpenCrystalCaves"

PlayerState::PlayerState(const int episode) : episode(episode)
{
  reset();
}

void PlayerState::reset()
{
  // Use default name
  // TODO: get player name somehow
  name = "<unnamed>";
  time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
  score = 0;
  ammo = 5;
  levels_completed.fill(false);
  used_lever = false;
  used_switch = false;
  got_reverse_gravity = false;
  got_power = false;
  got_stop_time = false;
  got_tough = false;
}

bool PlayerState::can_load(const int episode)
{
  PlayerState state{episode};
  return state.load();
}

bool PlayerState::load()
{
  // Load from file
  char cfgfile[MAX_PATH];
  get_user_config_file(cfgfile, sizeof(cfgfile), CONFIG_NAME);
  if (cfgfile[0] == 0)
  {
    LOG_INFO("Could not find config file, using defaults");
    return false;
  }
  LOG_INFO("Using config file at %s", cfgfile);
  // Load from file
  CSimpleIniA ini;
  if (ini.LoadFile(cfgfile) != SI_OK)
  {
    LOG_ERROR("Could not load config file at %s, using defaults", cfgfile);
    return false;
  }
  // Use section with the same player name and episode
  const auto section = std::format("{}_{}", name, episode);
  time = std::stoll(ini.GetValue(section.c_str(), "time", std::to_string(time).c_str()));
  score = std::stoi(ini.GetValue(section.c_str(), "score", std::to_string(score).c_str()));
  ammo = std::stoi(ini.GetValue(section.c_str(), "ammo", std::to_string(ammo).c_str()));
  for (size_t i = 0; i < levels_completed.size(); i++)
  {
    levels_completed[i] = std::stoi(ini.GetValue(section.c_str(), std::format("level_completed_{}", i).c_str(), "0")) != 0;
  }
  used_lever = std::stoi(ini.GetValue(section.c_str(), "used_lever", "0")) != 0;
  used_switch = std::stoi(ini.GetValue(section.c_str(), "used_switch", "0")) != 0;
  got_reverse_gravity = std::stoi(ini.GetValue(section.c_str(), "got_reverse_gravity", "0")) != 0;
  got_power = std::stoi(ini.GetValue(section.c_str(), "got_power", "0")) != 0;
  got_stop_time = std::stoi(ini.GetValue(section.c_str(), "got_stop_time", "0")) != 0;
  got_tough = std::stoi(ini.GetValue(section.c_str(), "got_tough", "0")) != 0;
  return true;
}

void PlayerState::save()
{
  // Save to file
  char cfgfile[MAX_PATH];
  get_user_config_file(cfgfile, sizeof(cfgfile), CONFIG_NAME);
  if (cfgfile[0] == 0)
  {
    LOG_ERROR("Could not find config file, could not save player state");
    return;
  }
  LOG_INFO("Saving player state to config file at %s", cfgfile);
  CSimpleIniA ini;
  // Use section with the same player name and episode
  const auto section = std::format("{}_{}", name, episode);
  ini.SetValue(section.c_str(), "time", std::to_string(time).c_str());
  ini.SetValue(section.c_str(), "score", std::to_string(score).c_str());
  ini.SetValue(section.c_str(), "ammo", std::to_string(ammo).c_str());
  for (size_t i = 0; i < levels_completed.size(); i++)
  {
    ini.SetValue(section.c_str(), std::format("level_completed_{}", i).c_str(), levels_completed[i] ? "1" : "0");
  }
  ini.SetValue(section.c_str(), "used_lever", used_lever ? "1" : "0");
  ini.SetValue(section.c_str(), "used_switch", used_switch ? "1" : "0");
  ini.SetValue(section.c_str(), "got_reverse_gravity", got_reverse_gravity ? "1" : "0");
  ini.SetValue(section.c_str(), "got_power", got_power ? "1" : "0");
  ini.SetValue(section.c_str(), "got_stop_time", got_stop_time ? "1" : "0");
  ini.SetValue(section.c_str(), "got_tough", got_tough ? "1" : "0");
  if (ini.SaveFile(cfgfile) != SI_OK)
  {
    LOG_ERROR("Could not write to config file at %s", cfgfile);
  }
}
