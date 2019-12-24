#include "purry.hpp"
#include <iostream>

constexpr inline auto out =
    _o_{[](error_ray *, auto...) { std::cerr << "error\n"; },
        [](auto cstr) { std::cout << cstr << '\n'; }};
struct A {};
struct B {};
struct C {};

void show0() {
  auto f = [](A) { return B{}; };
  auto g = [](B) { return C{}; };
  auto compose = [](auto f, auto g) { return [=](auto a) { return g(f(a)); }; };
  auto gof = compose(f, g);
  g(f(A{}));
  // ::==
  gof(A{});
}

void show1() {
  auto f = OB()(A) { o(B{}); };
  auto g = OB()(B) { o(C{}); };
  auto compose = [](auto f, auto g) {
    return OB(=)(auto a) { o(g ^ (f ^ a)); };
  };
  auto gof = compose(f, g);

  (gof ^ A{})(_o_{[](error_ray *) {}, [](C) {}});
}

int main() {
  show0();
  show1();
  int r = 0;
  (
      OB()(auto a, auto b) { o(a + b); } ^ OB()() { o(3); } ^
      OB()() { o(6); })(out);
  return r;
}
