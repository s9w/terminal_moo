#pragma once

#include <entt/fwd.hpp>

namespace moo {

   auto set_ufo_abducting(entt::entity ufo_entity, entt::registry& registry) -> void;
   auto set_ufo_shooting(entt::entity ufo_entity, entt::registry& registry) -> void;

}
