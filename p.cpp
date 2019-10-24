#include "_o_.hpp"
#include <functional>

template <typename... Ts> struct print;

namespace parse {

struct str {
  const char *str_;
  constexpr explicit str(const char *rhs) noexcept : str_(rhs) {}
  template <typename O>
  void operator()(const char *in, size_t avail, const O &o) const {
    int i = 0;
    while (char c = str_[i]) {
      if (c != in[i] || avail <= static_cast<size_t>(i))
        return o(-1);
      i++;
    }
    o(i);
  }
};

template <typename F> ///
struct u8cp {
  F f;
  template <typename U, typename = std::enable_if_t<std::is_convertible_v<
                            decltype(std::declval<U>()(9)), bool>>>
  constexpr explicit u8cp(U &&u) noexcept : f(std::forward<U>(u)) {}

  template <typename O>
  void operator()(const char *s, size_t avail, const O &o) const { ///
    if (avail < 1)
      return o(-1);
    if (0x00 == (0x80 & s[0])) {
      if (f(s[0]))
        o(1);
      else
        o(-1);
    } else if (0xc0 == (0xe0 & s[0])) {
      if (f(((0x1f & s[0]) << 6) | (0x3f & s[1])))
        o(2);
      else
        o(-1);
    } else if (0xe0 == (0xf0 & s[0])) {
      if (f(((0x0f & s[0]) << 12) | ((0x3f & s[1]) << 6) | (0x3f & s[2])))
        o(3);
      else
        o(-1);
    } else if (0xf0 == (0xf8 & s[0])) {
      if (f(((0x07 & s[0]) << 18) | ((0x3f & s[1]) << 12) |
            ((0x3f & s[2]) << 6) | (0x3f & s[3])))
        o(4);
      else
        o(-1);
    } else
      o(-1);
  };
};
template <typename F> u8cp(F)->u8cp<F>;

template <typename F> ///
struct chr {
  F f;
  template <typename U, typename = std::enable_if_t<std::is_convertible_v<
                            decltype(std::declval<U>()(' ')), bool>>>
  constexpr explicit chr(U &&u) noexcept : f(std::forward<U>(u)) {}

  template <typename O>
  void operator()(const char *in, size_t avail, const O &o) const { ///
    if (0 < avail && f(in[0]))
      o(1);
    else
      o(-1);
  };
};
template <typename F> chr(F)->chr<F>;

constexpr inline auto aray = [](auto) {};
using aray_t = decltype(aray);

template <typename T>
using if_bark_t = std::enable_if_t<
    std::is_invocable_r_v<void, T, const char *, size_t, aray_t>>;

template <typename P> ///
struct run {
  P p;
  template <typename U, typename = if_bark_t<U>>
  constexpr explicit run(U &&u) noexcept : p(std::forward<U>(u)) {}

  template <typename O>
  void operator()(const char *in, size_t avail, const O &o) const { ///
    int next = 0;
    p(in, avail, [&next](int x) {
      if (0 <= next) {
        if (x < 0)
          next = x;
        else
          next += x;
      }
    });
    o(next);
  };
};
template <typename P> run(P)->run<P>;

template <typename L, typename R> ///
struct or_ {
  L l;
  R r;
  template <typename UL, typename UR, typename = if_bark_t<UL>,
            typename = if_bark_t<UR>>
  constexpr explicit or_(UL &&ul, UR &&ur) noexcept
      : l(std::forward<UL>(ul)), r(std::forward<UR>(ur)) {}

  template <typename O>
  void operator()(const char *in, size_t avail, const O &o) const { ///
    l(in, avail, [&](int x) {
      if (x < 0)
        r(in, avail, o);
      else
        o(x);
    });
  };
};
template <typename L, typename R> or_(L, R)->or_<L, R>;

template <typename L, typename R, typename = if_bark_t<L>,
          typename = if_bark_t<R>>
constexpr auto operator|(L &&l, R &&r) {
  return or_(std::forward<L>(l), std::forward<R>(r));
}
template <typename L, typename = if_bark_t<L>>
constexpr auto operator|(L &&l, const char *r) {
  return or_(std::forward<L>(l), str{r});
}

template <typename L, typename R> ///
struct and_ {
  L l;
  R r;
  template <typename UL, typename UR, typename = if_bark_t<UL>,
            typename = if_bark_t<UR>>
  constexpr explicit and_(UL &&ul, UR &&ur) noexcept

      : l(std::forward<UL>(ul)), r(std::forward<UR>(ur)) {}

  template <typename O>
  void operator()(const char *in, size_t avail, const O &o) const { ///
    l(in, avail, [&](int x) {
      o(x);
      if (0 <= x)
        r(in + x, avail - x, o);
    });
  };
};
template <typename L, typename R> and_(L, R)->and_<L, R>;

template <typename L, typename R, typename = if_bark_t<L>,
          typename = if_bark_t<R>>
constexpr auto operator&(L &&l, R &&r) {
  return and_(std::forward<L>(l), std::forward<R>(r));
}
template <typename T, typename = if_bark_t<T>>
constexpr auto operator&(const char *l, T &&r) {
  return and_(str{l}, std::forward<T>(r));
}
template <typename T, typename = if_bark_t<T>>
constexpr auto operator&(T &&l, const char *r) {
  return and_(std::forward<T>(l), str{r});
}

template <typename P> struct many {
  P p;
  template <typename U, typename = if_bark_t<U>>
  constexpr explicit many(U &&u) noexcept : p(std::forward<U>(u)) {}

  template <typename O>
  void operator()(const char *in, size_t avail, const O &o) const {
    p(in, avail, [&](int x) {
      if (x < 0)
        o(0);
      else {
        o(x);
        this->operator()(in + x, avail - x, o);
      }
    });
  }
};
template <typename P> many(P)->many<P>;

template <typename P> struct one_or_many {
  P p;
  template <typename U, typename = if_bark_t<U>>
  constexpr explicit one_or_many(U &&u) noexcept : p(std::forward<U>(u)) {}

  template <typename O>
  void operator()(const char *in, size_t avail, const O &o) const {
    p(in, avail, [&](int x) {
      o(x);
      if (0 <= x)
        many{p}(in + x, avail - x, o);
    });
  }
};
template <typename P> one_or_many(P)->one_or_many<P>;

template <typename P> struct opt {
  P p;
  template <typename U, typename = if_bark_t<U>>
  constexpr explicit opt(U &&u) noexcept : p(std::forward<U>(u)) {}

  template <typename O>
  void operator()(const char *in, size_t avail, const O &o) const {
    p(in, avail, [&](int x) { o(x < 0 ? 0 : x); });
  }
};
template <typename P> opt(P)->opt<P>;
} // namespace parse

namespace parse::xml {
#define C constexpr inline auto
C Char = u8cp([](auto c) {
  return (c == 0x9) | (c == 0xA) | (c == 0xD) | (0x20 <= c && c <= 0xD7FF) |
         (0xE000 <= c && c <= 0xFFFD) | (0x10000 <= c && c <= 0x10FFFF);
});
C S = one_or_many(chr(
    [](auto c) { return (c == 0x20) | (c == 0x9) | (c == 0xD) | (c == 0xA); }));
C BaseChar = u8cp([](auto c) {
  return (0x41 <= c && c <= 0x5A) | (0x61 <= c && c <= 0x7A) |
         (0xC0 <= c && c <= 0xD6) | (0xD8 <= c && c <= 0xF6) |
         (0xF8 <= c && c <= 0xFF) | (0x100 <= c && c <= 0x131) |
         (0x134 <= c && c <= 0x13E) | (0x141 <= c && c <= 0x148) |
         (0x14A <= c && c <= 0x17E) | (0x180 <= c && c <= 0x1C3) |
         (0x1CD <= c && c <= 0x1F0) | (0x1F4 <= c && c <= 0x1F5) |
         (0x1FA <= c && c <= 0x217) | (0x250 <= c && c <= 0x2A8) |
         (0x2BB <= c && c <= 0x2C1) | (c == 0x386) |
         (0x388 <= c && c <= 0x38A) | (c == 0x38C) |
         (0x38E <= c && c <= 0x3A1) | (0x3A3 <= c && c <= 0x3CE) |
         (0x3D0 <= c && c <= 0x3D6) | (c == 0x3DA) | (c == 0x3DC) |
         (c == 0x3DE) | (c == 0x3E0) | (0x3E2 <= c && c <= 0x3F3) |
         (0x401 <= c && c <= 0x40C) | (0x40E <= c && c <= 0x44F) |
         (0x451 <= c && c <= 0x45C) | (0x45E <= c && c <= 0x481) |
         (0x490 <= c && c <= 0x4C4) | (0x4C7 <= c && c <= 0x4C8) |
         (0x4CB <= c && c <= 0x4CC) | (0x4D0 <= c && c <= 0x4EB) |
         (0x4EE <= c && c <= 0x4F5) | (0x4F8 <= c && c <= 0x4F9) |
         (0x531 <= c && c <= 0x556) | (c == 0x559) |
         (0x561 <= c && c <= 0x586) | (0x5D0 <= c && c <= 0x5EA) |
         (0x5F0 <= c && c <= 0x5F2) | (0x621 <= c && c <= 0x63A) |
         (0x641 <= c && c <= 0x64A) | (0x671 <= c && c <= 0x6B7) |
         (0x6BA <= c && c <= 0x6BE) | (0x6C0 <= c && c <= 0x6CE) |
         (0x6D0 <= c && c <= 0x6D3) | (c == 0x6D5) |
         (0x6E5 <= c && c <= 0x6E6) | (0x905 <= c && c <= 0x939) |
         (c == 0x93D) | (0x958 <= c && c <= 0x961) |
         (0x985 <= c && c <= 0x98C) | (0x98F <= c && c <= 0x990) |
         (0x993 <= c && c <= 0x9A8) | (0x9AA <= c && c <= 0x9B0) |
         (c == 0x9B2) | (0x9B6 <= c && c <= 0x9B9) |
         (0x9DC <= c && c <= 0x9DD) | (0x9DF <= c && c <= 0x9E1) |
         (0x9F0 <= c && c <= 0x9F1) | (0xA05 <= c && c <= 0xA0A) |
         (0xA0F <= c && c <= 0xA10) | (0xA13 <= c && c <= 0xA28) |
         (0xA2A <= c && c <= 0xA30) | (0xA32 <= c && c <= 0xA33) |
         (0xA35 <= c && c <= 0xA36) | (0xA38 <= c && c <= 0xA39) |
         (0xA59 <= c && c <= 0xA5C) | (c == 0xA5E) |
         (0xA72 <= c && c <= 0xA74) | (0xA85 <= c && c <= 0xA8B) |
         (c == 0xA8D) | (0xA8F <= c && c <= 0xA91) |
         (0xA93 <= c && c <= 0xAA8) | (0xAAA <= c && c <= 0xAB0) |
         (0xAB2 <= c && c <= 0xAB3) | (0xAB5 <= c && c <= 0xAB9) |
         (c == 0xABD) | (c == 0xAE0) | (0xB05 <= c && c <= 0xB0C) |
         (0xB0F <= c && c <= 0xB10) | (0xB13 <= c && c <= 0xB28) |
         (0xB2A <= c && c <= 0xB30) | (0xB32 <= c && c <= 0xB33) |
         (0xB36 <= c && c <= 0xB39) | (c == 0xB3D) |
         (0xB5C <= c && c <= 0xB5D) | (0xB5F <= c && c <= 0xB61) |
         (0xB85 <= c && c <= 0xB8A) | (0xB8E <= c && c <= 0xB90) |
         (0xB92 <= c && c <= 0xB95) | (0xB99 <= c && c <= 0xB9A) |
         (c == 0xB9C) | (0xB9E <= c && c <= 0xB9F) |
         (0xBA3 <= c && c <= 0xBA4) | (0xBA8 <= c && c <= 0xBAA) |
         (0xBAE <= c && c <= 0xBB5) | (0xBB7 <= c && c <= 0xBB9) |
         (0xC05 <= c && c <= 0xC0C) | (0xC0E <= c && c <= 0xC10) |
         (0xC12 <= c && c <= 0xC28) | (0xC2A <= c && c <= 0xC33) |
         (0xC35 <= c && c <= 0xC39) | (0xC60 <= c && c <= 0xC61) |
         (0xC85 <= c && c <= 0xC8C) | (0xC8E <= c && c <= 0xC90) |
         (0xC92 <= c && c <= 0xCA8) | (0xCAA <= c && c <= 0xCB3) |
         (0xCB5 <= c && c <= 0xCB9) | (c == 0xCDE) |
         (0xCE0 <= c && c <= 0xCE1) | (0xD05 <= c && c <= 0xD0C) |
         (0xD0E <= c && c <= 0xD10) | (0xD12 <= c && c <= 0xD28) |
         (0xD2A <= c && c <= 0xD39) | (0xD60 <= c && c <= 0xD61) |
         (0xE01 <= c && c <= 0xE2E) | (c == 0xE30) |
         (0xE32 <= c && c <= 0xE33) | (0xE40 <= c && c <= 0xE45) |
         (0xE81 <= c && c <= 0xE82) | (c == 0xE84) |
         (0xE87 <= c && c <= 0xE88) | (c == 0xE8A) | (c == 0xE8D) |
         (0xE94 <= c && c <= 0xE97) | (0xE99 <= c && c <= 0xE9F) |
         (0xEA1 <= c && c <= 0xEA3) | (c == 0xEA5) | (c == 0xEA7) |
         (0xEAA <= c && c <= 0xEAB) | (0xEAD <= c && c <= 0xEAE) |
         (c == 0xEB0) | (0xEB2 <= c && c <= 0xEB3) | (c == 0xEBD) |
         (0xEC0 <= c && c <= 0xEC4) | (0xF40 <= c && c <= 0xF47) |
         (0xF49 <= c && c <= 0xF69) | (0x10A0 <= c && c <= 0x10C5) |
         (0x10D0 <= c && c <= 0x10F6) | (c == 0x1100) |
         (0x1102 <= c && c <= 0x1103) | (0x1105 <= c && c <= 0x1107) |
         (c == 0x1109) | (0x110B <= c && c <= 0x110C) |
         (0x110E <= c && c <= 0x1112) | (c == 0x113C) | (c == 0x113E) |
         (c == 0x1140) | (c == 0x114C) | (c == 0x114E) | (c == 0x1150) |
         (0x1154 <= c && c <= 0x1155) | (c == 0x1159) |
         (0x115F <= c && c <= 0x1161) | (c == 0x1163) | (c == 0x1165) |
         (c == 0x1167) | (c == 0x1169) | (0x116D <= c && c <= 0x116E) |
         (0x1172 <= c && c <= 0x1173) | (c == 0x1175) | (c == 0x119E) |
         (c == 0x11A8) | (c == 0x11AB) | (0x11AE <= c && c <= 0x11AF) |
         (0x11B7 <= c && c <= 0x11B8) | (c == 0x11BA) |
         (0x11BC <= c && c <= 0x11C2) | (c == 0x11EB) | (c == 0x11F0) |
         (c == 0x11F9) | (0x1E00 <= c && c <= 0x1E9B) |
         (0x1EA0 <= c && c <= 0x1EF9) | (0x1F00 <= c && c <= 0x1F15) |
         (0x1F18 <= c && c <= 0x1F1D) | (0x1F20 <= c && c <= 0x1F45) |
         (0x1F48 <= c && c <= 0x1F4D) | (0x1F50 <= c && c <= 0x1F57) |
         (c == 0x1F59) | (c == 0x1F5B) | (c == 0x1F5D) |
         (0x1F5F <= c && c <= 0x1F7D) | (0x1F80 <= c && c <= 0x1FB4) |
         (0x1FB6 <= c && c <= 0x1FBC) | (c == 0x1FBE) |
         (0x1FC2 <= c && c <= 0x1FC4) | (0x1FC6 <= c && c <= 0x1FCC) |
         (0x1FD0 <= c && c <= 0x1FD3) | (0x1FD6 <= c && c <= 0x1FDB) |
         (0x1FE0 <= c && c <= 0x1FEC) | (0x1FF2 <= c && c <= 0x1FF4) |
         (0x1FF6 <= c && c <= 0x1FFC) | (c == 0x2126) |
         (0x212A <= c && c <= 0x212B) | (c == 0x212E) |
         (0x2180 <= c && c <= 0x2182) | (0x3041 <= c && c <= 0x3094) |
         (0x30A1 <= c && c <= 0x30FA) | (0x3105 <= c && c <= 0x312C) |
         (0xAC00 <= c && c <= 0xD7A3);
});
C Ideographic = u8cp([](auto c) {
  return (0x4E00 <= c && c <= 0x9FA5) | (c == 0x3007) |
         (0x3021 <= c && c <= 0x3029);
});
C CombiningChar = u8cp([](auto c) {
  return (0x300 <= c && c <= 0x345) | (0x360 <= c && c <= 0x361) |
         (0x483 <= c && c <= 0x486) | (0x591 <= c && c <= 0x5A1) |
         (0x5A3 <= c && c <= 0x5B9) | (0x5BB <= c && c <= 0x5BD) |
         (c == 0x5BF) | (0x5C1 <= c && c <= 0x5C2) | (c == 0x5C4) |
         (0x64B <= c && c <= 0x652) | (c == 0x670) |
         (0x6D6 <= c && c <= 0x6DC) | (0x6DD <= c && c <= 0x6DF) |
         (0x6E0 <= c && c <= 0x6E4) | (0x6E7 <= c && c <= 0x6E8) |
         (0x6EA <= c && c <= 0x6ED) | (0x901 <= c && c <= 0x903) |
         (c == 0x93C) | (0x93E <= c && c <= 0x94C) | (c == 0x94D) |
         (0x951 <= c && c <= 0x954) | (0x962 <= c && c <= 0x963) |
         (0x981 <= c && c <= 0x983) | (c == 0x9BC) | (c == 0x9BE) |
         (c == 0x9BF) | (0x9C0 <= c && c <= 0x9C4) |
         (0x9C7 <= c && c <= 0x9C8) | (0x9CB <= c && c <= 0x9CD) |
         (c == 0x9D7) | (0x9E2 <= c && c <= 0x9E3) | (c == 0xA02) |
         (c == 0xA3C) | (c == 0xA3E) | (c == 0xA3F) |
         (0xA40 <= c && c <= 0xA42) | (0xA47 <= c && c <= 0xA48) |
         (0xA4B <= c && c <= 0xA4D) | (0xA70 <= c && c <= 0xA71) |
         (0xA81 <= c && c <= 0xA83) | (c == 0xABC) |
         (0xABE <= c && c <= 0xAC5) | (0xAC7 <= c && c <= 0xAC9) |
         (0xACB <= c && c <= 0xACD) | (0xB01 <= c && c <= 0xB03) |
         (c == 0xB3C) | (0xB3E <= c && c <= 0xB43) |
         (0xB47 <= c && c <= 0xB48) | (0xB4B <= c && c <= 0xB4D) |
         (0xB56 <= c && c <= 0xB57) | (0xB82 <= c && c <= 0xB83) |
         (0xBBE <= c && c <= 0xBC2) | (0xBC6 <= c && c <= 0xBC8) |
         (0xBCA <= c && c <= 0xBCD) | (c == 0xBD7) |
         (0xC01 <= c && c <= 0xC03) | (0xC3E <= c && c <= 0xC44) |
         (0xC46 <= c && c <= 0xC48) | (0xC4A <= c && c <= 0xC4D) |
         (0xC55 <= c && c <= 0xC56) | (0xC82 <= c && c <= 0xC83) |
         (0xCBE <= c && c <= 0xCC4) | (0xCC6 <= c && c <= 0xCC8) |
         (0xCCA <= c && c <= 0xCCD) | (0xCD5 <= c && c <= 0xCD6) |
         (0xD02 <= c && c <= 0xD03) | (0xD3E <= c && c <= 0xD43) |
         (0xD46 <= c && c <= 0xD48) | (0xD4A <= c && c <= 0xD4D) |
         (c == 0xD57) | (c == 0xE31) | (0xE34 <= c && c <= 0xE3A) |
         (0xE47 <= c && c <= 0xE4E) | (c == 0xEB1) |
         (0xEB4 <= c && c <= 0xEB9) | (0xEBB <= c && c <= 0xEBC) |
         (0xEC8 <= c && c <= 0xECD) | (0xF18 <= c && c <= 0xF19) |
         (c == 0xF35) | (c == 0xF37) | (c == 0xF39) | (c == 0xF3E) |
         (c == 0xF3F) | (0xF71 <= c && c <= 0xF84) |
         (0xF86 <= c && c <= 0xF8B) | (0xF90 <= c && c <= 0xF95) |
         (c == 0xF97) | (0xF99 <= c && c <= 0xFAD) |
         (0xFB1 <= c && c <= 0xFB7) | (c == 0xFB9) |
         (0x20D0 <= c && c <= 0x20DC) | (c == 0x20E1) |
         (0x302A <= c && c <= 0x302F) | (c == 0x3099) | (c == 0x309A);
});
C Digit = u8cp([](auto c) {
  return (0x30 <= c && c <= 0x39) | (0x660 <= c && c <= 0x669) |
         (0x6F0 <= c && c <= 0x6F9) | (0x966 <= c && c <= 0x96F) |
         (0x9E6 <= c && c <= 0x9EF) | (0xA66 <= c && c <= 0xA6F) |
         (0xAE6 <= c && c <= 0xAEF) | (0xB66 <= c && c <= 0xB6F) |
         (0xBE7 <= c && c <= 0xBEF) | (0xC66 <= c && c <= 0xC6F) |
         (0xCE6 <= c && c <= 0xCEF) | (0xD66 <= c && c <= 0xD6F) |
         (0xE50 <= c && c <= 0xE59) | (0xED0 <= c && c <= 0xED9) |
         (0xF20 <= c && c <= 0xF29);
});
C Extender = u8cp([](auto c) {
  return (c == 0xB7) | (c == 0x2D0) | (c == 0x2D1) | (c == 0x387) |
         (c == 0x640) | (c == 0xE46) | (c == 0xEC6) | (c == 0x3005) |
         (0x3031 <= c && c <= 0x3035) | (0x309D <= c && c <= 0x309E) |
         (0x30FC <= c && c <= 0x30FE);
});

// Eq           ::=  S? '=' S?
C Eq = opt(S) & "=" & opt(S);

// Letter         ::=  BaseChar | Ideographic
C Letter = BaseChar | Ideographic;

// NameChar  ::=  Letter | Digit |  '.' | '-' | '_' | ':' |  CombiningChar |
// Extender
C NameChar = Letter | Digit | "." | "-" | "_" | ":" | CombiningChar | Extender;

// Name      ::=  (Letter | '_' | ':') (NameChar)*
C Name = run(Letter | "_" | ":") & many(NameChar);

C prolog = str{""};
C Misc = str{""};


C digit = chr{[](char c) { return '0' <= c && c <= '9'; }};
C hexdigit = chr{[](char c) {
  return '0' <= c && c <= '9' | 'a' <= c && c <= 'f' | 'A' <= c && c <= 'F';
}};
// CharRef      ::=  '&#' [0-9]+ ';' | '&#x' [0-9a-fA-F]+ ';'
C CharRef =
    "&#" & one_or_many(digit) & ";" | "&#x" & one_or_many(hexdigit) & ";";

C EntityRef = "&" & Name & ";";

// Reference    ::=  EntityRef | CharRef
C Reference = EntityRef | CharRef;

C noneof = [](const char *chars) {
  return chr([=](char c) { ///
    int i = 0;
    while (char m = chars[i]) {
      if (m == c)
        return false;
      i++;
    }
    return true;
  });
};

// AttValue       ::=  '"' ([^<&"] | Reference)* '"'
//                 |  "'" ([^<&'] | Reference)* "'"
C AttValue = "\"" & many(noneof("<&\"") | Reference) & "\"" |
             "'" & many(noneof("<&'") | Reference) & "'";

// Attribute     ::=  Name Eq AttValue
C Attribute = Name & Eq & AttValue;

// EmptyElemTag  ::=  '<' Name (S Attribute)* S? '/>'
C EmptyElemTag = "<" & Name & many(S & Attribute) & opt(S) & "/>";

// STag          ::=  '<' Name (S Attribute)* S? '>'
C STag = "<" & Name & many(S & Attribute) & opt(S) & ">";

// ETag          ::=  '</' Name S? '>'
C ETag = "</" & Name & opt(S) & ">";

// CharData  ::=  [^<&]* - ([^<&]* ']]>' [^<&]*)

// content       ::=  CharData?
//                   ((element | Reference | CDSect | PI | Comment) CharData?)*
C content = str{""};

// element       ::=  EmptyElemTag  | STag content ETag
C element = EmptyElemTag | STag & content & ETag;

// document  ::=  prolog element Misc*
C document = prolog & element & many(Misc);

} // namespace parse::xml

#include "zip.hpp"
#include <fstream>
#include <iostream>
#include <tuple>

static void t() {
  using namespace parse;
  auto run = [](const char *in, const auto &parser) {
    size_t Size = 0;
    while (in[Size] != '\0')
      Size++;
    parse::run{parser}(in, Size, [=](int x) {
      if (x < 0)
        std::cout << "error\n";
      else
        std::cout << "[" << std::string_view(in, x) << "] [" << in + x << "]\n";
    });
  };
  run("01!`ა\001ბAB", one_or_many{xml::Char});

  run("აბვ", str{"ა"});

  run("ACBABABAB", many{str{"A"} | str{"B"} | str{"C"}});
}

auto main() -> int {
  t();
  std::ifstream s("MyTest.xlsx", std::ios::binary);
  s.seekg(0, std::ios_base::end);
  const size_t size = static_cast<std::size_t>(s.tellg());
  std::vector<char> buf;
  buf.reserve(size);
  s.seekg(0);
  s.read(buf.data(), size);

  zip::unzip(reinterpret_cast<const uint8_t *>(buf.data()), size,
             _o_{[](const char *err) { std::cout << err << "\n"; },
                 [](std::string_view name, auto &&p) {
                   p(_o_{[&](auto err) { std::cout << err << "\n"; },
                         [&](auto, auto size) {
                           std::cout << name << " - " << size << "\n";
                         }});
                 }});

  // auto l = pand(pstr("a"), pstr("b"));
  // l("abo", _o_{[](int) { std::cout << "error\n"; },
  //                    [](const char *f, const char *s, const char *rest) {
  //                      std::cout << '[' << f << s << "] [" << rest << "]\n";
  //                    }});

  // auto l2 = por(pstr("a"), pstr("b"));
  // l2("b", _o_{[](int) { std::cout << "error\n"; },
  //                   [](const char *f, const char *rest) {
  //                     std::cout << '[' << f << "] [" << rest << "]\n";
  //                   }});
  //
}
