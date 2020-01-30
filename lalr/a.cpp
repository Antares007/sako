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

template <typename F> struct pith {
  F f;
  void operator()(head_ray *, const auto &x, size_t i, size_t s) const {
    f(x, *this, i, s);
  }
  void operator()(tail_ray *, const auto &tail) const {
    std::cout << "N\n";
    tail(*this);
  }
};
template <typename F> pith(F) -> pith<F>;

#include <set>
struct variables {
  template <typename O, typename V>
  void operator()(const O &o, const V &variable) const {
    size_t ident = 0;
    auto set = std::set<std::type_index>{};
    LA(LA(variable))
    (pith{[&](const auto &prod, const auto &rec, auto...) {
      prod(pith{[&](const auto &x, auto...) {
        if constexpr (!parsec::is_parser_bark_v<decltype(x)>) {
          auto ti = std::type_index(typeid(x));
          if (set.contains(ti))
            return;
          set.insert(ti);
          o(x, ident++);
          x(rec);
        }
      }});
    }});
  }
};
struct printgrammar {
  template <typename V> void operator()(const V &variable) const { //
    variables{}(
        [](auto v, auto i) {
          v(pith{[&](const auto &prod, auto...) {
            size_t ident = i;
            while (ident--)
              std::cout << "  ";
            std::cout << demangle(typeid(v).name()) << " ->";
            prod(pith{[](auto &symbol, auto...) {
              std::cout << ' ' << demangle(typeid(symbol).name());
            }});
            std::cout << '\n';
          }});
        },
        variable);
  }
};
constexpr inline auto aaa = [](auto o) {
  o(head_ray_v, [](auto o) { o(head_ray_v, 2); });
};

int main() {
  printgrammar{}(expr{});
  // expr::factor{}(pith{[](auto a, const auto &) {
  //  a(pith{[](auto b) { std::cout << demangle(typeid(b).name()) << ' '; }});
  //  std::cout << '\n';
  //}});
  return 9;
}
