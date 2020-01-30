#include "../parsec.hpp"
#include "g41.hpp"
#include <cxxabi.h>
#include <iostream>
#include <string>
#include <typeindex>

constexpr inline auto demangle = [](const char *name) {
  int stat = 0;
  auto buff = abi::__cxa_demangle(name, nullptr, nullptr, &stat);
  auto rez = std::string(stat ? name : buff);
  free(buff);
  return rez;
};
template <typename T> struct print;

template <typename F, typename G> struct pith {
  F f;
  G g;
  void operator()(head_ray *, auto &&v, size_t i, size_t s) const {
    f(static_cast<decltype(v) &&>(v), *this, i, s);
  }
  void operator()(tail_ray *, auto &&tail) const {
    g(static_cast<decltype(tail) &&>(tail), *this);
  }
};
template <typename F, typename G> pith(F, G) -> pith<F, G>;

#include <set>
struct variables {
  template <typename O, typename V>
  void operator()(const O &o, const V &variable) const {
    size_t ident = 0;
    auto set = std::set<std::type_index>{};
    LA(LA(variable))
    (pith{[&](auto prod, const auto &rec, auto...) {
            prod(pith{[&](auto x, auto...) {
                        if constexpr (!parsec::is_parser_bark_v<decltype(x)>) {
                          auto ti = std::type_index(typeid(x));
                          if (set.contains(ti))
                            return;
                          set.insert(ti);
                          o(x, ident++);
                          x(rec);
                        }
                      },
                      [](auto f, auto s) { f(s); }});
          },
          [](auto f, auto s) { f(s); }});
  }
};
struct printgrammar {
  template <typename V> void operator()(const V &variable) const { //
    variables{}(
        [](auto v, auto i) {
          v(pith{[&](auto prod, auto...) {
                   size_t ident = i;
                   while (ident--)
                     std::cout << "  ";
                   std::cout << demangle(typeid(v).name()) << " ->";
                   prod(pith{[](auto symbol, auto...) {
                               std::cout << ' '
                                         << demangle(typeid(symbol).name());
                             },
                             [](auto t, auto p) { t(p); }});
                   std::cout << '\n';
                 },
                 [](auto t, auto p) { t(p); }});
        },
        variable);
  }
};
constexpr inline auto aaa = [](auto o) {
  o(head_ray_v, [](auto o) { o(head_ray_v, 2); });
};

int main() {
  printgrammar{}(expr{});
  return 9;
}
