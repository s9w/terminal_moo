#pragma once

#include <vector>
#include <iterator>

namespace moo {

   struct LongRect {
      long left;
      long top;
      long right;
      long bottom;
   };
   bool operator==(const LongRect& a, const LongRect& b);
   bool operator!=(const LongRect& a, const LongRect& b);


   //template <class T>
   //auto contains(const std::vector<T>& vec, const T& element) -> bool {
   //   return std::find(std::cbegin(vec), std::cend(vec), element) != std::cend(vec);
   //}


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