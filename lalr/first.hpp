#pragma once
#include "terminals.hpp"
#include <typeindex>

// 1. If X is a terminal, then FIRST(X) = {'x'}
// 2. If X->Є, is a production rule, then add Є to FIRST(X).
// 3. If X->Y1 Y2 Y3 ..., Yn is a production,
// a. FIRST(X) = FIRST(Y1)
// b. If FIRST(Y1) contains Є then FIRST(X) = { FIRST(Y1) – Є } U { FIRST(Y2) }
// c. If FIRST(Yi) contains Є for all i = 1 to n, then add Є to FIRST(X).
struct first {
  template <typename O, typename V> struct pith {
    const O &o;
    const V &variable;
    bool &hasEpsilonProduction;
    template <typename P> void operator()(const P &production) const {
      bool done = false;
      production([&](const auto &symbol) {
        if (done)
          return;
        if constexpr (parsec::is_parser_bark_v<decltype(symbol)>) {
          done = true;
          o(this->variable, symbol);
        } else {
          done = std::type_index(typeid(this->variable)) ==
                 std::type_index(typeid(symbol));
          if (done)
            return;
          auto hasEpsilonProduction = false;
          auto p =
              pith<O, decltype(symbol)>{this->o, symbol, hasEpsilonProduction};
          symbol(p);
          done |= !hasEpsilonProduction;
        };
      });
      this->hasEpsilonProduction |= !done;
    }
  };
  template <typename O, typename V>
  void operator()(const O &o, const V &variable,
                  const bool includeEpsilon = true) const {
    bool hasEpsilonProduction = false;
    auto p = pith<O, V>{o, variable, hasEpsilonProduction};
    variable(p);
    if (hasEpsilonProduction && includeEpsilon)
      o(variable, EPSILON{});
  }
};
