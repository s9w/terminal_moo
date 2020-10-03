#pragma once

#include <vector>

#include "bullet.h"
#include "color.h"

namespace moo {

   [[nodiscard]] constexpr auto get_gradient_at_fraction(const RGB* const gradient, const size_t gradient_size, const double fraction) -> RGB;

   struct GameColors {
      GameColors();
      [[nodiscard]] constexpr auto get_sky_color(const double fraction) const -> RGB;
      [[nodiscard]] constexpr auto get_ground_color(const double fraction) const -> RGB;
      [[nodiscard]] constexpr static auto get_shot_trail_start_color(const Bullet::Style& style) -> RGB;
      [[nodiscard]] constexpr static auto get_shot_trail_end_color(const Bullet::Style& style) -> RGB;

      [[nodiscard]] constexpr auto get_white() const -> RGB {
         return { 255, 255, 255 };
      }
      [[nodiscard]] constexpr auto get_red() const -> RGB {
         return { 255, 0, 0 };
      }

   private:
      std::vector<moo::RGB> m_sky_colors;
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


constexpr auto moo::GameColors::get_sky_color(const double fraction) const -> RGB {
   return get_gradient_at_fraction(m_sky_colors.data(), m_sky_colors.size(), fraction);
}


constexpr auto moo::GameColors::get_ground_color(const double fraction) const -> RGB {
   return get_gradient_at_fraction(m_ground_colors.data(), m_ground_colors.size(), fraction);
}



constexpr auto moo::GameColors::get_shot_trail_start_color(const Bullet::Style& bullet_style) -> RGB {
   constexpr RGB purple{ 255, 0, 255 };
   constexpr RGB rocket_orange{ 239, 147, 0 };
   if (bullet_style == Bullet::Style::Rocket)
      return rocket_orange;
   else
      return purple;
}


constexpr auto moo::GameColors::get_shot_trail_end_color(const Bullet::Style& bullet_style) -> RGB {
   constexpr RGB gray = { 200, 200, 200 };
   constexpr RGB green = { 0, 255, 0 };

   if (bullet_style == Bullet::Style::Rocket)
      return gray;
   else
      return green;
}
