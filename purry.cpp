#include "purry.hpp"

#include "_o_.hpp"
#include <iostream>
int main() {
  int r = 0;

  purry{[](auto o, auto a, auto, int b, int c) { o(a, b + c); }}([](auto o) {
    o(-1);
    o("A", "B");
  })(6)(9)(_o_{[](int) {}, [&](auto, auto a) { r = a; }});

  return r;
}
