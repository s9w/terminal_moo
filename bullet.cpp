#include "bullet.h"

#include "config.h"
#include "rng.h"
#include "tweening.h"

#if _MSC_VER < 1928 // Visual Studio 2019 version 16.8
constexpr double pi = 3.14159265359;
#else
#include <numbers>
constexpr double pi = std::numbers::pi;
#endif // _MSC_VER

namespace {


   [[nodiscard]] auto get_smoke_puff_pos(
      const moo::ScreenCoord& rocket_pos,
      const double smoke_spread,
      const moo::ScreenCoord& bullet_trajectory,
      const moo::BulletStyle style,
      const double path_progress
   ) -> moo::ScreenCoord
   {
      if (style == moo::BulletStyle::Rocket) {
         std::uniform_real_distribution<double> smoke_spread_dist(-smoke_spread, smoke_spread);
         return rocket_pos + moo::ScreenCoord{ smoke_spread_dist(moo::get_rng()), smoke_spread_dist(moo::get_rng()) };
      }
      constexpr double alien_trail_sin_freq = 50.0;
      constexpr double alien_trail_sin_ampl = 0.02;
      return rocket_pos + alien_trail_sin_ampl * std::sin(alien_trail_sin_freq * path_progress) * get_orthogonal(bullet_trajectory);
   }

} // namespace {}


moo::Bullet::Bullet(const ScreenCoord& initial_pos, const ScreenCoord& trajectory, const BulletStyle style, entt::entity trail)
   : m_pos(initial_pos)
   , m_initial_pos(initial_pos)
   , m_trajectory(trajectory)
   , m_style(style)
   , m_trail(trail)
{

}


auto moo::Bullet::progress(
   const Seconds dt
) -> void
{
   if (m_head_alive) {
      double gravity_accel = moo::get_config().gravity_strength;
      if (m_style == BulletStyle::Alien)
         gravity_accel = 0.0;
      m_gravity_speed += dt.m_value * ScreenCoord{ 0.0, gravity_accel };
      const ScreenCoord pos_change = dt.m_value * (m_bullet_speed * m_trajectory + m_gravity_speed);
      const ScreenCoord new_pos = m_pos + pos_change;
      m_pos = new_pos;
   }
}


moo::Trail::Trail(const BulletStyle style, entt::entity bullet)
   : m_style(style)
   , m_bullet_ref(bullet)
{
}


auto moo::Trail::thin_trail(
   const Seconds dt
) -> void
{
   std::uniform_real_distribution<double> one_dist(0.0, 1.0);
   auto it = m_smoke_puffs.begin();
   while (it != m_smoke_puffs.end()) {
      const double elim_threshold = 5.0 * dt;
      if (one_dist(get_rng()) < elim_threshold)
         it = m_smoke_puffs.erase(it);
      else
         ++it;
   }
}


auto moo::Trail::add_puff(
   const ScreenCoord& new_bullet_pos,
   const ScreenCoord& old_bullet_pos,
   const double path_progress
) -> void
{
   if (!new_bullet_pos.is_on_screen())
      return;

   const ScreenCoord pos_diff = old_bullet_pos - new_bullet_pos;
   const ScreenCoord norm_pos_diff = get_normalized(pos_diff);

   double smoke_spread = moo::get_config().smoke_puff_spread;
   if (m_style == BulletStyle::Alien)
      smoke_spread = 0.0;
   const RGB smoke_color = get_shot_trail_start_color(m_style);
   if (m_smoke_puffs.empty()) {
      m_smoke_puffs.push_back({ get_smoke_puff_pos(new_bullet_pos, smoke_spread, norm_pos_diff, m_style, path_progress), smoke_color });
      return;
   }

   // Trace the path from the new bullet pos to the last one. That way the density of the smoke does not
   // depend on the framerate.
   constexpr double min_smoke_puff_distance = 0.007;
   
   const double pos_diff_len = get_length(pos_diff);
   for (double trace_progress = 0.0; trace_progress < pos_diff_len; trace_progress += min_smoke_puff_distance) {
      const ScreenCoord pos = new_bullet_pos + trace_progress * norm_pos_diff;
      m_smoke_puffs.push_back({ get_smoke_puff_pos(pos, smoke_spread, norm_pos_diff, m_style, path_progress), smoke_color });
   }
}


auto moo::Trail::update_puff_colors(
   const ScreenCoord& bullet_pos
) -> void
{
   const RGB start_color = get_shot_trail_start_color(m_style);
   const RGB end_color = get_shot_trail_end_color(m_style);
   for (size_t i = 0; i < m_smoke_puffs.size(); ++i) {
      TrailPuff& puff = m_smoke_puffs[i];
      const double progress = get_rising(get_length(puff.pos - bullet_pos), 0.0, 0.5);
      puff.color = get_color_mix(start_color, end_color, progress);
   }
}
