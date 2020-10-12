#include "ufo.h"

#include "config.h"
#include "entt_types.h"
#include "lane_position.h"

#include <entt/entt.hpp>


moo::Ufo::Ufo(const ScreenCoord& initial_pos, const double anim_progress)
   : m_pos(initial_pos)
   , m_animation_frame(5, 1.0, anim_progress)
{

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
   m_shooting_cooldown = get_config().ufo_shooting_interal;
   const ScreenCoord initial_bullet_pos = m_pos;
   const ScreenCoord norm_pos_diff = get_normalized(player_pos - m_pos);

   auto bullet_entity = registry.create();
   auto trail_entity = registry.create();
   registry.emplace<Bullet>(bullet_entity, initial_bullet_pos, norm_pos_diff, BulletStyle::Alien, trail_entity);
   registry.emplace<Trail>(trail_entity, BulletStyle::Alien, bullet_entity);
}

