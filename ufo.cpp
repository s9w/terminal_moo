#include "config.h"
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

   //if (std::holds_alternative<Shoot>(m_strategy)) {
   //   fire(player_pos, registry);
   //}
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


auto moo::Ufo::fire(const ScreenCoord& player_pos, entt::registry& registry) -> void{
   if (!is_zero(m_shooting_cooldown.m_value))
      return;
   m_shooting_cooldown = shooting_interval_s;
   const ScreenCoord initial_bullet_pos = m_pos;
   const ScreenCoord norm_pos_diff = get_normalized(player_pos - m_pos);

   auto entity = registry.create();
   registry.emplace<Bullet>(entity, initial_bullet_pos, norm_pos_diff, BulletStyle::Alien);
}

