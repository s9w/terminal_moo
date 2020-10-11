#pragma once

#include "entt_types.h"

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
      constexpr auto operator+=(const RGB& other)->RGB&;
      constexpr auto operator<=>(const RGB& other) const = default;
   };

   struct RGBA {
      RGB m_rgb;
      double m_alpha = 0.0;
   };

   using TwoColors = std::pair<RGB, RGB>;

   [[nodiscard]] constexpr auto operator*(const double factor, const moo::RGB& color) -> moo::RGB;
   [[nodiscard]] constexpr auto operator+(const moo::RGB& a, const moo::RGB& b) -> moo::RGB;
   [[nodiscard]] constexpr auto is_color_visible(const moo::RGB& color) -> bool;
   [[nodiscard]] auto get_noised_color(const moo::RGB& color, const int noise_strength, std::mt19937_64& rng) -> RGB;
   [[nodiscard]] constexpr auto get_offsetted_color(const moo::RGB& color, const int noise) -> RGB;
   [[nodiscard]] auto get_gradient(const RGB& from, const RGB& to, const unsigned int n) -> std::vector<RGB>;
   [[nodiscard]] constexpr auto get_color_mix(const RGB& a, const RGB& b, const double factor) -> RGB;
   [[nodiscard]] constexpr auto get_sky_color(const double fraction)->RGB;
   [[nodiscard]] constexpr auto get_ground_color(const double fraction)->RGB;
   [[nodiscard]] constexpr auto get_shot_trail_start_color(const BulletStyle& style)->RGB;
   [[nodiscard]] constexpr auto get_shot_trail_end_color(const BulletStyle& style)->RGB;
}


[[nodiscard]] constexpr auto get_clamped_uchar(
   const int c
) -> unsigned char
{
   return static_cast<unsigned char>(std::clamp(c, 0, 255));
}


constexpr auto moo::RGB::is_visible() const -> bool {
   return r != 0 || g != 0 || b != 0;
}


constexpr auto moo::operator*(const double factor, const RGB& color) -> RGB{
   return {
      get_clamped_uchar(static_cast<int>(factor * color.r)),
      get_clamped_uchar(static_cast<int>(factor * color.g)),
      get_clamped_uchar(static_cast<int>(factor * color.b))
   };
}

constexpr auto moo::operator+(const RGB& a, const RGB& b) -> RGB {
   return {
      get_clamped_uchar(static_cast<int>(a.r + b.r)),
      get_clamped_uchar(static_cast<int>(a.g + b.g)),
      get_clamped_uchar(static_cast<int>(a.b + b.b))
   };
}

constexpr auto moo::RGB::operator+=(const RGB& other) ->RGB&{
   *this = *this + other;
   return *this;
}

constexpr auto moo::is_color_visible(const moo::RGB& color) -> bool {
   return color.is_visible();
}

constexpr auto moo::RGB::is_invisible() const -> bool {
   return r == 0 && g == 0 && b == 0;
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


constexpr auto moo::get_offsetted_color(
   const moo::RGB& color,
   const int noise
) -> moo::RGB
{
   return {
      get_clamped_uchar(color.r + noise),
      get_clamped_uchar(color.g + noise),
      get_clamped_uchar(color.b + noise)
   };
}


constexpr auto moo::get_sky_color(const double fraction) ->RGB{
   constexpr RGB horizon{ 149, 179, 228 };
   constexpr RGB sky{ 245, 244, 240 };
   //constexpr RGB horizon{ 167, 142, 172 };
   //constexpr RGB sky{ 237, 215, 155 };
   const RGB final_color = get_color_mix(sky, horizon, fraction);

   return final_color;
}


constexpr auto moo::get_ground_color(const double fraction) ->RGB{
   constexpr moo::RGB light_green{ 55, 108, 48 };
   constexpr moo::RGB dark_green{ 21, 42, 31 };
   return get_color_mix(dark_green, light_green, fraction);
}

constexpr auto moo::get_shot_trail_start_color(const BulletStyle& bullet_style) -> RGB {
   constexpr RGB purple{ 255, 0, 255 };
   constexpr RGB rocket_orange{ 239, 147, 0 };
   if (bullet_style == BulletStyle::Rocket)
      return rocket_orange;
   else
      return purple;
}


constexpr auto moo::get_shot_trail_end_color(const BulletStyle& bullet_style) -> RGB {
   constexpr RGB gray = { 200, 200, 200 };
   constexpr RGB green = { 0, 255, 0 };

   if (bullet_style == BulletStyle::Rocket)
      return gray;
   else
      return green;
}
