#pragma once
#include "terminals.hpp"
#define Derives MBark()()

struct S {
  Derives {
    o([](auto o) { o(E{}); });
  }
  struct E {
    Derives {
      o([](auto o) {
        o(E{});
        o(_PLUS_{});
        o(E{});
      });
      o([](auto o) {
        o(E{});
        o(_MUL_{});
        o(E{});
      });
      o([](auto o) {
        o(_LPAREN_{});
        o(E{});
        o(_RPAREN_{});
      });
      o([](auto o) { o(_ID_{}); });
    }
  };
};
