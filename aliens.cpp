#include "aliens.h"

moo::Aliens::Aliens(const ScreenCoord& ufo_dimensions)
   : m_ufo_dimensions(ufo_dimensions)
{
   const ScreenCoord desired{ 0.83, 0.3 };
   m_ufos.emplace_back(get_beam_aligned_coord(desired), 0.0);
}


void moo::Aliens::process_bullets(Bullet& bullet){
   auto ufo_it = m_ufos.begin();
   while (ufo_it != m_ufos.end()) {
      bool ufo_killed = false;
      if (bullet.m_head_alive && bullet.m_style == BulletStyle::Rocket && !ufo_it->is_invul() && is_hit(bullet.m_pos, ufo_it->m_pos, m_ufo_dimensions.x, m_ufo_dimensions.y)) {
         bullet.m_head_alive = false;
         ufo_killed = ufo_it->hit();
      }
      if (ufo_killed)
         ufo_it = m_ufos.erase(ufo_it);
      else
         ++ufo_it;
   }
}
