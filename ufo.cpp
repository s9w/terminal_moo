#include "ufo.h"

moo::Ufo::Ufo(const FractionalPos initial_pos, const double anim_progress)
   : m_pos(initial_pos)
   , m_animation_frame(5, 1.0, anim_progress)
{

}

auto moo::Ufo::progress(const Seconds& dt) -> void{
   m_is_hit = false;
   m_animation_frame.progress(dt);
}

auto moo::Ufo::hit() -> bool{
   m_health -= 0.1;
   m_is_hit = true;
   return less_equal(m_health, 0.0);
}

