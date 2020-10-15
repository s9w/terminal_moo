#pragma once

#include "lane_position.h"

#include <entt/fwd.hpp>

#include <optional>


namespace moo {

   struct Seconds;
   struct ScreenCoord;
   struct Ufo;
   struct Bullet;

   struct BulletRunResult {
      bool m_kill_bullet = false;
      bool m_ufo_killed = false;
   };

   auto set_ufo_abducting(Ufo& ufo, entt::registry& registry) -> void;
   auto set_ufo_shooting(Ufo& ufo, entt::registry& registry) -> void;

   auto ufo_progress(const Seconds& dt, const ScreenCoord& player_pos, Ufo& ufo, entt::registry& registry, const int level) -> void;

   [[nodiscard]] auto get_new_cow_position(entt::registry& registry) -> std::optional<LanePosition>;
   auto spawn_new_cows(entt::registry& registry, const bool inactive) -> void;
   auto do_trail_logic(entt::registry& registry, const Seconds dt) -> void;
   auto discard_ground_bullet(entt::registry& registry, entt::entity bullet_entity, const Bullet& bullet) -> void;
   auto set_new_ufo_strategies(entt::registry& registry, Ufo& ufo) -> void;
   [[nodiscard]] auto process_human_bullet(Bullet& bullet, entt::registry& registry, Ufo& ufo, const ScreenCoord& ufo_dimensions)->BulletRunResult;
}
