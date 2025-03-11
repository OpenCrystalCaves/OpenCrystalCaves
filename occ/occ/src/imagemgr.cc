/*
 https://moddingwiki.shikadi.net/wiki/Crystal_Caves
*/
#include "imagemgr.h"

#include <filesystem>
#include <fstream>

#include "logger.h"
#include "path.h"

static std::string image_extensions[] = {"APG", "TTL", "CDT", "END"};

std::filesystem::path get_image_path(const int episode, const CCImage image, const int index)
{
  // CC1.END or CC1-END1.END
  std::string filename = "CC" + std::to_string(episode);
  if (index > 0)
  {
    filename += "-" + image_extensions[image] + std::to_string(index);
  }
  filename += "." + image_extensions[image];
  return get_data_path(filename);
}

std::unique_ptr<Surface> load_surface(Window& window, const int episode, const CCImage image, const int index)
{
  auto path = get_image_path(episode, image, index);
  if (path.empty())
  {
    return nullptr;
  }
  auto surface = Surface::from_image(path, window);
  if (!surface)
  {
    LOG_CRITICAL("Could not load '%s'", path.c_str());
    return nullptr;
  }
  return surface;
}

bool ImageManager::load_images(Window& window)
{
  // Check if images already loaded
  if (!episode_images_.empty())
  {
    return false;
  }

  // Load images
  bool end = false;
  for (int episode = 1; !end; episode++)
  {
    std::array<std::vector<std::unique_ptr<Surface>>, 4> episode_surfaces;
    int loaded = 0;
    for (int image = IMAGE_APOGEE; image <= IMAGE_END; image++)
    {
      std::vector<std::unique_ptr<Surface>> image_surfaces;
      auto surface = load_surface(window, episode, (CCImage)image, 0);
      if (surface == nullptr)
      {
        // Try again but with different image indices
        for (int index = 1;; index++)
        {
          surface = load_surface(window, episode, (CCImage)image, index);
          if (surface == nullptr)
          {
            break;
          }
          loaded++;
          image_surfaces.emplace_back(std::move(surface));
        }
        if (image_surfaces.empty())
        {
          if (episode == 1)
          {
            LOG_CRITICAL("Could not find game image %d", image);
            return false;
          }
          end = true;
          continue;
        }
      }
      else
      {
        loaded++;
        image_surfaces.emplace_back(std::move(surface));
      }
      episode_surfaces[image] = std::move(image_surfaces);
    }
    if (loaded > 0)
    {
      episode_images_.emplace_back(std::move(episode_surfaces));
    }
  }

  return true;
}

std::vector<Surface*> ImageManager::get_images(size_t episode, CCImage image) const
{
  std::vector<Surface*> images;
  const auto& episode_images = episode_images_[episode - 1];
  const auto& type_images = episode_images[(int)image];
  for (auto&& surface : type_images)
  {
    images.push_back(surface.get());
  }
  return images;
}
