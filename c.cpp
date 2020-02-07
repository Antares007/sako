struct head_ray;
constexpr inline auto head_ray_v = static_cast<head_ray *>(nullptr);

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
          head_ray_v,
          [&](const auto &o) {
            o(head_ray_v, E{}, [&](const auto &o) {
              o(head_ray_v, plus{}, [&](const auto &o) { o(head_ray_v, T{}); });
            });
          },
          [&](const auto &o) {
            o(head_ray_v, [&](const auto &o) { o(head_ray_v, T{}); });
          });
    })(o);
  };
  struct T {
    void operator()(const auto &o) const {
      ([&](const auto &o) {
        o(
            head_ray_v,
            [&](const auto &o) {
              o(head_ray_v, T{}, [&](const auto &o) {
                o(head_ray_v, mul{},
                  [&](const auto &o) { o(head_ray_v, F{}); });
              });
            },
            [&](const auto &o) {
              o(head_ray_v, [&](const auto &o) { o(head_ray_v, F{}); });
            });
      })(o);
    };
    struct F {
      void operator()(const auto &o) const {
        ([&](const auto &o) {
          o(
              head_ray_v,
              [&](const auto &o) {
                o(head_ray_v, lparen{}, [&](const auto &o) {
                  o(head_ray_v, E{},
                    [&](const auto &o) { o(head_ray_v, rparen{}); });
                });
              },
              [&](const auto &o) {
                o(head_ray_v, [&](const auto &o) { o(head_ray_v, id{}); });
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
          head_ray_v,
          [&](const auto &o) {
            o(head_ray_v, S{}, [&](const auto &o) { o(head_ray_v, A{}); });
          },
          [&](const auto &o) {
            o(head_ray_v, [&](const auto &o) { o(head_ray_v, Є{}); });
          });
    })(o);
  };
  struct A {
    void operator()(const auto &o) const {
      ([&](const auto &o) {
        o(head_ray_v, [&](const auto &o) { o(head_ray_v, a{}); });
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
      o(head_ray_v, [&](const auto &o) {
        o(head_ray_v, A{}, [&](const auto &o) { o(head_ray_v, A{}); });
      });
    })(o);
  };
  struct A {
    void operator()(const auto &o) const {
      ([&](const auto &o) {
        o(
            head_ray_v,
            [&](const auto &o) {
              o(head_ray_v, a{}, [&](const auto &o) { o(head_ray_v, A{}); });
            },
            [&](const auto &o) {
              o(head_ray_v, [&](const auto &o) { o(head_ray_v, b{}); });
            });
      })(o);
    };
  };
};
} // namespace grammar::aabb

namespace grammar::ss {
struct a {
  void operator()(const auto &o, const char *b) const {
    o(b[0] == 'a' ? 1 : -1);
  }
};
struct S {
  void operator()(const auto &o) const {
    ([&](const auto &o) {
      o(
          head_ray_v,
          [&](const auto &o) {
            o(head_ray_v, S{}, [&](const auto &o) {
              o(head_ray_v, S{}, [&](const auto &o) { o(head_ray_v, S{}); });
            });
          },
          [&](const auto &o) {
            o(head_ray_v, [&](const auto &o) { o(head_ray_v, a{}); });
          });
    })(o);
  };
};
} // namespace grammar::ss
