#include "aliens.h"

#include "entt_types.h"

#include <entt/entt.hpp>

namespace {


} // namespace {}


moo::Aliens::Aliens(const ScreenCoord& ufo_dimensions)
   : m_ufo_dimensions(ufo_dimensions)
{

}


void moo::Aliens::process_bullets(
   Bullet& bullet,
   entt::registry& registry
) {
   for (auto entity : registry.view<Ufo>()) {
      Ufo& ufo = registry.get<Ufo>(entity);
      bool ufo_killed = false;
      if (!ufo.is_invul() && does_bullet_hit(bullet, ufo.m_pos, m_ufo_dimensions, BulletStyle::Rocket)) {
         bullet.m_head_alive = false;
         ufo_killed = ufo.hit();
      }
      if (ufo_killed) {
         registry.destroy(entity);
         if (std::holds_alternative<Abduct>(ufo.m_strategy)) {
            auto target_cow = std::get<Abduct>(ufo.m_strategy).m_target_cow;
            BeingBeamed& cow_being_beamed = registry.get<BeingBeamed>(target_cow);
            cow_being_beamed.value = false;
         }
      }
   }
}
