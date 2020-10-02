#pragma once

#include "helpers.h"

namespace moo {

   struct Cloud {
      Cloud(const FractionalPos& initial_pos, const size_t image_index, const double fractional_width, const double speed_factor, const double alpha);
      auto progress(const Seconds dt) -> bool;

      FractionalPos m_pos;
      size_t m_image_index;
      double m_fractional_width = 0.0;
      double m_speed_factor = 1.0;
      double m_alpha = 1.0;
   };

}
