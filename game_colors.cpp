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

moo::GameColors::GameColors()
   : m_ground_colors(get_ground_colors(50))
{
   
}

