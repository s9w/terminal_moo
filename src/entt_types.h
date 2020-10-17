#pragma once

#include <entt/entt.hpp>

namespace moo {

   struct Alpha {
      double value = 1.0;
   };

   struct BeingBeamed {
      bool value = false;
   };

   struct GravitySpeed {
      double value = 0.0;
   };

   struct IsCow {};
   struct IsCloud {};
   struct IsPuff {};
   struct PuffSpawner {};
   using CowVariant = entt::entity;
   using CloudImageRef = entt::entity;

   enum class BulletStyle { Rocket, Alien };
}