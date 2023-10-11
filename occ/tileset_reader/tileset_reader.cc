/*
https://moddingwiki.shikadi.net/wiki/ProGraphx_Toolbox_tileset_format
*/
#include <filesystem>
#include <fstream>
#include <iostream>

#include <find_steam_game.h>
#include <SDL.h>
#include <SDL_surface.h>

#define GOG_ID "1207665273"
#define GAME_NAME "Crystal Caves"
#define STRIDE 52
#define FILLER 2
#define SPRITE_W 16
#define SPRITE_H 16

struct Header
{
  uint8_t count;
  uint8_t width;
  uint8_t height;
};

// ARGB
#define PIXEL_FORMAT SDL_PIXELFORMAT_ARGB8888
const Uint32 colors[] = {
  0xFF000000, // "⚫",
  0xFF0000AA, // "🔵",
  0xFF00AA00, // "🟢",
  0xFF00AAAA, // "💧",
  0xFFAA0000, // "🔴",
  0xFFAA00AA, // "🟣",
  0xFFAA5500, // "🟠",
  0xFFAAAAAA, // "⚪",
  0xFF555555, // "⬛",
  0xFF5555FF, // "🟦",
  0xFF55FF55, // "🟩",
  0xFF55FFFF, // "🐬",
  0xFFFF5555, // "🟥",
  0xFFFF55FF, // "🟪",
  0xFFFFFFAA, // "🟨",
  0xFFFFFFFF, // "⬜",
};

int read_sprite_count(std::ifstream& input)
{
	Header header;
	int count = 0;
	while (input.read(reinterpret_cast<char*>(&header), sizeof header))
	{
		count += header.count;
		if (header.count > 0)
		{
			count += FILLER;
		}
		const auto size = header.count * header.width * header.height * 5;
		input.seekg(size, std::ios_base::cur);
	}
	input.clear();
	input.seekg(0, std::ios_base::beg);
	return count;
}

std::string load_pixels(std::ifstream& input, const int count)
{
	const int sheet_w =STRIDE*SPRITE_W;
	const int sheet_h =count*SPRITE_H/STRIDE;
	std::string all_pixels(sheet_w*sheet_h*sizeof(uint32_t), '\0');
  Header header;
	int index = 0;
  while (input.read(reinterpret_cast<char*>(&header), sizeof header))
  {
	const auto size = header.count * header.width * header.height * 5;
	if (size > 0)
	{
	  std::string pixels(size, '\0');
	  input.read(&pixels[0], size);
	  uint8_t* pp = (uint8_t*)(&pixels[0]);
	  for (int c = 0; c < header.count; c++, index++)
	  {
		  int x_start = (index % STRIDE) * SPRITE_W;
		  int y_start = (index / STRIDE) * SPRITE_H;
		  int x = x_start;
		  int y = y_start;
		for (int h = 0; h < header.height; h++)
		{
		  for (int w = 0; w < header.width; w++)
		  {
			const uint8_t t_plane = *pp++;
			const uint8_t b_plane = *pp++;
			const uint8_t g_plane = *pp++;
			const uint8_t r_plane = *pp++;
			const uint8_t i_plane = *pp++;
			  int pi = 0;
			for (int bit = 7; bit >= 0; bit--, pi++)
			{
			  const bool t = (t_plane >> bit) & 1;
				const int pixel_i = x + y * STRIDE * SPRITE_H;
			  if (!t)
			  {
				  ((uint32_t*)all_pixels.data())[pixel_i] = 0;
			  }
			  else
			  {
				const bool b = (b_plane >> bit) & 1;
				const bool g = (g_plane >> bit) & 1;
				const bool r = (r_plane >> bit) & 1;
				const bool i = (i_plane >> bit) & 1;
				  ((uint32_t*)all_pixels.data())[pixel_i] = colors[((int)i << 3) | ((int)r << 2) | ((int)g << 1) | (int)b];
			  }
				x++;
				if (x == x_start + SPRITE_W)
				{
					y++;
					x = x_start;
				}
			}
		  }
		}
	  }
		index += FILLER;
	}
  }
	return all_pixels;
}

int main()
{
  char buf[4096];
  fsg_get_gog_game_path(buf,
#ifdef _WIN32
                        GOG_ID
#else
                        GAME_NAME
#endif
  );
  if (!buf[0])
  {
    std::cout << "Cannot find game data!\n";
    return 1;
  }
  const auto path = std::filesystem::path(buf)
#ifdef __APPLE__
    / "game"
#endif
    / "CC1.GFX";
	std::ifstream input{path, std::ios::binary};
	const int count = read_sprite_count(input);
	std::cout << count << " sprites total\n";
	const auto pixels = load_pixels(input, count);

	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		std::cout << "Could not initialize SDL: " << SDL_GetError() << "\n";
	  return 1;
	}
	const int sheet_w =STRIDE*SPRITE_W;
	const int sheet_h =count*SPRITE_H/STRIDE;
	auto sdl_surface = SDL_CreateRGBSurfaceWithFormat(0, sheet_w, sheet_h, 32, PIXEL_FORMAT);
 if (!sdl_surface)
 {
	 std::cout << "Could not load BMP: " << SDL_GetError() << "\n";
   return 1;
 }
	SDL_LockSurface(sdl_surface);
	memcpy(sdl_surface->pixels, pixels.data(), sheet_w*sheet_h*sizeof(uint32_t));
	SDL_UnlockSurface(sdl_surface);
	
	auto sdl_window = SDL_CreateWindow("Tileset Reader", 0, 0, sdl_surface->w, sdl_surface->h, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
	auto sdl_renderer = SDL_CreateRenderer(sdl_window, -1, 0);
	auto sdl_texture = SDL_CreateTextureFromSurface(sdl_renderer, sdl_surface);
	SDL_FreeSurface(sdl_surface);
	
	bool quit = false;
	while (!quit)
	{
		SDL_Event event;
		while (SDL_PollEvent(&event) != 0)
		{
			if (event.type == SDL_QUIT)
			{
				quit = true;
			}
		}
		SDL_RenderClear(sdl_renderer);
		SDL_RenderCopy(sdl_renderer, sdl_texture, nullptr, nullptr);
		SDL_RenderPresent(sdl_renderer);
		SDL_Delay(10);
	}
	
	SDL_DestroyTexture(sdl_texture);
	SDL_DestroyRenderer(sdl_renderer);
	SDL_DestroyWindow(sdl_window);
	SDL_Quit();
  return 0;
}
