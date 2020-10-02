#include "config.h"
#include "bullet.h"
#include "tweening.h"

#if _MSC_VER < 1928 // Visual Studio 2019 version 16.8
constexpr double pi = 3.14159265359;
#else
#include <numbers>
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
   const Seconds dt, 
   std::mt19937_64& rng,
   const RGB smoke_color
) -> bool
{
   if (m_head_alive) {
      constexpr double bullet_speed = 2.5;
      m_gravity_speed = m_gravity_speed + dt.m_value * FractionalPos{ 0.0, moo::get_config().gravity_strength };
      const FractionalPos pos_change = dt.m_value * (bullet_speed * m_trajectory + m_gravity_speed);
      const FractionalPos new_pos = m_pos + pos_change;
      m_trail.expand_trail(rng, smoke_color, new_pos, m_pos);
      m_pos = new_pos;
   }
   m_trail.thin_trail(rng, dt);

   const bool head_can_be_deleted = !m_head_alive || !m_pos.is_on_screen();
   const bool should_be_deleted = m_trail.m_smoke_puffs.empty() && head_can_be_deleted;
   return should_be_deleted;
}


auto moo::Bullet::recolor_puffs(const std::vector<RGB>& smoke_colors) -> void{
   m_trail.recolor_puffs(smoke_colors, m_pos);
}


auto moo::Trail::thin_trail(
   std::mt19937_64& rng,
   const Seconds dt
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
   const RGB smoke_color,
   const FractionalPos& new_bullet_pos,
   const FractionalPos& old_bullet_pos
) -> void
{
   if (!new_bullet_pos.is_on_screen())
      return;
   
   if (m_smoke_puffs.empty()) {
      m_smoke_puffs.push_back({ get_smoke_puff_pos(new_bullet_pos, rng), smoke_color });
      return;
   }

   // Trace the path from the new bullet pos to the last one. That way the density of the smoke does not
   // depend on the framerate.
   constexpr double min_smoke_puff_distance = 0.007;
   const FractionalPos pos_diff = old_bullet_pos - new_bullet_pos;
   const FractionalPos norm_pos_diff = get_normalized(pos_diff);
   const double pos_diff_len = length(pos_diff);
   for (double trace_progress = 0.0; trace_progress < pos_diff_len; trace_progress += min_smoke_puff_distance) {
      const FractionalPos pos = new_bullet_pos + trace_progress * norm_pos_diff;
      m_smoke_puffs.push_back({ get_smoke_puff_pos(pos, rng), smoke_color });
   }
}


auto moo::Trail::recolor_puffs(
   const std::vector<RGB>& smoke_colors,
   const FractionalPos bullet_pos
) -> void{
   for (size_t i = 0; i < m_smoke_puffs.size(); ++i) {
      TrailPuff& puff = m_smoke_puffs[i];
      const double progress = get_rising(length(puff.pos - bullet_pos), 0.0, 1.0);
      const size_t color_index = static_cast<int>(progress * (smoke_colors.size() - 1));
      puff.color = smoke_colors[color_index];
   }
}
