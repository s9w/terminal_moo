#pragma once

#include <chrono>

namespace moo {

   struct FpsCounter {
      using tp = std::chrono::time_point<std::chrono::system_clock>;

      FpsCounter();
      void step(const tp& now);

      double m_current_fps = 0;

   private:
      void reset(const tp& now);

      tp m_last_tp;
      int m_fps_count = 0;
   };

}