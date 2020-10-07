#pragma once

#include "color.h"
#include "helpers.h"

#include <random>

#include <entt/entt.hpp>

namespace moo {

   struct TrailPuff {
      ScreenCoord pos;
      RGB color;
   };


   enum class BulletStyle { Rocket, Alien };

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


   struct Bullet {
      Bullet(const ScreenCoord& initial_pos, const ScreenCoord& trajectory, const BulletStyle style, entt::entity trail);
      Bullet(const Bullet& copy) = delete;
      Bullet(Bullet&& move) noexcept = default;
      Bullet& operator=(Bullet&& move) noexcept = default;

      [[nodiscard]] auto progress(const Seconds dt) -> void;

      ScreenCoord m_trajectory;
      ScreenCoord m_pos;
      ScreenCoord m_initial_pos;
      ScreenCoord m_gravity_speed;
      bool m_head_alive = true;
      entt::entity m_trail;
      double m_bullet_speed = 1.0;
      BulletStyle m_style = BulletStyle::Rocket;
   };

}

