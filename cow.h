#pragma once

#include "animation_frame.h"
#include "helpers.h"
#include "lane_position.h"

#include <entt/entt.hpp>


namespace moo {

   struct Cow {
      Cow(const LanePosition& pos, const double initial_progress, const entt::entity variant);
      auto progress(const Seconds dt) -> void;
      auto move(const double distance) -> void;
      bool is_gone() const;

      LanePosition m_pos;
      entt::entity m_variant;
      //int m_variant = 0;
      AnimationFrame m_animation_frame;
   };

}
