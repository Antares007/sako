struct head_ray;
struct tail_ray;

constexpr inline auto lhead = static_cast<head_ray *>(nullptr);
constexpr inline auto ltail = static_cast<tail_ray *>(nullptr);

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
              o(lhead, plus{},
                [&](const auto &o) { o(lhead, T{}, [](auto) {}); });
            });
          },
          [&](const auto &o) {
            o(
                lhead, [&](const auto &o) { o(lhead, T{}, [](auto) {}); },
                [](auto) {});
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
                o(lhead, mul{},
                  [&](const auto &o) { o(lhead, F{}, [](auto) {}); });
              });
            },
            [&](const auto &o) {
              o(
                  lhead, [&](const auto &o) { o(lhead, F{}, [](auto) {}); },
                  [](auto) {});
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
                    [&](const auto &o) { o(lhead, rparen{}, [](auto) {}); });
                });
              },
              [&](const auto &o) {
                o(
                    lhead, [&](const auto &o) { o(lhead, id{}, [](auto) {}); },
                    [](auto) {});
              });
        })(o);
      };
    };
  };
};
;

namespace grammar {
struct a {
  void operator()(const auto &o, const char *b) const { o(b[0] == 'a'); }
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
            o(lhead, S{}, [&](const auto &o) { o(lhead, A{}, [](auto) {}); });
          },
          [&](const auto &o) {
            o(
                lhead, [&](const auto &o) { o(lhead, Є{}, [](auto) {}); },
                [](auto) {});
          });
    })(o);
  };

  struct A {
    void operator()(const auto &o) const {
      ([&](const auto &o) {
        o(
            lhead, [&](const auto &o) { o(lhead, a{}, [](auto) {}); },
            [](auto) {});
      })(o);
    };
  };
};
} // namespace grammar
namespace grammar_aabb {
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
            o(lhead, A{}, [&](const auto &o) { o(lhead, A{}, [](auto) {}); });
          },
          [](auto) {});
    })(o);
  };
  struct A {
    void operator()(const auto &o) const {
      ([&](const auto &o) {
        o(
            lhead,
            [&](const auto &o) {
              o(lhead, a{}, [&](const auto &o) { o(lhead, A{}, [](auto) {}); });
            },
            [&](const auto &o) {
              o(
                  lhead, [&](const auto &o) { o(lhead, b{}, [](auto) {}); },
                  [](auto) {});
            });
      })(o);
    };
  };
};
} // namespace grammar_aabb
