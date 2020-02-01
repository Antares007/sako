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
//
constexpr inline auto prn = [](const auto &o, auto &&svar) {
  auto set = std::set<std::type_index>{};
  argumented_variable{Forward(svar)}(o::rec{o::rays{
      [&](const auto &rec, head_ray *, auto &&argumented_production, auto...) {
        argumented_production(o::rec{o::rays{
            [&](auto, head_ray *, auto &&variable, size_t, size_t) {
              auto var_type = std::type_index(typeid(variable));
              if (set.contains(var_type))
                return;
              set.insert(var_type);
              variable(o::rec{o::rays{
                  [&](auto, head_ray *, auto &&production, auto...) {
                    auto name = type_name(variable);
                    auto s = name + " ->";
                    production(o::rec{o::rays{
                        [&](auto, head_ray *, auto &&symbol, auto...) {
                          s = s + " " + type_name(symbol);
                          if constexpr (!std::is_invocable_r_v<
                                            void, decltype(symbol),
                                            void (*)(int), const char *>) {
                            argumented_variable{Forward(symbol)}(rec);
                          }
                        },
                        LRec(true)}});
                    o(s);
                  },
                  LRec(true)}});
            },
            LRec(true)}});
      },
      LRec(true)}});
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
  auto ok = false;
  //
  argumented_variable{Forward(svar)}(o::rec{o::rays{
      [&](const auto &rec, head_ray *, auto &&argumented_production, auto...) {
        //
        auto v_skip = false;
        argumented_production(o::rec{o::rays{
            [&](auto, head_ray *, auto &&variable, size_t, size_t) {
              if (eq(svar, variable) && b[pos] == dollar)
                return o("accept!", ident);
              ++ident;
              auto v_start_pos = pos;
              variable(o::rec{o::rays{
                  [&](auto, head_ray *, auto &&production, size_t pi,
                      size_t ps) {
                    auto p_skip = false;
                    production(o::rec{o::rays{
                        [&](auto, head_ray *, auto &&symbol, size_t, size_t) {
                          if constexpr (is_terminal_v<decltype(symbol)>) {
                            symbol(
                                [&](int len) {
                                  if (len < 0)
                                    p_skip = true;
                                  else
                                    pos += len;
                                },
                                b + pos);
                          }
                          // else if (si == 0 && eq(variable, symbol))
                          //  p_skip = true;
                          else {
                            argumented_variable{Forward(symbol)}(rec);
                            if (!ok)
                              p_skip = true;
                          }
                        },
                        LRec(!p_skip)}});
                    if (p_skip) {
                      pos = v_start_pos;
                      if (pi == ps - 1)
                        v_skip = true;
                    } else
                      ; // o("r", ident);
                  },
                  LRec(!v_skip)}});
              ok = v_skip;
              if (ok)
                o(type_name(variable), ident);

              --ident;
            },
            LRec(!v_skip)}});
      },
      LRec(true)}});
};

int main() {
  // prn([](auto v) { std::cout << v << '\n'; }, E{});
  olr(
      [](auto v, size_t ident) {
        while (ident--)
          std::cout << "  ";
        std::cout << v << '\n';
      },
      grammar_aabb::S{}, "aabb");
  return 9;
}
