#include "first.hpp"
#include "g42.hpp"
#include <cxxabi.h>
#include <iostream>
#include <string>

constexpr inline auto demangle = [](const char *name) {
  int stat = 0;
  auto buff = abi::__cxa_demangle(name, nullptr, nullptr, &stat);
  auto rez = std::string(stat ? name : buff);
  free(buff);
  return rez;
};

template <typename V> struct index_ring {
  V v;
  template <typename O> void operator()(const O &o) const {
    size_t index = 0;
    v([&]<typename P>(P &&production) {
      o(
          [&, production = static_cast<P &&>(production)](const auto &o) {
            size_t index = 0;
            production([&]<typename S>(S &&symbol) {
              if constexpr (parsec::is_parser_bark_v<S>)
                o(static_cast<S &&>(symbol), v, index++);
              else
                o(index_ring<S>{static_cast<S &&>(symbol)}, v, index++);
            });
          },
          v, index++);
    });
  }
};
template <typename... Args> index_ring(Args...) -> index_ring<Args...>;

template <typename T> struct print;

#include <set>
struct virables {
  template <typename O> struct pith {
    const O &o;
    size_t &ident;
    std::set<std::type_index> &set;
    template <typename P> void operator()(const P &production) const {
      production([&](const auto &symbol) {
        if constexpr (!parsec::is_parser_bark_v<decltype(symbol)>) {
          auto ti = std::type_index(typeid(symbol));
          if (set.contains(ti))
            return;
          set.insert(ti);
          ident++;
          o(symbol, ident);
          symbol(*this);
          ident--;
        }
      });
    }
  };
  template <typename O, typename V>
  void operator()(const O &o, const V &variable) const { //
    size_t ident = 0;
    auto set = std::set<std::type_index>();
    set.insert(std::type_index(typeid(variable)));
    o(variable, ident);
    variable(pith<O>{o, ident, set});
  }
};
struct printgrammar {
  template <typename V> void operator()(const V &variable) const { //
    virables{}(
        [](auto v, auto i) {
          v([&](auto prod) {
            size_t ident = i;
            while (ident--)
              std::cout << "  ";
            std::cout << demangle(typeid(v).name()) << " ->";
            prod([](auto symbol) {
              std::cout << ' ' << demangle(typeid(symbol).name());
            });
            std::cout << '\n';
          });
        },
        variable);
  }
};

constexpr inline auto takefirst = [](const auto &o, const auto &production) {
  auto first = true;
  production([&](const auto &symbol) {
    if (first) {
      first = false;
      o(symbol);
    }
  });
};
constexpr inline auto skipfirst = [](const auto &o, const auto &production) {
  auto first = true;
  production([&](const auto &symbol) {
    if (first)
      first = false;
    else
      o(symbol);
  });
};

template <typename V, typename S> struct go {
  V variable;
  S to;
  template <typename O> void operator()(const O &o) const {
    const auto toty = std::type_index(typeid(to));
    virables{}(
        [&](const auto &variable, auto) {
          variable([&](const auto &production) {
            auto found = false;
            takefirst(
                [&](const auto &symbol) { //
                  found = toty == std::type_index(typeid(symbol));
                },
                production);
            if (found)
              o([production](const auto &o) { skipfirst(o, production); });
          });
        },
        variable);
  }
};
template <typename... Args> go(Args...) -> go<Args...>;

template <typename V> struct argumented {
  V v;
  void operator()(const auto &o) const {
    o([&](const auto &o) { o(v); });
  }
};
template <typename... Args> argumented(Args...) -> argumented<Args...>;

template <typename V> struct olr {
  V v;

  template <typename O> void operator()(const O &o, const char *b) {
    struct pith {
      const O &o;
      const char *b;
      void operator()() const { o(b); }
    };

    auto pp = pith{o, b};
    pp();
  }
};
template <typename... Args> olr(Args...) -> olr<Args...>;

int main() { //
  index_ring{expr{}}([](auto p, auto v, auto i) {
    p([&](auto, auto s, auto j) {
      std::cout << i << ' ' << j << ' ' << demangle(typeid(v).name()) << ' '
                << demangle(typeid(s).name()) << '\n';
    });
  });

  std::cout << "\n";
  auto ag = argumented{expr{}};
  auto print = [](const auto &variable) {
    std::cout << "\n";
    printgrammar{}(variable);
    std::cout << "first set:\n";
    first{}(
        [](const auto &v, const auto &x) {
          std::cout << demangle(typeid(v).name()) << " "
                    << demangle(typeid(x).name()) << '\n';
        },
        variable);
    return variable;
  };
  olr{expr{}}([](auto x) { std::cout << x << "\n"; }, "hey");

  auto g0t0 = [&](auto v, auto s) { return go{v, s}; };
  print(g0t0(print(ag), ID{}));

  return 9;
}
