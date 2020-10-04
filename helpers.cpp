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
TEST_CASE("get_ground_row_height()") {
   using namespace moo;
   CHECK(get_ground_row_height(30) == 7);
}

