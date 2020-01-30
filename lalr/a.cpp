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
constexpr inline auto type_name = [](auto s) {
  return demangle(typeid(s).name());
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

template <typename V> struct argumented_variable {
  V v;
  void operator()(const auto &o) const { L1(L1(v))(o); }
};

#include <set>
constexpr inline auto prn = [](const auto &o, auto &&svar) {
  auto set = std::set<std::type_index>{};
  argumented_variable<decltype(svar)>{
      static_cast<decltype(svar) &&>(svar)}(pith{
      [&](auto &&argumented_production, const auto &rec, auto...) {
        argumented_production(pith{
            [&](auto &&variable, auto, size_t, size_t) {
              auto var_type = std::type_index(typeid(variable));
              if (set.contains(var_type))
                return;
              set.insert(var_type);
              variable(pith{
                  [&](auto &&production, auto...) {
                    auto name = type_name(variable);
                    auto s = name + " ->";
                    production(pith{
                        [&](auto &&symbol, auto...) {
                          s = s + " " + type_name(symbol);
                          if constexpr (!std::is_invocable_r_v<
                                            void, decltype(symbol),
                                            void (*)(int), const char *>) {
                            argumented_variable<decltype(symbol)>{
                                static_cast<decltype(symbol) &&>(symbol)}(rec);
                          }
                        },
                        [](auto t, auto &&p) {
                          t(static_cast<decltype(p) &&>(p));
                        }});
                    o(s);
                  },
                  [](auto t, auto &&p) { t(static_cast<decltype(p) &&>(p)); }});
            },
            [](auto t, auto &&p) { t(static_cast<decltype(p) &&>(p)); }});
      },
      [](auto t, auto &&p) { t(static_cast<decltype(p) &&>(p)); }});
};
int main() {
  prn([](auto v) { std::cout << v << '\n'; }, E{});
  return 9;
}

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
