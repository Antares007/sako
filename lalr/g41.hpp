#pragma once
#include "terminals.hpp"
#define Derives MBark()()
#define Production [](const auto &o)

struct expr {
  template <typename O> void operator()(const O &o) const {
    o([](const auto &o) {
      o(expr{});
      o(PLUS{});
      o(term{});
    });
    o([](const auto &o) { o(term{}); });
  }
  struct term {
    template <typename O> void operator()(const O &o) const {
      o([](const auto &o) {
        o(term{});
        o(MUL{});
        o(factor{});
      });
      o([](const auto &o) { o(factor{}); });
    }
  };
  struct factor {
    template <typename O> void operator()(const O &o) const {
      o([](const auto &o) {
        o(LPAREN{});
        o(expr{});
        o(RPAREN{});
      });
      o([](const auto &o) { o(ID{}); });
    }
  };
};
