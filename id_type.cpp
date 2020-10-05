#include "id_type.h"

#include <utility>

namespace {
   inline unsigned int next_id_val = 0;
}


auto moo::get_id() ->ID{
   ID return_id(next_id_val);
   ++next_id_val;
   return return_id;
}
