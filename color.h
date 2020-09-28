#pragma once

#include <compare>
#include <vector>

namespace moo {

   struct RGB {
      unsigned char r = 0;
      unsigned char g = 0;
      unsigned char b = 0;
      auto operator<=>(const RGB& other) const = default;
   };

   [[nodiscard]] auto get_gradient(const RGB& from, const RGB& to, const int n) -> std::vector<RGB>;

}
