#include "game_colors.h"



size_t moo::GameColors::get_white() const {
   return 1;
}

moo::GameColors::GameColors() {
   rgbs.push_back({ 0, 0, 0 }); // black, but used as alpha mask
   rgbs.push_back({ 255, 255, 255 }); // white
}


size_t moo::GameColors::get_sky_color(const double fraction) const {
   return sky_color_start + static_cast<size_t>(fraction * sky_color_count);
}


size_t moo::GameColors::get_ground_color(const double fraction) const {
   return ground_color_start + static_cast<size_t>(fraction * ground_color_count);
}