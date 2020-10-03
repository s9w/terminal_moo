#pragma once

#include <vector>

#include "bullet.h"
#include "color.h"
#include "tweening.h"

namespace moo {

   [[nodiscard]] constexpr auto get_gradient_at_fraction(const RGB* const gradient, const size_t gradient_size, const double fraction) -> RGB;

   struct GameColors {
      GameColors();
      [[nodiscard]] constexpr auto get_sky_color(const double fraction, const double day_progress) const -> RGB;
      [[nodiscard]] constexpr auto get_ground_color(const double fraction) const -> RGB;
      [[nodiscard]] constexpr static auto get_shot_trail_start_color(const BulletStyle& style) -> RGB;
      [[nodiscard]] constexpr static auto get_shot_trail_end_color(const BulletStyle& style) -> RGB;

      [[nodiscard]] constexpr auto get_white() const -> RGB {
         return { 255, 255, 255 };
      }
      [[nodiscard]] constexpr auto get_red() const -> RGB {
         return { 255, 0, 0 };
      }

   private:
      std::vector<moo::RGB> m_ground_colors;
      moo::RGB m_rocket_smoke_color;
   };

}


constexpr auto moo::get_gradient_at_fraction(
   const moo::RGB* const gradient,
   const size_t gradient_size,
   const double fraction
) -> moo::RGB
{
   const size_t index = static_cast<size_t>(fraction * (gradient_size - 1));
   if (index < 0 || index >(gradient_size - 1)) {
      printf("Fraction not in [0.0, 1.0]\n");
      std::terminate();
   }
   return gradient[index];
}


constexpr auto moo::GameColors::get_sky_color(
   const double fraction,
   const double day_progress
) const -> RGB
{
   constexpr RGB day_horizon{ 235, 239, 192 };
   constexpr RGB night_horizon{ 53, 95, 126 };

   constexpr RGB day_sky{ 0, 82, 135 };
   constexpr RGB night_sky{ 0, 34, 57 };

   const RGB horizon_color = get_color_mix(night_horizon, day_horizon, get_triangle(day_progress));
   const RGB sky_color = get_color_mix(night_sky, day_sky, get_triangle(day_progress));
   const RGB final_color = get_color_mix(sky_color, horizon_color, fraction);

   return final_color;
}


constexpr auto moo::GameColors::get_ground_color(const double fraction) const -> RGB {
   return get_gradient_at_fraction(m_ground_colors.data(), m_ground_colors.size(), fraction);
}



constexpr auto moo::GameColors::get_shot_trail_start_color(const BulletStyle& bullet_style) -> RGB {
   constexpr RGB purple{ 255, 0, 255 };
   constexpr RGB rocket_orange{ 239, 147, 0 };
   if (bullet_style == BulletStyle::Rocket)
      return rocket_orange;
   else
      return purple;
}


constexpr auto moo::GameColors::get_shot_trail_end_color(const BulletStyle& bullet_style) -> RGB {
   constexpr RGB gray = { 200, 200, 200 };
   constexpr RGB green = { 0, 255, 0 };

   if (bullet_style == BulletStyle::Rocket)
      return gray;
   else
      return green;
}
