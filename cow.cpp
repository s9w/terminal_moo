#include "cow.h"

#include <chrono>
#include <random>


constexpr moo::Seconds cow_grazing_time = 5.0;


moo::Cow::Cow(
   const LanePosition& pos,
   const double initial_progress,
   const int variant
)
   : m_pos(pos)
   , m_variant(variant)
   , m_animation_frame(2, 1.0, initial_progress)
   , m_id(get_id())
{
   
}


auto moo::Cow::progress(const Seconds dt) -> void{
   m_animation_frame.progress(dt);
}


auto moo::Cow::move(const double distance) -> void{
   m_pos.m_x_pos -= distance;
}


bool moo::Cow::is_gone() const{
   if (m_pos.m_x_pos < 0)
      return true;
   return false;
}
