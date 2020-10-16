#pragma once

#include "rng.h"

#include <entt/entt.hpp>

namespace moo {

   template<typename TView>
   [[nodiscard]] auto get_view_size(const TView& view) -> int {
      int i = 0;
      for (auto it = view.begin(); it != view.end(); ++it)
         ++i;
      return i;
   }


   template<typename T>
   [[nodiscard]] auto get_random_view_entity(
      entt::registry& registry
   ) -> entt::entity
   {
      const auto view = registry.view<T>();
      const std::uniform_int_distribution<> index_dist(0, get_view_size(view) - 1);
      auto it = view.begin();
      const auto index = index_dist(moo::get_rng());
      for (int i = 0; i < index; ++i)
         ++it;
      return *it;
   }

}
