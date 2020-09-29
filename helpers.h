#pragma once

#include <compare>
#include <vector>
#include <iterator>

namespace moo {

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

}