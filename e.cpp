#include "parsec.hpp"
#include "xml.hpp"
#include <iostream>

using namespace parsec;
using namespace parsec::xml;

#define C constexpr inline auto
#define Derives template <typename O> void operator()(O o, const char *in) const
C id = many{0, chr{[](auto c) { return 'a' <= c && c <= 'z'; }}};

// E -> E + T | T
// T -> T * F | F
// F -> ( E ) | id

struct Eps {
  Derives {}
};
// F  -> ( E ) | id
struct F {
  Derives {}
};
// T_ -> * F T_ | Eps
struct T_ {
  Derives { (str{"*"} & F{} & *this)(o, in); }
};
// T  -> F T_
struct T {
  Derives { (F{} & T_{})(o, in); }
};
// E_ -> + T E_ | Eps
struct E_ {
  Derives { (str{"+"} & T{} & *this | Eps{})(o, in); }
};
// E  -> T E_
struct E {
  Derives { (T{} & E_{})(o, in); }
};

int main() {
  const char *expr = R"xml(a+b*c)xml";

  (E{}, expr)(::rays{
      [](error_ray *, int err) { std::cout << "error: " << err << '\n'; },
      [&](size_t len) { std::cout << "len: " << len << std::endl; }});
  return 9;
}
