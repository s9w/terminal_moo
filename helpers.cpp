#include <cmath>

#include "config.h"
#include "helpers.h"
#include "tweening.h"

namespace {

   constexpr auto get_height_fraction(const moo::ScreenFraction& pos, const double sky_fraction) -> double {
      const double ground_fraction = 1.0 - sky_fraction;
      const double height_range = sky_fraction + 0.5 * ground_fraction;
      const double height_fraction = pos.y / height_range;
      return height_fraction;
   }
   TEST_CASE("get_height_fraction()") {
      using namespace moo;
      const double sky_fraction = 0.8;
      constexpr double irrelevant = 0.0;
      CHECK_EQ(get_height_fraction({irrelevant, 0.0}, sky_fraction), doctest::Approx(0.0));
      CHECK_EQ(get_height_fraction({irrelevant, 0.9}, sky_fraction), doctest::Approx(1.0));
   }

}

auto moo::operator*(const double factor, const ScreenFraction& pos) -> ScreenFraction {
   ScreenFraction result = pos;
   result.x *= factor;
   result.y *= factor;
   return result;
}


auto moo::operator-(const ScreenFraction& a, const ScreenFraction& b) -> ScreenFraction{
   ScreenFraction result = a;
   result.x -= b.x;
   result.y -= b.y;
   return result;
}

auto moo::operator+(const ScreenFraction& a, const ScreenFraction& b) -> ScreenFraction{
   ScreenFraction result = a;
   result.x += b.x;
   result.y += b.y;
   return result;
}

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

auto moo::get_indep_normalized(const ScreenFraction& a) -> ScreenFraction{
   ScreenFraction result;
   result.x = is_zero(a.x) ? 0.0 : get_sign(a.x);
   result.y = is_zero(a.y) ? 0.0 : get_sign(a.y);
   return result;
}

auto moo::length(const ScreenFraction& a) -> double{
   return std::sqrt(a.x * a.x + a.y * a.y);
}

auto moo::get_normalized(const ScreenFraction& a) -> ScreenFraction{
   const double vec_length = length(a);
   return 1.0 / vec_length * a;
}


auto moo::get_height_fraction(const ScreenFraction& pos) -> double{
   return ::get_height_fraction(pos, get_config().sky_fraction);
}


auto moo::get_lane_speed(
   const int lane, 
   const int rows,
   const Seconds& dt
) -> double
{
   constexpr double base_speed = 0.05;
   return base_speed * dt.m_value * (0.5 + 1.5 * get_rising(1.0 * lane, 0.0, 1.0 * get_ground_row_height(rows)));
}


auto moo::operator+(const PixelPos& a, const PixelPos& b) -> PixelPos{
   return { a.i + b.i, a.j + b.j };
}

