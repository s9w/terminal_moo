#pragma once

#include "color.h"
#include "helpers.h"

#include <random>

namespace moo {

   struct TrailPuff {
      ScreenFraction pos;
      RGB color;
   };

   enum class BulletStyle { Rocket, Alien };

   struct Trail {
      Trail(const BulletStyle style);
      auto thin_trail(std::mt19937_64& rng, const Seconds dt) -> void;
      auto add_puff(std::mt19937_64& rng, const ScreenFraction& new_bullet_pos, const ScreenFraction& old_bullet_pos, const BulletStyle style, const double path_progress) -> void;
      auto update_puff_colors(const ScreenFraction& bullet_pos) -> void;

      std::vector<TrailPuff> m_smoke_puffs;
      BulletStyle m_style = BulletStyle::Rocket;
   };


   struct Bullet {
      Bullet(const ScreenFraction& initial_pos, const ScreenFraction& trajectory, const BulletStyle style);
      [[nodiscard]] auto progress(const Seconds dt, std::mt19937_64& rng) -> bool;
      auto update_puff_colors() -> void;

      ScreenFraction m_trajectory;
      ScreenFraction m_pos;
      ScreenFraction m_initial_pos;
      ScreenFraction m_gravity_speed;
      bool m_head_alive = true;
      Trail m_trail;
      double m_bullet_speed = 1.0;
      BulletStyle m_style = BulletStyle::Rocket;
   };

}

