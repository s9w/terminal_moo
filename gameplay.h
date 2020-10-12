#pragma once

#include <entt/fwd.hpp>

namespace moo {

   struct Seconds;
   struct ScreenCoord;
   struct Ufo;

   auto set_ufo_abducting(Ufo& ufo, entt::registry& registry) -> void;
   auto set_ufo_shooting(Ufo& ufo, entt::registry& registry) -> void;

   auto ufo_progress(
      const Seconds& dt,
      const ScreenCoord& player_pos,
      Ufo& ufo,
      entt::registry& registry
   ) -> void;
}
