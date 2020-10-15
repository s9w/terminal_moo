#pragma once

#include <compare>
#include <variant>

#include "helpers.h"

#include <fmt\format.h>


namespace moo {

   struct Cooldown {

      struct Inactive {
         constexpr auto operator==(const Inactive) const -> bool {
            return true;
         }
      };

      struct ReadyToFire {
         constexpr auto operator==(const ReadyToFire) const -> bool {
            return true;
         }
      };

      static auto get_ready_cooldown(const double period) -> Cooldown {
         Cooldown cd(period);
         cd.m_state.emplace<ReadyToFire>();
         return cd;
      }
      constexpr Cooldown(const double period);
      constexpr auto iterate(const Seconds& dt) -> void;
      [[nodiscard]] inline auto get_ready() -> bool;
      inline auto restart() -> void;
      inline auto set_inactive() -> void;
      [[nodiscard]] inline auto to_string() const -> std::string;
      
      

      constexpr auto operator<=>(const Cooldown&) const = default;

   private:
      std::variant<Inactive, ReadyToFire, Seconds> m_state;
      double m_period;
   };

}


constexpr moo::Cooldown::Cooldown(const double period)
   : m_period(period)
   , m_state(Inactive{})
{

}


constexpr auto moo::Cooldown::iterate(const Seconds& dt) -> void{
   if (!std::holds_alternative<Seconds>(m_state))
      return;
   Seconds& seconds = std::get<Seconds>(m_state);
   seconds -= dt;
   seconds = std::clamp(seconds.m_value, 0.0, m_period);
   if (is_zero(seconds.m_value)) {
      m_state.emplace<ReadyToFire>();
   }
}


auto moo::Cooldown::get_ready() -> bool{
   const bool is_ready = std::holds_alternative<ReadyToFire>(m_state);
   if (is_ready) {
      m_state.emplace<Inactive>();
   }
   return is_ready;
}


auto moo::Cooldown::restart() -> void{
   m_state.emplace<Seconds>(m_period);
}


auto moo::Cooldown::to_string() const -> std::string{
   if (std::holds_alternative<Inactive>(m_state))
      return "Inactive";
   else if (std::holds_alternative<ReadyToFire>(m_state))
      return "ReadyToFire";
   else
      return fmt::format("{:.1f}", std::get<Seconds>(m_state).m_value);
}


auto moo::Cooldown::set_inactive() -> void{
   m_state.emplace<Inactive>();
}
