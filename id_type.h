#pragma once

#include <compare>

namespace moo {

   struct ID {
      constexpr explicit ID(const unsigned int value)
         : m_val(value)
      {

      }

      auto operator<=>(const ID&) const = default;

   protected:
      unsigned int m_val;
   };


   [[nodiscard]] auto get_id()->ID;

}
