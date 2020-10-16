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
   return base_speed * dt.m_value * (0.5 + 1.5 * get_rising(1.0 * lane, 0.0, 1.0 * get_ground_row_height()));
}


auto moo::to_line_coord(const ScreenCoord& pos) -> LineCoord{
   const int i = static_cast<int>(pos.y * static_rows);
   const int j = static_cast<int>(pos.x * static_columns);
   return { i, j };
}


auto moo::get_screen_coord(const PixelCoord& pos) ->ScreenCoord{
   const double x = static_cast<double>(pos.j) / (2 * static_columns);
   const double y = static_cast<double>(pos.i) / (2 * static_rows);
   return { x, y };
}


auto moo::to_screen_index(const PixelCoord& pos) ->size_t{
   return pos.i * 2 * static_columns + pos.j;
}


auto moo::to_screen_index(const LineCoord& pos) -> size_t{
   return pos.i * static_columns + pos.j;
}


auto moo::get_screen_clamped(const PixelCoord& pos) ->PixelCoord{
   PixelCoord clamped = pos;
   clamped.i = std::clamp(clamped.i, 0, 2 * static_rows - 1);
   clamped.j = std::clamp(clamped.j, 0, 2 * static_columns - 1);
   return clamped;
}


auto moo::get_beam_aligned_pixel_coord(const ScreenCoord& pos) ->PixelCoord{
   PixelCoord pixel_coord = to_pixel_coord(pos);
   pixel_coord.i = (pixel_coord.i / 2) * 2;
   pixel_coord.j = (pixel_coord.j / 2) * 2;
   return pixel_coord;
}


auto moo::get_beam_aligned_coord(const ScreenCoord& pos) ->ScreenCoord{
   return get_screen_coord(get_beam_aligned_pixel_coord(pos));
}


auto moo::to_pixel_coord(const ScreenCoord& pos) -> PixelCoord{
   const int i = static_cast<int>(pos.y * 2 * static_rows);
   const int j = static_cast<int>(pos.x * 2 * static_columns);
   return { i, j };
}


auto moo::to_line_coord(const PixelCoord& pos) -> LineCoord{
   return { pos.i / 2, pos.j / 2 };
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

