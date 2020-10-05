#include "config.h"
#include "player.h"

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

   /// <summary>If the position diff would not result in at least a change of "half" a pixel,
   /// the resulting position could immediately jump back. For small differences, that would
   /// result in optical flickering</summary>
   [[nodiscard]] auto get_sanitized_position_diff(
      const moo::ScreenCoord& position_diff,
      const int rows,
      const int columns
   ) -> moo::ScreenCoord
   {
      moo::ScreenCoord new_diff = position_diff;
      if (abs(new_diff.x * columns) < 0.5)
         new_diff.x = 0.0;
      if (abs(new_diff.y * rows) < 0.5)
         new_diff.y = 0.0;
      return new_diff;
   }


   /// <summary>Limits the player position to not go too far off screen. Otherwise it
   /// would take a long time to go 'back' after mouse was somewhere else.</summary>
   [[nodiscard]] auto get_limited_pos(
      const moo::ScreenCoord& pos,
      const int rows
   ) -> moo::ScreenCoord
   {
      const double horiz_cage_padding = moo::get_config().horizontal_cage_padding;
      const double vert_cage_padding = 0.5 * horiz_cage_padding; // pixels are about half as high as they are wide
      moo::ScreenCoord new_pos = pos;
      new_pos.x = std::clamp(new_pos.x, 0.0 - horiz_cage_padding, 1.0 + horiz_cage_padding);

      const double helicopter_height = 3.0 / rows;
      const double lowest_height = moo::get_config().sky_fraction + 0.5 * (1.0 - moo::get_config().sky_fraction);
      const double y_max = lowest_height - 0.5 * helicopter_height;
      new_pos.y = std::clamp(new_pos.y, 0.0 - vert_cage_padding, y_max);

      return new_pos;
   }

   constexpr moo::Seconds shooting_interval_s = 0.2;


   [[nodiscard]] auto get_bullet_trajectory(
      std::mt19937_64& rng,
      const double base_spread
   ) -> moo::ScreenCoord
   {
      const double negative_angle_spread = -1.0 * base_spread * 2.0 * pi;
      const double positive_angle_spread = -5.0 * base_spread * negative_angle_spread; // this points down

      std::uniform_real_distribution<double> phi_dist(negative_angle_spread, positive_angle_spread);
      const double phi = phi_dist(rng);
      return moo::get_normalized(moo::ScreenCoord{ std::cos(phi), std::sin(phi) });
   }

} // namespace {}


auto moo::Player::move_towards(
   const ScreenCoord& target_pos,
   const Seconds dt,
   const int rows,
   const int columns
) -> void
{
   ZoneScoped;
   const ScreenCoord diff_to_target = get_sanitized_position_diff(target_pos - m_pos, rows, columns);
   const auto pos_diff = dt.m_value * m_speed * get_indep_normalized(diff_to_target);
   m_pos = get_limited_pos(m_pos + pos_diff, rows);

   m_shooting_cooldown = std::clamp(m_shooting_cooldown - dt, Seconds{ 0.0 }, shooting_interval_s);
}


auto moo::Player::try_to_fire(
   std::mt19937_64& rng
   , entt::registry& registry
) -> void 
{
   if (!is_zero(m_shooting_cooldown.m_value))
      return;
   m_shooting_cooldown = shooting_interval_s;
   const ScreenCoord initial_bullet_pos = m_pos + ScreenCoord{0.05, 0.0};

   auto entity = registry.create();
   registry.emplace<Bullet>(entity, initial_bullet_pos, get_bullet_trajectory(rng, 0.1), BulletStyle::Rocket);
   //return std::make_optional<Bullet>(initial_bullet_pos, get_bullet_trajectory(rng, 0.1), BulletStyle::Rocket);
   //return Bullet(initial_bullet_pos, get_bullet_trajectory(rng, 0.1), BulletStyle::Rocket);
}
