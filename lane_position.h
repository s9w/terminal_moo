#pragma once

#include <random>

namespace moo {
   struct ScreenFraction;

   struct LanePosition{
      auto get_screen_pos(const int rows) -> ScreenFraction;

      double m_x_pos = 0.0;
      int m_lane = 0;
   };

   [[nodiscard]] auto get_new_lane_position(std::mt19937_64& rng, const int grass_rows, const double bitmap_width) -> LanePosition;

}

