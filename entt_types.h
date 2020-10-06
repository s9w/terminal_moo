#pragma once

#include <entt/entt.hpp>

namespace moo {

   struct Alpha {
      double value = 1.0;
   };

   struct BeingBeamed {
      bool value = false;
   };

   using IsCow = entt::tag<"is_cow"_hs>;
   using CowVariant = entt::entity;

}