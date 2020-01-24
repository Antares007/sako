#pragma once
#include "terminals.hpp"
#define Derives MBark()()
#define Production [](const auto &o)
struct expr {
  Derives {
    o(Production {
      o(expr{});
      o(PLUS{});
      o(term{});
    });
    o(Production { o(term{}); });
  }
  struct term {
    Derives {
      o(Production {
        o(term{});
        o(MUL{});
        o(factor{});
      });
      o(Production { o(factor{}); });
    }
  };
  struct factor {
    Derives {
      o(Production {
        o(LPAREN{});
        o(expr{});
        o(RPAREN{});
      });
      o(Production { o(ID{}); });
    }
  };
};
