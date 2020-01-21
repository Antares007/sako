#pragma once
#include "terminals.hpp"

struct S {
  Derives {
    o("S->E", [](auto o) { o(E{}); });
  }
  struct E {
    Derives {
      o("E->E+E", [](auto o) {
        o(E{});
        o(PLUS);
        o(E{});
      });
      o("E->E*E", [](auto o) {
        o(E{});
        o(MUL);
        o(E{});
      });
      o("E->(E)", [](auto o) {
        o(LPAREN);
        o(E{});
        o(RPAREN);
      });
      o("E->id", [](auto o) { o(ID); });
    }
  };
};
