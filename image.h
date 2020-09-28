#pragma once

#include <filesystem>
namespace fs = std::filesystem;
#include <vector>

#include "color_index.h"
#include "game_colors.h"

namespace moo {

   struct Image {
      Image() = default;
      Image(const unsigned int width, const unsigned int height);

      std::vector<ColorIndex> m_color_indices;
      int m_width = 0;
      int m_height = 0;
   };

   [[nodiscard]] auto load_image(const fs::path& path, moo::ColorLoader& color_loader) -> moo::Image;
   [[nodiscard]] auto load_images(const fs::path& path_base, ColorLoader& color_loader) -> std::vector<moo::Image>;

}
