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
constexpr inline auto rec_tail = [](auto &&o, tail_ray *, auto &&tail) {
  tail(Forward(o));
};

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
                        rec_tail}});
                    o(s);
                  },
                  rec_tail}});
            },
            rec_tail}});
      },
      rec_tail}});
};

constexpr inline auto olr = [](const auto &o, auto &&svar) {
  size_t ident = 0;
  o("MB", ident++);
  o(type_name(svar), ident);
  argumented_variable{Forward(svar)}(o::rec{o::rays{
      [&](const auto &, head_ray *, auto &&argumented_production, auto...) {
        o("BAP", ident++);

        argumented_production(o::rec{o::rays{
            [&](auto, head_ray *, auto &&variable, size_t, size_t) {
              o("BV", ident++);

              o(type_name(variable), ident);
              variable(o::rec{o::rays{
                  [&](auto, head_ray *, auto &&production, auto...) {
                    o("BP", ident++);

                    production(o::rec{o::rays{
                        [&](auto, head_ray *, auto &&symbol, auto...) {
                          o("BS", ident++);

                          o(type_name(symbol), ident);
                          if constexpr (!std::is_invocable_r_v<
                                            void, decltype(symbol),
                                            void (*)(int), const char *>) {
                            // argumented_variable{Forward(symbol)}(rec);
                          } else {
                            //
                          }

                          o("ES", --ident);
                        },
                        rec_tail}});
                    o("EP", --ident);
                  },
                  rec_tail}});
              o("EV", --ident);
            },
            rec_tail}});
        o("EAP", --ident);
      },
      rec_tail}});
  o("ME", --ident);
};

int main() {
  prn([](auto v) { std::cout << v << '\n'; }, E{});
  olr(
      [](auto v, size_t ident) {
        while (ident--)
          std::cout << "..";
        std::cout << v << '\n';
      },
      E{});
  return 9;
}
