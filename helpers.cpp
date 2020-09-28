#include "helpers.h"


auto moo::operator*(const double factor, const FractionalPos& pos) -> FractionalPos {
   FractionalPos result = pos;
   result.x_fraction *= factor;
   result.y_fraction *= factor;
   return result;
}


auto moo::operator-(const FractionalPos& a, const FractionalPos& b) -> FractionalPos{
   FractionalPos result = a;
   result.x_fraction -= b.x_fraction;
   result.y_fraction -= b.y_fraction;
   return result;
}

auto moo::operator+(const FractionalPos& a, const FractionalPos& b) -> FractionalPos{
   FractionalPos result = a;
   result.x_fraction += b.x_fraction;
   result.y_fraction += b.y_fraction;
   return result;
}

auto moo::get_indep_normalized(const FractionalPos& a) -> FractionalPos{
   FractionalPos result;
   result.x_fraction = is_zero(a.x_fraction) ? 0.0 : get_sign(a.x_fraction);
   result.y_fraction = is_zero(a.y_fraction) ? 0.0 : get_sign(a.y_fraction);
   return result;
}

auto moo::length(const FractionalPos& a) -> double{
   return std::sqrt(a.x_fraction * a.x_fraction + a.y_fraction * a.y_fraction);
}
