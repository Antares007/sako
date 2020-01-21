#define RaySymbol                                                              \
  template <typename Symbol> void operator()(Symbol symbol) const
#define RayProduction                                                          \
  template <typename Production>                                               \
  void operator()(const char *n, Production production) const
#define NOP(x) (void(x))

#include "g42.hpp"
#include <iostream>
#include <typeindex>

// E -> E + E
// E -> E * E
// E -> (E)
// E -> id
// first(E) = { (, id }

struct E2 {
  Derives {
    o("1", [](auto o) { o(E3{}); });
    o("2", [](auto) {});
  };
  struct E3 {
    Derives {
      o("3", [](auto o) { o("A"); });
      o("4", [](auto) {});
    };
  };
};

constexpr inline auto countProductionSymbols = [](auto production) {
  size_t count = 0;
  production([&](auto) { count++; });
  return count;
};
constexpr inline auto hasEpsilonProduction = [](auto variable) {
  bool value = false;
  variable([&](const char *, auto production) {
    if (!value)
      value = countProductionSymbols(production) == 0;
  });
  return value;
};

// 1. If X is a terminal, then FIRST(X) = {'x'}
// 2. If X->Є, is a production rule, then add Є to FIRST(X).
// 3. If X->Y1 Y2 Y3 ..., Yn is a production,
// a. FIRST(X) = FIRST(Y1)
// b. If FIRST(Y1) contains Є then FIRST(X) = { FIRST(Y1) – Є } U { FIRST(Y2) }
// c. If FIRST(Yi) contains Є for all i = 1 to n, then add Є to FIRST(X).
template <typename T> struct print;
struct first {
  // std::cout << std::type_index(typeid(a)).hash_code();
  template <typename O> struct pith {
    const O &o;
    bool &hasEpsilonProduction;
    bool &done;
    RayProduction {
      NOP(n);
      size_t index = 0;
      production([&](auto symbol) { //
        if (done)
          return;
        if constexpr (std::is_same_v<const char *,
                                     std::decay_t<decltype(symbol)>>) {
          done = true;
          o(symbol);
        } else {
          auto hasEpsilonProduction = false;
          auto done = false;
          auto p = pith<O>{this->o, hasEpsilonProduction, done};
          symbol(p);
          this->done |= hasEpsilonProduction;
        };
        index++;
      });
      hasEpsilonProduction |= index == 0;
    }
  };
  template <typename O, typename V> void operator()(O o, V variable) const {
    bool hasEpsilonProduction = false;
    bool done = false;
    auto p = pith<O>{o, hasEpsilonProduction, done};
    variable(p);
    if (hasEpsilonProduction)
      o("epsilon");
  }
};

int main() { //
  first{}([](auto x) { std::cout << x << ", "; }, E2{});
  return 9;
}

// struct first {
//  struct ppith {
//    bool done = false;
//    size_t index = 0;
//    template <typename S> void operator()(S symbol) {
//      if (done)
//        return;
//      //
//      index++;
//    }
//  };
//  template <typename P> void operator()(const char *, P production) const {
//    //
//  }
//};
