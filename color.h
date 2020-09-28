#pragma once

#include <vector>

namespace moo {

   struct RGB {
      unsigned char r = 0;
      unsigned char g = 0;
      unsigned char b = 0;
   };
   bool operator==(const RGB& a, const RGB& b);

   [[nodiscard]] auto get_gradient(const RGB& from, const RGB& to, const int n) -> std::vector<RGB>;

}
