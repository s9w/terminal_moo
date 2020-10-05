#pragma once

#include "ufo.h"

#include <vector>


namespace moo {

   struct Aliens{
      Aliens(const ScreenCoord& ufo_dimensions);
      void process_bullets(Bullet& bullet);
      void abduct_cow(const ID target_id);

      std::vector<Ufo> m_ufos;
      ScreenCoord m_ufo_dimensions;
   };

}

