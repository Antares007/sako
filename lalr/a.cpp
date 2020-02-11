#include "g41.hpp"
#include <cxxabi.h>
#include <iostream>
#include <string>
#include <typeindex>

#define Is_End(x) std::is_same_v<decltype(nullptr), std::decay_t<decltype(x)>>
#define Is_Term(x) is_terminal_v<decltype(x)>
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

template <typename V> struct a_variable {
  const V &v;
  void operator()(const auto &o) const { //
    L1(L1(v))(o);
  }
};
template <typename V> a_variable(V) -> a_variable<V>;

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

template <typename V> struct lolr {
  V v;
  template <typename O, typename T> struct p_pith {
    const O &o;
    const char *bc;
    const char *ad;
    const T &tail;

    void operator()(head_ray *, Car h, Car t) const {
      if constexpr (Is_Term(h)) {
        int len;
        h(len, ad);
        if (len < 0) {
          if constexpr (Is_End(tail)) {
            o(-9, 0);
          } else {
            tail(v_pith<O>{o, bc});
          }
        } else {
          if constexpr (!Is_End(t)) {
            t(p_pith<O, T>{o, bc, ad + len, tail});
          } else if constexpr (!Is_End(tail)) {
            tail(v_pith<O>{o, bc});
          } else {
            o(-6, 0);
          }
        }
      } else {
      }
    }
  };
  template <typename O> struct v_pith {
    const O &o;
    const char *b;
    void operator()(head_ray *, Car v1, Car pt) const {
      v1([&](head_ray *, Car v1, Car rt) {
        if constexpr (std::is_invocable_r_v<void, decltype(v1), int &,
                                            const char *>)
          ; // print<decltype(h), decltype(pt), decltype(rt)> terminal;
        else
          v1([&](head_ray *, Car v2, Car) {
            v2([&](head_ray *, Car v2, Car) {
              print<decltype(v1), decltype(v2), decltype(pt), decltype(rt)>
                  variable;
            });
          });
      });
    }
  };
  void operator()(Car o, const char *b) const { //
    // auto a_variable = L1(L1(v));
    // auto vid = ID(a_variable);
    // a_variable(v_pith{o, b});
    v(v_pith<decltype(o)>{o, b});
  }
};
template <typename... Args> lolr(Args...) -> lolr<Args...>;
template <typename O, typename R> struct append_pith {
  const O &o;
  const R &r;
  void operator()(head_ray *, Car h, Car t) const {
    o(head_ray_v, h, [&](Car o) {
      if constexpr (std::is_invocable_r_v<void, decltype(t), decltype(*this)>)
        t(*this);
      else
        r(o);
    });
  };
};
template <typename O, typename R> append_pith(O, R) -> append_pith<O, R>;
int main() {
  auto input = "abb";
  using namespace grammar::aabb;
  using namespace grammar;
  auto l1 = L3(1, 2, 3);
  auto l2 = L2('a', "bo");
  l1(append_pith{o::rec{//
                        [](const auto &rec, head_ray *, Car h, Car t) {
                          std::cout << h;
                          if constexpr (std::is_invocable_r_v<void, decltype(t),
                                                              decltype(rec)>)
                            t(rec);
                        }},
                 l2});

  // auto p = a_pith{o::rays{
  //    [](int d, const char *err) { std::cerr << d << " " << err << '\n';
  //    },
  //    [](Car terminal, Car tail) { //
  //      auto n = type_name(terminal);
  //      if (n.find("operator") != std::string::npos)
  //        n = n.substr(0, n.find("operator"));
  //      auto n2 = type_name(tail);
  //      if (n2.find("operator") != std::string::npos)
  //        n2 = n2.substr(0, n2.find("operator"));
  //      std::cout << n << " - " << n2 << '\n';
  //    }}};
  // S{}(p);

  // lolr{E43::E{}}(
  //    [](auto v, size_t ident) {
  //      while (ident--)
  //        std::cout << "    ";
  //      std::cout << v << '\n';
  //    },
  //    input);
  return 9;
}
