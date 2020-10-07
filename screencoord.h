#pragma once

#include "tools_math.h"

#include <cmath>
#include <type_traits>


namespace moo {

   struct Seconds;


   struct DoubleCoord {
      [[nodiscard]] constexpr auto is_on_screen() const -> bool {
         return x >= 0.0 &&
            x <= 1.0 &&
            y >= 0.0 &&
            y <= 1.0;
      }


      auto constexpr operator+=(const DoubleCoord& other)->DoubleCoord& {
         x += other.x;
         y += other.y;
         return *this;
      }

      double x = 0.0;
      double y = 0.0;
   };

   // maybe upgrade to std::is_layout_compatible when it lands
   template<typename T>
   concept CoordType = requires(T t) {
      t.x;
      t.y;
   };

   struct ScreenCoord : public DoubleCoord {};
   //struct Trajectory : public DoubleCoord {};
   //struct InitialBulletPos : public DoubleCoord {};


   template<moo::CoordType T> // I don't understand why I need to repeat the namespace
   constexpr auto operator*(const double factor, const T& pos)->T;

   template<moo::CoordType T>
   constexpr auto operator-(const T& a, const T& b)->T;

   template<moo::CoordType T>
   constexpr auto operator+(const T& a, const T& b)->T;

   template<moo::CoordType T>
   constexpr auto get_indep_normalized(const T& a)->T;

   template<moo::CoordType T>
   auto get_length(const T& a) -> double {
      return std::sqrt(a.x * a.x + a.y * a.y);
   }

   template<moo::CoordType T>
   auto get_normalized(const T& a)->T {
      const double vec_length = get_length(a);
      return 1.0 / vec_length * a;
   }
   

   [[nodiscard]] auto get_lane_speed(const int lane, const Seconds& dt) -> double;

   template<moo::CoordType T>
   [[nodiscard]] constexpr auto get_orthogonal(const T& vec)->T;

}


template<moo::CoordType T>
constexpr auto moo::operator*(const double factor, const T& pos) -> T {
   T result = pos;
   result.x *= factor;
   result.y *= factor;
   return result;
}


template<moo::CoordType T>
constexpr auto moo::operator-(const T& a, const T& b) -> T {
   T result = a;
   result.x -= b.x;
   result.y -= b.y;
   return result;
}


template<moo::CoordType T>
constexpr auto moo::operator+(const T& a, const T& b) -> T {
   T result = a;
   result.x += b.x;
   result.y += b.y;
   return result;
}


template<moo::CoordType T>
constexpr auto moo::get_indep_normalized(const T& a) -> T {
   T result;
   result.x = is_zero(a.x) ? 0.0 : get_sign(a.x);
   result.y = is_zero(a.y) ? 0.0 : get_sign(a.y);
   return result;
}


template<moo::CoordType T>
constexpr auto moo::get_orthogonal(
   const T& vec
) -> T
{
   return { -vec.y, vec.x };
}
