#include "exe_data.h"

#include <format>

#include "misc.h"
#include "path.h"
#include <decompress.h>

#define EXE_FILENAME_FMT "CC{}.EXE"

ExeData::ExeData(const int episode) : episode(0)
{
  reset(episode);
}

void ExeData::reset(const int new_episode)
{
  episode = new_episode;
  const auto exe_file = std::format(EXE_FILENAME_FMT, episode);
  const auto exe_path = get_data_path(exe_file);
  size_t exe_len;
  const auto exe_data = decompress(exe_path.string().c_str(), &exe_len);
  data = std::string(exe_data, exe_len);
  free(exe_data);
}
