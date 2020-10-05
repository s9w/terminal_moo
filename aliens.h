#pragma once

#include "ufo.h"

#include <vector>

#include <entt/fwd.hpp>


namespace moo {

   struct Aliens{
      Aliens(const ScreenCoord& ufo_dimensions);
      void process_bullets(Bullet& bullet, entt::registry& registry);
      void abduct_cow(const ID target_id, entt::registry& registry);

      ScreenCoord m_ufo_dimensions;
   };

}

