#pragma once

#include "color.h"
#include "helpers.h"

#include <random>

namespace moo {

   struct TrailPuff {
      ScreenCoord pos;
      RGB color;
   };


   enum class BulletStyle { Rocket, Alien };

   struct Trail {
      Trail(const BulletStyle style);

      Trail(const Trail& copy) = delete;
      Trail(Trail&& move) noexcept = default;
      Trail& operator=(Trail&& move) noexcept = default;

      auto thin_trail(std::mt19937_64& rng, const Seconds dt) -> void;
      auto add_puff(std::mt19937_64& rng, const ScreenCoord& new_bullet_pos, const ScreenCoord& old_bullet_pos, const BulletStyle style, const double path_progress) -> void;
      auto update_puff_colors(const ScreenCoord& bullet_pos) -> void;

      std::vector<TrailPuff> m_smoke_puffs;
      BulletStyle m_style = BulletStyle::Rocket;
   };
   static_assert(!std::is_copy_constructible_v<Trail>);
   static_assert(!std::is_copy_assignable_v<Trail>);
   static_assert(std::is_move_constructible_v<Trail>);
   static_assert(std::is_move_assignable_v<Trail>);
   


   struct Bullet {
      Bullet(const ScreenCoord& initial_pos, const ScreenCoord& trajectory, const BulletStyle style);
      [[nodiscard]] auto progress(const Seconds dt, std::mt19937_64& rng) -> bool;
      auto update_puff_colors() -> void;

      ScreenCoord m_trajectory;
      ScreenCoord m_pos;
      ScreenCoord m_initial_pos;
      ScreenCoord m_gravity_speed;
      bool m_head_alive = true;
      Trail m_trail;
      double m_bullet_speed = 1.0;
      BulletStyle m_style = BulletStyle::Rocket;
   };
   static_assert(!std::is_copy_constructible_v<Bullet>);
   static_assert(!std::is_copy_assignable_v<Bullet>);
   static_assert(std::is_move_constructible_v<Bullet>);
   static_assert(std::is_move_assignable_v<Bullet>);

}

