#include "helpers.h"

bool moo::operator==(const LongRect& a, const LongRect& b){
   return std::tie(a.left, a.top, a.right, a.bottom) == std::tie(b.left, b.top, b.right, b.bottom);
}


bool moo::operator!=(const LongRect& a, const LongRect& b){
   return !(a == b);
}
