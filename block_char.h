#pragma once

#include "color_index.h"

#include <algorithm>
#include <compare>
#include <optional>

#include <doctest/doctest.h>


namespace moo {

   struct BlockChar {
      [[nodiscard]] constexpr auto is_all_invisible() const -> bool;
      [[nodiscard]] constexpr auto is_all_visible() const -> bool;
      [[nodiscard]] constexpr auto get_max_color() const -> ColorIndex;
      [[nodiscard]] constexpr auto get_first_different_visible_color(const ColorIndex& taboo = ColorIndex{}) const -> std::optional<ColorIndex>;
      [[nodiscard]] constexpr auto get_two_colors() const -> std::optional<TwoColors>;

      ColorIndex top_left;
      ColorIndex top_right;
      ColorIndex bottom_left;
      ColorIndex bottom_right;
   };

}


constexpr auto moo::BlockChar::is_all_invisible() const -> bool {
   return !top_left.is_visible() && !top_right.is_visible() && !bottom_left.is_visible() && !bottom_right.is_visible();
}


constexpr auto moo::BlockChar::is_all_visible() const -> bool {
   return top_left.is_visible() && top_right.is_visible() && bottom_left.is_visible() && bottom_right.is_visible();
}
TEST_CASE("BlockChar visibility members") {
   using namespace moo;
   CHECK(BlockChar{ -1, -1, -1, -1 }.is_all_invisible());
   CHECK_FALSE(BlockChar{ 1, -1, -1, -1 }.is_all_invisible());
   CHECK(BlockChar{ 1, 1, 1, 1 }.is_all_visible());
   CHECK_FALSE(BlockChar{ -1, 1, 1, 1 }.is_all_visible());
}


constexpr auto moo::BlockChar::get_max_color() const -> ColorIndex {
   return std::max(std::max(top_left, top_right), std::max(bottom_left, bottom_right));
}
TEST_CASE("BlockChar::get_max_color()") {
   using namespace moo;
   CHECK(BlockChar{ 1, 6, 4, -1 }.get_max_color().index() == 6);
}


constexpr auto moo::BlockChar::get_first_different_visible_color(const ColorIndex& taboo) const
-> std::optional<ColorIndex>
{
   if (top_left.is_visible() && top_left != taboo)
      return top_left;
   else if (top_right.is_visible() && top_right != taboo)
      return top_right;
   else if (bottom_left.is_visible() && bottom_left != taboo)
      return bottom_left;
   else if (bottom_right.is_visible() && bottom_right != taboo)
      return bottom_right;
   return std::nullopt;
}


constexpr auto moo::BlockChar::get_two_colors() const -> std::optional<TwoColors>{
   const std::optional<ColorIndex> color1 = get_first_different_visible_color();
   if (!color1.has_value()) {
      printf("All pixels invisible. This should not happen.\n");
      std::terminate();
   }
   const std::optional<ColorIndex> color2 = get_first_different_visible_color(color1.value());
   if (!color2.has_value())
      return std::nullopt;
   return std::make_pair(color1.value(), color2.value());
}
TEST_CASE("BlockChar::get_two_colors()") {
   using namespace moo;
   CHECK(BlockChar{ 1, 6, 4, -1 }.get_two_colors() == TwoColors{ 1, 6 });
   CHECK(BlockChar{ -1, 3, 1, 0 }.get_two_colors() == TwoColors{ 3, 1 });
}
