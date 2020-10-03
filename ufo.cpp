#include "config.h"
#include "ufo.h"

moo::Ufo::Ufo(const ScreenFraction initial_pos, const double anim_progress)
   : m_pos(initial_pos)
   , m_animation_frame(5, 1.0, anim_progress)
{

}

constexpr moo::Seconds shooting_interval_s = 1.0;

auto moo::Ufo::progress(const Seconds& dt) -> void{
   m_animation_frame.progress(dt);
   if (is_invul()) {
      m_hit_timer -= dt;
      if (m_hit_timer < 0.0)
         m_hit_timer = 0.0;
   }
   
   m_shooting_cooldown = std::clamp(m_shooting_cooldown - dt, Seconds{ 0.0 }, shooting_interval_s);
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


auto moo::Ufo::fire(const ScreenFraction& player_pos) -> std::optional<Bullet>{
   if (!is_zero(m_shooting_cooldown.m_value))
      return std::nullopt;
   m_shooting_cooldown = shooting_interval_s;
   const ScreenFraction initial_bullet_pos = m_pos;
   const ScreenFraction trajectory = get_normalized(player_pos - m_pos);
   Bullet bullet(initial_bullet_pos, trajectory, BulletStyle::Alien);
   return bullet;
}

