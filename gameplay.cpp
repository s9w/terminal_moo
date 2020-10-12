#include "gameplay.h"

#include "entt_types.h"
#include "lane_position.h"
#include "screencoord.h"
#include "ufo.h"

#include <entt/entt.hpp>




namespace {

   [[nodiscard]] auto get_closest_cow(
      const moo::ScreenCoord& pos,
      entt::registry& registry
   ) -> std::optional<entt::entity>
   {
      const auto cows = registry.view<moo::IsCow, moo::LanePosition>();
      if (cows.empty())
         return std::nullopt;
      entt::entity closest_entity;
      double closest_dist = 10.0;
      cows.each([&](entt::entity entity, const moo::LanePosition& cow_pos) {
         const double dist = std::abs(cow_pos.m_x_pos - pos.x);
         if (dist < closest_dist) {
            closest_entity = entity;
            closest_dist = dist;
         }
         });
      return closest_entity;
   }

} // namespace {}


auto moo::set_ufo_abducting(
   entt::entity ufo_entity,
   entt::registry& registry
) -> void
{
   if (!registry.valid(ufo_entity)) {
      printf("invalid entity\n");
      std::terminate();
   }
   moo::Ufo& ufo = registry.get<moo::Ufo>(ufo_entity);
   auto cows = registry.view<moo::IsCow>();
   const auto closest_cow = get_closest_cow(ufo.m_pos, registry);
   if (!closest_cow.has_value()) {
      ufo.m_strategy = moo::Shoot{};
      ufo.m_beaming = false;
      return;
   }
   registry.view<moo::Ufo>().each([&](moo::Ufo& ufo) {
      ufo.m_strategy = moo::Abduct{ closest_cow.value() };
      });
}


auto moo::set_ufo_shooting(
   entt::entity ufo_entity,
   entt::registry& registry
) -> void
{
   if (!registry.valid(ufo_entity))
      return;
   moo::Ufo& ufo = registry.get<moo::Ufo>(ufo_entity);
   if (std::holds_alternative<moo::Abduct>(ufo.m_strategy)) {
      const moo::Abduct& strategy = std::get<moo::Abduct>(ufo.m_strategy);
      moo::BeingBeamed& being_beamed = registry.get<moo::BeingBeamed>(strategy.m_target_cow);
      being_beamed.value = false;
      ufo.m_beaming = false;
   }
   ufo.m_strategy = moo::Shoot{};
}
