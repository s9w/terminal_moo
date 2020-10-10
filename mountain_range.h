#pragma once

#include <vector>

#include "buffer.h"

namespace moo {

   struct Seconds;

   struct MountainGenerator {
      MountainGenerator(const int min_height, const int max_height);
      auto get_next_height() -> int;

      int m_current_height = 0;
      int m_step = 0;
      int m_min_height = 0;
      int m_max_height = 0;
   };


   struct MountainRange {
      MountainRange(const int height_baseline, const RGB& color);
      auto move(const Seconds& dt) -> void;
      auto shift_mountain() -> void;
      auto write_new_right_column() -> void;

      BgColorBuffer m_mountain;
      BgColorBuffer m_next_mountain;
      int m_height_baseline = 0;
      double m_position = 0.0;
      MountainGenerator m_generator;
      const RGB m_color;
   };

}
