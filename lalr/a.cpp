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
template <typename... Ts> struct print;

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
constexpr inline auto lrec = [](auto t, auto &&p) {
  t(static_cast<decltype(p) &&>(p));
};

#include <set>
struct variables {
  template <typename O, typename V>
  void operator()(const O &o, const V &variable) const {
    size_t ident = 0;
    auto set = std::set<std::type_index>{};
    L1(L1(variable))
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
                      lrec});
          },
          lrec});
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
                             lrec});
                   std::cout << '\n';
                 },
                 lrec});
        },
        variable);
  }
};

constexpr inline auto olr = [](const auto &o, auto &&svar, const char *b) {
  L1(L1(static_cast<decltype(svar) &&>(svar)))
  (pith{
      [&](auto &&prod, auto, size_t, size_t) {
        // bool inrec;
        prod(pith{
            [&](auto &&svar, auto, size_t, size_t) {
              o(b);
              o("Begin");
              svar(pith{
                  [&](auto &&prod, auto, size_t, size_t) {
                    prod(pith{[&](auto &&svar, auto, size_t i, size_t j) {
                                // print<decltype(svar)> p;
                                std::cout << demangle(typeid(svar).name())
                                          << ' ' << i << ' ' << j << '\n';
                              },
                              [](auto t, auto &&p) {
                                t(static_cast<decltype(p) &&>(p));
                              }});
                  },
                  [](auto t, auto &&p) { t(static_cast<decltype(p) &&>(p)); }});
              o("End");
            },
            [](auto t, auto &&p) { t(static_cast<decltype(p) &&>(p)); }});
        //
      },
      [](auto t, auto &&p) { t(static_cast<decltype(p) &&>(p)); }});
  //
};
template <typename O, size_t D = 2> struct prn_pith {
  const O &o;
  std::set<std::type_index> &set;
  void operator()(tail_ray *, auto &&t) const { t(*this); }
  void operator()(head_ray *, auto &&argumented_production, size_t,
                  size_t) const {
    argumented_production(pith{
        [&](auto &&variable, auto, size_t, size_t) {
          auto var_type = std::type_index(typeid(variable));
          if (set.contains(var_type))
            return;
          // set.insert(var_type);
          variable(pith{
              [&](auto &&production, auto, size_t, size_t) {
                auto name = demangle(typeid(variable).name());
                // name = name.substr(name.size() - 1, 1);
                auto s = name + " ->";
                production(
                    pith{[&](auto &&symbol, auto, size_t, size_t) {
                           auto name = demangle(typeid(symbol).name());
                           if constexpr (std::is_invocable_r_v<
                                             void, decltype(symbol),
                                             void (*)(int), const char *>)
                             ; // terminal
                           else if constexpr (D == 0)
                             ; //     name = name.substr(name.size() - 1, 1);
                           else {
                             //    name = name.substr(name.size() - 1, 1);
                             L1(L1(symbol))(prn_pith<O, D - 1>{o, set});
                           }
                           s = s + " " + name;
                         },
                         [](auto t, auto &&p) {
                           t(static_cast<decltype(p) &&>(p));
                         }});
                o(s);
              },
              [](auto t, auto &&p) { t(static_cast<decltype(p) &&>(p)); }});
        },
        [](auto t, auto &&p) { t(static_cast<decltype(p) &&>(p)); }});
  }
};
constexpr inline auto prn = [](const auto &o, auto &&svar) {
  auto set = std::set<std::type_index>{};
  L1(L1(static_cast<decltype(svar) &&>(svar)))
  (prn_pith<decltype(o)>{o, set});
};

int main() {
  //  printgrammar{}(E{});
  prn([](auto v) { std::cout << v << '\n'; }, E{});
  return 9;
}
