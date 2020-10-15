#include "trail.h"

#include "config.h"
#include "helpers.h"
#include "rng.h"
#include "tweening.h"


namespace {

   using namespace moo;

   [[nodiscard]] auto get_smoke_puff_pos(
      const ScreenCoord& rocket_pos,
      const double smoke_spread,
      const ScreenCoord& bullet_trajectory,
      const BulletStyle style,
      const double path_progress
   ) -> ScreenCoord
   {
      if (style == BulletStyle::Rocket) {
         std::uniform_real_distribution<double> smoke_spread_dist(-smoke_spread, smoke_spread);
         return rocket_pos + ScreenCoord{ smoke_spread_dist(get_rng()), smoke_spread_dist(get_rng()) };
      }
      constexpr double alien_trail_sin_freq = 50.0;
      constexpr double alien_trail_sin_ampl = 0.02;
      return rocket_pos + alien_trail_sin_ampl * std::sin(alien_trail_sin_freq * path_progress) * get_orthogonal(bullet_trajectory);
   }

} // namespace {}


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
