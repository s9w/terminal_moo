#include "gameplay.h"

#include "config.h"
#include "entt_helper.h"
#include "entt_types.h"
#include "image.h"
#include "lane_position.h"
#include "rng.h"
#include "screencoord.h"
#include "strategy.h"
#include "ufo.h"

#include <entt/entt.hpp>




namespace {

   template<class T>
   [[nodiscard]] auto get_closest_cow_if(
      const moo::ScreenCoord& pos,
      entt::registry& registry,
      const T& pred
   ) -> std::optional<entt::entity>
   {
      const auto cows = registry.view<moo::IsCow, moo::LanePosition>();
      if (cows.empty())
         return std::nullopt;
      entt::entity closest_entity;
      double closest_dist = 10.0;
      cows.each([&](entt::entity entity, const moo::LanePosition& cow_pos) {
         const double dist = std::abs(cow_pos.m_x_pos - pos.x);
         if (dist < closest_dist && pred(cow_pos)) {
            closest_entity = entity;
            closest_dist = dist;
         }
         });
      if (!registry.valid(closest_entity))
         return std::nullopt;
      return closest_entity;
   }


   /// <summary>First look for closest cow on the right. Then look everywhere.</summary>
   [[nodiscard]] auto get_closest_cow(
      const moo::ScreenCoord& pos,
      entt::registry& registry
   ) -> std::optional<entt::entity>
   {
      const auto is_right = [&](const moo::LanePosition& cow_pos) {
         return cow_pos.m_x_pos > pos.x;
      };
      const auto always_true = [](const moo::LanePosition&) {return true; };
      const auto closest_right_cow = get_closest_cow_if(pos, registry, is_right);
      if (closest_right_cow.has_value())
         return closest_right_cow.value();
      return get_closest_cow_if(pos, registry, always_true);
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


   [[nodiscard]] auto get_cow_width(entt::registry& registry) -> double {
      const auto cow_anim_ett = registry.view<moo::CowAnimation>().front();
      const moo::CowAnimation& cow_animation = registry.get<moo::CowAnimation>(cow_anim_ett);
      const double fractional_cow_bitmap_width = 1.0 * cow_animation.m_width / moo::static_columns;
      return fractional_cow_bitmap_width;
   }


   [[nodiscard]] auto get_ufo_speed(const int level) -> double {
      return moo::get_config().ufo_base_speed + level * moo::get_config().ufo_speed_bonus;
   }

} // namespace {}


auto moo::set_ufo_abducting(
   Ufo& ufo,
   entt::registry& registry
) -> void
{
   const auto closest_cow = get_closest_cow(ufo.m_pos, registry);
   if (!closest_cow.has_value()) {
      ufo.m_strategy = moo::Shoot{};
      ufo.m_beaming = false;
      return;
   }
   ufo.m_strategy = moo::Abduct{ closest_cow.value() };
}


auto moo::set_ufo_shooting(
   Ufo& ufo,
   entt::registry& registry
) -> void
{
   if (std::holds_alternative<moo::Abduct>(ufo.m_strategy)) {
      const moo::Abduct& strategy = std::get<moo::Abduct>(ufo.m_strategy);
      if (registry.valid(strategy.m_target_cow)) {
         moo::BeingBeamed& being_beamed = registry.get<moo::BeingBeamed>(strategy.m_target_cow);
         being_beamed.value = false;
      }
      ufo.m_beaming = false;
   }
   ufo.m_strategy = moo::Shoot{};
}


auto moo::ufo_progress(
   const Seconds& dt, 
   const ScreenCoord& player_pos,
   Ufo& ufo,
   entt::registry& registry,
   const int level
) -> void
{
   ufo.m_animation_frame.progress(dt);
   if (ufo.is_invul()) {
      ufo.m_hit_timer -= dt;
      if (ufo.m_hit_timer < 0.0)
         ufo.m_hit_timer = 0.0;
   }

   ufo.m_shooting_cooldown.iterate(dt);

   if (std::holds_alternative<Shoot>(ufo.m_strategy)) {
      ufo.fire(player_pos, registry);
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
      const bool position_reached = to_pixel_coord(ufo.m_pos) == target_pixel_coord;
      if (position_reached) {
         BeingBeamed& being_beamed = registry.get<BeingBeamed>(strategy.m_target_cow);
         being_beamed.value = true;
         ufo.m_beaming = true;
      }
      ufo.m_pos = get_new_ufo_pos(ufo.m_pos, get_ufo_speed(level), target_pixel_coord, dt);
   }
}


auto moo::get_new_cow_position(entt::registry& registry) -> std::optional<LanePosition> {
   constexpr double free_area_threshold = 0.8;
   auto cows = registry.view<IsCow, LanePosition>();
   for (auto cow : cows) {
      const bool cow_in_right_area = cows.get<LanePosition>(cow).m_x_pos > free_area_threshold;
      if (cow_in_right_area)
         return std::nullopt;
   }
   LanePosition new_cow_position = get_new_lane_position(get_ground_row_height(), get_cow_width(registry));
   return new_cow_position;
}


auto moo::spawn_new_cows(
   entt::registry& registry,
   const bool inactive
) -> void
{
   if (inactive)
      return;
   std::uniform_real_distribution<double> grazing_progress_dist(0.0, 1.0);
   if (const auto cow_pos = get_new_cow_position(registry); cow_pos.has_value()) {
      auto cow_entity = registry.create();
      registry.emplace<IsCow>(cow_entity);
      registry.emplace<Alpha>(cow_entity, 1.0);
      registry.emplace<BeingBeamed>(cow_entity, false);
      registry.emplace<LanePosition>(cow_entity, cow_pos.value());
      registry.emplace<AnimationFrame>(cow_entity, 2, 1.0, grazing_progress_dist(get_rng()));
      registry.emplace<CowVariant>(cow_entity, get_random_view_entity<CowAnimation>(registry));
   }
}
