#pragma once

#include "helpers.h"

namespace moo {

   struct Cloud {
      Cloud(const FractionalPos& initial_pos, const size_t image_index, const double fractional_width, const double alpha);
      auto progress(const double dx) -> bool;

      FractionalPos m_pos;
      size_t m_image_index;
      double m_fractional_width = 0.0;
      double m_alpha = 1.0;
   };

}
