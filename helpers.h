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


   [[nodiscard]] constexpr bool is_hit(const ScreenCoord& bullet_pos, const ScreenCoord& target_pos, const double target_width, const double target_height);


   template<typename T, typename... Args, typename TFun>
   void emplace_back_invocation(const TFun& fun, std::vector<T>& v, Args&&... args) {
      (v.emplace_back(fun(args)), ...);
   }
   
   [[nodiscard]] auto get_sky_row_height(const int rows) -> int;
   [[nodiscard]] auto get_ground_row_height(const int rows) -> int;
   [[nodiscard]] auto get_shadow_center_pos(const ScreenCoord& player_pos)->LineCoord;


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
   const double target_width, 
   const double target_height
) -> bool
{
   const bool is_x_in = moo::greater_equal(bullet_pos.x, target_pos.x - 0.5 * target_width) &&
   moo::less_equal(bullet_pos.x, target_pos.x + 0.5 * target_width);
   const bool is_y_in = moo::greater_equal(bullet_pos.y, target_pos.y - 0.5 * target_height) &&
   moo::less_equal(bullet_pos.y, target_pos.y + 0.5 * target_height);
   return is_x_in && is_y_in;
}

