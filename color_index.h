#pragma once

#include <cstddef> // ptrdiff_t
#include <compare>
#include <utility>

#include <doctest/doctest.h>

namespace moo {

   constexpr ptrdiff_t invisible_value = -1;

   struct ColorIndex {
      constexpr ColorIndex() = default;
      constexpr ColorIndex(const ptrdiff_t index);

      [[nodiscard]] constexpr ptrdiff_t index() const;
      [[nodiscard]] constexpr auto is_visible() const -> bool;

      constexpr auto operator<=>(const ColorIndex& other) const = default;

   private:
      ptrdiff_t m_index = invisible_value;
   };

   using TwoColors = std::pair<ColorIndex, ColorIndex>;

}


TEST_CASE("ColorIndex() default") {
   using namespace moo;
   const ColorIndex default_color_index{};
   CHECK_FALSE(default_color_index.is_visible());
}

constexpr moo::ColorIndex::ColorIndex(const ptrdiff_t index)
   : m_index(index)
{

}

constexpr ptrdiff_t moo::ColorIndex::index() const {
   return m_index;
}


constexpr auto moo::ColorIndex::is_visible() const -> bool {
   return m_index != invisible_value;
}
