struct head_ray;
constexpr inline auto head_ray_v = static_cast<head_ray *>(nullptr);

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
          head_ray_v,
          [&](const auto &o) {
            o(              //
                head_ray_v, //
                E{},        //
                [&](const auto &o) {
                  o(              //
                      head_ray_v, //
                      plus{},     //
                      [&](const auto &o) {
                        o(              //
                            head_ray_v, //
                            T{},        //
                            nullptr     //
                        );
                      });
                });
          },
          [&](const auto &o) {
            o(
                head_ray_v, //
                [&](const auto &o) {
                  o(              //
                      head_ray_v, //
                      T{},        //
                      nullptr     //
                  );
                },
                nullptr //
            );
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
                  [&](const auto &o) { o(head_ray_v, F{}, nullptr); });
              });
            },
            [&](const auto &o) {
              o(
                  head_ray_v,
                  [&](const auto &o) { o(head_ray_v, F{}, nullptr); }, nullptr);
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
                    [&](const auto &o) { o(head_ray_v, rparen{}, nullptr); });
                });
              },
              [&](const auto &o) {
                o(
                    head_ray_v,
                    [&](const auto &o) { o(head_ray_v, id{}, nullptr); },
                    nullptr);
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
            o(head_ray_v, S{},
              [&](const auto &o) { o(head_ray_v, A{}, nullptr); });
          },
          [&](const auto &o) {
            o(
                head_ray_v, [&](const auto &o) { o(head_ray_v, Є{}, nullptr); },
                nullptr);
          });
    })(o);
  };
  struct A {
    void operator()(const auto &o) const {
      ([&](const auto &o) {
        o(
            head_ray_v, [&](const auto &o) { o(head_ray_v, a{}, nullptr); },
            nullptr);
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
            o(              //
                head_ray_v, //
                A{},        //
                [&](const auto &o) {
                  o(              //
                      head_ray_v, //
                      A{},        //
                      nullptr     //
                  );
                });
          },
          nullptr //
      );
    })(o);
  };
  struct A {
    void operator()(const auto &o) const {
      ([&](const auto &o) {
        o(
            head_ray_v,
            [&](const auto &o) {
              o(              //
                  head_ray_v, //
                  a{},        //
                  [&](const auto &o) {
                    o(              //
                        head_ray_v, //
                        A{},        //
                        nullptr     //
                    );
                  });
            },
            [&](const auto &o) {
              o(
                  head_ray_v,
                  [&](const auto &o) {
                    o(              //
                        head_ray_v, //
                        b{},        //
                        nullptr     //
                    );
                  },
                  nullptr //
              );
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
                [&](const auto &o) { o(head_ray_v, S{}, nullptr); });
            });
          },
          [&](const auto &o) {
            o(
                head_ray_v, [&](const auto &o) { o(head_ray_v, a{}, nullptr); },
                nullptr);
          });
    })(o);
  };
};
} // namespace grammar::ss
