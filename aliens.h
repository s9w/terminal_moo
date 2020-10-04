#pragma once

#include "ufo.h"

#include <vector>


namespace moo {
   struct Aliens{
      Aliens(const ScreenCoord& ufo_dimensions);
      void process_bullets(Bullet& bullet);

      std::vector<Ufo> m_ufos;
      ScreenCoord m_ufo_dimensions;
   };

}

