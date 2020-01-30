#pragma once

struct head_ray;
struct tail_ray;

constexpr inline auto lhead = static_cast<head_ray *>(nullptr);
constexpr inline auto ltail = static_cast<tail_ray *>(nullptr);

#define LA(a)                                                                  \
  [&](const auto &o) {                                                         \
    o(lhead, a, 0, 1);                                                         \
    (void(true));                                                              \
  }

#define LAB(a, b)                                                              \
  [&](const auto &o) {                                                         \
    o(lhead, a, 0, 1);                                                         \
    o(ltail, [&](const auto &o) {                                              \
      o(lhead, b, 1, 2);                                                       \
      (void(true));                                                            \
    });                                                                        \
  }

#define LABC(a, b, c)                                                          \
  [&](const auto &o) {                                                         \
    o(lhead, a, 0, 3);                                                         \
    o(ltail, [&](const auto &o) {                                              \
      o(lhead, b, 1, 3);                                                       \
      o(ltail, [&](const auto &o) {                                            \
        o(lhead, c, 2, 3);                                                     \
        (void(true));                                                          \
      });                                                                      \
    });                                                                        \
  }
#define Regular                                                                \
  template <typename O> void operator()(const O &o, const char *b) const
#define RG(name)                                                               \
  struct name {                                                                \
    void operator()(const auto &o, const char *b) const { o(RG_
#define RG_(expr) expr);                                                       \
  }                                                                            \
  }

RG(PLUS)(b[0] == '+' && b[1] != '=' && b[1] != '+' ? 1 : -1);
RG(MUL)(b[0] == '*' && b[1] != '=' ? 1 : -1);
RG(LPAREN)(b[0] == '(' ? 1 : -1);
RG(RPAREN)(b[0] == ')' ? 1 : -1);
RG(ID)('a' <= b[0] && b[0] <= 'z' ? 1 : -1);

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
