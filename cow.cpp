#include "cow.h"

#include <chrono>
#include <random>


constexpr double cow_grazing_time_ms = 5.0;


moo::Cow::Cow(const FractionalPos& pos, const double initial_progress)
   : m_pos(pos)
   , m_grazing_time(initial_progress * cow_grazing_time_ms)
{
   
}


auto moo::Cow::progress(const double dt) -> double{
   m_grazing_time += dt;
   
   if (m_grazing_time > cow_grazing_time_ms)
      m_grazing_time = 0.0;
   const double progress = m_grazing_time / cow_grazing_time_ms;
   return progress;
}
