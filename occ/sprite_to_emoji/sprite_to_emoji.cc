/*
https://moddingwiki.shikadi.net/wiki/ProGraphx_Toolbox_tileset_format
*/
#include <format>
#include <fstream>
#include <iostream>

#include <misc.h>
#include <path.h>

#define GFX_FILENAME_FMT "CC{}.GFX"

struct Header
{
  uint8_t count;
  uint8_t width;
  uint8_t height;
};

const std::string blank = "⬛";
const std::string colors[] = {
  "⚫",
  "🇪🇺",
  "🦚",
  "📘",
  "🟥",
  "🟣",
  "🟠",
  "⚪",
  "🪦",
  "🟦",
  "🟩",
  "🩵",
  "🚨",
  "🟪",
  "🟨",
  "⬜",
};

int main()
{
  const int episode = 1;
  const auto path = get_data_path(std::format(GFX_FILENAME_FMT, episode));
  std::ifstream input{path, std::ios::binary};
  Header header;
  int idx = 0;
  while (input.read(reinterpret_cast<char*>(&header), sizeof header))
  {
    const auto size = header.count * header.width * header.height * 5;
    if (size > 0)
    {
      std::string pixels(size, '\0');
      input.read(&pixels[0], size);
      uint8_t* pp = (uint8_t*)(&pixels[0]);
      for (int i = 0; i < header.count; i++, idx++)
      {
        for (int h = 0; h < header.height; h++)
        {
          for (int w = 0; w < header.width; w++)
          {
            const uint8_t t_plane = *pp++;
            const uint8_t b_plane = *pp++;
            const uint8_t g_plane = *pp++;
            const uint8_t r_plane = *pp++;
            const uint8_t i_plane = *pp++;
            for (int bit = 7; bit >= 0; bit--)
            {
              const bool t = (t_plane >> bit) & 1;
              if (!t)
              {
                std::cout << blank;
              }
              else
              {
                const bool b = (b_plane >> bit) & 1;
                const bool g = (g_plane >> bit) & 1;
                const bool r = (r_plane >> bit) & 1;
                const bool i_ = (i_plane >> bit) & 1;
                std::cout
                  << colors[(static_cast<int>(i_) << 3) | (static_cast<int>(r) << 2) | (static_cast<int>(g) << 1) | static_cast<int>(b)];
              }
            }
          }
          std::cout << "\n";
        }
      }
    }
  }
  return 0;
}
