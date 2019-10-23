#include "_o_.hpp"
#include "utf8.hpp"
#include <functional>

template <typename... Ts> struct print;

namespace parse {

struct str {
  const char *str_;
  constexpr explicit str(const char *rhs) noexcept : str_(rhs) {}
  template <typename O> void operator()(const char *in, const O &o) const {
    int i = 0;
    while (char c = str_[i]) {
      if (c != in[i])
        return o(-1);
      i++;
    }
    o(i);
  }
};

template <typename F> ///
struct satisfy {
  F f;
  template <typename U,
            typename = std::enable_if_t<std::is_convertible_v<
                decltype(std::declval<U>()(utf8::codepoint(""))), bool>>>
  constexpr explicit satisfy(U &&u) noexcept : f(std::forward<U>(u)) {}

  template <typename O> void operator()(const char *in, const O &o) const { ///
    if (in && f(utf8::codepoint(in)))
      o(utf8::next(in) - in);
    else
      o(-1);
  };
};
template <typename F> satisfy(F)->satisfy<F>;

constexpr inline auto aray = [](auto) {};
using aray_t = decltype(aray);

template <typename T>
using if_bark_t =
    std::enable_if_t<std::is_invocable_r_v<void, T, const char *, aray_t>>;

template <typename P> ///
struct run {
  P p;
  template <typename U, typename = if_bark_t<U>>
  constexpr explicit run(U &&u) noexcept : p(std::forward<U>(u)) {}

  template <typename O> void operator()(const char *in, const O &o) const { ///
    int next = 0;
    p(in, [&next](int x) {
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

  template <typename O> void operator()(const char *in, const O &o) const { ///
    l(in, [&](int x) {
      if (x < 0)
        r(in, o);
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

  template <typename O> void operator()(const char *in, const O &o) const { ///
    l(in, [&](int x) {
      o(x);
      if (0 <= x)
        r(in + x, o);
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

template <typename P> struct many {
  P p;
  template <typename U, typename = if_bark_t<U>>
  constexpr explicit many(U &&u) noexcept : p(std::forward<U>(u)) {}

  template <typename O> void operator()(const char *in, const O &o) const {
    p(in, [&](int x) {
      if (x < 0)
        o(0);
      else {
        o(x);
        this->operator()(in + x, o);
      }
    });
  }
};
template <typename P> many(P)->many<P>;

template <typename P> struct one_or_many {
  P p;
  template <typename U, typename = if_bark_t<U>>
  constexpr explicit one_or_many(U &&u) noexcept : p(std::forward<U>(u)) {}

  template <typename O> void operator()(const char *in, const O &o) const {
    p(in, [&](int x) {
      o(x);
      if (0 <= x)
        many{p}(in + x, o);
    });
  }
};
template <typename P> one_or_many(P)->one_or_many<P>;

template <typename P> struct opt {
  P p;
  template <typename U, typename = if_bark_t<U>>
  constexpr explicit opt(U &&u) noexcept : p(std::forward<U>(u)) {}

  template <typename O> void operator()(const char *in, const O &o) const {
    p(in, [&](int x) { o(x < 0 ? 0 : x); });
  }
};
template <typename P> opt(P)->opt<P>;
} // namespace parse

namespace parse {
template <size_t... N> struct cp;
template <size_t N> struct cp<N> {
  template <typename O> void operator()(const char *in, const O &o) const { ///
    if (utf8::codepoint(in) == N)
      o(utf8::next(in) - in);
    else
      o(-1);
  };
};
template <size_t S, size_t E> struct cp<S, E> {
  template <typename O> void operator()(const char *in, const O &o) const { ///
    const auto cp = utf8::codepoint(in);
    if (S <= cp && cp <= E)
      o(utf8::next(in) - in);
    else
      o(-1);
  };
};
} // namespace parse

namespace parse::xml {
constexpr inline auto Char = cp<0x9>() | cp<0xA>() | cp<0xD>() |
                             cp<0x20, 0xD7FF>() | cp<0xE000, 0xFFFD>() |
                             cp<0x10000, 0x10FFFF>();
constexpr inline auto S =
    one_or_many(cp<0x20>() | cp<0x9>() | cp<0xD>() | cp<0xA>());
constexpr inline auto prolog = str{""};
constexpr inline auto Misc = str{""};
constexpr inline auto Attribute = str{""};
constexpr inline auto STag = str{""};
constexpr inline auto ETag = str{""};
constexpr inline auto content = str{""};

constexpr inline auto BaseChar =
    cp<0x41, 0x5A>() | cp<0x61, 0x7A>() | cp<0xC0, 0xD6>() | cp<0xD8, 0xF6>() |
    cp<0xF8, 0xFF>() | cp<0x100, 0x131>() | cp<0x134, 0x13E>() |
    cp<0x141, 0x148>() | cp<0x14A, 0x17E>() | cp<0x180, 0x1C3>() |
    cp<0x1CD, 0x1F0>() | cp<0x1F4, 0x1F5>() | cp<0x1FA, 0x217>() |
    cp<0x250, 0x2A8>() | cp<0x2BB, 0x2C1>() | cp<0x386>() | cp<0x388, 0x38A>() |
    cp<0x38C>() | cp<0x38E, 0x3A1>() | cp<0x3A3, 0x3CE>() | cp<0x3D0, 0x3D6>() |
    cp<0x3DA>() | cp<0x3DC>() | cp<0x3DE>() | cp<0x3E0>() | cp<0x3E2, 0x3F3>() |
    cp<0x401, 0x40C>() | cp<0x40E, 0x44F>() | cp<0x451, 0x45C>() |
    cp<0x45E, 0x481>() | cp<0x490, 0x4C4>() | cp<0x4C7, 0x4C8>() |
    cp<0x4CB, 0x4CC>() | cp<0x4D0, 0x4EB>() | cp<0x4EE, 0x4F5>() |
    cp<0x4F8, 0x4F9>() | cp<0x531, 0x556>() | cp<0x559>() | cp<0x561, 0x586>() |
    cp<0x5D0, 0x5EA>() | cp<0x5F0, 0x5F2>() | cp<0x621, 0x63A>() |
    cp<0x641, 0x64A>() | cp<0x671, 0x6B7>() | cp<0x6BA, 0x6BE>() |
    cp<0x6C0, 0x6CE>() | cp<0x6D0, 0x6D3>() | cp<0x6D5>() | cp<0x6E5, 0x6E6>() |
    cp<0x905, 0x939>() | cp<0x93D>() | cp<0x958, 0x961>() | cp<0x985, 0x98C>() |
    cp<0x98F, 0x990>() | cp<0x993, 0x9A8>() | cp<0x9AA, 0x9B0>() | cp<0x9B2>() |
    cp<0x9B6, 0x9B9>() | cp<0x9DC, 0x9DD>() | cp<0x9DF, 0x9E1>() |
    cp<0x9F0, 0x9F1>() | cp<0xA05, 0xA0A>() | cp<0xA0F, 0xA10>() |
    cp<0xA13, 0xA28>() | cp<0xA2A, 0xA30>() | cp<0xA32, 0xA33>() |
    cp<0xA35, 0xA36>() | cp<0xA38, 0xA39>() | cp<0xA59, 0xA5C>() | cp<0xA5E>() |
    cp<0xA72, 0xA74>() | cp<0xA85, 0xA8B>() | cp<0xA8D>() | cp<0xA8F, 0xA91>() |
    cp<0xA93, 0xAA8>() | cp<0xAAA, 0xAB0>() | cp<0xAB2, 0xAB3>() |
    cp<0xAB5, 0xAB9>() | cp<0xABD>() | cp<0xAE0>() | cp<0xB05, 0xB0C>() |
    cp<0xB0F, 0xB10>() | cp<0xB13, 0xB28>() | cp<0xB2A, 0xB30>() |
    cp<0xB32, 0xB33>() | cp<0xB36, 0xB39>() | cp<0xB3D>() | cp<0xB5C, 0xB5D>() |
    cp<0xB5F, 0xB61>() | cp<0xB85, 0xB8A>() | cp<0xB8E, 0xB90>() |
    cp<0xB92, 0xB95>() | cp<0xB99, 0xB9A>() | cp<0xB9C>() | cp<0xB9E, 0xB9F>() |
    cp<0xBA3, 0xBA4>() | cp<0xBA8, 0xBAA>() | cp<0xBAE, 0xBB5>() |
    cp<0xBB7, 0xBB9>() | cp<0xC05, 0xC0C>() | cp<0xC0E, 0xC10>() |
    cp<0xC12, 0xC28>() | cp<0xC2A, 0xC33>() | cp<0xC35, 0xC39>() |
    cp<0xC60, 0xC61>() | cp<0xC85, 0xC8C>() | cp<0xC8E, 0xC90>() |
    cp<0xC92, 0xCA8>() | cp<0xCAA, 0xCB3>() | cp<0xCB5, 0xCB9>() | cp<0xCDE>() |
    cp<0xCE0, 0xCE1>() | cp<0xD05, 0xD0C>() | cp<0xD0E, 0xD10>() |
    cp<0xD12, 0xD28>() | cp<0xD2A, 0xD39>() | cp<0xD60, 0xD61>() |
    cp<0xE01, 0xE2E>() | cp<0xE30>() | cp<0xE32, 0xE33>() | cp<0xE40, 0xE45>() |
    cp<0xE81, 0xE82>() | cp<0xE84>() | cp<0xE87, 0xE88>() | cp<0xE8A>() |
    cp<0xE8D>() | cp<0xE94, 0xE97>() | cp<0xE99, 0xE9F>() | cp<0xEA1, 0xEA3>() |
    cp<0xEA5>() | cp<0xEA7>() | cp<0xEAA, 0xEAB>() | cp<0xEAD, 0xEAE>() |
    cp<0xEB0>() | cp<0xEB2, 0xEB3>() | cp<0xEBD>() | cp<0xEC0, 0xEC4>() |
    cp<0xF40, 0xF47>() | cp<0xF49, 0xF69>() | cp<0x10A0, 0x10C5>() |
    cp<0x10D0, 0x10F6>() | cp<0x1100>() | cp<0x1102, 0x1103>() |
    cp<0x1105, 0x1107>() | cp<0x1109>() | cp<0x110B, 0x110C>() |
    cp<0x110E, 0x1112>() | cp<0x113C>() | cp<0x113E>() | cp<0x1140>() |
    cp<0x114C>() | cp<0x114E>() | cp<0x1150>() | cp<0x1154, 0x1155>() |
    cp<0x1159>() | cp<0x115F, 0x1161>() | cp<0x1163>() | cp<0x1165>() |
    cp<0x1167>() | cp<0x1169>() | cp<0x116D, 0x116E>() | cp<0x1172, 0x1173>() |
    cp<0x1175>() | cp<0x119E>() | cp<0x11A8>() | cp<0x11AB>() |
    cp<0x11AE, 0x11AF>() | cp<0x11B7, 0x11B8>() | cp<0x11BA>() |
    cp<0x11BC, 0x11C2>() | cp<0x11EB>() | cp<0x11F0>() | cp<0x11F9>() |
    cp<0x1E00, 0x1E9B>() | cp<0x1EA0, 0x1EF9>() | cp<0x1F00, 0x1F15>() |
    cp<0x1F18, 0x1F1D>() | cp<0x1F20, 0x1F45>() | cp<0x1F48, 0x1F4D>() |
    cp<0x1F50, 0x1F57>() | cp<0x1F59>() | cp<0x1F5B>() | cp<0x1F5D>() |
    cp<0x1F5F, 0x1F7D>() | cp<0x1F80, 0x1FB4>() | cp<0x1FB6, 0x1FBC>() |
    cp<0x1FBE>() | cp<0x1FC2, 0x1FC4>() | cp<0x1FC6, 0x1FCC>() |
    cp<0x1FD0, 0x1FD3>() | cp<0x1FD6, 0x1FDB>() | cp<0x1FE0, 0x1FEC>() |
    cp<0x1FF2, 0x1FF4>() | cp<0x1FF6, 0x1FFC>() | cp<0x2126>() |
    cp<0x212A, 0x212B>() | cp<0x212E>() | cp<0x2180, 0x2182>() |
    cp<0x3041, 0x3094>() | cp<0x30A1, 0x30FA>() | cp<0x3105, 0x312C>() |
    cp<0xAC00, 0xD7A3>();

// Ideographic    ::=  [#x4E00-#x9FA5] | #x3007 | [#x3021-#x3029]
constexpr inline auto Ideographic =
    cp<0x4E00, 0x9FA5>() | cp<0x3007>() | cp<0x3021, 0x3029>();

constexpr inline auto CombiningChar =
    cp<0x300, 0x345>() | cp<0x360, 0x361>() | cp<0x483, 0x486>() |
    cp<0x591, 0x5A1>() | cp<0x5A3, 0x5B9>() | cp<0x5BB, 0x5BD>() | cp<0x5BF>() |
    cp<0x5C1, 0x5C2>() | cp<0x5C4>() | cp<0x64B, 0x652>() | cp<0x670>() |
    cp<0x6D6, 0x6DC>() | cp<0x6DD, 0x6DF>() | cp<0x6E0, 0x6E4>() |
    cp<0x6E7, 0x6E8>() | cp<0x6EA, 0x6ED>() | cp<0x901, 0x903>() | cp<0x93C>() |
    cp<0x93E, 0x94C>() | cp<0x94D>() | cp<0x951, 0x954>() | cp<0x962, 0x963>() |
    cp<0x981, 0x983>() | cp<0x9BC>() | cp<0x9BE>() | cp<0x9BF>() |
    cp<0x9C0, 0x9C4>() | cp<0x9C7, 0x9C8>() | cp<0x9CB, 0x9CD>() | cp<0x9D7>() |
    cp<0x9E2, 0x9E3>() | cp<0xA02>() | cp<0xA3C>() | cp<0xA3E>() | cp<0xA3F>() |
    cp<0xA40, 0xA42>() | cp<0xA47, 0xA48>() | cp<0xA4B, 0xA4D>() |
    cp<0xA70, 0xA71>() | cp<0xA81, 0xA83>() | cp<0xABC>() | cp<0xABE, 0xAC5>() |
    cp<0xAC7, 0xAC9>() | cp<0xACB, 0xACD>() | cp<0xB01, 0xB03>() | cp<0xB3C>() |
    cp<0xB3E, 0xB43>() | cp<0xB47, 0xB48>() | cp<0xB4B, 0xB4D>() |
    cp<0xB56, 0xB57>() | cp<0xB82, 0xB83>() | cp<0xBBE, 0xBC2>() |
    cp<0xBC6, 0xBC8>() | cp<0xBCA, 0xBCD>() | cp<0xBD7>() | cp<0xC01, 0xC03>() |
    cp<0xC3E, 0xC44>() | cp<0xC46, 0xC48>() | cp<0xC4A, 0xC4D>() |
    cp<0xC55, 0xC56>() | cp<0xC82, 0xC83>() | cp<0xCBE, 0xCC4>() |
    cp<0xCC6, 0xCC8>() | cp<0xCCA, 0xCCD>() | cp<0xCD5, 0xCD6>() |
    cp<0xD02, 0xD03>() | cp<0xD3E, 0xD43>() | cp<0xD46, 0xD48>() |
    cp<0xD4A, 0xD4D>() | cp<0xD57>() | cp<0xE31>() | cp<0xE34, 0xE3A>() |
    cp<0xE47, 0xE4E>() | cp<0xEB1>() | cp<0xEB4, 0xEB9>() | cp<0xEBB, 0xEBC>() |
    cp<0xEC8, 0xECD>() | cp<0xF18, 0xF19>() | cp<0xF35>() | cp<0xF37>() |
    cp<0xF39>() | cp<0xF3E>() | cp<0xF3F>() | cp<0xF71, 0xF84>() |
    cp<0xF86, 0xF8B>() | cp<0xF90, 0xF95>() | cp<0xF97>() | cp<0xF99, 0xFAD>() |
    cp<0xFB1, 0xFB7>() | cp<0xFB9>() | cp<0x20D0, 0x20DC>() | cp<0x20E1>() |
    cp<0x302A, 0x302F>() | cp<0x3099>() | cp<0x309A>();

constexpr inline auto Digit =
    cp<0x30, 0x39>() | cp<0x660, 0x669>() | cp<0x6F0, 0x6F9>() |
    cp<0x966, 0x96F>() | cp<0x9E6, 0x9EF>() | cp<0xA66, 0xA6F>() |
    cp<0xAE6, 0xAEF>() | cp<0xB66, 0xB6F>() | cp<0xBE7, 0xBEF>() |
    cp<0xC66, 0xC6F>() | cp<0xCE6, 0xCEF>() | cp<0xD66, 0xD6F>() |
    cp<0xE50, 0xE59>() | cp<0xED0, 0xED9>() | cp<0xF20, 0xF29>();

constexpr inline auto Extender =
    cp<0xB7>() | cp<0x2D0>() | cp<0x2D1>() | cp<0x387>() | cp<0x640>() |
    cp<0xE46>() | cp<0xEC6>() | cp<0x3005>() | cp<0x3031, 0x3035>() |
    cp<0x309D, 0x309E>() | cp<0x30FC, 0x30FE>();

// Letter         ::=  BaseChar | Ideographic
constexpr inline auto Letter = BaseChar | Ideographic;

// NameChar  ::=  Letter | Digit |  '.' | '-' | '_' | ':' |  CombiningChar |
// Extender
constexpr inline auto NameChar =
    Letter | Digit | "." | "-" | "_" | ":" | CombiningChar | Extender;

// Name      ::=  (Letter | '_' | ':') (NameChar)*
constexpr inline auto Name = run(Letter | "_" | ":") & many(NameChar);

// EmptyElemTag  ::=  '<' Name (S Attribute)* S? '/>'
constexpr inline auto EmptyElemTag = "<" & Name & many(S & Attribute) & opt(S);

// element       ::=  EmptyElemTag  | STag content ETag
constexpr inline auto element = EmptyElemTag | STag & content & ETag;

// document  ::=  prolog element Misc*
constexpr inline auto document = prolog & element & many(Misc);

} // namespace parse::xml

#include "zip.hpp"
#include <fstream>
#include <iostream>
#include <tuple>

static void t() {
  using namespace parse;
  auto run = [](const char *in, const auto &parser) {
    parse::run{parser}(in, [=](int x) {
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
