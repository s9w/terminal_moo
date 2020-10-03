#pragma once

#include "ufo.h"

#include <vector>


namespace moo {
   struct Aliens{
      Aliens(const ScreenFraction& ufo_dimensions);
      void process_bullets(Bullet& bullet);

      std::vector<Ufo> m_ufos;
      ScreenFraction m_ufo_dimensions;
   };

}

