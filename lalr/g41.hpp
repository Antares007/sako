#pragma once
#include "terminals.hpp"
#define Derives MBark()()
#define Production [](const auto &o)
struct _expr_ {
  Derives {
    o(Production {
      o(_expr_{});
      o(_PLUS_{});
      o(_term_{});
    });
    o(Production { o(_term_{}); });
  }
  struct _term_ {
    Derives {
      o(Production {
        o(_term_{});
        o(_MUL_{});
        o(_factor_{});
      });
      o(Production { o(_factor_{}); });
    }
  };
  struct _factor_ {
    Derives {
      o(Production {
        o(_LPAREN_{});
        o(_expr_{});
        o(_RPAREN_{});
      });
      o(Production { o(_ID_{}); });
    }
  };
};
