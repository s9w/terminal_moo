#include "player.h"

auto moo::Player::move_towards(
   const FractionalPos& target_pos,
   const double dt
) -> void
{
   const FractionalPos position_diff = target_pos - m_pos;
   if (length(position_diff) < 0.01)
      return;
   m_pos = m_pos + dt * m_speed * get_indep_normalized(position_diff);
}
