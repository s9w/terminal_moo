#include "config.h"
#include "ufo.h"

moo::Ufo::Ufo(const FractionalPos initial_pos, const double anim_progress)
   : m_pos(initial_pos)
   , m_animation_frame(5, 1.0, anim_progress)
{

}

auto moo::Ufo::progress(const Seconds& dt) -> void{
   m_animation_frame.progress(dt);
   if (is_hit()) {
      m_hit_timer -= dt;
      if (m_hit_timer < 0.0)
         m_hit_timer = 0.0;
   }
}

auto moo::Ufo::hit() -> bool{
   m_health -= 0.1;
   m_hit_timer = get_config().ufo_hit_invul_duration;
   const bool is_dead = less_equal(m_health, 0.0);
   return is_dead;
}


auto moo::Ufo::is_hit() const -> bool{
   return !is_zero(m_hit_timer.m_value);
}

