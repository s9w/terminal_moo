#pragma once

#include <compare>
#include <random>
#include <numeric>
#include <vector>

namespace moo {

   struct RGB {
      unsigned char r = 0;
      unsigned char g = 0;
      unsigned char b = 0;
      
      [[nodiscard]] constexpr auto is_visible() const -> bool;
      [[nodiscard]] constexpr auto is_invisible() const -> bool;
      constexpr auto operator<=>(const RGB& other) const = default;
   };

   using TwoColors = std::pair<RGB, RGB>;

   [[nodiscard]] constexpr auto is_color_visible(const moo::RGB& color);
   [[nodiscard]] auto get_gradient(const RGB& from, const RGB& to, const unsigned int n) -> std::vector<RGB>;
   [[nodiscard]] constexpr auto get_color_mix(const RGB& a, const RGB& b) -> RGB;
   [[nodiscard]] constexpr auto get_color_mix(const RGB& a, const RGB& b, const double factor) -> RGB;

   [[nodiscard]] auto get_smoke_colors(const int smoke_color_count, std::mt19937_64& rng) -> std::vector<moo::RGB>;
   [[nodiscard]] auto get_sky_colors(const int sky_color_count) -> std::vector<moo::RGB>;
   [[nodiscard]] auto get_health_colors(const int health_color_count) -> std::vector<moo::RGB>;
   [[nodiscard]] auto get_ground_colors(const int ground_color_count) -> std::vector<moo::RGB>;

}


constexpr auto moo::RGB::is_visible() const -> bool {
   return r != 0 || g != 0 || b != 0;
}

constexpr auto moo::is_color_visible(const moo::RGB& color) {
   return color.is_visible();
}

constexpr auto moo::RGB::is_invisible() const -> bool {
   return r == 0 && g == 0 && b == 0;
}


constexpr auto moo::get_color_mix(const RGB& a, const RGB& b) -> RGB {
   return {
      std::midpoint(a.r, b.r),
      std::midpoint(a.g, b.g),
      std::midpoint(a.b, b.b)
   };
}


[[nodiscard]] constexpr auto get_uc_mix(
   const unsigned char a,
   const unsigned char b,
   const double factor
) -> unsigned char
{
   return static_cast<unsigned char>(a + factor * (b - a));
}


constexpr auto moo::get_color_mix(
   const RGB& a, 
   const RGB& b, 
   const double factor
) -> RGB
{
   return {
      get_uc_mix(a.r, b.r, factor),
      get_uc_mix(a.g, b.g, factor),
      get_uc_mix(a.b, b.b, factor)
   };
}