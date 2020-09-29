#include <cmath>

#include "config.h"
#include "helpers.h"

namespace {

   constexpr auto get_height_fraction(const moo::FractionalPos& pos, const double sky_fraction) -> double {
      const double ground_fraction = 1.0 - sky_fraction;
      const double height_range = sky_fraction + 0.5 * ground_fraction;
      const double height_fraction = pos.y_fraction / height_range;
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

auto moo::operator*(const double factor, const FractionalPos& pos) -> FractionalPos {
   FractionalPos result = pos;
   result.x_fraction *= factor;
   result.y_fraction *= factor;
   return result;
}


auto moo::operator-(const FractionalPos& a, const FractionalPos& b) -> FractionalPos{
   FractionalPos result = a;
   result.x_fraction -= b.x_fraction;
   result.y_fraction -= b.y_fraction;
   return result;
}

auto moo::operator+(const FractionalPos& a, const FractionalPos& b) -> FractionalPos{
   FractionalPos result = a;
   result.x_fraction += b.x_fraction;
   result.y_fraction += b.y_fraction;
   return result;
}

auto moo::get_indep_normalized(const FractionalPos& a) -> FractionalPos{
   FractionalPos result;
   result.x_fraction = is_zero(a.x_fraction) ? 0.0 : get_sign(a.x_fraction);
   result.y_fraction = is_zero(a.y_fraction) ? 0.0 : get_sign(a.y_fraction);
   return result;
}

auto moo::length(const FractionalPos& a) -> double{
   return std::sqrt(a.x_fraction * a.x_fraction + a.y_fraction * a.y_fraction);
}

auto moo::get_normalized(const FractionalPos& a) -> FractionalPos{
   const double vec_length = length(a);
   return 1.0 / vec_length * a;
}


auto moo::get_height_fraction(const FractionalPos& pos) -> double{
   return ::get_height_fraction(pos, get_config().sky_fraction);
}

auto moo::operator+(const PixelPos& a, const PixelPos& b) -> PixelPos{
   return { a.i + b.i, a.j + b.j };
}

