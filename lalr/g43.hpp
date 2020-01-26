#pragma once
#include "terminals.hpp"
#define Derives MBark()()

struct E {
  Derives {
    o([](auto o) {
      o(E{});
      o(PLUS{});
      o(E{});
    });
    o([](auto o) {
      o(E{});
      o(MUL{});
      o(E{});
    });
    o([](auto o) {
      o(LPAREN{});
      o(E{});
      o(RPAREN{});
    });
    o([](auto o) { o(ID{}); });
  }
};
