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
      const double smoke_spread,
      const moo::ScreenFraction& bullet_trajectory,
      const moo::BulletStyle style,
      const double path_progress
   ) -> moo::ScreenFraction
   {
      if (style == moo::BulletStyle::Rocket) {
         std::uniform_real_distribution<double> smoke_spread_dist(-smoke_spread, smoke_spread);
         return rocket_pos + moo::ScreenFraction{ smoke_spread_dist(rng), smoke_spread_dist(rng) };
      }
      constexpr double alien_trail_sin_freq = 50.0;
      constexpr double alien_trail_sin_ampl = 0.02;
      return rocket_pos + alien_trail_sin_ampl * std::sin(alien_trail_sin_freq * path_progress) * get_orthogonal(bullet_trajectory);
   }

} // namespace {}


moo::Bullet::Bullet(const ScreenFraction& initial_pos, const ScreenFraction& trajectory, const BulletStyle style)
   : m_pos(initial_pos)
   , m_initial_pos(initial_pos)
   , m_trajectory(trajectory)
   , m_style(style)
   , m_trail(style)
{

}


auto moo::Bullet::progress(
   const Seconds dt, 
   std::mt19937_64& rng
) -> bool
{
   if (m_head_alive) {
      double gravity_strength = moo::get_config().gravity_strength;
      if (m_style == BulletStyle::Alien)
         gravity_strength = 0.0;
      m_gravity_speed = m_gravity_speed + dt.m_value * ScreenFraction{ 0.0, gravity_strength };
      const ScreenFraction pos_change = dt.m_value * (m_bullet_speed * m_trajectory + m_gravity_speed);
      const ScreenFraction new_pos = m_pos + pos_change;
      {
         const double path_progress = length(m_pos - m_initial_pos);
         m_trail.add_puff(rng, new_pos, m_pos, m_style, path_progress);
      }
      m_pos = new_pos;
   }
   m_trail.thin_trail(rng, dt);

   const bool head_can_be_deleted = !m_head_alive || !m_pos.is_on_screen();
   const bool should_be_deleted = m_trail.m_smoke_puffs.empty() && head_can_be_deleted;
   return should_be_deleted;
}


auto moo::Bullet::update_puff_colors() -> void{
   m_trail.update_puff_colors(m_pos);
}


moo::Trail::Trail(const BulletStyle style)
   : m_style(style)
{
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


auto moo::Trail::add_puff(
   std::mt19937_64& rng,
   const ScreenFraction& new_bullet_pos,
   const ScreenFraction& old_bullet_pos,
   const BulletStyle style,
   const double path_progress
) -> void
{
   if (!new_bullet_pos.is_on_screen())
      return;

   const ScreenFraction pos_diff = old_bullet_pos - new_bullet_pos;
   const ScreenFraction norm_pos_diff = get_normalized(pos_diff);

   double smoke_spread = moo::get_config().smoke_puff_spread;
   if (m_style == BulletStyle::Alien)
      smoke_spread = 0.0;
   const RGB smoke_color = GameColors::get_shot_trail_start_color(m_style);
   if (m_smoke_puffs.empty()) {
      m_smoke_puffs.push_back({ get_smoke_puff_pos(new_bullet_pos, rng, smoke_spread, norm_pos_diff, style, path_progress), smoke_color });
      return;
   }

   // Trace the path from the new bullet pos to the last one. That way the density of the smoke does not
   // depend on the framerate.
   constexpr double min_smoke_puff_distance = 0.007;
   
   const double pos_diff_len = length(pos_diff);
   for (double trace_progress = 0.0; trace_progress < pos_diff_len; trace_progress += min_smoke_puff_distance) {
      const ScreenFraction pos = new_bullet_pos + trace_progress * norm_pos_diff;
      m_smoke_puffs.push_back({ get_smoke_puff_pos(pos, rng, smoke_spread, norm_pos_diff, style, path_progress), smoke_color });
   }
}


auto moo::Trail::update_puff_colors(
   const ScreenFraction& bullet_pos
) -> void
{
   const RGB start_color = GameColors::get_shot_trail_start_color(m_style);
   const RGB end_color = GameColors::get_shot_trail_end_color(m_style);
   for (size_t i = 0; i < m_smoke_puffs.size(); ++i) {
      TrailPuff& puff = m_smoke_puffs[i];
      const double progress = get_rising(length(puff.pos - bullet_pos), 0.0, 0.5);
      puff.color = get_color_mix(start_color, end_color, progress);
   }
}
