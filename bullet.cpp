#include "bullet.h"

#if _MSC_VER < 1928
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
      constexpr double smoke_spread = 0.01;
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


auto moo::Bullet::progress(const double dt, std::mt19937_64& rng, const ColorIndex smoke_color) -> bool{
   expand_trail(rng, smoke_color);
   thin_trail(rng, dt);
   constexpr double bullet_speed = 2.5;
   constexpr double gravity_strength = 0.01;
   m_gravity_influence = m_gravity_influence + FractionalPos{0.0, dt * gravity_strength};
   const FractionalPos pos_change = bullet_speed * dt * m_trajectory + m_gravity_influence;
   m_pos = m_pos + pos_change;
   return m_trail.empty() && !m_pos.is_on_screen();
}


auto moo::Bullet::thin_trail(std::mt19937_64& rng, const double dt) -> void{
   std::uniform_real_distribution<double> standard(0.0, 1.0);
   auto it = m_trail.begin();
   while (it != m_trail.end()) {
      const double elim_threshold = 5.0 * dt;
      if (standard(rng) < elim_threshold)
         it = m_trail.erase(it);
      else
         ++it;
   }
}


auto moo::Bullet::expand_trail(std::mt19937_64& rng, const ColorIndex smoke_color) -> void{
   constexpr double min_smoke_puff_distance = 0.007;
   if (m_trail.empty() || length(m_pos - m_trail.back().pos) > min_smoke_puff_distance)
      m_trail.push_back({ get_smoke_puff_pos(m_pos, rng), smoke_color });
}
