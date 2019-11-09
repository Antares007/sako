template <typename...> struct _p_;
template <typename Pith> struct _p_<Pith> {
  Pith pith;
  //
};

auto main() -> int { return 99; }
