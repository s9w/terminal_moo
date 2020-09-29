#pragma once

#include <optional>

#include "bullet.h"
#include "helpers.h"

namespace moo {

   struct Player {
      auto move_towards(const FractionalPos& target_pos, const double dt, const int pixel_rows, const int pixel_columns) -> void;
      auto fire(std::mt19937_64& rng) -> std::optional<Bullet>;

      FractionalPos m_pos{0.5, 0.5};
      double m_speed = 0.2;
      double m_bullet_wait = 0.0;
   };

}

