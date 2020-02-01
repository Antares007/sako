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

constexpr inline auto olr = [](const auto &o, auto &&svar, const char *b) {
  auto dollar = '\0';
  auto eq = [](const auto &a, const auto &b) {
    return std::type_index(typeid(a)) == std::type_index(typeid(b));
  };
  size_t ident = 0;
  size_t pos = 0;
  argumented_variable{Forward(svar)}(o::rec{o::rays{
      [&](const auto &, head_ray *, auto &&argumented_production, auto...) {
        argumented_production(o::rec{o::rays{
            [&](auto, head_ray *, auto &&variable, size_t, size_t) {
              if (eq(svar, variable) && b[pos] == dollar)
                return o("accept!", ident);
              o(type_name(variable), ident);
              ++ident;
              variable(o::rec{o::rays{
                  [&](auto, head_ray *, auto &&production, size_t, size_t) {
                    //
                    //++ident;
                    auto skip = false;
                    production(o::rec{o::rays{
                        [&](auto, head_ray *, auto &&symbol, size_t, size_t) {
                          //
                          o(type_name(symbol), ident);
                          if constexpr (!std::is_invocable_r_v<
                                            void, decltype(symbol),
                                            void (*)(int), const char *>) {
                            if (eq(symbol, variable))
                              skip = true;
                            return;
                            // argumented_variable{Forward(symbol)}(rec);
                          } else {
                            //
                          }
                          //
                        },
                        LRec(!skip)}});
                    //--ident;
                  },
                  LRec(true)}});
              --ident;
            },
            LRec(true)}});
      },
      LRec(true)}});
};

int main() {
  // prn([](auto v) { std::cout << v << '\n'; }, E{});
  olr(
      [](auto v, size_t ident) {
        while (ident--)
          std::cout << "..";
        std::cout << v << '\n';
      },
      S{}, "aaa");
  return 9;
}
