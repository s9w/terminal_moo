#include "cc.h"

#include "config.h"
#include "helpers.h"
#include "image.h"
#include "tweening.h"


auto moo::get_lane_speed(
   const int lane,
   const Seconds& dt
) -> double
{
   constexpr double base_speed = 0.05;
   return base_speed * dt.m_value * (0.5 + 1.5 * get_rising(1.0 * lane, 0.0, 1.0 * get_ground_row_height(static_rows)));
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

