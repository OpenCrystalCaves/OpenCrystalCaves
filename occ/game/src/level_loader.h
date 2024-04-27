#pragma once

#include <memory>
#include <string>
#include <vector>

#include "exe_data.h"

enum class LevelId;
struct Level;

namespace LevelLoader
{

std::unique_ptr<Level> load(const ExeData& exe_data, const LevelId level_id);

}
