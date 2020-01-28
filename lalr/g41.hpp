#pragma once
#include "terminals.hpp"
#define Derives template <typename O> void operator()(const O &o) const
constexpr inline auto nil = [](const auto &) {};
constexpr inline auto cons = [](auto...) { return 1; };
struct head;
struct tail;
#define P(...)
struct expr {
  Derives {
    cons(                                              //
        cons(expr{}, cons(PLUS{}, cons(term{}, nil))), //
        cons(cons(term{}, nil), nil)                   //
        )(o);
  }
  struct term {
    Derives {
      o(term{} >>= MUL{} >>= factor{});
      o(factor{});
    }
  };
  struct factor {
    Derives {
      o(LPAREN{} >>= expr{} >>= RPAREN{});
      o(ID{});
    }
  };
};
