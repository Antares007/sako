struct head_ray;
constexpr inline auto lhead = static_cast<head_ray *>(nullptr);
struct ltail {
  void operator()(const auto &) const {}
};

namespace grammar::E41 {
struct plus {
  void operator()(const auto &o, const char *b) const {
    o(b[0] == '+' && b[1] != '=' && b[1] != '+' ? 1 : -1);
  }
};
struct mul {
  void operator()(const auto &o, const char *b) const {
    o(b[0] == '*' && b[1] != '=' ? 1 : -1);
  }
};
struct lparen {
  void operator()(const auto &o, const char *b) const {
    o(b[0] == '(' ? 1 : -1);
  }
};
struct rparen {
  void operator()(const auto &o, const char *b) const {
    o(b[0] == ')' ? 1 : -1);
  }
};
struct id {
  void operator()(const auto &o, const char *b) const {
    int i = 0;
    while ('a' <= b[i] && b[i] <= 'z')
      i++;
    o(i > 0 ? i : -1);
  }
};

struct E {
  void operator()(const auto &o) const {
    ([&](const auto &o) {
      o(
          lhead,
          [&](const auto &o) {
            o(lhead, E{}, [&](const auto &o) {
              o(lhead, plus{}, [&](const auto &o) { o(lhead, T{}, ltail{}); });
            });
          },
          [&](const auto &o) {
            o(
                lhead, [&](const auto &o) { o(lhead, T{}, ltail{}); }, ltail{});
          });
    })(o);
  };
  struct T {
    void operator()(const auto &o) const {
      ([&](const auto &o) {
        o(
            lhead,
            [&](const auto &o) {
              o(lhead, T{}, [&](const auto &o) {
                o(lhead, mul{}, [&](const auto &o) { o(lhead, F{}, ltail{}); });
              });
            },
            [&](const auto &o) {
              o(
                  lhead, [&](const auto &o) { o(lhead, F{}, ltail{}); },
                  ltail{});
            });
      })(o);
    };
    struct F {
      void operator()(const auto &o) const {
        ([&](const auto &o) {
          o(
              lhead,
              [&](const auto &o) {
                o(lhead, lparen{}, [&](const auto &o) {
                  o(lhead, E{},
                    [&](const auto &o) { o(lhead, rparen{}, ltail{}); });
                });
              },
              [&](const auto &o) {
                o(
                    lhead, [&](const auto &o) { o(lhead, id{}, ltail{}); },
                    ltail{});
              });
        })(o);
      };
    };
  };
};
;
} // namespace grammar::E41

namespace grammar::ll_k_problem {
struct a {
  void operator()(const auto &o, const char *b) const {
    o(b[0] == 'a' ? 1 : -1);
  }
};
struct Є {
  void operator()(const auto &o, const char *b) const { (void(b), o(0)); }
};
struct S {
  void operator()(const auto &o) const {
    ([&](const auto &o) {
      o(
          lhead,
          [&](const auto &o) {
            o(lhead, S{}, [&](const auto &o) { o(lhead, A{}, ltail{}); });
          },
          [&](const auto &o) {
            o(
                lhead, [&](const auto &o) { o(lhead, Є{}, ltail{}); }, ltail{});
          });
    })(o);
  };
  struct A {
    void operator()(const auto &o) const {
      ([&](const auto &o) {
        o(
            lhead, [&](const auto &o) { o(lhead, a{}, ltail{}); }, ltail{});
      })(o);
    };
  };
};
} // namespace grammar::ll_k_problem

namespace grammar::aabb {
struct a {
  void operator()(const auto &o, const char *b) const {
    o(b[0] == 'a' ? 1 : -1);
  }
};
struct b {
  void operator()(const auto &o, const char *b) const {
    o(b[0] == 'b' ? 1 : -1);
  }
};
struct S {
  void operator()(const auto &o) const {
    ([&](const auto &o) {
      o(
          lhead,
          [&](const auto &o) {
            o(lhead, A{}, [&](const auto &o) { o(lhead, A{}, ltail{}); });
          },
          ltail{});
    })(o);
  };
  struct A {
    void operator()(const auto &o) const {
      ([&](const auto &o) {
        o(
            lhead,
            [&](const auto &o) {
              o(lhead, a{}, [&](const auto &o) { o(lhead, A{}, ltail{}); });
            },
            [&](const auto &o) {
              o(
                  lhead, [&](const auto &o) { o(lhead, b{}, ltail{}); },
                  ltail{});
            });
      })(o);
    };
  };
};
} // namespace grammar::aabb
