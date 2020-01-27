#pragma once
#include "terminals.hpp"
#define Production [](const auto &o)

struct expr {
  template <typename O> void operator()(const O &o) const {
    o(expr{} >>= PLUS{} >>= term{});
    o(term{});
  }
  struct term {
    template <typename O> void operator()(const O &o) const {
      o(term{} >>= MUL{} >>= factor{});
      o(factor{});
    }
  };
  struct factor {
    template <typename O> void operator()(const O &o) const {
      o(LPAREN{} >>= expr{} >>= RPAREN{});
      o(ID{});
    }
  };
};
