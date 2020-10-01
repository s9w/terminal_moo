#pragma once

#include <algorithm>
#include <compare>
#include <optional>

#include "color.h"

#include <doctest/doctest.h>
#include <limits>


namespace moo {

   struct BlockChar {
      [[nodiscard]] constexpr auto is_all_invisible() const -> bool;
      [[nodiscard]] constexpr auto is_all_visible() const -> bool;
      [[nodiscard]] constexpr auto get_best_color() const -> RGB;
      [[nodiscard]] constexpr auto get_first_different_visible_color(const RGB& taboo = RGB{}) const -> std::optional<RGB>;
      [[nodiscard]] constexpr auto get_two_colors() const -> std::optional<TwoColors>;

      RGB top_left;
      RGB top_right;
      RGB bottom_left;
      RGB bottom_right;
   };

}


constexpr auto moo::BlockChar::is_all_invisible() const -> bool {
   return !top_left.is_visible() && !top_right.is_visible() && !bottom_left.is_visible() && !bottom_right.is_visible();
}


constexpr auto moo::BlockChar::is_all_visible() const -> bool {
   return top_left.is_visible() && top_right.is_visible() && bottom_left.is_visible() && bottom_right.is_visible();
}


constexpr auto moo::BlockChar::get_best_color() const -> RGB {
   const std::optional<RGB> first_color = get_first_different_visible_color();
   const std::optional<RGB> second_color = get_first_different_visible_color(first_color.value());
   if (second_color.has_value())
      return get_color_mix(first_color.value(), second_color.value());
   return first_color.value();
}
TEST_CASE("BlockChar.get_best_color()") {
   using namespace moo;
   constexpr RGB red{ 255, 0, 0 };
   constexpr RGB green{ 0, 255, 0 };
   constexpr RGB mustard_ish{ 128, 127, 0 };
   CHECK(BlockChar({ red, {}, {}, {} }).get_best_color() == red);
   CHECK(BlockChar({ red, green, {}, {} }).get_best_color() == mustard_ish);

   // only mix unique colors, don't weight by pixel count
   CHECK(BlockChar({ red, red, green, {} }).get_best_color() == mustard_ish);
}


constexpr auto moo::BlockChar::get_first_different_visible_color(const RGB& taboo) const
-> std::optional<RGB>
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
   const std::optional<RGB> color1 = get_first_different_visible_color();
   if (!color1.has_value()) {
      printf("All pixels invisible. This should not happen.\n");
      std::terminate();
   }
   const std::optional<RGB> color2 = get_first_different_visible_color(color1.value());
   if (!color2.has_value())
      return std::nullopt;
   return std::make_pair(color1.value(), color2.value());
}
