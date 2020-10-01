#pragma once

#include <vector>

#include "color.h"

namespace moo {

   [[nodiscard]] constexpr auto get_gradient_at_fraction(const RGB* const gradient, const size_t gradient_size, const double fraction) -> RGB;

   struct GameColors {
      GameColors(std::mt19937_64& rng);
      [[nodiscard]] constexpr auto get_sky_color(const double fraction) const -> RGB;
      [[nodiscard]] constexpr auto get_ground_color(const double fraction) const -> RGB;
      [[nodiscard]] constexpr auto get_smoke_color(const double fraction) const -> RGB;
      [[nodiscard]] constexpr auto get_health_color(const double fraction) const->RGB;
      [[nodiscard]] auto get_smoke_colors_ref() const -> const std::vector<moo::RGB>&;

      [[nodiscard]] constexpr auto get_white() const -> RGB {
         return { 255, 255, 255 };
      }
      [[nodiscard]] constexpr auto get_red() const -> RGB {
         return { 255, 0, 0 };
      }

   private:
      std::vector<moo::RGB> m_sky_colors;
      std::vector<moo::RGB> m_ground_colors;
      std::vector<moo::RGB> m_smoke_colors;
      std::vector<moo::RGB> m_health_colors;
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


constexpr auto moo::GameColors::get_smoke_color(const double fraction) const -> RGB {
   return get_gradient_at_fraction(m_smoke_colors.data(), m_smoke_colors.size(), fraction);
}


constexpr auto moo::GameColors::get_health_color(const double fraction) const -> RGB {
   return get_gradient_at_fraction(m_health_colors.data(), m_health_colors.size(), fraction);
}
