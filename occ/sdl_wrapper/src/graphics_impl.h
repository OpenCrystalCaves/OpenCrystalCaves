#pragma once

#include "graphics.h"

#include <memory>
#include <utility>

#include <SDL.h>

#include "geometry.h"

class WindowImpl : public Window
{
 public:
  WindowImpl(std::unique_ptr<SDL_Window, decltype(&SDL_DestroyWindow)> sdl_window,
             std::unique_ptr<SDL_Renderer, decltype(&SDL_DestroyRenderer)> sdl_renderer)
    : sdl_window_(std::move(sdl_window)),
      sdl_renderer_(std::move(sdl_renderer))
  {
  }

  void set_size(geometry::Size size) override;
  void set_render_target(Surface* surface) override;
  std::unique_ptr<Surface> create_target_surface(geometry::Size size) override;
  void refresh() override;
  void fill_rect(const geometry::Rectangle& rect, const Color& color) override;
  void render_line(const geometry::Position& from, const geometry::Position& to, const Color& color) override;
  void render_rectangle(const geometry::Rectangle& rect, const Color& color) override;

  SDL_Renderer* get_renderer() const { return sdl_renderer_.get(); }

 private:
  std::unique_ptr<SDL_Window, decltype(&SDL_DestroyWindow)> sdl_window_;
  std::unique_ptr<SDL_Renderer, decltype(&SDL_DestroyRenderer)> sdl_renderer_;
};

class SurfaceImpl : public Surface
{
 public:
  SurfaceImpl(const int w,
              const int h,
              std::unique_ptr<SDL_Texture, decltype(&SDL_DestroyTexture)> sdl_texture,
              SDL_Renderer& sdl_renderer);

  int width() const override { return w_; }
  int height() const override { return h_; }

  void blit_surface(const geometry::Rectangle& source,
                    const geometry::Rectangle& dest,
                    const bool flip = false,
                    const Color color = {0xff, 0xff, 0xff}) const override;
  void blit_surface() const override;
  void set_render_target();
  void set_alpha(const uint8_t alpha) override;

 private:
  int w_;
  int h_;
  std::unique_ptr<SDL_Texture, decltype(&SDL_DestroyTexture)> sdl_texture_;
  SDL_Renderer& sdl_renderer_;
};
