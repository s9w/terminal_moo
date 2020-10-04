#pragma once

#include "screen_size.h"
#include "screencoord.h"
#include "tools_math.h"

#include <algorithm>


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

   struct LineCoordIt {
      constexpr LineCoordIt(const int max_width, const int max_height);
      LineCoordIt(const ImageWrapper& image); // this not constexpr to not introduce a dependency on that header
      constexpr LineCoordIt& operator++();
      [[nodiscard]] constexpr auto is_valid() const -> bool;
      [[nodiscard]] constexpr auto operator->() const -> const LineCoord*;
      [[nodiscard]] constexpr auto operator*() const -> const LineCoord&;
      [[nodiscard]] constexpr auto to_range_index() const -> size_t;
      [[nodiscard]] constexpr auto get_x_ratio() -> double;

   private:
      int m_max_width = 0;
      int m_max_height = 0;
      LineCoord m_pos;
   };

   [[nodiscard]] constexpr auto to_line_coord(const ScreenCoord& pos)->LineCoord;
   [[nodiscard]] constexpr auto to_pixel_coord(const ScreenCoord& pos)->PixelCoord;
   [[nodiscard]] constexpr auto get_screen_clamped(const PixelCoord& pos)->PixelCoord;
   [[nodiscard]] constexpr auto get_top_left(const LineCoord& center, const LineCoord& area_dim)->LineCoord;
   [[nodiscard]] constexpr auto to_screen_index(const LineCoord& pos)->size_t;
   [[nodiscard]] constexpr auto to_screen_index(const PixelCoord& pos)->size_t;
   [[nodiscard]] constexpr auto is_on_screen(const LineCoord& pos) -> bool;

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


constexpr moo::LineCoordIt::LineCoordIt(const int max_width, const int max_height)
   : m_max_width(max_width)
   , m_max_height(max_height)
{

}


constexpr auto moo::LineCoordIt::get_x_ratio() -> double {
   return static_cast<double>(m_pos.j) / m_max_width;
}

constexpr auto moo::LineCoordIt::to_range_index() const -> size_t {
   return m_pos.i * m_max_width + m_pos.j;
}

constexpr auto moo::LineCoordIt::operator*() const -> const LineCoord& {
   return m_pos;
}

constexpr auto moo::LineCoordIt::operator->() const -> const LineCoord* {
   return &m_pos;
}

constexpr auto moo::LineCoordIt::is_valid() const -> bool {
   return m_pos.i < m_max_height && m_pos.j < m_max_width;
}

constexpr moo::LineCoordIt& moo::LineCoordIt::operator++() {
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


constexpr auto moo::get_top_left(
   const LineCoord& center,
   const LineCoord& area_dim
)->LineCoord
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
