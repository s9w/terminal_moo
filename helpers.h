#pragma once

#include <compare>
#include <vector>
#include <iterator>

namespace moo {

   struct LongRect {
      long left;
      long top;
      long right;
      long bottom;
      auto operator<=>(const LongRect& other) const = default;
   };


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