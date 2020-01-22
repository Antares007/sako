#define RaySymbol                                                              \
  template <typename Symbol> void operator()(const Symbol &symbol) const
#define RayProduction                                                          \
  template <typename Production>                                               \
  void operator()(const char *n, const Production &production) const
#define NOP(x) (void(x))

#include "g43.hpp"
#include <iostream>
#include <typeindex>

// E -> E + E
// E -> E * E
// E -> (E)
// E -> id
// first(E) = { (, id }

struct E2 {
  Derives {
    o("1", [](auto o) {
      o(EP{});
      o(EM{});
    });
    o("2", [](auto) {});
  };
  struct EP {
    Derives {
      o("3", [](auto o) { o(PLUS); });
      o("4", [](auto) {});
    };
  };
  struct EM {
    Derives {
      o("5", [](auto o) { o(MUL); });
      // o("6", [](auto) {});
    };
  };
};

// 1. If X is a terminal, then FIRST(X) = {'x'}
// 2. If X->Є, is a production rule, then add Є to FIRST(X).
// 3. If X->Y1 Y2 Y3 ..., Yn is a production,
// a. FIRST(X) = FIRST(Y1)
// b. If FIRST(Y1) contains Є then FIRST(X) = { FIRST(Y1) – Є } U { FIRST(Y2) }
// c. If FIRST(Yi) contains Є for all i = 1 to n, then add Є to FIRST(X).
struct first {
  template <typename O> struct pith {
    const O &o;
    const std::type_index &ti;
    bool &hasEpsilonProduction;
    RayProduction {
      NOP(n);
      bool done = false;
      production([&](const auto &symbol) {
        if (done)
          return;
        if constexpr (parsec::is_parser_bark_v<decltype(symbol)>) {
          done = true;
          o(symbol);
        } else {
          auto ti = std::type_index(typeid(symbol));
          done = this->ti == ti;
          if (done)
            return;
          auto hasEpsilonProduction = false;
          auto p = pith<O>{this->o, ti, hasEpsilonProduction};
          symbol(p);
          done |= !hasEpsilonProduction;
        };
      });
      this->hasEpsilonProduction |= !done;
    }
  };
  template <typename O, typename V>
  void operator()(const O &o, const V &variable) const {
    bool hasEpsilonProduction = false;
    auto p =
        pith<O>{o, std::type_index(typeid(variable)), hasEpsilonProduction};
    variable(p);
    if (hasEpsilonProduction)
      o(parsec::str{"#"});
  }
};

int main() { //
  first{}([](const auto &x) { std::cout << x.match << ", "; }, S{});
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
