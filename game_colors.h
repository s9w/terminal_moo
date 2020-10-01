#pragma once

#include <vector>

#include "color.h"

namespace moo {

   struct GameColors {
      GameColors(std::mt19937_64& rng);
      [[nodiscard]] auto get_sky_color(const double fraction) const -> RGB;
      [[nodiscard]] auto get_ground_color(const double fraction) const -> RGB;
      [[nodiscard]] auto get_smoke_color(const double fraction) const -> RGB;
      [[nodiscard]] auto get_smoke_colors_ref() const -> const std::vector<moo::RGB>&;
      [[nodiscard]] auto get_health_color(const double fraction) const -> RGB;
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