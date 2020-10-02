#include "cloud.h"


moo::Cloud::Cloud(
   const FractionalPos& initial_pos,
   const size_t image_index,
   const double fractional_width,
   const double speed_factor,
   const double alpha
)
   : m_pos(initial_pos)
   , m_image_index(image_index)
   , m_fractional_width(fractional_width)
   , m_speed_factor(speed_factor)
   , m_alpha(alpha)
{

}


auto moo::Cloud::progress(const Seconds dt) -> bool{
   m_pos.x_fraction -= 0.01 * m_speed_factor * dt;
   const FractionalPos rightmost_pos = m_pos + FractionalPos{0.5 * m_fractional_width, 0.0};
   const bool is_left_off_screen = rightmost_pos.x_fraction < 0.0;
   return is_left_off_screen;
}
