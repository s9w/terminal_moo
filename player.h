#pragma once

#include <optional>

#include "bullet.h"
#include "helpers.h"

namespace moo {

   struct Player {
      auto move_towards(const ScreenFraction& target_pos, const Seconds dt, const int pixel_rows, const int pixel_columns) -> void;
      [[nodiscard]] auto try_to_fire(std::mt19937_64& rng) -> std::optional<Bullet>;

      ScreenFraction m_pos{0.5, 0.5};
      double m_speed = 0.2;
      Seconds m_shooting_cooldown = 0.0;
   };

}

