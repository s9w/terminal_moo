#pragma once

#include "color.h"
#include "entt_types.h"
#include "screencoord.h"

#include <entt/fwd.hpp>


namespace moo {

   struct Seconds;

   struct TrailPuff {
      ScreenCoord pos;
      RGB color;
   };

   struct Trail {
      Trail(const BulletStyle style, entt::entity bullet);

      Trail(const Trail& copy) = delete;
      Trail(Trail&& move) noexcept = default;
      Trail& operator=(Trail&& move) noexcept = default;

      auto thin_trail(const Seconds dt) -> void;
      auto add_puff(const ScreenCoord& new_bullet_pos, const ScreenCoord& old_bullet_pos, const double path_progress) -> void;
      auto update_puff_colors(const ScreenCoord& bullet_pos) -> void;

      std::vector<TrailPuff> m_smoke_puffs;
      BulletStyle m_style = BulletStyle::Rocket;
      entt::entity m_bullet_ref;
   };

}
