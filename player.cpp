#include "player.h"

#include <algorithm>

namespace {

   /// <summary>If the position diff would not result in at least a change of "half" a pixel,
   /// the resulting position could immediately jump back. For small differences, that would
   /// result in optical flickering</summary>
   [[nodiscard]] auto get_sanitized_position_diff(
      const moo::FractionalPos& position_diff,
      const int rows,
      const int columns
   ) -> moo::FractionalPos
   {
      moo::FractionalPos new_diff = position_diff;
      if (std::abs(new_diff.x_fraction * columns) < 0.5)
         new_diff.x_fraction = 0.0;
      if (std::abs(new_diff.y_fraction * rows) < 0.5)
         new_diff.y_fraction = 0.0;
      return new_diff;
   }


   /// <summary>Limits the actual player position to not go too far off screen. Otherwise it
   /// would take a long time to go 'back' after mouse was somewhere else.</summary>
   [[nodiscard]] constexpr auto get_limited_pos(const moo::FractionalPos& pos) -> moo::FractionalPos {
      constexpr double x_limit = 0.1;
      constexpr double y_limit = 0.5 * x_limit; // pixels are about half as high as they are wide
      moo::FractionalPos new_pos = pos;
      new_pos.x_fraction = std::clamp(new_pos.x_fraction, 0.0 - x_limit, 1.0 + x_limit);
      new_pos.y_fraction = std::clamp(new_pos.y_fraction, 0.0 - y_limit, 1.0 + y_limit);
      return new_pos;
   }

} // namespace {}


auto moo::Player::move_towards(
   const FractionalPos& target_pos,
   const double dt,
   const int rows,
   const int columns
) -> void
{
   const FractionalPos position_diff = get_sanitized_position_diff(target_pos - m_pos, rows, columns);
   m_pos = get_limited_pos(m_pos + dt * m_speed * get_indep_normalized(position_diff));

   m_bullet_wait = std::clamp(m_bullet_wait - dt, 0.0, 1.0);
}


auto moo::Player::fire(std::mt19937_64& rng) -> std::optional<Bullet> {
   if (!is_zero(m_bullet_wait))
      return std::nullopt;
   m_bullet_wait = 1.0;
   FractionalPos initial_bullet_pos = m_pos + FractionalPos{0.05, 0.0};
   return Bullet(initial_bullet_pos, rng);
}
