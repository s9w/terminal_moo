#pragma once

#include "tools_math.h"

namespace moo {

   struct Seconds;

   struct ScreenCoord {
      [[nodiscard]] constexpr auto is_on_screen() const -> bool {
         return x >= 0.0 &&
            x <= 1.0 &&
            y >= 0.0 &&
            y <= 1.0;
      }

      double x = 0.0;
      double y = 0.0;
   };

   constexpr auto operator*(const double factor, const ScreenCoord& pos)->ScreenCoord;
   constexpr auto operator-(const ScreenCoord& a, const ScreenCoord& b)->ScreenCoord;
   constexpr auto operator+(const ScreenCoord& a, const ScreenCoord& b)->ScreenCoord;
   constexpr auto get_indep_normalized(const ScreenCoord& a)->ScreenCoord;
   auto get_length(const ScreenCoord& a) -> double;
   auto get_normalized(const ScreenCoord& a)->ScreenCoord;
   [[nodiscard]] auto get_height_fraction(const ScreenCoord& pos) -> double;
   [[nodiscard]] auto get_lane_speed(const int lane, const int rows, const Seconds& dt) -> double;
   [[nodiscard]] constexpr auto get_orthogonal(const moo::ScreenCoord& vec)->moo::ScreenCoord;

}


constexpr auto moo::operator*(const double factor, const ScreenCoord& pos) -> ScreenCoord {
   ScreenCoord result = pos;
   result.x *= factor;
   result.y *= factor;
   return result;
}


constexpr auto moo::operator-(const ScreenCoord& a, const ScreenCoord& b) -> ScreenCoord {
   ScreenCoord result = a;
   result.x -= b.x;
   result.y -= b.y;
   return result;
}


constexpr auto moo::operator+(const ScreenCoord& a, const ScreenCoord& b) -> ScreenCoord {
   ScreenCoord result = a;
   result.x += b.x;
   result.y += b.y;
   return result;
}


constexpr auto moo::get_indep_normalized(const ScreenCoord& a) -> ScreenCoord {
   ScreenCoord result;
   result.x = is_zero(a.x) ? 0.0 : get_sign(a.x);
   result.y = is_zero(a.y) ? 0.0 : get_sign(a.y);
   return result;
}


constexpr auto moo::get_orthogonal(
   const moo::ScreenCoord& vec
) -> moo::ScreenCoord
{
   return { -vec.y, vec.x };
}
