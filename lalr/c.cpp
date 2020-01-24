template <typename T> struct print;
#include "g41.hpp"
#include <iostream>
#include <typeindex>
#include <typeinfo>

constexpr inline auto log = [](size_t ident, auto... args) {
  while (ident--)
    std::cout << ' ';
  ((std::cout << args), ...);
  std::cout << std::endl;
};

struct olrParser {
  MBark(typename V)(const V &variable, const char *in) {}
};

int main() {
  auto pith = ::rays{
      [](error_ray *, int err) { std::cerr << "Error: " << err << '\n'; },
      [](size_t pos) { std::cout << "Pos: " << pos << '\n'; }};
  olrParser{}(
      pith, [](const auto &o) { o([](const auto &o) { o(E{}); }); }, "a+b*o");
  return 9;
}
