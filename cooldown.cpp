#include "cooldown.h"

#include <doctest/doctest.h>


TEST_CASE("Cooldown init") {
   using namespace moo;

   Cooldown c0(5.0);
   const Cooldown copy = c0;
   CHECK(std::holds_alternative<Cooldown::Inactive>(c0.m_state));

   c0.iterate(Seconds{ 1.0 });
   CHECK(c0 == copy);
}
