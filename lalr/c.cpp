#include "../parsec.hpp"
template <typename T> struct print;
#define Derives template <typename O> void operator()(O o) const

struct E {
  Derives {
    o(".E->", [](auto o) {
      o("T", T{});
      o("R", R{});
    });
  }
  struct R {
    Derives {
      o(".R->", [](auto o) {
        o("+", parsec::str{"+"});
        o("T", T{});
        o("R", R{});
      });
      o(".R->", [](auto o) { o("Є", parsec::str{""}); });
    }
  };
  struct T {
    Derives {
      o(".T->", [](auto o) {
        o("F", F{});
        o("U", U{});
      });
    }
    struct U {
      Derives {
        o(".U->", [](auto o) {
          o("*", parsec::str{"*"});
          o("F", F{});
          o("U", U{});
        });
        o(".U->", [](auto o) { o("Є", parsec::str{""}); });
      }
    };
    struct F {
      Derives {
        o(".F->", [](auto o) {
          o("(", parsec::str{"("});
          o("E", E{});
          o(")", parsec::str{")"});
        });
        o(".F->", [](auto o) {
          o("id", parsec::chr{[](auto c) { return c <= 'a' && c <= 'z'; }});
        });
      }
    };
  };
};

#include <iostream>
template <size_t D = 0> struct compile {
  template <typename O, typename V>
  void operator()(O o, V v, const char *b) const {
    bool done = false;
    std::cout << std::endl;
    (v)([&](auto name, auto production) {
      if (done)
        return;
      std::cout << name;
      size_t length = 0;
      bool error = false;
      production([&](auto name, auto symbol) {
        if (error)
          return;
        auto rays = ::rays{[&](error_ray *, int) { error = true; },
                           [&](size_t len) {
                             std::cout << name;
                             length += len;
                           }};

        if constexpr (std::is_invocable_r_v<void, decltype(symbol),
                                            decltype(parsec::rays),
                                            const char *>) {
          symbol(rays, b + length);
        } else {
          compile<D + 1>{}(rays, symbol, b + length);
        }
      });
      if (!error) {
        done = true;
      }
    });
    if (!done)
      o(error_ray_v, -99);
  }
};

template <> struct compile<7> {
  template <typename O, typename V>
  void operator()(O o, V, const char *) const {
    std::cout << " 1024 ";
    o(error_ray_v, -1024);
  }
};

int main() {
  // Є
  const char *input = "a+b*o";
  auto log = ::rays{[](error_ray *, int err) { std::cerr << err << std::endl; },
                    [](auto x) { std::cout << x << std::endl; }};

  compile{}(log, E{}, input);
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
