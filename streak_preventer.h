#pragma once

namespace moo {

   struct StreakPreventer {
      constexpr StreakPreventer(const int max_repetitions)
         : m_max_repetitions(max_repetitions)
      {

      }


      template<class T>
      constexpr auto get_checked_value(const T& generator) -> int {
         int new_value = generator();
         if (new_value == m_last_state)
            ++m_repetition_count;
         else
            m_repetition_count = 0;

         if (m_repetition_count >= m_max_repetitions) {
            while (new_value == m_last_state)
               new_value = generator();
            m_repetition_count = 0;
         }

         m_last_state = new_value;
         return new_value;
      }

      int m_last_state = 0;
      int m_max_repetitions = 0;
      int m_repetition_count = 0;
   };

}
