#include <cmath>

#include "config.h"
#include "helpers.h"
#include "tweening.h"


namespace {

   constexpr auto get_height_fraction(const moo::ScreenCoord& pos, const double sky_fraction) -> double {
      const double ground_fraction = 1.0 - sky_fraction;
      const double height_range = sky_fraction + 0.5 * ground_fraction;
      const double height_fraction = pos.y / height_range;
      return height_fraction;
   }
   TEST_CASE("get_height_fraction()") {
      using namespace moo;
      const double sky_fraction = 0.8;
      constexpr double irrelevant = 0.0;
      CHECK_EQ(get_height_fraction({ irrelevant, 0.0 }, sky_fraction), doctest::Approx(0.0));
      CHECK_EQ(get_height_fraction({ irrelevant, 0.9 }, sky_fraction), doctest::Approx(1.0));
   }

} // namespace {}


auto moo::get_sky_row_height(const int rows) -> int {
   const int sky_height = static_cast<int>(moo::get_config().sky_fraction * rows);
   return sky_height;
}


auto moo::get_ground_row_height(const int rows) -> int{
   return rows - get_sky_row_height(rows);
}
TEST_CASE("get_ground_row_height()") {
   using namespace moo;
   CHECK(get_ground_row_height(30) == 7);
}


auto moo::get_shadow_center_pos(const ScreenCoord& player_pos) ->LineCoord{
   const int sky_height = get_sky_row_height(static_rows);
   const int ground_height = static_rows - sky_height;
   const int i = static_cast<int>(sky_height + 0.5 * ground_height);
   const int j_center = static_cast<int>(player_pos.x * static_columns);
   return{ i, j_center };
}


template<class T>
[[nodiscard]] auto moo::get_height_fraction(const T& pos) -> double{
   return ::get_height_fraction(pos, get_config().sky_fraction);
}
template auto moo::get_height_fraction(const ScreenCoord& pos) -> double;