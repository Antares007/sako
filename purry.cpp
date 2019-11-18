#include "purry.hpp"

int main() {
  int r = 0;

  auto p = purry{[](auto o, auto a, auto b, auto c) { o(a + b + c); }} ^
           [](auto o) { o(1, 2); } ^ 6;

  p([&](auto x) { r = x; });

  return r;
}
