#pragma once
#include "terminals.hpp"

struct E {
  MBark()() {
    o(
        "E->TE'", Bark()() {
          o(T{});
          o(E_{});
        });
  }
  struct E_ {
    MBark()() {
      o(
          "E'->+TE'", Bark()() {
            o(PLUS);
            o(T{});
            o(E_{});
          });
      o(
          "E'->Є", Bark()() { o(Bark()(auto) { o(0); }); });
    }
  };
  struct T {
    MBark()() {
      o(
          "T->FT'", Bark()() {
            o(F{});
            o(T_{});
          });
    }
    struct T_ {
      MBark()() {
        o(
            "T'->*FT'", Bark()() {
              o(MUL);
              o(F{});
              o(T_{});
            });
        o(
            "T'->Є", Bark()() { o(Bark()(auto) { o(0); }); });
      }
    };
    struct F {
      MBark()() {
        o(
            "F->(E)", Bark()() {
              o(LPAREN);
              o(E{});
              o(RPAREN);
            });
        o(
            "F->id", Bark()() { o(ID); });
      }
    };
  };
};
