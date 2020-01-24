#include "first.hpp"
#include "g41.hpp"
#include <iostream>

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
            std::cout << typeid(v).name() << " ->";
            prod(
                [](auto symbol) { std::cout << ' ' << typeid(symbol).name(); });
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

constexpr inline auto go = [](const auto &o, const auto &variable,
                              const auto &to) {
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
};

int main() { //
  constexpr auto ag = [](const auto &o) { o([](const auto &o) { o(_E_{}); }); };
  auto s = (go, (go, ag, LPAREN{}), ID{});
  printgrammar{}(s);
  std::cout << "first set:\n";
  first{}(
      [](const auto &v, const auto &x) {
        std::cout << typeid(v).name() << " " << typeid(x).name() << '\n';
      },
      s);
  return 9;
}
