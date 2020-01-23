#pragma once
#include "terminals.hpp"
#define Derives MBark()()
#define Production Bark()()

struct E {
  Derives {
    o(Production {
      o(E{});
      o(PLUS);
      o(T{});
    });
    o(Production { o(T{}); });
  }
  struct T {
    Derives {
      o(Production {
        o(T{});
        o(MUL);
        o(F{});
      });
      o(Production { o(F{}); });
    }
    struct F {
      Derives {
        o(Production {
          o(LPAREN);
          o(E{});
          o(RPAREN);
        });
        o(Production { o(ID); });
      }
    };
  };
};
