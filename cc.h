#pragma once

#include "color.h"
#include "screen_size.h"
#include "screencoord.h"
#include "tools_math.h"

#include <algorithm>
#include <optional>
#include <vector>


namespace moo {
   
   struct ImageWrapper;

   template<class T>
   struct GenericIntCoord {
      int i = 0;
      int j = 0;
   };
   template<class T>
   [[nodiscard]] constexpr auto operator+(const GenericIntCoord<T>& a, const GenericIntCoord<T>& b)->GenericIntCoord<T>;
   template<class T>
   [[nodiscard]] constexpr auto operator-(const GenericIntCoord<T>& a, const GenericIntCoord<T>& b)->GenericIntCoord<T>;
   template<class T>
   [[nodiscard]] constexpr auto operator/(const GenericIntCoord<T>& pos, const int div)->GenericIntCoord<T>;

   struct PixelCoordType{};
   struct LineCoordType{};
   using PixelCoord = GenericIntCoord<PixelCoordType>;
   using LineCoord = GenericIntCoord<LineCoordType>;

   struct Rect {
      PixelCoord top_left;
      PixelCoord bottom_right;
      [[nodiscard]] constexpr auto get_width() -> int {
         return bottom_right.j - top_left.j;
      }
      [[nodiscard]] constexpr auto get_height() -> int {
         return bottom_right.i - top_left.i;
      }
   };

   template<class T>
   struct IntCoordIt {
      
      constexpr IntCoordIt(const int max_width, const int max_height);
      IntCoordIt(const ImageWrapper& image); // this not constexpr to not introduce a dependency on that header
      constexpr IntCoordIt& operator++();
      [[nodiscard]] constexpr auto is_valid() const -> bool;
      [[nodiscard]] constexpr auto operator->() const -> const T*;
      [[nodiscard]] constexpr auto operator*() const -> const T&;
      [[nodiscard]] constexpr auto to_range_index() const -> size_t;
      [[nodiscard]] constexpr auto get_x_ratio() -> double;
      [[nodiscard]] constexpr auto get_image_pixel() -> RGB;

   protected:
      int m_max_width = 0;
      int m_max_height = 0;
      T m_pos;
      std::optional<std::reference_wrapper<const std::vector<RGB>>> m_image_pixels;
   };

   using PixelCoordIt = IntCoordIt<PixelCoord>;
   using LineCoordIt = IntCoordIt<LineCoord>;

   [[nodiscard]] constexpr auto get_screen_it() ->LineCoordIt;

   [[nodiscard]] constexpr auto to_line_coord(const ScreenCoord& pos)->LineCoord;
   [[nodiscard]] constexpr auto to_pixel_coord_tl(const LineCoord& pos)->PixelCoord;
   [[nodiscard]] constexpr auto to_pixel_coord(const ScreenCoord& pos)->PixelCoord;
   [[nodiscard]] constexpr auto get_screen_clamped(const PixelCoord& pos)->PixelCoord;

   template<class T>
   [[nodiscard]] constexpr auto get_top_left(const T& center, const T& area_dim)->T;

   [[nodiscard]] constexpr auto to_screen_index(const LineCoord& pos)->size_t;
   [[nodiscard]] constexpr auto to_screen_index(const PixelCoord& pos)->size_t;
   [[nodiscard]] constexpr auto is_on_screen(const LineCoord& pos) -> bool;
   [[nodiscard]] constexpr auto is_on_screen(const PixelCoord& pos) -> bool;

} // namespace moo


template<class T>
constexpr auto moo::operator+(const GenericIntCoord<T>& a, const GenericIntCoord<T>& b)->GenericIntCoord<T> {
   return { a.i + b.i, a.j + b.j };
}

template<class T>
constexpr auto moo::operator-(const GenericIntCoord<T>& a, const GenericIntCoord<T>& b)->GenericIntCoord<T> {
   return { a.i - b.i, a.j - b.j };
}

template<class T>
constexpr auto moo::operator/(const GenericIntCoord<T>& pos, const int div) -> GenericIntCoord<T> {
   return { pos.i / div, pos.j / div };
}


constexpr auto moo::get_screen_it()->LineCoordIt {
   return LineCoordIt(static_columns, static_rows);
}


template<class T>
constexpr moo::IntCoordIt<T>::IntCoordIt(const int max_width, const int max_height)
   : m_max_width(max_width)
   , m_max_height(max_height)
{

}


template<class T>
constexpr auto moo::IntCoordIt<T>::get_x_ratio() -> double {
   return static_cast<double>(m_pos.j) / m_max_width;
}

template<class T>
constexpr auto moo::IntCoordIt<T>::get_image_pixel() -> RGB{
   return m_image_pixels.value().get()[to_range_index()];
}

template<class T>
constexpr auto moo::IntCoordIt<T>::to_range_index() const -> size_t {
   return m_pos.i * m_max_width + m_pos.j;
}

template<class T>
constexpr auto moo::IntCoordIt<T>::operator*() const -> const T& {
   return m_pos;
}

template<class T>
constexpr auto moo::IntCoordIt<T>::operator->() const -> const T* {
   return &m_pos;
}

template<class T>
constexpr auto moo::IntCoordIt<T>::is_valid() const -> bool {
   return m_pos.i < m_max_height && m_pos.j < m_max_width;
}

template<class T>
constexpr moo::IntCoordIt<T>& moo::IntCoordIt<T>::operator++() {
   ++m_pos.j;
   if (m_pos.j == m_max_width) {
      m_pos.j = 0;
      ++m_pos.i;
   }
   return *this;
}

constexpr auto moo::to_line_coord(const ScreenCoord& pos) -> LineCoord {
   const int i = static_cast<int>(pos.y * static_rows);
   const int j = static_cast<int>(pos.x * static_columns);
   return { i, j };
}


constexpr auto moo::to_pixel_coord_tl(const LineCoord& pos)->PixelCoord {
   return { 2 * pos.i, 2 * pos.j };
}


constexpr auto moo::to_pixel_coord(const ScreenCoord& pos) -> PixelCoord {
   const int i = static_cast<int>(pos.y * 2 * static_rows);
   const int j = static_cast<int>(pos.x * 2 * static_columns);
   return { i, j };
}


constexpr auto moo::get_screen_clamped(const PixelCoord& pos)->PixelCoord {
   PixelCoord clamped = pos;
   clamped.i = std::clamp(clamped.i, 0, 2 * static_rows - 1);
   clamped.j = std::clamp(clamped.j, 0, 2 * static_columns - 1);
   return clamped;
}


template<class T>
constexpr auto moo::get_top_left(
   const T& center,
   const T& area_dim
)->T
{
   return center - area_dim / 2;
}


constexpr auto moo::to_screen_index(const LineCoord& pos) -> size_t {
   return pos.i * static_columns + pos.j;
}

constexpr auto moo::to_screen_index(const PixelCoord& pos)->size_t {
   return pos.i * 2 * static_columns + pos.j;
}

constexpr auto moo::is_on_screen(const LineCoord& pos) -> bool {
   return pos.i >= 0 && pos.i < static_rows && pos.j >= 0 && pos.j < static_columns;
}

constexpr auto moo::is_on_screen(const PixelCoord& pos) -> bool {
   return pos.i >= 0 && pos.i < (2 * static_rows) && pos.j >= 0 && pos.j < (2 * static_columns);
}
