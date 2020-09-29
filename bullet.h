#pragma once

#include "helpers.h"
#include "color_index.h"

#include <random>

namespace moo {

   struct TrailPuff {
      FractionalPos pos;
      ColorIndex color;
   };

   struct Bullet {
      Bullet(const FractionalPos& initial_pos, std::mt19937_64& rng);
      [[nodiscard]] auto progress(const double dt, std::mt19937_64& rng, const ColorIndex smoke_color) -> bool;
      auto thin_trail(std::mt19937_64& rng, const double dt) -> void;
      auto expand_trail(std::mt19937_64& rng, const ColorIndex smoke_color) -> void;

      FractionalPos m_trajectory;
      FractionalPos m_pos;
      FractionalPos m_initial_pos;
      FractionalPos m_gravity_influence;
      std::vector<TrailPuff> m_trail;
   };

}

