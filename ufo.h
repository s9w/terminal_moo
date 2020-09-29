#pragma once

#include "helpers.h"

namespace moo {

   struct Ufo {
      Ufo(const FractionalPos initial_pos);
      auto progress() -> void;
      auto hit() -> bool;

      FractionalPos m_pos;
      double m_health = 1.0;
      bool m_is_hit = false;
   };

}
