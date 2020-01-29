#pragma once
#include "../parsec.hpp"
#define Regular                                                                \
  template <typename O> void operator()(const O &o, const char *b) const
struct MUL {
  Regular {
    if (b[0] == '*' && b[1] != '=')
      o(1);
    else
      o(error_ray_v, -1);
  }
};
struct LPAREN {
  Regular {
    if (b[0] == '(')
      o(1);
    else
      o(error_ray_v, -1);
  }
};
struct RPAREN {
  Regular {
    if (b[0] == ')')
      o(1);
    else
      o(error_ray_v, -1);
  }
};
struct ID {
  Regular {
    if ('a' <= b[0] && b[0] <= 'z')
      o(1);
    else
      o(error_ray_v, -1);
  }
};
struct EPSILON {
  Regular {
    o(0);
    (void(b));
  }
};
