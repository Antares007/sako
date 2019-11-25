#include "purry.hpp"
struct L;
#include <iostream>

constexpr inline auto out = _o_{[](int err) { std::cerr << err << '\n'; },
                                [](auto cstr) { std::cout << cstr << '\n'; }};

struct A {};
struct B {};
struct C {};

int main() {
  auto f = purry{[](auto o, A) { o(B{}); }};
  auto g = purry{[](auto o, B) { o(C{}); }};

  auto gof = g ^ (f ^ A{});

  gof([](auto) {});
}
