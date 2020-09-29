#pragma once

#include <compare>
#include <vector>

namespace moo {

   struct RGB {
      unsigned char r = 0;
      unsigned char g = 0;
      unsigned char b = 0;
      auto operator<=>(const RGB& other) const = default;
   };

   [[nodiscard]] auto get_gradient(const RGB& from, const RGB& to, const unsigned int n) -> std::vector<RGB>;

   [[nodiscard]] auto get_smoke_colors(const int smoke_color_count) -> std::vector<moo::RGB>;
   [[nodiscard]] auto get_sky_colors(const int sky_color_count) -> std::vector<moo::RGB>;
   [[nodiscard]] auto get_ground_colors(const int ground_color_count) -> std::vector<moo::RGB>;

}
