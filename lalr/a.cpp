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

#include <set>
struct variables {
  template <typename O> struct pith {
    const O &o;
    size_t &ident;
    std::set<std::type_index> &set;
    const bool invar;

    void operator()(head_ray *, const auto &x) const {
      if constexpr (std::is_invocable_r_v<void, decltype(x), list_pith>) {
        if (invar) { // x is production
          x(pith{o, ident, set, false});
        } else { // x is variable
          auto ti = std::type_index(typeid(x));
          if (!set.contains(ti)) {
            set.insert(ti);
            o(x, ++ident);
            x(pith{o, ident, set, true});
          }
        }
      } else // x is terminal
        ;
    }
    void operator()(tail_ray *, const auto &tail) const { tail(*this); }
  };
  template <typename O, typename V>
  void operator()(const O &o, const V &variable) const { //
    size_t ident = 0;
    o(variable, ident);
    auto set = std::set<std::type_index>{};
    set.insert(std::type_index(typeid(variable)));
    variable(pith<O>{o, ident, set, true});
  }
};
struct printgrammar {
  template <typename F> struct pith {
    F f;
    void operator()(head_ray *, const auto &x) const { f(x); }
    void operator()(tail_ray *, const auto &tail) const { tail(*this); }
  };
  template <typename F> pith(F) -> pith<F>;

  template <typename V> void operator()(const V &variable) const { //
    variables{}(
        [](auto v, auto i) {
          v(pith{[&](auto prod) {
            size_t ident = i;
            while (ident--)
              std::cout << "  ";
            std::cout << demangle(typeid(v).name()) << " ->";
            // std::cout << ' ' << demangle(typeid(prod).name());
            prod(pith{[](auto &symbol) {
              std::cout << ' ' << demangle(typeid(symbol).name());
            }});
            std::cout << '\n';
          }});
        },
        variable);
  }
};
int main() { //
  printgrammar{}(expr{});
  // expr::term{}(printgrammar::pith{[](auto a) { //
  //  std::cout << 'a' << demangle(typeid(a).name()) << '\n';
  //  a(printgrammar::pith{[](auto b) { //
  //    std::cout << 'b' << demangle(typeid(b).name());
  //  }});
  //  std::cout << '\n';
  //}});
  return 9;
}
