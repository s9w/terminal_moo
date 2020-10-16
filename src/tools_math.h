#pragma once

#include <cmath>

#include <doctest/doctest.h>

namespace moo {

   [[nodiscard]] constexpr auto get_tol() -> double;
   [[nodiscard]] constexpr auto less(const double first, const double second) -> bool;
   [[nodiscard]] constexpr auto less_equal(const double first, const double second) -> bool;
   [[nodiscard]] constexpr auto equal(const double first, const double second) -> bool;
   [[nodiscard]] constexpr auto greater_equal(const double first, const double second) -> bool;
   [[nodiscard]] constexpr auto get_sign(const double val) -> double;
   [[nodiscard]] constexpr auto is_zero(const double number) -> bool;
   [[nodiscard]] inline auto sane_fmod(const double a, const double b) -> double;
   [[nodiscard]] inline auto get_rect_fun(const double x, const double period) -> double;

   // literally having to rewrite constexpr std::abs() because it's not in the standard
   [[nodiscard]] constexpr auto abs(const double val) -> double;
}


constexpr auto moo::get_tol() -> double {
   return 0.001;
}
static_assert(moo::get_tol() == 0.001);


constexpr auto moo::abs(const double val) -> double {
   return val < 0 ? -val : val;
}
static_assert(moo::abs(-1.0) == 1.0);


constexpr auto moo::less_equal(const double first, const double second) -> bool {
   return first - second < get_tol();
}
static_assert(moo::less_equal(1.0, 1.0) == true);
static_assert(moo::less_equal(1.0, 1.002) == true);
static_assert(moo::less_equal(1.002, 1.0) == false);


constexpr auto moo::equal(const double first, const double second) -> bool {
   return abs(first - second) < get_tol();
}
static_assert(moo::equal(1.0, 1.0) == true);


constexpr auto moo::greater_equal(const double first, const double second) -> bool {
   return first - second > -get_tol();
}


auto constexpr moo::less(const double first, const double second) -> bool {
   return first - second <= - get_tol();
}


constexpr auto moo::get_sign(const double val) -> double {
   return val < 0.0 ? -1.0 : 1.0;
}
static_assert(moo::get_sign(1.0) == 1.0);
static_assert(moo::get_sign(0.0) == 1.0);


constexpr auto moo::is_zero(const double number) -> bool {
   return equal(number, 0.0);
}


auto moo::sane_fmod(const double a, const double b) -> double{
   const double a_2 = b * std::ceil(moo::abs(a / b));
   return std::fmod(a + a_2, b);
}
TEST_CASE("sane_fmod()") {
   using namespace moo;
   CHECK(sane_fmod(0.1, 1.0) == doctest::Approx(0.1));
   CHECK(sane_fmod(-0.1, 1.0) == doctest::Approx(0.9));
   CHECK(sane_fmod(2.2, 2.0) == doctest::Approx(0.2));
   CHECK(sane_fmod(-0.5, 1.0) == doctest::Approx(0.5));
   CHECK(sane_fmod(-0.2, 1.0) == doctest::Approx(0.8));
}


auto moo::get_rect_fun(const double x, const double period) -> double{
   const double x_interval = sane_fmod(x, period);
   const double middle = 0.5 * period;
   if (equal(x_interval, middle) || less(x_interval, middle)) {
      return 1.0;
   }
   return 0.0;
}
TEST_CASE("sane_fmod()") {
   CHECK(moo::get_rect_fun(0.0, 1.0) == 1.0);
   CHECK(moo::get_rect_fun(0.5, 1.0) == 1.0);
   CHECK(moo::get_rect_fun(0.8, 1.0) == 0.0);
   CHECK(moo::get_rect_fun(1.1, 1.0) == 1.0);
   CHECK(moo::get_rect_fun(1.8, 1.0) == 0.0);
}
