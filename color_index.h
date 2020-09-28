#pragma once

#include <cstddef> // ptrdiff_t

namespace moo {

   struct ColorIndex {

      constexpr ColorIndex() = default;
      constexpr ColorIndex(const ptrdiff_t index)
         : m_index(index)
      {

      }

      [[nodiscard]] constexpr ptrdiff_t index() const {
         return m_index;
      }

   private:
      ptrdiff_t m_index = -1;
   };

   [[nodiscard]] constexpr auto operator==(const ColorIndex& a, const ColorIndex& b) -> bool {
      return a.index() == b.index();
   }

   [[nodiscard]] constexpr auto operator!=(const ColorIndex& a, const ColorIndex& b) -> bool {
      return a.index() != b.index();
   }

}
