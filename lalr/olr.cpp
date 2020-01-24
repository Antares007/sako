#include "g41.hpp"
#include <iostream>
#include <typeindex>
#include <typeinfo>
template <typename T> struct print;
constexpr inline auto typeindex = [](auto v) {
  return std::type_index(typeid(v));
};
template <typename V> struct closure {
  V v;
  Derives {
    o(
        [&](auto) {
          v([&](auto production) {        //
            production([&](auto symbol) { //

            });
          });
        },
        0);
  }
};
struct A {};
int main() { //
  std::cout << typeindex(E::T::F{}).name() << '\n';
  return 9;
}
