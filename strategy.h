#pragma once

#include <variant>

#include <entt/entt.hpp>


namespace moo {

   struct Abduct {
      entt::entity m_target_cow;
   };

   struct Shoot {

   };

   using UfoStrategy = std::variant<Abduct, Shoot>;

}