#pragma once

#include "helpers.h"

namespace moo {

   struct Cow {
      Cow(const FractionalPos& pos, const double initial_progress);
      auto progress(const double dt) -> double;

      FractionalPos m_pos;
      double m_grazing_time = 0.0;
   };

}
