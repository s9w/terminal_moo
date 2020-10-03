#pragma once

#include "helpers.h"
#include "lane_position.h"

namespace moo {

   struct Cow {
      Cow(const LanePosition& pos, const double initial_progress);
      auto progress(const Seconds dt) -> double;
      auto move(const double distance) -> void;

      LanePosition m_pos;
      Seconds m_grazing_time = 0.0;
   };

}
