#include "ufo.h"

moo::Ufo::Ufo(const FractionalPos initial_pos)
   : m_pos(initial_pos)
{

}

auto moo::Ufo::progress() -> void{
   m_is_hit = false;
}

auto moo::Ufo::hit() -> bool{
   m_health -= 0.1;
   m_is_hit = true;
   return less_equal(m_health, 0.0);
}

