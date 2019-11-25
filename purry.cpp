#include "purry.hpp"
#include <iostream>
constexpr inline auto out = _o_{[](int err) { std::cerr << err << '\n'; },
                                [](auto cstr) { std::cout << cstr << '\n'; }};

int main() {
  int r = 0;
  (
      purry{
          [](auto o, auto a, auto b) { o(std::string(a) + std::string(b)); }} ^
      [](auto o) { o(std::string_view("A")); } ^
      [](auto o) { o(std::string_view("B")); })(out);
  return r;
}
