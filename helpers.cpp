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
   return { get_sign(a.x_fraction), get_sign(a.y_fraction) };
}

auto moo::length(const FractionalPos& a) -> double{
   return std::sqrt(a.x_fraction * a.x_fraction + a.y_fraction * a.y_fraction);
}
