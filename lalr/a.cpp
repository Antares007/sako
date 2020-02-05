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
  V v;
  void operator()(const auto &o) const { L1(L1(v))(o); }
};
template <typename V> a_variable(V) -> a_variable<V>;

template <typename> struct is_tail : std::false_type {};
template <> struct is_tail<ltail> : std::true_type {};
template <typename T>
constexpr inline bool is_tail_v = is_tail<std::decay_t<T>>::value;

template <typename S>
constexpr inline bool is_terminal_v =
    std::is_invocable_r_v<void, S, void (*)(int), const char *>;

constexpr inline auto print_production = [](Car production) {
  auto str_prod = std::string{};
  production(o::rec{[&](auto s_rec, head_ray *, auto symbol, auto s_tail) {
    str_prod += " " + type_name(symbol);
    s_tail(s_rec);
  }});
  return str_prod;
};

#include <set>
constexpr inline auto prn = [](const auto &o, const auto &svar) {
  auto set = std::set<std::type_index>();
  a_variable{svar}(o::rec{[&](Car arec, head_ray *, Car a_production, Car) {
    a_production([&](head_ray *, Car variable, Car) {
      variable(o::rec{[&](Car prec, head_ray *, Car production, Car p_tail) {
        auto str_prod = std::string{};
        production(o::rec{[&](Car s_rec, head_ray *, Car symbol, Car s_tail) {
          str_prod += " " + type_name(symbol);
          s_tail(s_rec);
        }});
        o(type_name(variable) + " ->" + str_prod);
        p_tail(prec);
      }});
      set.insert(std::type_index(typeid(variable)));
      variable(o::rec{[&](Car prec, head_ray *, Car production, Car p_tail) {
        production(o::rec{[&](Car srec, head_ray *, Car symbol, Car s_tail) {
          if constexpr (!is_terminal_v<decltype(symbol)>)
            if (!set.contains(std::type_index(typeid(symbol))))
              a_variable{symbol}(arec);
          s_tail(srec);
        }});
        p_tail(prec);
      }});
    });
  }});
};

// o("->" + print_production(production) +
//      (seen != "" ? " seen(" + seen + ")" : ""),
//  ident);
#include <deque>
constexpr inline auto olr = [](const auto &o, auto svar, const char *b) {
  o("tree for:", -1);
  o(b, 0);
  auto dollar = '\0';
  auto eq = [](const auto &a, const auto &b) {
    return std::type_index(typeid(a)) == std::type_index(typeid(b));
  };
  size_t ident = 0;
  size_t pos = 0;
  auto error{0};
  std::deque<std::type_index> gotos{};
  auto seen = std::string{};
  a_variable{svar}(o::rec{[&](auto a_rec, head_ray *, auto a_production, auto) {
    a_production(o::rec{[&](auto, head_ray *, auto variable, auto) {
      if (eq(svar, variable) && b[pos] == dollar)
        return o("accept!", ident);
      o("v(" + type_name(variable) + ")", ident);
      ++ident;
      variable(o::rec{[&](auto p_rec, head_ray *, auto production,
                          auto p_tail) {
        auto saved_pos = pos;
        auto index = -1;
        o("->" + print_production(production), ident);
        production(o::rec{[&](auto s_rec, head_ray *, auto symbol,
                              auto s_tail) {
          index++;
          if constexpr (is_terminal_v<decltype(symbol)>)
            symbol(
                [&](int len) {
                  if (len >= 0) {
                    o("p " + type_name(symbol) + " = " +
                          std::string(b + pos, len) +
                          " pos:" + std::to_string(pos),
                      ident);
                    pos += len;
                  } else {
                    o("e " + type_name(symbol), ident);
                    error = -1;
                  }
                },
                b + pos);
          else {
            if (index == 0 && std::is_same_v<std::decay_t<decltype(variable)>,
                                             std::decay_t<decltype(symbol)>>) {
              o("left recursion" + std::to_string(index), ident);
              error = -8;
            } else
              a_variable{symbol}(a_rec);
          }
          if (error) {
            if (!is_tail_v<decltype(p_tail)>) {
              o("reset", ident);
              pos = saved_pos;
              error = 0;
              p_tail(p_rec);
            }
          } else {
            if constexpr (is_tail_v<decltype(s_tail)>)
              o("reduce " + print_production(production) + " to " +
                    type_name(variable),
                ident);
            else
              s_tail(s_rec);
          }
        }});
      }});
      o("hello " + type_name(variable), ident);
      --ident;
    }});
  }});
  if (error)
    o("error!", ident);
  else
    o(pos, ident);
};

template <typename V, typename S> struct g0t0 {
  V v;
  S s;
  template <typename O> struct vpith {
    const O &o;
    const S &s;
    void operator()(head_ray *, Car production, Car p_tail) const { //
      production([&](head_ray *, Car symbol, Car s_tail) {
        if constexpr (std::is_same_v<std::decay_t<decltype(s)>,
                                     std::decay_t<decltype(symbol)>>)
          o(
              lhead,
              [&](Car o) {
                s_tail(spith<decltype(o)>{o, s});
              },
              [&](Car o) {
                p_tail(vpith<decltype(o)>{o, s});
              });
      });
    }
  };
  template <typename O> struct spith {
    const O &o;
    const S &s;
    void operator()(head_ray *, Car symbol, Car s_tail) const { //
      auto tail = [&](Car o) { s_tail(spith<decltype(o)>{o, s}); };
      if constexpr (is_terminal_v<decltype(symbol)>)
        o(lhead, symbol, tail);
      else
        o(lhead, g0t0<decltype(symbol), S>{symbol, s}, tail);
    }
  };
  template <typename O> void operator()(const O &o) const { //
    v(vpith<O>{o, s});
  }
};
template <typename... Args> g0t0(Args...) -> g0t0<Args...>;

int main() {
  auto var = grammar::E41::E{};
  // auto input = "a+b*o+a";
  auto gvar = g0t0{var, grammar::E41::E{}};
  prn([](auto v) { std::cout << v << '\n'; }, var);
  prn([](auto v) { std::cout << v << '\n'; }, gvar);
  // olr(
  //    [](auto v, size_t ident) {
  //      while (ident--)
  //        std::cout << "    ";
  //      std::cout << v << '\n';
  //    },
  //    var, input);
  return 9;
}
