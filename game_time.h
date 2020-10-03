
#pragma once

#include "config.h"
#include "helpers.h"

namespace moo {

   struct GameTime {
      constexpr GameTime() = default;
      auto progress(const Seconds& dt) -> void;

      int m_day = 0;
      double m_day_progress = 0.5;
   };

}

