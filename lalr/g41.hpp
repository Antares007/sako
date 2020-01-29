#pragma once

struct head_ray;
struct tail_ray;

#define LHead static_cast<head_ray *>(nullptr)
#define LTail static_cast<tail_ray *>(nullptr)

#define LA(a)                                                                  \
  [&](const auto &o) {                                                         \
    o(LHead, a, 0, 1);                                                         \
    (void(true));                                                              \
  }

#define LAB(a, b)                                                              \
  [&](const auto &o) {                                                         \
    o(LHead, a, 0, 1);                                                         \
    o(LTail, [&](const auto &o) {                                              \
      o(LHead, b, 1, 2);                                                       \
      (void(true));                                                            \
    });                                                                        \
  }

#include "terminals.hpp"
#define LABC(a, b, c)                                                          \
  [&](const auto &o) {                                                         \
    o(LHead, a, 0, 3);                                                         \
    o(LTail, [&](const auto &o) {                                              \
      o(LHead, b, 1, 3);                                                       \
      o(LTail, [&](const auto &o) {                                            \
        o(LHead, c, 2, 3);                                                     \
        (void(true));                                                          \
      });                                                                      \
    });                                                                        \
  }
struct PLUS {
  void operator()(const auto &o, const char *b) const {
    o(b[0] == '+' && b[1] != '=' && b[1] != '+' ? 1 : -1);
  }
};
struct expr {
  void operator()(const auto &o) const {
    LAB(                              //
        LABC(expr{}, PLUS{}, term{}), //
        LA(term{})                    //
    )
    (o);
  }
  struct term {
    void operator()(const auto &o) const {
      LAB(                               //
          LABC(term{}, MUL{}, factor{}), //
          LA(factor{})                   //
      )
      (o);
    }
  };
  struct factor {
    void operator()(const auto &o) const {
      LAB(                                  //
          LABC(LPAREN{}, expr{}, RPAREN{}), //
          LA(ID{})                          //
      )
      (o);
    }
  };
};

// struct my_list_pith {
//  void operator()(head_ray *, auto symbol) const {}
//  void operator()(tail_ray *, auto tail) const { tail(*this); }
//};
