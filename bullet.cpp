#include "bullet.h"

#include "config.h"

#if _MSC_VER < 1928 // Visual Studio 2019 version 16.8
constexpr double pi = 3.14159265359;
#else
#include <numbers>
constexpr double pi = std::numbers::pi;
#endif // _MSC_VER




moo::Bullet::Bullet(const ScreenCoord& initial_pos, const ScreenCoord& trajectory, const BulletStyle style, entt::entity trail)
   : m_pos(initial_pos)
   , m_initial_pos(initial_pos)
   , m_trajectory(trajectory)
   , m_style(style)
   , m_trail(trail)
{

}


auto moo::Bullet::move(
   const Seconds dt
) -> void
{
   double gravity_accel = moo::get_config().gravity_strength;
   if (m_style == BulletStyle::Alien)
      gravity_accel = 0.0;
   m_gravity_speed += dt.m_value * ScreenCoord{ 0.0, gravity_accel };
   const ScreenCoord pos_change = dt.m_value * (m_bullet_speed * m_trajectory + m_gravity_speed);
   const ScreenCoord new_pos = m_pos + pos_change;
   m_pos = new_pos;
}


