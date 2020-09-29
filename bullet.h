#pragma once

#include "helpers.h"
#include "color_index.h"

#include <random>

namespace moo {

   struct TrailPuff {
      FractionalPos pos;
      ColorIndex color;
   };

   struct Trail {
      auto thin_trail(std::mt19937_64& rng, const double dt) -> void;
      auto expand_trail(std::mt19937_64& rng, const ColorIndex smoke_color, const FractionalPos& new_bullet_pos, const FractionalPos& old_bullet_pos) -> void;

      std::vector<TrailPuff> m_smoke_puffs;
   };

   struct Bullet {
      Bullet(const FractionalPos& initial_pos, std::mt19937_64& rng);
      [[nodiscard]] auto progress(const double dt, std::mt19937_64& rng, const ColorIndex smoke_color) -> bool;

      FractionalPos m_trajectory;
      FractionalPos m_pos;
      FractionalPos m_initial_pos;
      FractionalPos m_gravity_speed;
      Trail m_trail;
   };

}

