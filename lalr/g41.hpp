#pragma once
#include "terminals.hpp"
struct S {
  Derives {
    o("S->E", [](auto o) { o(E{}); });
  }
  struct E {
    Derives {
      o("E->E+T", [](auto o) {
        o(E{});
        o(PLUS);
        o(T{});
      });
      o("E->T", [](auto o) { o(T{}); });
    }
    struct T {
      Derives {
        o("T->T*F", [](auto o) {
          o(T{});
          o(MUL);
          o(F{});
        });
        o("T->F", [](auto o) { o(F{}); });
      }
      struct F {
        Derives {
          o("F->(E)", [](auto o) {
            o(LPAREN);
            o(E{});
            o(RPAREN);
          });
          o("F->id", [](auto o) { o(ID); });
        }
      };
    };
  };
};
