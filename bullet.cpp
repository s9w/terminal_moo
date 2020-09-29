#include "config.h"
#include "bullet.h"

#if _MSC_VER < 1928 // Visual Studio 2019 version 16.8
constexpr double pi = 3.14159265359;
#else
//#include <numbers>
constexpr double pi = std::numbers::pi;
#endif // _MSC_VER

namespace {

   [[nodiscard]] auto get_smoke_puff_pos(
      const moo::FractionalPos& rocket_pos,
      std::mt19937_64& rng
   ) -> moo::FractionalPos
   {
      const double smoke_spread = moo::get_config().smoke_puff_spread;
      std::uniform_real_distribution<double> smoke_spread_dist(-smoke_spread, smoke_spread);
      return rocket_pos + moo::FractionalPos{ smoke_spread_dist(rng), smoke_spread_dist(rng) };
   }

} // namespace {}


moo::Bullet::Bullet(const FractionalPos& initial_pos, std::mt19937_64& rng)
   : m_pos(initial_pos)
   , m_initial_pos(initial_pos)
{
   constexpr double negative_angle_spread = -0.1 * 2.0 * pi;
   constexpr double positive_angle_spread = - 0.5 * negative_angle_spread; // this points down
   
   std::uniform_real_distribution<double> phi_dist(negative_angle_spread, positive_angle_spread);
   const double phi = phi_dist(rng);
   m_trajectory = get_normalized({ std::cos(phi), std::sin(phi) });
}


auto moo::Bullet::progress(
   const double dt, 
   std::mt19937_64& rng,
   const ColorIndex smoke_color
) -> bool
{
   m_trail.expand_trail(rng, smoke_color, m_pos);
   m_trail.thin_trail(rng, dt);
   constexpr double bullet_speed = 2.5;
   m_gravity_speed = m_gravity_speed + dt * FractionalPos{0.0, moo::get_config().gravity_strength };
   const FractionalPos pos_change = dt * (bullet_speed * m_trajectory + m_gravity_speed);
   m_pos = m_pos + pos_change;
   const bool should_be_deleted = m_trail.m_smoke_puffs.empty() && !m_pos.is_on_screen();
   return should_be_deleted;
}


auto moo::Trail::thin_trail(
   std::mt19937_64& rng,
   const double dt
) -> void
{
   std::uniform_real_distribution<double> one_dist(0.0, 1.0);
   auto it = m_smoke_puffs.begin();
   while (it != m_smoke_puffs.end()) {
      const double elim_threshold = 5.0 * dt;
      if (one_dist(rng) < elim_threshold)
         it = m_smoke_puffs.erase(it);
      else
         ++it;
   }
}


auto moo::Trail::expand_trail(
   std::mt19937_64& rng,
   const ColorIndex smoke_color,
   const FractionalPos& bullet_pos
) -> void
{
   constexpr double min_smoke_puff_distance = 0.007;
   if (m_smoke_puffs.empty() || length(bullet_pos - m_smoke_puffs.back().pos) > min_smoke_puff_distance)
      m_smoke_puffs.push_back({ get_smoke_puff_pos(bullet_pos, rng), smoke_color });
}
