#include "aliens.h"

namespace {

   [[nodiscard]] constexpr auto does_bullet_hit(
      const moo::Bullet& bullet,
      const moo::Ufo& ufo,
      const moo::ScreenCoord& ufo_dimensions
   ) -> bool
   {
      return bullet.m_head_alive &&
         bullet.m_style == moo::BulletStyle::Rocket &&
         !ufo.is_invul() &&
         is_hit(bullet.m_pos, ufo.m_pos, ufo_dimensions);
   }

} // namespace {}


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
      if (does_bullet_hit(bullet, *ufo_it, m_ufo_dimensions)) {
         bullet.m_head_alive = false;
         ufo_killed = ufo_it->hit();
      }
      if (ufo_killed)
         ufo_it = m_ufos.erase(ufo_it);
      else
         ++ufo_it;
   }
}


void moo::Aliens::abduct_cow(const ID target_id){
   if (m_ufos.empty())
      return;
   m_ufos.front().m_strategy = Abduct{ target_id };
}
