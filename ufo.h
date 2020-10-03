#pragma once

#include "animation_frame.h"
#include "bullet.h"
#include "helpers.h"

#include <optional>

namespace moo {

   struct Ufo {
      Ufo(const ScreenFraction initial_pos, const double anim_progress);
      auto progress(const Seconds& dt) -> void;
      [[nodiscard]] auto hit() -> bool;
      [[nodiscard]] auto is_invul() const -> bool;
      auto fire(const ScreenFraction& player_pos) -> std::optional<Bullet>;

      ScreenFraction m_pos;
      double m_health = 1.0;
      Seconds m_hit_timer{0.0};
      AnimationFrame m_animation_frame;
      Seconds m_shooting_cooldown = 0.0;
   };

}
