#include "../parsec.hpp"
template <typename T> struct print;
#define Derives template <typename O> void operator()(O o) const

struct E {
  Derives {
    o([](auto o) {
      o(T{});
      o(R{});
    });
  }
  struct R {
    Derives {
      o([](auto o) {
        o(parsec::str{"+"});
        o(T{});
        o(R{});
      });
      o([](auto o) { o(parsec::str{""}); });
    }
  };
  struct T {
    Derives {
      o([](auto o) {
        o(F{});
        o(U{});
      });
    }
    struct U {
      Derives {
        o([](auto o) {
          o(parsec::str{"*"});
          o(F{});
          o(U{});
        });
        o([](auto o) { o(parsec::str{""}); });
      }
    };
    struct F {
      Derives {
        o([](auto o) {
          o(parsec::str{"("});
          o(E{});
          o(parsec::str{")"});
        });
        o([](auto o) {
          o(parsec::chr{[](auto c) { return c <= 'a' && c <= 'z'; }});
        });
      }
    };
  };
};

struct compile {
  template <typename O, typename V>
  void operator()(O o, V v, const char *b) const {
    bool skip = false;

    (v)([&](auto production) { // print<decltype(production)> p;
      if (skip)
        return;
      production([&](auto symbol) {
        if constexpr (std::is_invocable_r_v<void, decltype(symbol),
                                            decltype(parsec::rays),
                                            const char *>) {

          // symbol(::rays{[](error_ray *, int) {}, [](size_t) {}}, b);
          print<decltype(symbol)> p;
        } else {
          this->operator()(o, symbol, b);
        }
      });
    });
    //
  }
};

#include <iostream>

int main() {
  // Є
  const char *input = "a+b*o";
  compile{}(::rays{[](error_ray *, int err) { std::cerr << err << std::endl; },
                   [](auto x) { std::cout << x << std::endl; }},
            E{}, input);
  return 8;
}

/*
FIRST(X) for a grammar symbol X is the set of terminals that begin the
strings derivable from X.
Rules to compute FIRST set:
1. If x is a terminal, then FIRST(x) = { ‘x’ }
2. If x-> Є, is a production rule, then add Є to FIRST(x).
3. If X->Y1 Y2 Y3….Yn is a production,
  a. FIRST(X) = FIRST(Y1)
  b. If FIRST(Y1) contains Є then FIRST(X) = { FIRST(Y1) – Є } U { FIRST(Y2) }
  c. If FIRST (Yi) contains Є for all i = 1 to n, then add Є to FIRST(X).
*/
