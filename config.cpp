#include "config.h"

#include <fstream> //required for parse_file()

#include <toml++/toml.h>


static moo::Config config;


auto moo::setup_config() -> void{
   toml::table tbl;
   try{
      tbl = toml::parse_file("config.toml");
   }
   catch (const toml::parse_error & err){
      printf("Parsing failed: %s\n", std::string(err.description()).c_str());
      std::terminate();
   }
   config.gravity_strength = tbl["game"]["gravity"].value_or(0.0);
   config.horizontal_cage_padding = tbl["game"]["horizontal_cage_padding"].value_or(0.0);
   config.smoke_puff_spread = tbl["game"]["smoke_puff_spread"].value_or(0.0);
   config.enable_mouse = tbl["game"]["enable_mouse"].value_or(true);
}


auto moo::get_config() -> const Config&{
   return config;
}
