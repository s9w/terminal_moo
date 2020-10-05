#include "config.h"
#include "cow.h"
#include "ufo.h"

#include <entt/entt.hpp>


moo::Ufo::Ufo(const ScreenCoord initial_pos, const double anim_progress)
   : m_pos(initial_pos)
   , m_animation_frame(5, 1.0, anim_progress)
{

}

constexpr moo::Seconds shooting_interval_s = 1.0;


auto moo::Ufo::progress(
   const Seconds& dt,
   const ScreenCoord& player_pos,
   entt::registry& registry
) -> void
{
   m_animation_frame.progress(dt);
   if (is_invul()) {
      m_hit_timer -= dt;
      if (m_hit_timer < 0.0)
         m_hit_timer = 0.0;
   }
   
   m_shooting_cooldown = std::clamp(m_shooting_cooldown - dt, Seconds{ 0.0 }, shooting_interval_s);

   if (std::holds_alternative<Shoot>(m_strategy)) {
      fire(player_pos, registry);
   }
   else if (std::holds_alternative<Abduct>(m_strategy)) {
      if (m_beaming)
         return;
      const Abduct strategy = std::get<Abduct>(m_strategy);
      Cow& target_cow = registry.get<Cow>(strategy.m_target_cow);
      const ScreenCoord raw_target_pos = target_cow.m_pos.get_screen_pos() + ScreenCoord{0.02, -0.3};
      const PixelCoord target_pixel_coord = get_beam_aligned_pixel_coord(raw_target_pos);
      const bool position_reached = to_pixel_coord(m_pos) == target_pixel_coord;
      if (position_reached) {
         target_cow.m_being_beamed = true;
         m_beaming = true;
      }
      move_towards(target_pixel_coord, dt);
   }
}


auto moo::Ufo::hit() -> bool{
   m_health -= 0.1;
   m_hit_timer = get_config().ufo_hit_invul_duration;
   const bool is_dead = less_equal(m_health, 0.0);
   return is_dead;
}


auto moo::Ufo::is_invul() const -> bool{
   return !is_zero(m_hit_timer.m_value);
}


auto moo::Ufo::fire(
   const ScreenCoord& player_pos, 
   entt::registry& registry
) -> void
{
   if (!is_zero(m_shooting_cooldown.m_value))
      return;
   m_shooting_cooldown = shooting_interval_s;
   const ScreenCoord initial_bullet_pos = m_pos;
   const ScreenCoord norm_pos_diff = get_normalized(player_pos - m_pos);

   auto entity = registry.create();
   registry.emplace<Bullet>(entity, initial_bullet_pos, norm_pos_diff, BulletStyle::Alien);
}


auto moo::Ufo::move_towards(const PixelCoord& target, const Seconds& dt) -> void {
   const ScreenCoord pos_diff = get_screen_coord(target) - m_pos;
   const ScreenCoord pos_change = dt.m_value * m_speed * get_normalized(pos_diff);
   m_pos = m_pos + pos_change;
}


auto moo::Ufo::move_towards(const ScreenCoord& target, const Seconds& dt) -> void{
   const ScreenCoord pos_diff = target - m_pos;
   const ScreenCoord pos_change = dt.m_value * m_speed * get_sanitized_position_diff(get_normalized(pos_diff));
   m_pos = m_pos + pos_change;
}

