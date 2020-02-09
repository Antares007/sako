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
constexpr inline auto prn = [](const auto &o, const auto &svar) {
  auto set = std::set<std::type_index>();
  a_variable{svar}(o::rec{[&](Car arec, head_ray *, Car a_production, Car...) {
    a_production([&](head_ray *, Car variable, Car...) {
      variable(o::rec{[&](Car prec, head_ray *, Car production, Car... v_tail) {
        auto str_prod = std::string{};
        production(
            o::rec{[&](Car s_rec, head_ray *, Car symbol, Car... p_tail) {
              str_prod += " " + type_name(symbol);
              (p_tail(s_rec), ...);
            }});
        o(type_name(variable) + " ->" + str_prod);
        (v_tail(prec), ...);
      }});
      set.insert(std::type_index(typeid(variable)));
      variable(o::rec{[&](Car prec, head_ray *, Car production, Car... v_tail) {
        production(o::rec{[&](Car srec, head_ray *, Car symbol, Car... p_tail) {
          if constexpr (!is_terminal_v<decltype(symbol)>)
            if (!set.contains(std::type_index(typeid(symbol))))
              a_variable{symbol}(arec);
          (p_tail(srec), ...);
        }});
        (v_tail(prec), ...);
      }});
    });
  }});
};

template <typename V> struct lolr {
  V v;
#define CLOSURE                                                                \
  const O &o;                                                                  \
  const char *b;                                                               \
  const std::type_index &vid;                                                  \
  int &len;
#define ID(x) std::type_index(typeid(x))
  template <typename O> struct v_pith {
    CLOSURE
    void operator()(head_ray *, Car prod, Car v_tail) const {
      o("P>" + std::string(b), 1);
      prod(p_pith{o, b, vid, len});
      if (len < 0)
        (v_tail(v_pith{o, b, vid, len}));
      o("P<" + std::string(b + std::max(len, 0)), -1);
    }
  };
  template <typename O> v_pith(O, ...) -> v_pith<O>;
  template <typename O> struct p_pith {
    CLOSURE
    void operator()(head_ray *, Car symbol, Car p_tail) const { //
      o(type_name(symbol) + ">" + std::string(b), 1);
      if constexpr (is_terminal_v<decltype(symbol)>) {
        symbol(len, b);
      } else {
        symbol(v_pith{o, b, ID(symbol), len});
      }
      if (len >= 0) {
        (p_tail(p_pith{o, b + len, vid, len}));
      }
      o(type_name(symbol) + "<" + std::string(b + std::max(len, 0)), -1);
    }
  };
  template <typename O> p_pith(O, ...) -> p_pith<O>;
  void operator()(Car o, const char *b) const { //
    auto a_variable = L1(L1(v));
    auto vid = ID(a_variable);
    int len = 0;
    int pos = 0;
    a_variable(v_pith{[&](auto v, int i) {
                        pos += std::min(i, 0);
                        o(v, pos);
                        pos += std::max(i, 0);
                      },
                      b, vid, len});
    o(pos, 3);
  }
};
template <typename... Args> lolr(Args...) -> lolr<Args...>;

int main() {
  auto input = "abb";
  using namespace grammar::aabb;
  lolr{S{}}(
      [](auto v, size_t ident) {
        while (ident--)
          std::cout << "    ";
        std::cout << v << '\n';
      },
      input);
  return 9;
}
