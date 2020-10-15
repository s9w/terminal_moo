#pragma once

#include "helpers.h"

namespace moo {

   struct Config {
      double gravity_strength = 0.0;
      double horizontal_cage_padding = 0.0;
      double smoke_puff_spread = 0.0;
      double sky_fraction = 1.0 * 23 / 30;
      bool enable_mouse = true;
      int cloud_count = 3;
      double ufo_hit_invul_duration = 0.1;
      double player_hit_invul_duration = 0.1;
      double day_length = 60.0;
      double new_strategy_interval = 8.0;
      double ufo_shooting_interal = 1.0;
      double ufo_base_speed = 0.1;
      double ufo_speed_bonus = 0.1;
   };

   auto setup_config() -> void;
   auto get_config() -> const Config&;

}

