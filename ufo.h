#pragma once

#include "animation_frame.h"
#include "bullet.h"
#include "cooldown.h"
#include "helpers.h"
#include "strategy.h"

#include <optional>

#include <entt/fwd.hpp>

namespace moo {

   struct Ufo {
      Ufo(const ScreenCoord& initial_pos, const double anim_progress);
      [[nodiscard]] auto hit() -> bool;
      [[nodiscard]] auto is_invul() const -> bool;
      auto fire(const ScreenCoord& player_pos, entt::registry& registry) -> void;

      double m_health = 1.0;
      Seconds m_hit_timer{0.0};
      AnimationFrame m_animation_frame;
      Cooldown m_shooting_cooldown;
      bool m_beaming = false;
      UfoStrategy m_strategy = Shoot{};
      ScreenCoord m_pos;
   };

}
