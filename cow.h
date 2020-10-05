#pragma once

#include "animation_frame.h"
#include "helpers.h"
#include "lane_position.h"
#include "id_type.h"

namespace moo {

   struct Cow {
      Cow(const LanePosition& pos, const double initial_progress, const int variant);
      auto progress(const Seconds dt) -> void;
      auto move(const double distance) -> void;

      LanePosition m_pos;
      int m_variant = 0;
      AnimationFrame m_animation_frame;
      ID m_id;
      bool m_being_beamed = false;
   };

}
