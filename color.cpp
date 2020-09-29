#include "color.h"

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

} // namespace {}


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


auto moo::get_smoke_colors(const int smoke_color_count) -> std::vector<RGB>{
   constexpr moo::RGB dark_grey{ 100, 100, 100 };
   constexpr moo::RGB light_grey{ 230, 230, 230 };
   return get_gradient(dark_grey, light_grey, smoke_color_count);
}


auto moo::get_sky_colors(const int sky_color_count) -> std::vector<RGB>{
   constexpr moo::RGB light_blue{ 166, 180, 190 };
   constexpr moo::RGB dark_blue{ 0, 82, 135 };
   return get_gradient(light_blue, dark_blue, sky_color_count);
}


auto moo::get_ground_colors(const int ground_color_count) -> std::vector<RGB>{
   constexpr moo::RGB light_green{ 55, 108, 48 };
   constexpr moo::RGB dark_green{ 21, 42, 31 };
   return get_gradient(light_green, dark_green, ground_color_count);
}
