#pragma once

#include <compare>
#include <vector>
#include <iterator>

#include "cc.h"
#include "tools_math.h"

#include <doctest/doctest.h>

namespace moo {

   struct Seconds {
      constexpr Seconds(const double value)
         : m_value(value)
      {

      }

      constexpr auto operator+=(const Seconds& other) -> Seconds& {
         m_value += other.m_value;
         return *this;
      }
      constexpr auto operator-=(const Seconds& other) -> Seconds& {
         m_value -= other.m_value;
         return *this;
      }

      constexpr auto operator<=>(const Seconds& other) const = default;

      double m_value = 0.0;
   };
   [[nodiscard]] constexpr auto operator-(const Seconds& a, const Seconds& b) -> Seconds {
      return Seconds(a.m_value - b.m_value);
   }
   [[nodiscard]] constexpr auto operator*(const double factor, const Seconds& sec) -> double {
      return factor * sec.m_value;
   }
   [[nodiscard]] constexpr auto operator/(const Seconds& a, const Seconds& b) -> double {
      return a.m_value / b.m_value;
   }


   [[nodiscard]] constexpr bool is_hit(const ScreenCoord& bullet_pos, const ScreenCoord& target_pos, const ScreenCoord& target_dimensions);


   template<typename T, typename... Args, typename TFun>
   void emplace_back_invocation(const TFun& fun, std::vector<T>& v, Args&&... args) {
      (v.emplace_back(fun(args)), ...);
   }
   
   [[nodiscard]] auto get_sky_row_height(const int rows) -> int;
   [[nodiscard]] auto get_ground_row_height(const int rows) -> int;
   [[nodiscard]] auto get_shadow_center_pos(const ScreenCoord& player_pos)->LineCoord;

   /// <summary>If the position diff would not result in at least a change of "half" a pixel,
   /// the resulting position could immediately jump back. For small differences, that would
   /// result in optical flickering</summary>
   [[nodiscard]] auto get_sanitized_position_diff(const moo::ScreenCoord& position_diff) -> moo::ScreenCoord;

   template<class T>
   [[nodiscard]] auto get_height_fraction(const T& pos) -> double;

   template <class T>
   void append_moved(std::vector<T>& dst, std::vector<T>& src) {
      if (dst.empty())
         dst = std::move(src);
      else {
         dst.reserve(dst.size() + src.size());
         std::move(std::begin(src), std::end(src), std::back_inserter(dst));
         src.clear();
      }
   }

} // moo



[[nodiscard]] constexpr auto moo::is_hit(
   const ScreenCoord& bullet_pos, 
   const ScreenCoord& target_pos,
   const moo::ScreenCoord& target_dimensions
) -> bool
{
   const bool is_x_in = greater_equal(bullet_pos.x, target_pos.x - 0.5 * target_dimensions.x) &&
      less_equal(bullet_pos.x, target_pos.x + 0.5 * target_dimensions.x);
   const bool is_y_in = greater_equal(bullet_pos.y, target_pos.y - 0.5 * target_dimensions.y) &&
      less_equal(bullet_pos.y, target_pos.y + 0.5 * target_dimensions.y);
   return is_x_in && is_y_in;
}

