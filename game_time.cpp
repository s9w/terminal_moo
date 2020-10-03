#include "game_time.h"

auto moo::GameTime::progress(const Seconds& dt) -> void
{
   const double day_len_in_s = get_config().day_length;
   m_day_progress += dt / day_len_in_s;
   if (m_day_progress > 1.0) {
      ++m_day;
      m_day_progress -= 1.0;
   }
}
