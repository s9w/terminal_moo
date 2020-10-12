#include "gameplay.h"

#include "config.h"
#include "entt_types.h"
#include "lane_position.h"
#include "screencoord.h"
#include "strategy.h"
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


   auto get_new_ufo_pos(
      const moo::ScreenCoord& ufo_pos,
      const double ufo_speed,
      const moo::PixelCoord& target,
      const moo::Seconds& dt
   ) -> moo::ScreenCoord
   {
      const moo::ScreenCoord pos_diff = get_screen_coord(target) - ufo_pos;
      const moo::ScreenCoord pos_change = dt.m_value * ufo_speed * moo::get_normalized(pos_diff);
      return ufo_pos + pos_change;
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
   moo::Ufo& ufo = registry.get<Ufo>(ufo_entity);
   auto cows = registry.view<IsCow>();
   const ScreenCoord& ufo_pos = registry.get<ScreenCoord>(ufo_entity);
   const auto closest_cow = get_closest_cow(ufo_pos, registry);
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


auto moo::ufo_progress(
   const Seconds& dt, 
   const ScreenCoord& player_pos,
   Ufo& ufo,
   ScreenCoord& ufo_pos, 
   entt::registry& registry
) -> void
{
   ufo.m_animation_frame.progress(dt);
   if (ufo.is_invul()) {
      ufo.m_hit_timer -= dt;
      if (ufo.m_hit_timer < 0.0)
         ufo.m_hit_timer = 0.0;
   }

   ufo.m_shooting_cooldown = std::clamp(ufo.m_shooting_cooldown - dt, Seconds{ 0.0 }, get_config().ufo_shooting_interal);

   if (std::holds_alternative<Shoot>(ufo.m_strategy)) {
      ufo.fire(player_pos, ufo_pos, registry);
   }
   else if (std::holds_alternative<Abduct>(ufo.m_strategy)) {
      if (ufo.m_beaming)
         return;
      const Abduct strategy = std::get<Abduct>(ufo.m_strategy);
      if (!registry.valid(strategy.m_target_cow)) { // moved off screen
         ufo.m_strategy = Shoot{};
         return;
      }
      const ScreenCoord raw_target_pos = registry.get<LanePosition>(strategy.m_target_cow).get_screen_pos() + ScreenCoord{ 0.0, -0.3 };
      const PixelCoord target_pixel_coord = get_beam_aligned_pixel_coord(raw_target_pos);
      const bool position_reached = to_pixel_coord(ufo_pos) == target_pixel_coord;
      if (position_reached) {
         BeingBeamed& being_beamed = registry.get<BeingBeamed>(strategy.m_target_cow);
         being_beamed.value = true;
         ufo.m_beaming = true;
      }
      ufo_pos = get_new_ufo_pos(ufo_pos, get_config().ufo_speed, target_pixel_coord, dt);
   }
}
