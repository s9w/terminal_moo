#include "config.h"
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


   /// <summary>Limits the player position to not go too far off screen. Otherwise it
   /// would take a long time to go 'back' after mouse was somewhere else.</summary>
   [[nodiscard]] auto get_limited_pos(
      const moo::FractionalPos& pos,
      const int rows
   ) -> moo::FractionalPos
   {
      const double horiz_cage_padding = moo::get_config().horizontal_cage_padding;
      const double vert_cage_padding = 0.5 * horiz_cage_padding; // pixels are about half as high as they are wide
      moo::FractionalPos new_pos = pos;
      new_pos.x_fraction = std::clamp(new_pos.x_fraction, 0.0 - horiz_cage_padding, 1.0 + horiz_cage_padding);

      const double helicopter_height = 3.0 / rows;
      const double lowest_height = moo::get_config().sky_fraction + 0.5 * (1.0 - moo::get_config().sky_fraction);
      const double y_max = lowest_height - 0.5 * helicopter_height;
      new_pos.y_fraction = std::clamp(new_pos.y_fraction, 0.0 - vert_cage_padding, y_max);

      return new_pos;
   }

   constexpr moo::Seconds shooting_interval_s = 0.2;

} // namespace {}


auto moo::Player::move_towards(
   const FractionalPos& target_pos,
   const Seconds dt,
   const int rows,
   const int columns
) -> void
{
   const FractionalPos position_diff = get_sanitized_position_diff(target_pos - m_pos, rows, columns);
   m_pos = get_limited_pos(m_pos + dt.m_value * m_speed * get_indep_normalized(position_diff), rows);

   m_shooting_cooldown = std::clamp(m_shooting_cooldown - dt, Seconds{ 0.0 }, shooting_interval_s);
}


auto moo::Player::try_to_fire(std::mt19937_64& rng) -> std::optional<Bullet> {
   if (!is_zero(m_shooting_cooldown.m_value))
      return std::nullopt;
   m_shooting_cooldown = shooting_interval_s;
   FractionalPos initial_bullet_pos = m_pos + FractionalPos{0.05, 0.0};
   return Bullet(initial_bullet_pos, rng);
}
