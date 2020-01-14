#include "parsec.hpp"
#include "xml.hpp"
#include <iostream>

using namespace parsec;
using namespace parsec::xml;

int main() {
  const char *expr = R"xml(a+b*c)xml";
  auto grammar = str{"a+b*c"};
  (grammar, expr)(::rays{
      [](error_ray *, int err) { std::cout << "error: " << err << '\n'; },
      [&](size_t len) { std::cout << "len: " << len << std::endl; }});
  return 9;
}
