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


   struct FractionalPos {
      [[nodiscard]] constexpr auto is_on_screen() const -> bool {
         return x_fraction >= 0.0 &&
            x_fraction <= 1.0 &&
            y_fraction >= 0.0 &&
            y_fraction <= 1.0;
      }

      double x_fraction = 0.0;
      double y_fraction = 0.0;
   };
   auto operator*(const double factor, const FractionalPos& pos) -> FractionalPos;
   auto operator-(const FractionalPos& a, const FractionalPos& b) -> FractionalPos;
   auto operator+(const FractionalPos& a, const FractionalPos& b) -> FractionalPos;
   auto get_indep_normalized(const FractionalPos& a) -> FractionalPos;
   auto length(const FractionalPos& a) -> double;
   auto get_normalized(const FractionalPos& a) -> FractionalPos;
   [[nodiscard]] auto get_height_fraction(const FractionalPos& pos) -> double;


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

   [[nodiscard]] constexpr auto get_triangle(const double x) -> double;
   [[nodiscard]] constexpr auto get_rising(const double x, const double start, const double end) -> double;

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

constexpr auto moo::get_triangle(const double x) -> double {
   if (less(x, 0.5))
      return 2.0 * x;
   else
      return 2.0 - 2.0 * x;
}
TEST_CASE("get_triangle()") {
   using namespace moo;
   CHECK_EQ(get_triangle(0.0), doctest::Approx(0.0));
   CHECK_EQ(get_triangle(0.5), doctest::Approx(1.0));
   CHECK_EQ(get_triangle(1.0), doctest::Approx(0.0));
}


constexpr auto moo::get_rising(
   const double x,
   const double start,
   const double end
) -> double
{
   if (less_equal(x, start))
      return 0.0;
   else if (greater_equal(x, end))
      return 1.0;
   const double x0 = (x - start) / (end - start);
   return x0;
}
TEST_CASE("get_rising()") {
   using namespace moo;
   CHECK_EQ(get_rising(0.0, 0.5, 1.0), doctest::Approx(0.0));
   CHECK_EQ(get_rising(0.5, 0.5, 1.0), doctest::Approx(0.0));
   CHECK_EQ(get_rising(0.75, 0.5, 1.0), doctest::Approx(0.5));
   CHECK_EQ(get_rising(1.0, 0.5, 1.0), doctest::Approx(1.0));
   CHECK_EQ(get_rising(1.5, 0.5, 1.0), doctest::Approx(1.0));
}
