#pragma once

#include "helpers.h"

#include <algorithm>

namespace moo {

   struct AnimationFrame {
      constexpr AnimationFrame(const size_t frame_count, const Seconds period, const double initial_progress)
         : m_frame_count(frame_count)
         , m_period(period)
         , m_time_state(initial_progress * m_period)
      {

      }


      constexpr auto progress(const Seconds& dt) -> void {
         m_time_state += dt;
         if (m_time_state > m_period) {
            m_time_state -= m_period;
         }
      }


      constexpr auto get_index() const -> size_t {
         const double progress = m_time_state / m_period;
         constexpr size_t zero = 0;
         return std::clamp(static_cast<size_t>(progress * m_frame_count), zero, m_frame_count - 1);
      }


      size_t m_frame_count = 0;
      Seconds m_period;
      Seconds m_time_state;
   };

}
