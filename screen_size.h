#pragma once

namespace moo {

   inline int static_rows = 0;
   inline int static_columns = 0;

   constexpr auto update_screen_size(const int rows, const int columns) -> void;

}


constexpr auto moo::update_screen_size(const int new_rows, const int new_columns) -> void {
   static_rows = new_rows;
   static_columns = new_columns;
}

