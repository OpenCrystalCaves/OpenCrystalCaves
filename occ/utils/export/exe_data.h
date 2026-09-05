#pragma once

#include <string>

class ExeData
{
  // Crystal caves data from the .EXE file
 public:
  ExeData(const int episode);

  std::string data;
  int episode;
};
