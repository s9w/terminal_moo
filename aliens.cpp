#include "aliens.h"
#include <entt/entt.hpp>

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

}


void moo::Aliens::process_bullets(
   Bullet& bullet,
   entt::registry& registry
){
   auto ufos = registry.view<Ufo>();
   for (auto entity : ufos) {
      Ufo& ufo = ufos.get<Ufo>(entity);
      bool ufo_killed = false;
      if (does_bullet_hit(bullet, ufo, m_ufo_dimensions)) {
         bullet.m_head_alive = false;
         ufo_killed = ufo.hit();
      }
      if (ufo_killed)
         registry.remove(entity);
   }
}


void moo::Aliens::abduct_cow(const ID target_id, entt::registry& registry){
   if(registry.empty<Ufo>())
      return;
   auto ufo_entity = registry.view<Ufo>().front();
   Ufo& ufo = registry.get<Ufo>(ufo_entity);
}
