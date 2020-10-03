#pragma once

#include "animation_frame.h"
#include "helpers.h"

namespace moo {

   struct Ufo {
      Ufo(const FractionalPos initial_pos, const double anim_progress);
      auto progress(const Seconds& dt) -> void;
      auto hit() -> bool;

      FractionalPos m_pos;
      double m_health = 1.0;
      bool m_is_hit = false;
      AnimationFrame m_animation_frame;
   };

}
