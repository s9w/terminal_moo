#pragma once

#include "animation_frame.h"
#include "helpers.h"

namespace moo {

   struct Ufo {
      Ufo(const FractionalPos initial_pos, const double anim_progress);
      auto progress(const Seconds& dt) -> void;
      [[nodiscard]] auto hit() -> bool;
      [[nodiscard]] auto is_hit() const -> bool;

      FractionalPos m_pos;
      double m_health = 1.0;
      Seconds m_hit_timer{0.0};
      AnimationFrame m_animation_frame;
   };

}
