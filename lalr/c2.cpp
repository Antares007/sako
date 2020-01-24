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
      o(Bark()() { o(PLUS); });
      o([](const auto &) {});
    };
  };
  struct factor {
    Derives {
      o([](const auto &o) { o(PLUS); });
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
            prod([](auto sym) {
              std::cout << ' ';
              if constexpr (parsec::is_parser_bark_v<decltype(sym)>)
                std::cout << sym.match;
              else
                std::cout << typeid(sym).name();
            });
            std::cout << '\n';
          });
        },
        variable);
  }
};

int main() { //
  printgrammar{}(exprsample{});
  printgrammar{}(expr{});
  // first{}(
  //    [](const auto &v, const auto &x) {
  //      std::cout << typeid(v).name() << " " << x.match << ", ";
  //    },
  //    expr{});
  return 9;
}
