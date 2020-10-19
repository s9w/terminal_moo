#pragma once

#include <entt/entt.hpp>

namespace moo {

   template<class T, entt::id_type i>
   struct Value {
      using underlying_type = T;

      constexpr Value() : m_value{} {}  // if default-constr is wanted
      constexpr Value(const T& p_value) : m_value(p_value) {}
      constexpr operator T() const { return m_value; }

      // explicit getter, for std::clamp and things I forgot
      constexpr auto get() const -> const T& { return m_value; }

      constexpr Value& operator+=(const T& other) {
         m_value += other;
         return *this;
      }
      constexpr Value& operator-=(const T& other) {
         m_value -= other;
         return *this;
      }
      constexpr Value& operator*=(const T& other) {
         m_value *= other;
         return *this;
      }
      constexpr Value& operator/=(const T& other) {
         m_value /= other;
         return *this;
      }

      auto operator++() -> Value& {
         ++m_value;
         return *this;
      }
      auto operator++(int) -> Value {
         Value old(*this);
         ++(*this);
         return old;
      }
      auto operator--() -> Value& {
         --m_value;
         return *this;
      }
      auto operator--(int) -> Value {
         Value old(*this);
         --(*this);
         return old;
      }

   private:
      T m_value;
   };

   using Alpha = Value<double, "alpha"_hs>;
   using BeingBeamed = Value<bool, "being_beamed"_hs>;
   using GravitySpeed = Value<double, "gravity_speed"_hs>;

   struct IsCow {};
   struct IsCloud {};
   struct IsPuff {};
   struct PuffSpawner {};
   using CowVariant = entt::entity;
   using CloudImageRef = entt::entity;

   enum class BulletStyle { Rocket, Alien };
}