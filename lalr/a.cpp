#include "../c.cpp"
#include "list_piths.hpp"
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
  auto name = demangle(typeid(s).name());
  // int pos = name.find_last_of(":");
  // if (pos > 0)
  //  name = name.substr(pos + 1, name.size() - pos - 1);
  return name;
};
template <typename... Ts> struct print;

#include "../purry.hpp"

#define LEND(x) std::is_same_v<decltype(nullptr), std::decay_t<decltype(x)>>
#define Is_terminal(x)                                                         \
  std::is_invocable_r_v<void, decltype(x), int &, const char *>
template <typename T>
constexpr inline bool is_tail_v =
    std::is_same_v<std::decay_t<T>, decltype(nullptr)>;

template <typename S>
constexpr inline bool is_terminal_v =
    std::is_invocable_r_v<void, S, int &, const char *>;

constexpr inline auto print_production = [](Car production) {
  auto str_prod = std::string{};
  production(o::rec{[&](auto s_rec, head_ray *, auto symbol, auto p_tail) {
    str_prod += " " + type_name(symbol);
    p_tail(s_rec);
  }});
  return str_prod;
};

#include <set>

template <typename O, int D = 0> struct a_pith {
  const O &o;
  void operator()(head_ray *, Car h, Car t) const { //
    using H = decltype(h);
    using T = decltype(t);
    if constexpr (D < -13)
      o(D, "depth");
    else if constexpr (std::is_invocable_r_v<void, H, a_pith<O, D - 1>> &&
                       std::is_invocable_r_v<void, T, a_pith<O, D - 1>>)
      o((h(a_pith<O, D - 1>{o}), h), (t(a_pith<O, D - 1>{o}), t));
    else if constexpr (std::is_invocable_r_v<void, H, a_pith<O, D - 1>>)
      o((h(a_pith<O, D - 1>{o}), h), t);
    else if constexpr (std::is_invocable_r_v<void, T, a_pith<O, D - 1>>)
      o(h, (t(a_pith<O, D - 1>{o}), t));
    else
      o(h, t);
  }
};
template <typename O, char D = 0> a_pith(O) -> a_pith<O, D>;

#define PPP                                                                    \
  print<decltype(s), decltype(s2), decltype(pt), decltype(pt2), decltype(st),  \
        decltype(st2)>                                                         \
      variable
template <typename V> struct lolr {
  V v;
  void operator()(Car o, const char *b) const { //
    unsigned int pos = 0;
    auto v_pith = o::rec{[&](Car vrec, head_ray *, Car p, Car pt) {
      p(o::rec{[&](Car srec, head_ray *, Car s, Car st) {
        if constexpr (Is_terminal(s)) {
          // print<decltype(s), decltype(pt), decltype(st)> terminal;
          int len;
          s(len, b + pos);
          if (len < 0) {
            if constexpr (LEND(pt))
              o(-1, 0);
            else
              pt(vrec);
          } else {
            o(len, pos);
            pos += len;
            if constexpr (LEND(st))
              o(9, 0);
            else
              st(srec);
          }
        } else {
          static_assert(!LEND(st), "teminal dollar must be at the end!");
          s([&](head_ray *, Car p2, Car pt2) {
            p2([&](head_ray *, Car s2, Car st2) {
              if constexpr (std::is_same_v<decltype(s),
                                           decltype(s2)>) { // left recusion
                PPP;
              } else if constexpr (Is_terminal(s2)) { // fitst terminal
                int len;
                s2(len, b + pos);
                if (len < 0) {
                  if constexpr (LEND(pt2)) {
                    o(-1, 0);
                  } else {
                    auto tvar = [&](Car o) {
                      pt2(concat_to_inners_pith{o, st});
                    };
                    if constexpr (LEND(pt))
                      tvar(vrec);
                    else
                      tvar(concat_pith{vrec, pt});
                  }
                } else { // throw tails (we found correct production to follow
                  o(len, pos);
                  pos += len;
                  if constexpr (LEND(st2))
                    st(srec);
                  else
                    st2(concat_pith{srec, st});
                }
              } else { // another variable
                PPP;
              };
            });
          });
        }
      }});
    }};
    [&](Car o) { v(concat_to_inners_pith{o, L1(grammar::dollar{})}); }(v_pith);
  }
};
template <typename... Args> lolr(Args...) -> lolr<Args...>;

int main() {
  auto input = "aabaab";
  using namespace grammar::aabb;
  using namespace grammar;
  lolr{aabb::S{}}(
      [](auto v, size_t ident) {
        while (ident--)
          std::cout << "    ";
        std::cout << v << '\n';
      },
      input);
  return 9;
}
