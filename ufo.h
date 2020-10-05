#pragma once

#include "animation_frame.h"
#include "bullet.h"
#include "helpers.h"
#include "strategy.h"

#include <optional>

#include <entt/fwd.hpp>

namespace moo {

   struct Ufo {
      Ufo(const ScreenCoord initial_pos, const double anim_progress);
      auto progress(const Seconds& dt, const ScreenCoord& player_pos, entt::registry& registry) -> void;
      [[nodiscard]] auto hit() -> bool;
      [[nodiscard]] auto is_invul() const -> bool;
      auto fire(const ScreenCoord& player_pos, entt::registry& registry) -> void;

      ScreenCoord m_pos;
      double m_health = 1.0;
      Seconds m_hit_timer{0.0};
      AnimationFrame m_animation_frame;
      Seconds m_shooting_cooldown = 0.0;
      bool m_beaming = true;
      UfoStrategy m_strategy = Shoot{};
   };

}
