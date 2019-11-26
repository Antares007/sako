#include "purry.hpp"
#include <iostream>

struct A {};
struct B {};
struct C {};
struct D {};

void show0() {
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
}

void show1() {
  auto compose = [](auto f, auto g) { return [=](auto a) { return g(f(a)); }; };
  auto f = [](A) { return B{}; };
  auto g = [](B) { return C{}; };
  auto h = [](C) { return D{}; };

  auto gofoh_ = compose(compose(f, g), h);
  auto rez = gofoh_(A{});

  static_assert(std::is_same_v<decltype(rez), D>);
}

int main() {
  show0();
  show1();
}
