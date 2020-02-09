struct head_ray;
constexpr inline auto head_ray_v = static_cast<head_ray *>(nullptr);
constexpr inline auto nil = [](const auto &) {};

namespace grammar::E41 {
struct plus {
  void operator()(int &o, const char *b) const {
    o = (b[0] == '+' && b[1] != '=' && b[1] != '+' ? 1 : -1);
  }
};
struct mul {
  void operator()(int &o, const char *b) const {
    o = (b[0] == '*' && b[1] != '=' ? 1 : -1);
  }
};
struct lparen {
  void operator()(int &o, const char *b) const { o = (b[0] == '(' ? 1 : -1); }
};
struct rparen {
  void operator()(int &o, const char *b) const { o = (b[0] == ')' ? 1 : -1); }
};
struct id {
  void operator()(int &o, const char *b) const {
    o = 0;
    for (; 'a' <= b[o] && b[o] <= 'z'; ++o)
      ;
    o = o ? o : -1;
  }
};

struct E {
  void operator()(const auto &o) const {
    ([&](const auto &o) {
      o(
          head_ray_v, // 0
          [&](const auto &o) {
            o(              //
                head_ray_v, // 0
                E{},        // 1
                [&](const auto &o) {
                  o(              //
                      head_ray_v, // 0
                      plus{},     // 1
                      [&](const auto &o) {
                        o(              //
                            head_ray_v, // 0
                            T{},        // 1
                            nil         // 2
                        );
                      } // 2
                  );
                } // 2
            );
          }, // 1
          [&](const auto &o) {
            o(
                head_ray_v, // 0
                [&](const auto &o) {
                  o(              //
                      head_ray_v, // 0
                      T{},        // 1
                      nil         // 2
                  );
                },  // 1
                nil // 2
            );
          } // 2
      );
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
                  [&](const auto &o) { o(head_ray_v, F{}, nil); });
              });
            },
            [&](const auto &o) {
              o(
                  head_ray_v, [&](const auto &o) { o(head_ray_v, F{}, nil); },
                  nil);
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
                    [&](const auto &o) { o(head_ray_v, rparen{}, nil); });
                });
              },
              [&](const auto &o) {
                o(
                    head_ray_v,
                    [&](const auto &o) { o(head_ray_v, id{}, nil); }, nil);
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
  void operator()(int &o, const char *b) const { o = (b[0] == 'a' ? 1 : -1); }
};
struct Є {
  void operator()(int &o, const char *b) const { (void(b), o = 0); }
};
struct S {
  void operator()(const auto &o) const {
    ([&](const auto &o) {
      o(
          head_ray_v,
          [&](const auto &o) {
            o(head_ray_v, S{}, [&](const auto &o) { o(head_ray_v, A{}, nil); });
          },
          [&](const auto &o) {
            o(
                head_ray_v, [&](const auto &o) { o(head_ray_v, Є{}, nil); },
                nil);
          });
    })(o);
  };

  struct A {
    void operator()(const auto &o) const {
      ([&](const auto &o) {
        o(
            head_ray_v, [&](const auto &o) { o(head_ray_v, a{}, nil); }, nil);
      })(o);
    };
  };
};
} // namespace grammar::ll_k_problem

namespace grammar::aabb {
struct a {
  void operator()(int &o, const char *b) const { o = (b[0] == 'a' ? 1 : -1); }
};
struct b {
  void operator()(int &o, const char *b) const { o = (b[0] == 'b' ? 1 : -1); }
};
struct S {
  void operator()(const auto &o) const {
    ([&](const auto &o) {
      o(
          head_ray_v,
          [&](const auto &o) {
            o(head_ray_v, A{}, [&](const auto &o) { o(head_ray_v, A{}, nil); });
          },
          nil);
    })(o);
  };
  struct A {
    void operator()(const auto &o) const {
      ([&](const auto &o) {
        o(
            head_ray_v,
            [&](const auto &o) {
              o(head_ray_v, a{},
                [&](const auto &o) { o(head_ray_v, A{}, nil); });
            },
            [&](const auto &o) {
              o(
                  head_ray_v, [&](const auto &o) { o(head_ray_v, b{}, nil); },
                  nil);
            });
      })(o);
    };
  };
};
} // namespace grammar::aabb

namespace grammar::ss {
struct a {
  void operator()(int &o, const char *b) const { o = (b[0] == 'a' ? 1 : -1); }
};
struct S {
  void operator()(const auto &o) const {
    ([&](const auto &o) {
      o(
          head_ray_v,
          [&](const auto &o) {
            o(head_ray_v, S{}, [&](const auto &o) {
              o(head_ray_v, S{},
                [&](const auto &o) { o(head_ray_v, S{}, nil); });
            });
          },
          [&](const auto &o) {
            o(
                head_ray_v, [&](const auto &o) { o(head_ray_v, a{}, nil); },
                nil);
          });
    })(o);
  };
};
} // namespace grammar::ss
