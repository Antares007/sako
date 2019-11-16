#include "purry.hpp"

#include "_o_.hpp"
#include <iostream>
int main() {
  int r = 0;

  purry{[](auto o, auto a, auto b) { o(a, b); }}('A')('B')(
      _o_{[](int err) { std::cout << err << '\n'; },
          [&](auto a, auto b) { std::cout << a << b << '\n'; }});

  return r;
}
