#include "rng.h"

#include <chrono>

namespace {

   inline std::mt19937_64 static_rng(std::chrono::system_clock::now().time_since_epoch().count());

} // namespace {}


auto moo::get_rng() -> std::mt19937_64&{
   return static_rng;
}
