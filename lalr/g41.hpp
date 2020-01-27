#pragma once
#include "terminals.hpp"
#define Derives template <typename O> void operator()(const O &o) const

struct expr {
  Derives {
    o([](auto o) {
      o(expr{});
      o([](auto o) { //
        o(PLUS{});
      });
    });
    o(expr{} >>= PLUS{} >>= term{});
    o(term{});
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
