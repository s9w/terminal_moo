#include <cmath>

#include "config.h"
#include "helpers.h"
#include "tweening.h"


auto moo::get_sky_row_height(const int rows) -> int {
   const int sky_height = static_cast<int>(moo::get_config().sky_fraction * rows);
   return sky_height;
}


auto moo::get_ground_row_height(const int rows) -> int{
   return rows - get_sky_row_height(rows);
}


auto moo::get_shadow_center_pos(const ScreenCoord& player_pos) ->LineCoord{
   const int sky_height = get_sky_row_height(static_rows);
   const int ground_height = static_rows - sky_height;
   const int i = static_cast<int>(sky_height + 0.5 * ground_height);
   const int j_center = static_cast<int>(player_pos.x * static_columns);
   return{ i, j_center };
}


TEST_CASE("get_ground_row_height()") {
   using namespace moo;
   CHECK(get_ground_row_height(30) == 7);
}

