#include "mountain_range.h"

#include "helpers.h"
#include "rng.h"
#include "screen_size.h"
#include "screencoord.h"


namespace {

   auto shift_left(moo::BgColorBuffer& buffer) -> void {
      for (int i = 0; i < moo::static_rows; ++i) {
         for (int j = 0; j < (moo::static_columns - 1); ++j) {
            const int index = i * moo::static_columns + j;
            const int right_index = i * moo::static_columns + (j + 1) % moo::static_columns;
            std::swap(buffer[index], buffer[right_index]);
         }
      }
   }

} // namespace {}


moo::MountainRange::MountainRange(const int height_baseline, const RGB& color)
   : m_generator(height_baseline, height_baseline + 3)
   , m_color(color)
{
   for (int i = 0; i < static_columns; ++i)
      iterate();
}


auto moo::MountainRange::move(const Seconds& dt) -> void{
   m_position -= get_lane_speed(0, dt);
   const double threshold = 1.0 / static_columns;
   if (m_position < -threshold) {
      iterate();
      m_position += threshold;
   }
}


auto moo::MountainRange::iterate() -> void{
   m_mountain = m_next_mountain;
   shift_left(m_next_mountain);
   new_right_column();
}


auto moo::MountainRange::new_right_column() -> void{
   const int sky_row_height = get_sky_row_height();
   const int new_height = m_generator.iterate();
   const int min_mountain_i = sky_row_height - 1 - new_height;

   for (int i = 0; i < sky_row_height; ++i) {
      const int j = static_columns - 1;
      const int index = i * static_columns + j;
      const bool is_mountain = i > min_mountain_i;
      if (is_mountain)
         m_next_mountain[index] = m_color;
      else
         m_next_mountain[index] = RGB{ 0, 0, 0 };
   }
}


auto moo::MountainGenerator::iterate() -> int{
   ++m_step;

   if (m_step % 4 == 0) {
      const std::uniform_int_distribution<> height_dist(-1, 1);
      const int new_height = m_last_height + height_dist(get_rng());
      if (new_height >= m_min_height && new_height <= m_max_height)
         m_last_height = new_height;
   }
   return m_last_height;
}


moo::MountainGenerator::MountainGenerator(const int min_height, const int max_height)
   : m_last_height(min_height)
   , m_min_height(min_height)
   , m_max_height(max_height)
{
   
}
