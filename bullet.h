#pragma once

#include "helpers.h"

#include <entt/entt.hpp>

namespace moo {


   struct Bullet {
      Bullet(const ScreenCoord& initial_pos, const ScreenCoord& trajectory, const BulletStyle style, entt::entity trail);
      Bullet(const Bullet& copy) = delete;
      Bullet(Bullet&& move) noexcept = default;
      Bullet& operator=(Bullet&& move) noexcept = default;

      [[nodiscard]] auto move(const Seconds dt) -> void;

      ScreenCoord m_trajectory;
      ScreenCoord m_pos;
      ScreenCoord m_initial_pos;
      ScreenCoord m_gravity_speed;
      entt::entity m_trail;
      double m_bullet_speed = 1.0;
      BulletStyle m_style = BulletStyle::Rocket;
   };


   [[nodiscard]] constexpr auto does_bullet_hit(
      const moo::Bullet& bullet,
      const moo::ScreenCoord& obj_pos,
      const moo::ScreenCoord& obj_dimensions,
      const moo::BulletStyle& hitting_style
   ) -> bool
   {
      return bullet.m_style == hitting_style && is_hit(bullet.m_pos, obj_pos, obj_dimensions);
   }

}

