#pragma once

#include <entt/entt.hpp>

namespace moo {

   struct Alpha {
      double value = 1.0;
   };

   struct BeingBeamed {
      bool value = false;
   };

   struct MountainPos {
      double value = 0.0;
   };

   using IsCow = entt::tag<"is_cow"_hs>;
   using IsCloud = entt::tag<"is_cloud"_hs>;
   using CowVariant = entt::entity;
   using CloudImageRef = entt::entity;
   using MountainImageRef = entt::entity;

   enum class BulletStyle { Rocket, Alien };
}