#pragma once

#include <filesystem>
namespace fs = std::filesystem;
#include <vector>

#include "color_index.h"
#include "game_colors.h"

namespace moo {

   struct Image {
      std::vector<ColorIndex> color_indices;
      int width = 0;
      int height = 0;
      void allocate() {
         color_indices.resize(width * height);
      }
   };

   [[nodiscard]] auto load_images(
      const fs::path& path_base,
      moo::GameColors& game_colors
   ) -> std::vector<moo::Image>;

}
