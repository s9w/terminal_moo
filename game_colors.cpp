#include "game_colors.h"
#include "helpers.h"

#include <algorithm>

namespace {


   TEST_CASE("get_gradient_at_fraction()") {
      using namespace moo;
      constexpr RGB first{ 255, 0, 0 };
      constexpr RGB second{ 0, 255, 0 };
      constexpr RGB third{ 0, 0, 255 };
      std::vector<RGB> gradient{ first, second, third };

      CHECK(get_gradient_at_fraction(gradient.data(), gradient.size(), 0.0) == first);
      CHECK(get_gradient_at_fraction(gradient.data(), gradient.size(), 0.5) == second);
      CHECK(get_gradient_at_fraction(gradient.data(), gradient.size(), 1.0) == third);
   }

}

moo::GameColors::GameColors(std::mt19937_64& rng)
   : m_sky_colors(get_sky_colors(50))
   , m_ground_colors(get_ground_colors(50))
   , m_smoke_colors(get_smoke_colors(100, rng))
   , m_health_colors(get_health_colors(50))
{
   
}


auto moo::GameColors::get_smoke_colors_ref() const -> const std::vector<moo::RGB>& {
   return m_smoke_colors;
}
