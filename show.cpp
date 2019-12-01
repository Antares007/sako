#include "purry.hpp"
#include <iostream>

struct A {};
struct B {};
struct C {};
struct D {};

void showadd() {
  auto add = purry{[](auto o, auto a, auto b) {
    if (false)
      o(A{});
    o(a + b);
  }};
  auto out = _o_{[](A) {}, [](int r) { std::cout << r << '\n'; }};
  (add ^ 3 ^ 6)(out);
  (add ^ [](auto o) { o(3, 6); })(out);
  (add ^ 3 ^ 6)(out);
  add(out, 3, 6);
}

void showo() {
  auto oset = _o_{[](A) { return C{}; }, [](B) {},
                  [](auto x) {
                    if constexpr (true)
                      std::cout << x << '\n';
                  }};
  oset("abo");
  oset(B{});
}

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
