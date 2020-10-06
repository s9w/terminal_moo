#pragma once

namespace moo {

   inline int static_rows = 30;
   inline int static_columns = 120;


   constexpr auto update_screen_size(const int rows, const int columns) -> void {
      static_rows = rows;
      static_columns = columns;
   }


   [[nodiscard]] constexpr auto get_pixel_count() -> size_t {
      return 2 * static_rows * 2 * static_columns;
   }


   [[nodiscard]] constexpr auto get_char_count() -> size_t {
      return static_rows * static_columns;
   }

}

