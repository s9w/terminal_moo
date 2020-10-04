#include "cc.h"

#include "config.h"
#include "helpers.h"
#include "image.h"
#include "tweening.h"

#include <doctest/doctest.h>


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


auto moo::get_length(const ScreenCoord& a) -> double {
   return std::sqrt(a.x * a.x + a.y * a.y);
}


auto moo::get_normalized(const ScreenCoord& a) -> ScreenCoord {
   const double vec_length = get_length(a);
   return 1.0 / vec_length * a;
}


auto moo::get_height_fraction(const ScreenCoord& pos) -> double {
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


template<class T>
moo::IntCoordIt<T>::IntCoordIt(const ImageWrapper& image)
   : m_max_width(image.m_width)
   , m_max_height(image.m_height)
   , m_image_pixels(image.m_pixels)
{
}
template moo::IntCoordIt<moo::PixelCoord>::IntCoordIt(const ImageWrapper& image);
template moo::IntCoordIt<moo::LineCoord>::IntCoordIt(const ImageWrapper& image);

