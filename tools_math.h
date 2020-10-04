#pragma once

namespace moo {

   [[nodiscard]] constexpr auto get_tol() -> double;
   [[nodiscard]] constexpr auto less(const double first, const double second) -> bool;
   [[nodiscard]] constexpr auto less_equal(const double first, const double second) -> bool;
   [[nodiscard]] constexpr auto greater_equal(const double first, const double second) -> bool;
   [[nodiscard]] constexpr auto get_sign(const double val) -> double;
   [[nodiscard]] constexpr auto is_zero(const double number) -> bool;

   // literally having to rewrite constexpr std::abs() because it's not in the standard
   [[nodiscard]] constexpr auto abs(const double val) -> double;
}


constexpr auto moo::get_tol() -> double {
   return 0.001;
}


constexpr auto moo::less_equal(const double first, const double second) -> bool {
   return first - second < get_tol();
}


constexpr auto moo::greater_equal(const double first, const double second) -> bool {
   return first - second > -get_tol();
}


auto constexpr moo::less(const double first, const double second) -> bool {
   return first - second <= - get_tol();
}


constexpr auto moo::get_sign(const double val) -> double {
   return val < 0.0 ? -1.0 : 1.0;
}


constexpr auto moo::abs(const double val) -> double {
   return val < 0 ? -val : val;
}


constexpr auto moo::is_zero(const double number) -> bool {
   return abs(number) < get_tol();
}
