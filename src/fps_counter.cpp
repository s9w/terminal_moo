#include "fps_counter.h"

#include <cmath>


moo::FpsCounter::FpsCounter()
   : m_last_tp(std::chrono::system_clock::now())
{

}


void moo::FpsCounter::step(const tp& now){
   ++m_fps_count;

   const std::chrono::duration<double> passed_duration(now - m_last_tp);
   constexpr std::chrono::milliseconds one_second(1000);
   if (passed_duration > one_second)
      reset(now);
}


void moo::FpsCounter::reset(const tp& now){
   const double passed_seconds = std::chrono::duration<double>(now - m_last_tp).count();
   m_current_fps = m_fps_count / passed_seconds;
   m_last_tp = now;
   m_fps_count = 0;
}

