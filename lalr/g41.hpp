#pragma once
#include "terminals.hpp"
#define Derives MBark()()
#define Production [](const auto &o)
struct _E_ {
  Derives {
    o(Production {
      o(_E_{});
      o(PLUS{});
      o(_T_{});
    });
    o(Production { o(_T_{}); });
  }
  struct _T_ {
    Derives {
      o(Production {
        o(_T_{});
        o(MUL{});
        o(_F_{});
      });
      o(Production { o(_F_{}); });
    }
  };
  struct _F_ {
    Derives {
      o(Production {
        o(LPAREN{});
        o(_E_{});
        o(RPAREN{});
      });
      o(Production { o(ID{}); });
    }
  };
};
