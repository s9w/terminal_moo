#pragma once

#include "color.h"
#include "helpers.h"

#include <random>

namespace moo {

   struct TrailPuff {
      ScreenFraction pos;
      RGB color;
   };

   struct Trail {
      auto thin_trail(std::mt19937_64& rng, const Seconds dt) -> void;
      auto expand_trail(std::mt19937_64& rng, const RGB smoke_color, const ScreenFraction& new_bullet_pos, const ScreenFraction& old_bullet_pos, const double smoke_spread) -> void;
      auto recolor_puffs(const ScreenFraction& bullet_pos, const RGB& rocket_orange, const RGB& end_color) -> void;

      std::vector<TrailPuff> m_smoke_puffs;
   };

   struct Bullet {
      enum class Style{Rocket, Alien};

      Bullet(const ScreenFraction& initial_pos, const ScreenFraction& trajectory, const Style style);
      [[nodiscard]] auto progress(const Seconds dt, std::mt19937_64& rng, const RGB smoke_color) -> bool;
      auto recolor_puffs(const RGB& rocket_orange) -> void;

      ScreenFraction m_trajectory;
      ScreenFraction m_pos;
      ScreenFraction m_initial_pos;
      ScreenFraction m_gravity_speed;
      bool m_head_alive = true;
      Trail m_trail;
      double m_bullet_speed = 1.0;
      Style m_style = Style::Rocket;
   };

}

