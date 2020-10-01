#pragma once

#include <filesystem>
namespace fs = std::filesystem;
#include <vector>

#include "game_colors.h"

namespace moo {

   struct Image {
      Image() = default;
      Image(const unsigned int width, const unsigned int height);

      std::vector<RGB> m_pixels;
      int m_width = 0;
      int m_height = 0;
   };

   [[nodiscard]] auto load_images(const fs::path& path_base, const bool dimension_checks = true) -> std::vector<moo::Image>;

}
