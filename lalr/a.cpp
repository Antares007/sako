#include "../c.cpp"
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

#define Is_nullptr(x)                                                          \
  std::is_same_v<decltype(nullptr), std::decay_t<decltype(x)>>
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

template <typename A, typename B> struct append {
  A a;
  B b;
  void operator()(Car o) const { a(pith<decltype(o)>{o, b}); }
  template <typename O> struct pith {
    const O &o;
    const B &b;
    void operator()(head_ray *, Car h, Car t) const {
      if constexpr (Is_nullptr(t))
        o(head_ray_v, h, b);
      else
        o(head_ray_v, h, [&](Car o) { t(pith<decltype(o)>{o, b}); });
    }
  };
};
template <typename A, typename B> append(A, B) -> append<A, B>;

template <typename A, typename B> struct map {
  A a;
  B b;
  void operator()(Car o) const { a(pith<decltype(o)>{o, b}); }
  template <typename O> struct pith {
    const O &o;
    const B &b;
    void operator()(head_ray *, auto &&h, auto &&t) const {
      if constexpr (Is_nullptr(t))
        o(head_ray_v, b(Forward(h)), Forward(t));
      else
        o(head_ray_v, b(Forward(h)), [&](Car o) {
          t(pith<decltype(o)>{o, b});
        });
    }
  };
};
template <typename A, typename B> map(A, B) -> map<A, B>;

template <typename P> struct avar {
  P p;
  void operator()(Car o) const { o(head_ray_v, p, nullptr); }
};
template <typename P> avar(P) -> avar<P>;

#define PPP                                                                    \
  print<decltype(s), decltype(s2), decltype(pt), decltype(pt2), decltype(st),  \
        decltype(st2)>                                                         \
      variable
template <typename V> struct lolr {
  V v;
  template <typename O> struct v_pith {
    const O &o;
    const char *b;
    void operator()(head_ray *, Car p, Car pt) const {
      p(o::rec{[&](Car srec, head_ray *, Car s, Car st) {
        if constexpr (Is_terminal(s)) {
          print<decltype(s), decltype(pt), decltype(st)> terminal;
        } else {
          s([&](head_ray *, Car p2, Car pt2) {
            p2([&](head_ray *, Car s2, Car st2) {
              if constexpr (std::is_same_v<decltype(s),
                                           decltype(s2)>) { // left recusion
                PPP;
              } else if constexpr (Is_terminal(s2)) { // fitst terminal
                int len;
                s2(len, b);
                if (len < 0) {
                  PPP;
                } else {
                  st2(append_pith{srec, st});
                }
              } else { // another variable
                PPP;
              };
            });
          });
        }
      }});
    }
  };
  void operator()(Car o, const char *b) const { //
    map{v, [](auto &&production) {
          return append{Forward(production), L1(grammar::dollar{})};
        }}(v_pith<decltype(o)>{o, b});
  }
  template <typename O, typename B> struct append_pith {
    const O &o;
    const B &b;
    void operator()(head_ray *, Car h, Car t) const {
      if constexpr (Is_nullptr(t))
        o(head_ray_v, h, b);
      else
        o(head_ray_v, h, [&](Car o) { t(append_pith<decltype(o), B>{o, b}); });
    }
  };
  template <typename O, typename B> append_pith(O, B) -> append_pith<O, B>;
};
template <typename... Args> lolr(Args...) -> lolr<Args...>;

int main() {
  auto input = "abb";
  using namespace grammar::aabb;
  using namespace grammar;
  map{L2(L3(1, 2, 3), L2('a', 'b')), [](auto &&i) {
        return append{Forward(i), L2(3, 6)};
      }}(o::rec{[](Car rec, head_ray *, Car v, Car t) {
    if constexpr (std::is_invocable_r_v<void, decltype(v), void (*)(...)>) {
      v(rec);
      std::cout << '\n';
    } else
      std::cout << v;
    if constexpr (!Is_nullptr(t))
      t(rec);
  }});

  lolr{aabb::S{}}(
      [](auto v, size_t ident) {
        while (ident--)
          std::cout << "    ";
        std::cout << v << '\n';
      },
      input);
  return 9;
}
