#include "game_colors.h"
#include "helpers.h"

#include <algorithm>


namespace {

   auto get_region_fraction(
      const std::vector<moo::RGB>& gradient,
      const double fraction
   ) -> moo::RGB 
   {
      const size_t index = static_cast<size_t>(std::round(fraction * (gradient.size() - 1)));
      if (index < 0 || index > (gradient.size() - 1)) {
         printf("Fraction not in [0.0, 1.0]\n");
         std::terminate();
      }
      return gradient[index];
   }

} // namespace {}


moo::GameColors::GameColors(std::mt19937_64& rng)
   : m_sky_colors(get_sky_colors(50))
   , m_ground_colors(get_ground_colors(50))
   , m_smoke_colors(get_smoke_colors(100, rng))
   , m_health_colors(get_health_colors(50))
{
   
}


auto moo::GameColors::get_sky_color(const double fraction) const -> RGB {
   return get_region_fraction(m_sky_colors, fraction);
}


auto moo::GameColors::get_ground_color(const double fraction) const -> RGB {
   return get_region_fraction(m_ground_colors, fraction);
}


auto moo::GameColors::get_smoke_color(const double fraction) const -> RGB {
   return get_region_fraction(m_smoke_colors, fraction);
}


auto moo::GameColors::get_smoke_colors_ref() const -> const std::vector<moo::RGB>&{
   return m_smoke_colors;
}


auto moo::GameColors::get_health_color(const double fraction) const -> RGB {
   return get_region_fraction(m_health_colors, fraction);
}
