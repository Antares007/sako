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
  int pos = name.find_last_of(":");
  if (pos > 0)
    name = name.substr(pos + 1, name.size() - pos - 1);
  return name;
};
template <typename... Ts> struct print;

#include "../purry.hpp"

template <typename V> struct argumented_variable {
  V v;
  void operator()(const auto &o) const { L1(L1(v))(o); }
};
template <typename V> argumented_variable(V) -> argumented_variable<V>;

#include <set>

template <typename> struct is_tail : std::false_type {};
template <> struct is_tail<ltail> : std::true_type {};
template <typename T>
constexpr inline bool is_tail_v = is_tail<std::decay_t<T>>::value;

constexpr inline auto prn = [](const auto &o, auto &&svar) {
  auto set = std::set<std::type_index>{};
  argumented_variable{Forward(svar)}(o::rec{[&](const auto &arec, head_ray *,
                                                auto &&argumented_production,
                                                const auto a_tail) {
    argumented_production(o::rec{[&](auto vrec, head_ray *, auto &&variable,
                                     auto v_tail) {
      auto var_type = std::type_index(typeid(variable));
      if (set.contains(var_type))
        return;
      set.insert(var_type);
      variable(o::rec{[&](auto prec, head_ray *, auto &&production,
                          auto p_tail) {
        auto name = type_name(variable);
        auto s = name + " ->";
        production(o::rec{[&](auto srec, head_ray *, auto &&symbol,
                              auto s_tail) {
          s = s + " " + type_name(symbol);
          if constexpr (!std::is_invocable_r_v<void, decltype(symbol),
                                               void (*)(int), const char *>) {
            argumented_variable{Forward(symbol)}(arec);
          }
          return s_tail(srec);
        }});
        o(s);
        return p_tail(prec);
      }});
      return v_tail(vrec);
    }});
    return a_tail(arec);
  }});
};

template <typename S>
constexpr inline bool is_terminal_v =
    std::is_invocable_r_v<void, S, void (*)(int), const char *>;

constexpr inline auto olr = [](const auto &o, auto &&svar, const char *b) {
  auto dollar = '\0';
  auto eq = [](const auto &a, const auto &b) {
    return std::type_index(typeid(a)) == std::type_index(typeid(b));
  };
  size_t ident = 0;
  size_t pos = 0;
  argumented_variable{Forward(svar)}(o::rec{
      [&](const auto &a_rec, head_ray *, auto &&argumented_production, auto) {
        argumented_production(
            o::rec{[&](auto, head_ray *, auto &&variable, auto) {
              if (eq(svar, variable) && b[pos] == dollar)
                return o("accept!", ident);
              ++ident;
              variable(o::rec{[&](auto, head_ray *, auto &&production, auto) {
                production(o::rec{[&](auto, head_ray *, auto &&symbol, auto) {
                  if constexpr (is_terminal_v<decltype(symbol)>) {
                    symbol(
                        [&](int len) {
                          if (len < 0)
                            ;
                          else
                            pos += len;
                        },
                        b + pos);
                  } else {
                    argumented_variable{Forward(symbol)}(a_rec);
                  }
                }});
              }});
              --ident;
            }});
      }});
};

int main() {
  prn([](auto v) { std::cout << v << '\n'; }, E{});
  olr(
      [](auto v, size_t ident) {
        while (ident--)
          std::cout << "  ";
        std::cout << v << '\n';
      },
      grammar_aabb::S{}, "aabb");
  return 9;
}
