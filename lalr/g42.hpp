#pragma once
#include "terminals.hpp"

struct E {
  Derives {
    o("E->TE'", [](auto o) {
      o(T{});
      o(E_{});
    });
  }
  struct E_ {
    Derives {
      o("E'->+TE'", [](auto o) {
        o(PLUS);
        o(T{});
        o(E_{});
      });
      o("E'->Є", [](auto o) { o([](auto o, auto) { o(0); }); });
    }
  };
  struct T {
    Derives {
      o("T->FT'", [](auto o) {
        o(F{});
        o(T_{});
      });
    }
    struct T_ {
      Derives {
        o("T'->*FT'", [](auto o) {
          o(MUL);
          o(F{});
          o(T_{});
        });
        o("T'->Є", [](auto o) { o([](auto o, auto) { o(0); }); });
      }
    };
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
