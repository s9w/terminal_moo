#pragma once

#include "color.h"
#include "helpers.h"

#include <random>

namespace moo {

   struct TrailPuff {
      FractionalPos pos;
      RGB color;
   };

   struct Trail {
      auto thin_trail(std::mt19937_64& rng, const Seconds dt) -> void;
      auto expand_trail(std::mt19937_64& rng, const RGB smoke_color, const FractionalPos& new_bullet_pos, const FractionalPos& old_bullet_pos) -> void;
      auto recolor_puffs(const std::vector<RGB>& smoke_colors, const FractionalPos bullet_pos) -> void;

      std::vector<TrailPuff> m_smoke_puffs;
   };

   struct Bullet {
      Bullet(const FractionalPos& initial_pos, std::mt19937_64& rng);
      [[nodiscard]] auto progress(const Seconds dt, std::mt19937_64& rng, const RGB smoke_color) -> bool;
      auto recolor_puffs(const std::vector<RGB>& smoke_colors) -> void;

      FractionalPos m_trajectory;
      FractionalPos m_pos;
      FractionalPos m_initial_pos;
      FractionalPos m_gravity_speed;
      bool m_head_alive = true;
      Trail m_trail;
   };

}

