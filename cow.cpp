#include "cow.h"

#include <chrono>
#include <random>


constexpr moo::Seconds cow_grazing_time = 5.0;


moo::Cow::Cow(const FractionalPos& pos, const double initial_progress)
   : m_pos(pos)
   , m_grazing_time(initial_progress * cow_grazing_time)
{
   
}


auto moo::Cow::progress(const Seconds dt) -> double{
   m_grazing_time += dt;
   
   if (m_grazing_time > cow_grazing_time)
      m_grazing_time = 0.0;
   const double progress = m_grazing_time / cow_grazing_time;
   return progress;
}
