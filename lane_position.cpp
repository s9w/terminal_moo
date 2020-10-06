#include "lane_position.h"
#include "helpers.h"
#include "config.h"


auto moo::LanePosition::get_screen_pos() const -> ScreenCoord{
   const double y = (get_row() + 0.5) / static_rows;
   return {m_x_pos, y};
}


auto moo::LanePosition::get_row() const -> int{
   return get_sky_row_height(static_rows) + m_lane;
}

auto moo::get_new_lane_position(
   std::mt19937_64& rng,
   const int grass_rows,
   const double bitmap_width
) -> LanePosition
{
   std::uniform_int_distribution<> lane_dist(0, grass_rows - 1);
   const int lane = lane_dist(rng);
   //const size_t lane = 6;
   //const double x_pos = 0.5;
   const double x_pos = 1.0 + 0.5 * bitmap_width;
   return {x_pos, lane};
}
