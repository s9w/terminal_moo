#pragma once

#include "lane_position.h"

#include <entt/fwd.hpp>

#include <optional>


namespace moo {

   struct Seconds;
   struct ScreenCoord;
   struct Ufo;

   auto set_ufo_abducting(Ufo& ufo, entt::registry& registry) -> void;
   auto set_ufo_shooting(Ufo& ufo, entt::registry& registry) -> void;

   auto ufo_progress(const Seconds& dt, const ScreenCoord& player_pos, Ufo& ufo, entt::registry& registry) -> void;

   [[nodiscard]] auto get_new_cow_position(entt::registry& registry) -> std::optional<LanePosition>;
   auto spawn_new_cows(entt::registry& registry) -> void;
}
