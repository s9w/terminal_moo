#pragma once

#include <vector>

#include "color.h"

namespace moo {

   struct GameColors {
      GameColors();
      std::vector<RGB> rgbs;
      [[nodiscard]] auto get_sky_color(const double fraction) const -> size_t;
      [[nodiscard]] auto get_ground_color(const double fraction) const -> size_t;
      [[nodiscard]] auto get_white() const -> size_t;

      size_t ship_color_start = 0;
      size_t ship_color_count = 0;
      size_t sky_color_start = 0;
      size_t sky_color_count = 0;
      size_t ground_color_start = 0;
      size_t ground_color_count = 0;
   };

}