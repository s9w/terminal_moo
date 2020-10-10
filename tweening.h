#pragma once

#include "helpers.h"

#include <doctest/doctest.h>

namespace moo {

   [[nodiscard]] constexpr auto get_falling(const double x, const double start, const double end) -> double;
   [[nodiscard]] constexpr auto get_rising(const double x, const double start, const double end) -> double;
   [[nodiscard]] constexpr auto get_triangle(const double x) -> double;

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


constexpr auto moo::get_falling(
   const double x,
   const double start,
   const double end
) -> double
{
   return 1.0 - get_rising(x, start, end);
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
