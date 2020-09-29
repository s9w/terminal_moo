#pragma once

namespace moo {

   struct Config {
      double gravity_strength = 0.0;
      double horizontal_cage_padding = 0.0;
      double smoke_puff_spread = 0.0;
      double sky_fraction = 1.0 * 23 / 30;
   };

   auto setup_config() -> void;
   auto get_config() -> const Config&;

}

