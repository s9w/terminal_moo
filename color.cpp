#include "color.h"

namespace {

   [[nodiscard]] auto get_gradient(
      const unsigned char from,
      const unsigned char to,
      const int n
   ) -> std::vector<unsigned char>
   {
      if (n < 2) {
         printf("n < 2\n");
         std::terminate();
      }
      const double delta = 1.0 * (to - from) / (n - 1);
      std::vector<unsigned char> gradient;
      gradient.reserve(n);
      for (int i = 0; i < n; ++i) {
         const double value = from + i * delta;
         gradient.emplace_back(static_cast<unsigned char>(value));
      }
      return gradient;
   }

} // namespace {}


bool moo::operator==(const RGB& a, const RGB& b){
   return std::tie(a.r, a.g, a.b) == std::tie(b.r, b.g, b.b);
}


auto moo::get_gradient(
   const RGB& from, 
   const RGB& to,
   const int n
) -> std::vector<RGB>
{
   const std::vector<unsigned char> r_grad = ::get_gradient(from.r, to.r, n);
   const std::vector<unsigned char> g_grad = ::get_gradient(from.g, to.g, n);
   const std::vector<unsigned char> b_grad = ::get_gradient(from.b, to.b, n);
   std::vector<RGB> gradient;
   gradient.reserve(n);
   for (int i = 0; i < n; ++i)
      gradient.push_back({ r_grad[i], g_grad[i] , b_grad[i] });
   return gradient;
}
