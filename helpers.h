#pragma once

#include <compare>
#include <vector>
#include <iterator>

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


   struct ScreenFraction {
      [[nodiscard]] constexpr auto is_on_screen() const -> bool {
         return x >= 0.0 &&
            x <= 1.0 &&
            y >= 0.0 &&
            y <= 1.0;
      }

      double x = 0.0;
      double y = 0.0;
   };
   auto operator*(const double factor, const ScreenFraction& pos) -> ScreenFraction;
   auto operator-(const ScreenFraction& a, const ScreenFraction& b) -> ScreenFraction;
   auto operator+(const ScreenFraction& a, const ScreenFraction& b) -> ScreenFraction;
   auto get_indep_normalized(const ScreenFraction& a) -> ScreenFraction;
   auto length(const ScreenFraction& a) -> double;
   auto get_normalized(const ScreenFraction& a) -> ScreenFraction;
   [[nodiscard]] auto get_height_fraction(const ScreenFraction& pos) -> double;
   [[nodiscard]] auto get_lane_speed(const int lane, const int rows, const Seconds& dt) -> double;
   [[nodiscard]] constexpr bool is_hit(const ScreenFraction& bullet_pos, const ScreenFraction& target_pos, const double target_width, const double target_height);
   [[nodiscard]] constexpr auto get_orthogonal(const moo::ScreenFraction& vec)->moo::ScreenFraction;


   struct PixelPos {
      int i = 0;
      int j = 0;
   };
   auto operator+(const PixelPos& a, const PixelPos& b) -> PixelPos;

   struct LongRect {
      long left;
      long top;
      long right;
      long bottom;
      auto operator<=>(const LongRect& other) const = default;
   };


   template<typename T, typename... Args, typename TFun>
   void emplace_back_invocation(const TFun& fun, std::vector<T>& v, Args&&... args) {
      (v.emplace_back(fun(args)), ...);
   }

   template<class T>
   constexpr T get_tol() {
      return static_cast<T>(0.001);
   }

   template<class T>
   [[nodiscard]] auto constexpr less(const T first, const T second) -> bool {
      return first - second <= -get_tol<T>();
   }
   [[nodiscard]] constexpr auto less_equal(const double first, const double second) -> bool;
   [[nodiscard]] constexpr auto greater_equal(const double first, const double second) -> bool;
   
   [[nodiscard]] auto get_sky_row_height(const int rows) -> int;
   [[nodiscard]] auto get_ground_row_height(const int rows) -> int;
   

   template <typename T>
   T get_sign(const T val) {
      return val < static_cast<T>(0) ? static_cast<T>(-1) : static_cast<T>(1);
   }
   template<class T>
   constexpr bool is_zero(const T number) {
      return std::abs(number) < get_tol<T>();
   }

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

constexpr auto moo::less_equal(const double first, const double second) -> bool {
   return first - second < get_tol<double>();
}
TEST_CASE("less_equal()") {
   using namespace moo;
   CHECK(less_equal(0.0, 0.0));
   CHECK(less_equal(0.0, 0.1));
   CHECK_FALSE(less_equal(0.1, 0.0));
}


constexpr auto moo::greater_equal(const double first, const double second) -> bool {
   return first - second > -get_tol<double>();
}


[[nodiscard]] constexpr auto moo::is_hit(
   const ScreenFraction& bullet_pos, 
   const ScreenFraction& target_pos,
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


constexpr auto moo::get_orthogonal(
   const moo::ScreenFraction& vec
) -> moo::ScreenFraction
{
   return { -vec.y, vec.x };
}
