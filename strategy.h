#pragma once

#include "id_type.h"

#include <variant>


namespace moo {

   struct Abduct {
      ID m_target_cow;
   };

   struct Shoot {

   };

   using UfoStrategy = std::variant<Abduct, Shoot>;

}