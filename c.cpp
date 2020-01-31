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
            o(lhead, E{}, 0, 3);
            o(ltail, [&](const auto &o) {
              o(lhead, plus{}, 1, 3);
              o(ltail, [&](const auto &o) {
                o(lhead, T{}, 2, 3); //
              });
            });
          },
          0, 1);
      o(ltail, [&](const auto &o) {
        o(
            lhead,
            [&](const auto &o) {
              o(lhead, T{}, 0, 1); //
            },
            1, 2);
      });
    })(o);
  };
  struct T {
    void operator()(const auto &o) const {
      ([&](const auto &o) {
        o(
            lhead,
            [&](const auto &o) {
              o(lhead, T{}, 0, 3);
              o(ltail, [&](const auto &o) {
                o(lhead, mul{}, 1, 3);
                o(ltail, [&](const auto &o) { o(lhead, F{}, 2, 3); });
              });
            },
            0, 1);
        o(ltail, [&](const auto &o) {
          o(
              lhead, [&](const auto &o) { o(lhead, F{}, 0, 1); }, 1, 2);
        });
      })(o);
    };
    struct F {
      void operator()(const auto &o) const {
        ([&](const auto &o) {
          o(
              lhead,
              [&](const auto &o) {
                o(lhead, lparen{}, 0, 3);
                o(ltail, [&](const auto &o) {
                  o(lhead, E{}, 1, 3);
                  o(ltail, [&](const auto &o) { o(lhead, rparen{}, 2, 3); });
                });
              },
              0, 1);
          o(ltail, [&](const auto &o) {
            o(
                lhead, [&](const auto &o) { o(lhead, id{}, 0, 1); }, 1, 2);
          });
        })(o);
      };
    };
  };
};
