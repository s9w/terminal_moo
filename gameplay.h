#pragma once

#include <entt/fwd.hpp>

namespace moo {

   struct Seconds;
   struct ScreenCoord;
   struct Ufo;

   auto set_ufo_abducting(entt::entity ufo_entity, entt::registry& registry) -> void;
   auto set_ufo_shooting(entt::entity ufo_entity, entt::registry& registry) -> void;

   auto ufo_progress(
      const Seconds& dt,
      const ScreenCoord& player_pos,
      Ufo& ufo,
      ScreenCoord& ufo_pos,
      entt::registry& registry
   ) -> void;
}
