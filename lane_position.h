#pragma once

#include "screencoord.h"

#include <random>

namespace moo {

   struct LanePosition{
      auto get_screen_pos() const -> ScreenCoord;
      auto get_row() const -> int;
      auto is_gone() const -> bool;

      double m_x_pos = 0.0;
      int m_lane = 0;
   };

   [[nodiscard]] auto get_new_lane_position(const int grass_rows, const double bitmap_width) -> LanePosition;

}

