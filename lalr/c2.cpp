#include "first.hpp"
#include "g41.hpp"
#include <cxxabi.h>
#include <iostream>
const char *demangle(const char *name);

struct exprsample {
  Derives {
    o([](const auto &o) {
      o(term{});
      o(factor{});
    });
    o([](const auto &) {});
  };
  struct term {
    Derives {
      o(Bark()() { o(PLUS{}); });
      o([](const auto &) {});
    };
  };
  struct factor {
    Derives {
      o([](const auto &o) { o(PLUS{}); });
      // o("6", [](const auto &) {});
    };
  };
};

constexpr inline auto log = [](size_t ident, auto... args) {
  while (ident) {
    std::cout << ' ';
    ident--;
  }
  ((std::cout << args), ...);
  std::cout << std::endl;
};

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
  Derives {
    o([&](const auto &o) { o(v); });
  }
};
template <typename... Args> argumented(Args...) -> argumented<Args...>;

struct olr {
  template <typename O> void operator()(const O &o, const char *b) {
    struct pith {
      const O &o;
      const char *b;
      void operator()() const { o(b); }
    };
    auto pp = pith{o, b};
    pp();
  };
};

int main() { //
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
  olr{}([](auto x) { std::cout << x << "\n"; }, "hey");

  auto g0t0 = [&](auto v, auto s) { return go{v, s}; };
  print(g0t0(print(ag), ID{}));

  return 9;
}

const char *demangle(const char *name) {
  char buf[10240];
  size_t size = 10240;
  int status;
  char *res = abi::__cxa_demangle(name, buf, &size, &status);
  return res;
}
