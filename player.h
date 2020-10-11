#pragma once

#include <optional>

#include "bullet.h"
#include "helpers.h"

#include <entt/fwd.hpp>

namespace moo {

   struct Player {
      auto move_towards(const ScreenCoord& target_pos, const Seconds dt) -> void;
      auto try_to_fire(entt::registry& registry) -> void;
      [[nodiscard]] auto is_invul() const -> bool;

      ScreenCoord m_pos{0.5, 0.5};
      double m_speed = 0.2;
      Seconds m_shooting_cooldown = 0.0;
      double m_hitpoints = 10.0;
      Seconds m_hit_timer = 0.0;
   };

}

