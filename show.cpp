#include "purry.hpp"
#include <iostream>

struct A {};
struct B {};
struct C {};
struct D {};

int main() {
  auto compose = [](auto f, auto g) {
    return purry{[=](auto o, auto a) { o(g ^ (f ^ a)); }};
  };

  auto f = purry{[](auto o, A) { o(B{}); }};
  auto g = purry{[](auto o, B) { o(C{}); }};
  auto h = purry{[](auto o, C) { o(D{}); }};

  auto gof_1 = compose(compose(f, g), h);
  auto gof_2 = compose(f, compose(g, h));

  (gof_1 ^ A{})(_o_{[](int) {}, [](D) {}});
  (gof_2 ^ A{})(_o_{[](int) {}, [](D) {}});

  auto compose_ = [](auto f, auto g) {
    return [=](auto a) { return g(f(a)); };
  };
  auto f_ = [](A) { return B{}; };
  auto g_ = [](B) { return C{}; };
  auto h_ = [](C) { return D{}; };

  auto gofoh_ = compose_(compose_(f_, g_), h_);
  auto r_ = gofoh_(A{});

  static_assert(std::is_same_v<decltype(r_), D>);
}
