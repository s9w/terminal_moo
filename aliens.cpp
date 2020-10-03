#include "aliens.h"

moo::Aliens::Aliens(const ScreenFraction& ufo_dimensions)
   : m_ufo_dimensions(ufo_dimensions)
{
   m_ufos.emplace_back(ScreenFraction{ 0.8, 0.3 }, 0.0);
   m_ufos.emplace_back(ScreenFraction{ 0.9, 0.5 }, 0.5);
   m_ufos.emplace_back(ScreenFraction{ 0.6, 0.6 }, 1.0);
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
