#include "color.h"
#include "helpers.h"

namespace {

   [[nodiscard]] auto get_gradient(
      const unsigned char from,
      const unsigned char to,
      const unsigned int n
   ) -> std::vector<unsigned char>
   {
      if (n < 2) {
         printf("n < 2\n");
         std::terminate();
      }
      const double delta = 1.0 * (to - from) / (n - 1);
      std::vector<unsigned char> gradient;
      gradient.reserve(n);
      for (unsigned int i = 0; i < n; ++i) {
         const double value = from + i * delta;
         gradient.emplace_back(static_cast<unsigned char>(value));
      }
      return gradient;
   }


   [[nodiscard]] auto get_noised_colors(
      const std::vector<moo::RGB>& colors,
      std::mt19937_64& rng,
      const int noise_strength
   ) -> std::vector<moo::RGB>
   {
      std::vector<moo::RGB> noised_colors;
      noised_colors.reserve(colors.capacity());
      std::uniform_int_distribution<> noise_dist(-noise_strength, noise_strength);
      for (const moo::RGB& rgb_color : colors) {
         const int noise = noise_dist(rng);
         noised_colors.push_back(moo::get_offsetted_color(rgb_color, noise));
      }

      return noised_colors;
   }

} // namespace {}


auto moo::get_noised_color(
   const RGB& color, 
   const int noise_strength,
   std::mt19937_64& rng
) -> RGB
{
   std::uniform_int_distribution<> noise_dist(-noise_strength, noise_strength);
   const int noise = noise_dist(rng);
   return {
      get_clamped_uchar(color.r + noise),
      get_clamped_uchar(color.g + noise),
      get_clamped_uchar(color.b + noise)
      };
}


auto moo::get_gradient(
   const RGB& from, 
   const RGB& to,
   const unsigned int n
) -> std::vector<RGB>
{
   const std::vector<unsigned char> r_grad = ::get_gradient(from.r, to.r, n);
   const std::vector<unsigned char> g_grad = ::get_gradient(from.g, to.g, n);
   const std::vector<unsigned char> b_grad = ::get_gradient(from.b, to.b, n);
   std::vector<RGB> gradient;
   gradient.reserve(n);
   for (unsigned int i = 0; i < n; ++i)
      gradient.push_back({ r_grad[i], g_grad[i] , b_grad[i] });
   return gradient;
}


auto moo::get_smoke_colors(
   const int smoke_color_count,
   std::mt19937_64& rng
) -> std::vector<RGB>
{
   constexpr moo::RGB orange{ 251, 140, 68 };
   constexpr moo::RGB grey{ 200, 200, 200};
   return get_noised_colors(get_gradient(orange, grey, smoke_color_count), rng, 20);
}


auto moo::get_sky_colors(const int sky_color_count) -> std::vector<RGB>{
   constexpr moo::RGB light_blue{ 166, 180, 190 };
   constexpr moo::RGB dark_blue{ 0, 82, 135 };
   return get_gradient(dark_blue, light_blue, sky_color_count);
}


auto moo::get_health_colors(const int health_color_count) -> std::vector<RGB>{
   const int first_size = health_color_count / 2;
   const int second_size = health_color_count - first_size;

   constexpr moo::RGB green{ 255, 0, 0 };
   constexpr moo::RGB yellow{ 255, 255, 0 };
   constexpr moo::RGB red{ 0, 255, 0 };
   std::vector<RGB> green_to_yellow = get_gradient(green, yellow, first_size);
   std::vector<RGB> yellow_to_red = get_gradient(yellow, red, second_size);
   std::vector<RGB> health_gradient;
   health_gradient.reserve(health_color_count);
   append_moved(health_gradient, green_to_yellow);
   append_moved(health_gradient, yellow_to_red);
   return health_gradient;
}


auto moo::get_ground_colors(const int ground_color_count) -> std::vector<RGB>{
   constexpr moo::RGB light_green{ 55, 108, 48 };
   constexpr moo::RGB dark_green{ 21, 42, 31 };
   return get_gradient(dark_green, light_green, ground_color_count);
}


TEST_CASE("get_color_mix()") {
   using namespace moo;
   constexpr RGB white{ 200, 200, 200};
   constexpr RGB gray{ 100, 100, 100 };
   constexpr RGB black{ 0, 0, 0 };
   
   CHECK(get_color_mix(black, white, 0.0) == black);
   CHECK(get_color_mix(black, white, 0.5) == gray);
   CHECK(get_color_mix(black, white, 1.0) == white);
}