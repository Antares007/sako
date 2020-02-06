#include "lalr/mb.hpp"
#include "purry.hpp"
#include <iostream>

constexpr inline auto l = [](const auto &o) {
  o(lhead, 1, [&](const auto &o) {
    o(lhead, 2, [&](const auto &o) { o(lhead, 3, nullptr); });
  });
};

template <typename... Ts> struct print;
template <char... Chrs> struct any {
  void operator()(Car o, const char *b) { o(((Chrs == b[0]) || ...) ? 1 : -1); }
};
template <char... Chrs> struct none {
  void operator()(Car o, const char *b) { o(((Chrs != b[0]) && ...) ? 1 : -1); }
};
int main() {
  none<'i', 'f'>{}([](auto len) { std::cout << len << '\n'; }, "f");
  ;
  l(o::rec{[](Car rec, head_ray *, int i, Car tail) {
    std::cout << i << ' ';
    if constexpr (std::is_same_v<std::decay_t<decltype(tail)>,
                                 decltype(nullptr)>)
      ; // print<decltype(tail)> pp;
    else
      tail(rec);
  }});
  return 9;
}
