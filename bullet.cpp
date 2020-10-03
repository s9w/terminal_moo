#include "bullet.h"
#include "config.h"
#include "game_colors.h"
#include "tweening.h"

#if _MSC_VER < 1928 // Visual Studio 2019 version 16.8
constexpr double pi = 3.14159265359;
#else
#include <numbers>
constexpr double pi = std::numbers::pi;
#endif // _MSC_VER

namespace {

   [[nodiscard]] auto get_smoke_puff_pos(
      const moo::ScreenFraction& rocket_pos,
      std::mt19937_64& rng,
      const double smoke_spread
   ) -> moo::ScreenFraction
   {
      std::uniform_real_distribution<double> smoke_spread_dist(-smoke_spread, smoke_spread);
      return rocket_pos + moo::ScreenFraction{ smoke_spread_dist(rng), smoke_spread_dist(rng) };
   }

} // namespace {}


moo::Bullet::Bullet(const ScreenFraction& initial_pos, const ScreenFraction& trajectory, const Style style)
   : m_pos(initial_pos)
   , m_initial_pos(initial_pos)
   , m_trajectory(trajectory)
   , m_style(style)
{

}


auto moo::Bullet::progress(
   const Seconds dt, 
   std::mt19937_64& rng,
   const RGB smoke_color
) -> bool
{
   if (m_head_alive) {
      double gravity_strength = moo::get_config().gravity_strength;
      if (m_style == Style::Alien)
         gravity_strength = 0.0;
      m_gravity_speed = m_gravity_speed + dt.m_value * ScreenFraction{ 0.0, gravity_strength };
      const ScreenFraction pos_change = dt.m_value * (m_bullet_speed * m_trajectory + m_gravity_speed);
      const ScreenFraction new_pos = m_pos + pos_change;
      {
         double smoke_spread = moo::get_config().smoke_puff_spread;
         if (m_style == Style::Alien)
            smoke_spread = 0.0;
         m_trail.expand_trail(rng, smoke_color, new_pos, m_pos, smoke_spread);
      }
      m_pos = new_pos;
   }
   m_trail.thin_trail(rng, dt);

   const bool head_can_be_deleted = !m_head_alive || !m_pos.is_on_screen();
   const bool should_be_deleted = m_trail.m_smoke_puffs.empty() && head_can_be_deleted;
   return should_be_deleted;
}


auto moo::Bullet::recolor_puffs(const RGB& rocket_orange) -> void{
   const RGB target_color = GameColors::get_shot_trail_end_color(m_style);
   m_trail.recolor_puffs(m_pos, rocket_orange, target_color);
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
   const ScreenFraction& new_bullet_pos,
   const ScreenFraction& old_bullet_pos,
   const double smoke_spread
) -> void
{
   if (!new_bullet_pos.is_on_screen())
      return;

   if (m_smoke_puffs.empty()) {
      m_smoke_puffs.push_back({ get_smoke_puff_pos(new_bullet_pos, rng, smoke_spread), smoke_color });
      return;
   }

   // Trace the path from the new bullet pos to the last one. That way the density of the smoke does not
   // depend on the framerate.
   constexpr double min_smoke_puff_distance = 0.007;
   const ScreenFraction pos_diff = old_bullet_pos - new_bullet_pos;
   const ScreenFraction norm_pos_diff = get_normalized(pos_diff);
   const double pos_diff_len = length(pos_diff);
   for (double trace_progress = 0.0; trace_progress < pos_diff_len; trace_progress += min_smoke_puff_distance) {
      const ScreenFraction pos = new_bullet_pos + trace_progress * norm_pos_diff;
      m_smoke_puffs.push_back({ get_smoke_puff_pos(pos, rng, smoke_spread), smoke_color });
   }
}


auto moo::Trail::recolor_puffs(
   const ScreenFraction& bullet_pos,
   const RGB& rocket_orange,
   const RGB& end_color
) -> void{
   for (size_t i = 0; i < m_smoke_puffs.size(); ++i) {
      TrailPuff& puff = m_smoke_puffs[i];
      const double progress = get_rising(length(puff.pos - bullet_pos), 0.0, 1.0);
      puff.color = get_color_mix(rocket_orange, end_color, progress);
   }
}
