#pragma once

#include <filesystem>
#include <memory>
#include <string>

#include "geometry.h"

class Surface;

struct Color
{
  std::uint8_t red = 0u;
  std::uint8_t green = 0u;
  std::uint8_t blue = 0u;
  std::uint8_t alpha = 0xffu;
};

class Window
{
 public:
  static std::unique_ptr<Window> create(const std::string& title,
                                        geometry::Size size,
                                        const std::filesystem::path& icon_path,
                                        const int flags = 0);

  virtual ~Window() = default;

  virtual void set_size(geometry::Size size) = 0;
  virtual void set_render_target(Surface* surface) = 0;
  virtual std::unique_ptr<Surface> create_target_surface(geometry::Size size) = 0;
  virtual void refresh() = 0;
  virtual void fill_rect(const geometry::Rectangle& rect, const Color& color) = 0;
  virtual void render_line(const geometry::Position& from, const geometry::Position& to, const Color& color) = 0;
  virtual void render_rectangle(const geometry::Rectangle& rect, const Color& color) = 0;
};

enum class BlitType
{
  CROP,
  SCALE
};

class Surface
{
 public:
  static std::unique_ptr<Surface> from_bmp(const std::filesystem::path& filename, Window& window);
  static std::unique_ptr<Surface> from_image(const std::filesystem::path& filename, Window& window);
  static std::unique_ptr<Surface> from_pcx_image(const std::filesystem::path& filename, Window& window);
  static std::unique_ptr<Surface> from_pixels(const int w, const int h, const uint32_t* pixels, Window& window);

  virtual ~Surface() = default;

  virtual int width() const = 0;
  virtual int height() const = 0;
  geometry::Size size() const { return geometry::Size(width(), height()); }
  virtual void set_alpha(const uint8_t alpha) = 0;

  virtual void blit_surface(const geometry::Rectangle& source,
                            const geometry::Rectangle& dest,
                            const bool flip = false,
                            const Color color = {0xff, 0xff, 0xff}) const = 0;
  virtual void blit_surface() const = 0;
};
