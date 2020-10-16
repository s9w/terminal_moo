#include "player.h"

#include "config.h"
#include "rng.h"
#include "trail.h"

#include <algorithm>

#include <entt/entt.hpp>
#include <Tracy.hpp>


#if _MSC_VER < 1928 // Visual Studio 2019 version 16.8
constexpr double pi = 3.14159265359;
#else
#include <numbers>
constexpr double pi = std::numbers::pi;
#endif // _MSC_VER


namespace {

   /// <summary>Limits the player position to not go too far off screen. Otherwise it
   /// would take a long time to go 'back' after mouse was somewhere else.</summary>
   [[nodiscard]] auto get_limited_pos(
      const moo::ScreenCoord& pos
   ) -> moo::ScreenCoord
   {
      const double horiz_cage_padding = moo::get_config().horizontal_cage_padding;
      const double vert_cage_padding = 0.5 * horiz_cage_padding; // pixels are about half as high as they are wide
      moo::ScreenCoord new_pos = pos;
      new_pos.x = std::clamp(new_pos.x, 0.0 - horiz_cage_padding, 1.0 + horiz_cage_padding);

      const double helicopter_height = 3.0 / moo::static_rows;
      const double lowest_height = moo::get_config().sky_fraction + 0.5 * (1.0 - moo::get_config().sky_fraction);
      const double y_max = lowest_height - 0.5 * helicopter_height;
      new_pos.y = std::clamp(new_pos.y, 0.0 - vert_cage_padding, y_max);

      return new_pos;
   }


   [[nodiscard]] auto get_bullet_trajectory(
      const double base_spread
   ) -> moo::ScreenCoord
   {
      const double negative_angle_spread = -1.0 * base_spread * 2.0 * pi;
      const double positive_angle_spread = -5.0 * base_spread * negative_angle_spread; // this points down

      std::uniform_real_distribution<double> phi_dist(negative_angle_spread, positive_angle_spread);
      const double phi = phi_dist(moo::get_rng());
      return moo::get_normalized(moo::ScreenCoord{ std::cos(phi), std::sin(phi) });
   }

} // namespace {}


auto moo::Player::move_towards(
   const ScreenCoord& target_pos,
   const Seconds dt
) -> void
{
   ZoneScoped;
   const ScreenCoord diff_to_target = get_sanitized_position_diff(target_pos - m_pos);
   const auto pos_diff = dt.m_value * m_speed * get_indep_normalized(diff_to_target);
   m_pos = get_limited_pos(m_pos + pos_diff);

   m_shooting_cooldown.iterate(dt);
}


auto moo::Player::try_to_fire(
   entt::registry& registry
) -> void 
{
   if(!m_shooting_cooldown.get_ready())
      return;
   m_shooting_cooldown.restart();
   const ScreenCoord initial_bullet_pos = m_pos + ScreenCoord{0.05, 0.0};

   auto bullet_entity = registry.create();
   auto trail_entity = registry.create();
   registry.emplace<Bullet>(bullet_entity, initial_bullet_pos, get_bullet_trajectory(0.1), BulletStyle::Rocket, trail_entity);
   registry.emplace<Trail>(trail_entity, BulletStyle::Rocket, bullet_entity);
}


auto moo::Player::is_invul() const -> bool{
   return !is_zero(m_hit_timer.m_value);
}

